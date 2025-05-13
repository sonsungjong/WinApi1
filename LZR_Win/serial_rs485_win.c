#ifdef _WIN32
#include "serial_rs485.h"
#include <Windows.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <memory.h>
#include "RingBuffer1.h"
#include <mmsystem.h>			//#include <unistd.h>
#include "QueueGlobal.h"

#pragma comment(lib, "winmm.lib")

/*
	0xA5 : ���� ��� ���� [��]
	50001 : ���� ��� ���� [��]
	50002 : ��������ȸ(REQ) + ��� ���� ���� (RES) [��]
	50003 : ���� ���� [��]
	50004 : ������ ��ȸ(REQ) + ������ ���� (RES) [��]
	50005 : RAM������ EEPROM�� ����
	50006 : �α� �� ī���� �ʱ�ȭ
	50007 : RAM ���� �ʱ�ȭ
	50008 : �������� �ϰ� ��ȸ
	50009 : ������ 3�� ON/OFF
	50010 : ���� ������ ���� ��ȸ(REQ) + ����(RES)
	50011 : MDI ��ȯ
	50012 : ��Ʈ��Ʈ ON/OFF
	50013 : ��Ʈ��Ʈ ���� ��ȸ(REQ) + ����(RES)
	50014 : SETRAWDATAFRAMECOUNTERRESET
	50015 : X

*/

#define MODE_UNKNOWN				 0
#define MODE_MEASURE				 1
#define MODE_CONFIG					 2

#define MAX_CFG_SEND_COUNT			100			// ������� ��û �ִ� Ƚ��

// static���� ���� ������ �� ���Ϸ� ������Ų��
static HANDLE g_hSerial = INVALID_HANDLE_VALUE;
static const char* g_portName = "\\\\.\\COM9";
static const int g_baudRate = 460800;
static ST_RingBuffer ring1;
static HANDLE g_hRecvThread;
static HANDLE g_hProcessThread;
static ST_DataConfig g_sensorConfig;

static UINT g_mmTimerID = 0;
static volatile LONG g_curMode = MODE_MEASURE;      // 1: ����, 2: ����, 0: �˼����� (�⺻���� �������� �س��� ����û�� �غ���)
static CRITICAL_SECTION g_txCS;

static CRITICAL_SECTION g_cs_end;
static CONDITION_VARIABLE g_cv_end;
static int g_running_end = 0;			// [0] ����X [1] ����O
static volatile LONG g_cfgSendCount = 0;      // ������� A5 ���� Ƚ�� ī��Ʈ


// ���ź�
static DWORD WINAPI recvThreadSerial(void* lpParam)
{

	while (g_running_end == 0)
	{
		unsigned int bytesRead = 0;
		unsigned char tempBuf[16384] = { 0, };
		// ReadFile �� ���Ź޴´�
		EnterCriticalSection(&g_txCS);
		BOOL read_result = ReadFile(g_hSerial, tempBuf, sizeof(tempBuf), &bytesRead, NULL);
		LeaveCriticalSection(&g_txCS);
		if (read_result)
		{
			if (bytesRead > 0)
			{
				RingBuffer_write(&ring1, (const unsigned char*)tempBuf, bytesRead);
			}
		}
		else {
			DWORD err = GetLastError();
			printf("ReadFile ����: %u\n", err);

			// ���� �ݰ�
			EnterCriticalSection(&g_txCS);
			if (g_hSerial != INVALID_HANDLE_VALUE)
			{
				CloseHandle(g_hSerial);
				g_hSerial = INVALID_HANDLE_VALUE;
			}
			LeaveCriticalSection(&g_txCS);
		}
	}

	return 0;
}

// ó����
DWORD WINAPI processMessageThread(void* lpParam)
{
	while (g_running_end == 0)
	{
		unsigned char* data = NULL;
		size_t buf_size = RingBuffer_wait_read(&ring1, &data);
		if (data != NULL && buf_size > 0)
		{
			while (buf_size >= 6)					// �ּ� ���
			{
				ST_LZR920 stData;
				size_t idx = 0;

				// 1. SYNC
				memcpy(&stData.header_sync, data + idx, sizeof(stData.header_sync));
				idx += sizeof(stData.header_sync);

				if (stData.header_sync != HEADER_SYNC_VAL)
				{
					// �߸��� SYNC�̸� 1����Ʈ ������ �絿��ȭ
					memmove(data, data + 1, buf_size - 1);
					buf_size--;
					unsigned char add = RingBuffer_readbyte(&ring1);
					data[buf_size] = add;
					continue;
				}

				if (stData.header_sync == HEADER_SYNC_VAL)			// SYNC �˻� (FF FD FE FF)
				{
					memcpy(&stData.header_size, data + idx, sizeof(stData.header_size));
					idx += sizeof(stData.header_size);
					size_t total_len = sizeof(stData.header_sync) + sizeof(stData.header_size) + stData.header_size + sizeof(stData.footer_chk);

					// 4) ��ü �������� ���� �� �� ������ �����и�ŭ �� �б�
					if (total_len > buf_size)
					{
						unsigned long long need = total_len - buf_size;
						unsigned char* extra = NULL;
						unsigned long long got = RingBuffer_readbytes(&ring1, &extra, need);
						if (got > 0)
						{
							data = (unsigned char*)realloc(data, buf_size + got);
							memcpy(data + buf_size, extra, got);
							buf_size += got;
						}
						free(extra);
						// �׷��� ���ڶ�� ���� Ż���ؼ� ���� ���ʿ� �ٽ� �õ�
						if (total_len > buf_size)
							break;
					}

					if (total_len <= buf_size)
					{
						memcpy(&stData.message_cmd, data + idx, sizeof(stData.message_cmd));
						idx += sizeof(stData.message_cmd);

						size_t size_body_msg = stData.header_size - sizeof(stData.message_cmd);
						stData.message_data = NULL;
						if (size_body_msg > 0)
						{
							stData.message_data = (unsigned char*)malloc(size_body_msg);
							memcpy(stData.message_data, data + idx, size_body_msg);
							idx += size_body_msg;
						}

						if (stData.message_data)
						{
							memcpy(&stData.footer_chk, data + idx, sizeof(stData.footer_chk));

							// CHK ����
							unsigned short calc_chk = 0;
							for (int j = 0; j < sizeof(stData.message_cmd); ++j)
							{
								calc_chk += ((unsigned char*)&stData.message_cmd)[j];
							}
							for (int i = 0; i < size_body_msg; ++i)
							{
								calc_chk += stData.message_data[i];
							}

							if (stData.footer_chk == calc_chk)
							{
								printf("[processThread] header_sync OK! CMD=%d, SIZE=%d, footer=%d\n", stData.message_cmd, stData.header_size, stData.footer_chk);
							}

							if (stData.message_cmd == 50011)			// MDI
							{
								void* msg_data = malloc(sizeof(unsigned short) + sizeof(unsigned short) + size_body_msg);
								if (msg_data)
								{
									unsigned long long copy_idx = 0;
									memcpy((char*)msg_data + copy_idx, &stData.header_size, sizeof(unsigned short));
									copy_idx += sizeof(unsigned short);
									memcpy((char*)msg_data + copy_idx, &stData.message_cmd, sizeof(unsigned short));
									copy_idx += sizeof(unsigned short);
									memcpy((char*)msg_data + copy_idx, stData.message_data, size_body_msg);
									copy_idx += size_body_msg;

									enqueue_CircularQueue(&g_recvQueue, msg_data, copy_idx);
									free(msg_data);
								}

								if (size_body_msg == 2202)
								{
									// ID + Frame counter
									// Plane Number + MDI

									// �տ� 6����Ʈ�� �ٽ� �����
									// �� ���� Plane Number 1����Ʈ�� �а�
									// �� ���� 548 ����Ʈ�� unsigned short�� ���� ��ȯ�Ѵ���
									// �ٽ� Plane Number 1����Ʈ�� �а�
									// �ٽ� 548 ����Ʈ�� unsigned short�� ���� ��ȯ�ϰ�... �� 4��
									//printf("MDI\n");
								}
								else if (size_body_msg == 2196)
								{
									// Plane Number + MDI
									//printf("MDI\n");
								}
								else if (size_body_msg == 2216)
								{
									// ID + Frame counter
									// CTN + VNR + Error log + Hot reset counter
									// Plane Number + MDI
									//printf("MDI\n");
								}
								else if (size_body_msg == 2210)
								{
									// CTN + VNR + Error log + Hot reset counter
									// Plane Number + MDI
									//printf("MDI\n");
								}
								else {

								}
								break;
							}
							else if (stData.message_cmd == 50002)			// ��� ���� ����
							{
								void* msg_data = malloc(sizeof(unsigned short) + sizeof(unsigned short) + size_body_msg);
								if (msg_data)
								{
									unsigned long long copy_idx = 0;
									memcpy((char*)msg_data + copy_idx, &stData.header_size, sizeof(unsigned short));
									copy_idx += sizeof(unsigned short);
									memcpy((char*)msg_data + copy_idx, &stData.message_cmd, sizeof(unsigned short));
									copy_idx += sizeof(unsigned short);
									memcpy((char*)msg_data + copy_idx, stData.message_data, size_body_msg);
									copy_idx += size_body_msg;

									enqueue_CircularQueue(&g_recvQueue, msg_data, copy_idx);
									free(msg_data);
								}
								unsigned char mode = stData.message_data[0];
								if (mode == 1)
								{
									printf("=============>���� ��� ����\n");
									if (InterlockedCompareExchange(&g_curMode, 0, 0) != MODE_MEASURE) {
										stopTimerRequestConfigurationMode();
										InterlockedExchange(&g_curMode, MODE_MEASURE);
									}
									break;
								}
								else if (mode == 2)
								{
									printf("=============>���� ��� ����\n");
									if (InterlockedCompareExchange(&g_curMode, 0, 0) != MODE_CONFIG) {
										stopTimerRequestConfigurationMode();
										InterlockedExchange(&g_curMode, MODE_CONFIG);
									}
									request_GetConfig();
									break;
								}
								else
								{
									printf("=============>�˼� ���� ���\n");
									InterlockedExchange(&g_curMode, MODE_UNKNOWN);
									break;
								}
							}
							else if (stData.message_cmd == 50010)			// GETREDLASER ����
							{
								unsigned char* p = stData.message_data;
								break;
							}
							else if (stData.message_cmd == 50004)			// GETRAWDATACONFIG ���� (������)
							{
								// 1) status bits(D0-D3) ����
								unsigned char* p = stData.message_data;
								unsigned long long copy_size = 0U;
								if (size_body_msg > sizeof(ST_DataConfig))
								{
									copy_size = sizeof(ST_DataConfig);
								}
								else {
									copy_size = size_body_msg;
								}
								memcpy(&g_sensorConfig, stData.message_data, size_body_msg);

								void* msg_data = malloc(sizeof(unsigned short) + sizeof(unsigned short) + size_body_msg);
								if (msg_data)
								{
									unsigned long long copy_idx = 0;
									memcpy((char*)msg_data + copy_idx, &stData.header_size, sizeof(unsigned short));
									copy_idx += sizeof(unsigned short);
									memcpy((char*)msg_data + copy_idx, &stData.message_cmd, sizeof(unsigned short));
									copy_idx += sizeof(unsigned short);
									memcpy((char*)msg_data + copy_idx, stData.message_data, size_body_msg);
									copy_idx += size_body_msg;

									enqueue_CircularQueue(&g_recvQueue, msg_data, copy_idx);
									free(msg_data);
								}
								break;
							}
							else if (stData.message_cmd == 50005)
							{
								unsigned char* p = stData.message_data;
								printf("EPPROM ���� ���ð� ���� ���� ����\n");
								break;
							}
							else if (stData.message_cmd == 50007)
							{
								unsigned char* p = stData.message_data;
								printf("���� �ʱ�ȭ ����(RAM��)\n");
								break;
							}
							else {
								break;
							}
						}

						if (stData.message_data)
						{
							free(stData.message_data);
							stData.message_data = NULL;
						}
					}
					else
					{
						// ����� ������ ũ�Ⱑ �ȸ����� ���� �б�
						//memmove(data, data + 1, buf_size - 1);
						//buf_size--;
						//unsigned char addByte = RingBuffer_readbytes(&ring1, );
						//data[buf_size] = addByte;
						//buf_size++;
						break;
					}
				}
				else
				{
					// ����� �ȸ����� �տ��� 1����Ʈ ������ 1����Ʈ�� �߰��� �о�� �� �ٽ� �õ��Ѵ�
					memmove(data, data + 1, buf_size - 1);
					buf_size--;
					unsigned char addByte = RingBuffer_readbyte(&ring1);
					data[buf_size] = addByte;
					buf_size++;
				}
			}
			if (data)
			{
				free(data);
				data = NULL;
			}
		}
	}
	return 0;
}


int openSerialPort(char* portName, int baudRate)
{
	int nResult = 0;
	static BOOL first = TRUE;
	if (first == TRUE)
	{
		InitializeCriticalSection(&g_cs_end);			// ť ũ��Ƽ�� ���� �ʱ�ȭ
		InitializeCriticalSection(&g_txCS);
		first = FALSE;
	}

	if (portName)
	{
		g_hSerial = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

		if (g_hSerial != INVALID_HANDLE_VALUE)
		{
			DCB dcb = { 0 };
			dcb.DCBlength = sizeof(DCB);
			BOOL state = GetCommState(g_hSerial, &dcb);
			if (TRUE == state)
			{
				dcb.BaudRate = baudRate;
				dcb.ByteSize = 8;
				dcb.Parity = NOPARITY;
				dcb.StopBits = ONESTOPBIT;
				SetCommState(g_hSerial, &dcb);

				// Ÿ�Ӿƿ� ����
				COMMTIMEOUTS timeouts = {
					MAXDWORD,  // ReadIntervalTimeout: ������ ����ŷ ���͹� ���
					0,         // ReadTotalTimeoutMultiplier
					18,         // ReadTotalTimeoutConstant: ù ����Ʈ�� �ִ� 15ms ��� (11ms ~ 19ms)
					0,         // WriteTotalTimeoutMultiplier
					0          // WriteTotalTimeoutConstant
				};
				SetCommTimeouts(g_hSerial, &timeouts);

				// ������ ����
				int max_buffer = 1024 * 1024 * 4;           // 4MB
				int ring_id = 0;
				RingBuffer_init(&ring1, max_buffer, ring_id);

				// ó�� �� ���ž����� ����
				initConfigData(&g_sensorConfig);
				g_hProcessThread = CreateThread(NULL, 0, processMessageThread, NULL, 0, NULL);
				g_hRecvThread = CreateThread(NULL, 0, recvThreadSerial, NULL, 0, NULL);
				g_curMode = MODE_MEASURE;				// ó���� �������� �ʱ�ȭ
				request_GetConfig();
				nResult = 1;
			}
		}
	}

	return nResult;
}

void setConfigData(ST_SETRAWDATACONFIG_50003* p_stData)
{
	p_stData->D0_baud_rate = g_sensorConfig.D6_baud_rate;
	p_stData->D1_reserved = 0;
	p_stData->D2_LZR_infos_enable = g_sensorConfig.D8_LZR_information;
	p_stData->D3_red_laser_timeout = g_sensorConfig.D9_Red_laser_timeout;
	p_stData->D4_test_frame_enable = g_sensorConfig.D10_test_frame_enable;
	p_stData->D5_plane0_enable = g_sensorConfig.D11_plane0_enable;
	p_stData->D6_plane1_enable = g_sensorConfig.D12_plane1_enable;
	p_stData->D7_plane2_enable = g_sensorConfig.D13_plane2_enable;
	p_stData->D8_plane3_enable = g_sensorConfig.D14_plane3_enable;
	p_stData->D9_pulse_width_enable = g_sensorConfig.D15_pulse_width_enable;
	p_stData->D10_11_number_distance_values = g_sensorConfig.D1617_number_distance_values;
	p_stData->D12_13_starting_spot = g_sensorConfig.D1819_starting_spot;
	p_stData->D14_15_gap_between_spots = g_sensorConfig.D2021_gap_between_spots;
	p_stData->D16_apd_distance_range = g_sensorConfig.D22_apd_distance_range;
	p_stData->D17_canid_fc_enable = g_sensorConfig.D23_can_id_frame_counter_enable;
	p_stData->D18_diode_lifetime_enable = g_sensorConfig.D24_diode_lifetime_management_enable;
	p_stData->D19_polarity_input1 = g_sensorConfig.D25_polarity_input1;
	p_stData->D20_heartbeat_delay = g_sensorConfig.D26_heartbeat_delay_second;
	p_stData->D21_led1_enable = g_sensorConfig.D27_led1_enable;
	p_stData->D22_led2_enable = g_sensorConfig.D28_led2_enable;
	p_stData->D23_led_blue_enable = g_sensorConfig.D29_led_blue_enable;
	p_stData->D24_led_error_enable = g_sensorConfig.D30_led_error_enable;
	p_stData->D25_led_boot_duration = g_sensorConfig.D31_led_boot_duration;
	p_stData->D26_27_max_distance_range_SW = g_sensorConfig.D3233_max_distance_range_SW;
	p_stData->D28_plane_number_in_frame = g_sensorConfig.D34_plane_number_inside_frame_enable;
	p_stData->D29_immunity_level = g_sensorConfig.D35_immunity_level;
	p_stData->D30_31_hot_reset_timer = g_sensorConfig.D3637_hot_reset_timer_second;
	p_stData->D32_hot_reset_counter = g_sensorConfig.D38_hot_reset_counter;
}

void closeSerialPort(void)
{
	// ���� �� ����
	g_running_end = 1;
	Sleep(100);
	CloseHandle(g_hSerial);
	RingBuffer_destroy(&ring1);
	DeleteCriticalSection(&g_txCS);
	DeleteCriticalSection(&g_cs_end);
	g_hSerial = INVALID_HANDLE_VALUE;
}

// ���� ��� ��û
DWORD WINAPI sendMeasurementThread(LPVOID lpParam)
{
	// ������ ����: SYNC(4) + SIZE(2) + CMD(2) + DATA(1) + CHK(2) = 11 bytes
	for (int i = 0; i < 3; i++) {
		unsigned char packet[11] = { 0, };
		unsigned int sync = HEADER_SYNC_VAL;
		unsigned short size = 3;             // CMD(2) + DATA(1)
		unsigned short cmd = 50001;         // SETRAWDATAMODE
		unsigned char d0 = 1;             // ���� ��� ��û

		// CHK ���
		unsigned short chk = 0;
		chk += cmd & 0xFF;        // LSB
		chk += (cmd >> 8);        // MSB
		chk += d0;

		// ��Ŷ �ۼ� (LSB first)
		memcpy(&packet[0], &sync, 4);           // SYNC
		memcpy(&packet[4], &size, 2);           // SIZE
		memcpy(&packet[6], &cmd, 2);            // CMD
		packet[8] = d0;                         // DATA
		memcpy(&packet[9], &chk, 2);            // CHK

		DWORD bytesWritten = 0;
		//EnterCriticalSection(&g_txCS);
		//WriteFile(g_hSerial, packet, sizeof(packet), &bytesWritten, NULL);
		//LeaveCriticalSection(&g_txCS);
		bytesWritten = sendPacket(packet, sizeof(packet));

		if (bytesWritten == sizeof(packet))
		{
			printf("���� ��� ���� ��� ���� �Ϸ� (0x5001)\n");
		}
	}

	return 0;
}

// ��������
void request_MeasurementMode(void)
{
	// ������尡 �ƴҶ��� ������� ��û ����
	if (InterlockedCompareExchange(&g_curMode, 0, 0) != MODE_MEASURE)
	{
		HANDLE hThread = (HANDLE)_beginthreadex(
			NULL,       // ���� �Ӽ�
			0,          // ���� ũ�� (0 = �⺻)
			sendMeasurementThread, // ������ �Լ�
			NULL,       // �Ķ����
			0,          // ���� �� �ٷ� ����
			NULL
		);
		CloseHandle(hThread);
	}
	else {
		printf("�̹� ���� ���\n");
	}
}

// ���� ��� ��ȸ ��û
DWORD WINAPI sendGetModeThread(LPVOID lpParam)
{
	// ������ ����: SYNC(4) + SIZE(2) + CMD(2) + CHK(2) = 10 bytes
	//for (int i = 0; i < 1; i++) 
	{
		unsigned char packet[10] = { 0, };
		unsigned int sync = HEADER_SYNC_VAL;           // 0xFFFEFDFC
		unsigned short size = sizeof(unsigned short);         // CMD�� �����Ƿ� 2
		unsigned short cmd = 50002;                    // GETRAWDATAMODE
		unsigned short chk = (cmd & 0xFF) + (cmd >> 8); // üũ�� = CMD LSB+MSB

		// LSB first
		memcpy(&packet[0], &sync, 4);
		memcpy(&packet[4], &size, 2);
		memcpy(&packet[6], &cmd, 2);
		memcpy(&packet[8], &chk, 2);

		DWORD bytesWritten = 0;
		//EnterCriticalSection(&g_txCS);
		//WriteFile(g_hSerial, packet, sizeof(packet), &bytesWritten, NULL);
		//LeaveCriticalSection(&g_txCS);
		bytesWritten = sendPacket(packet, sizeof(packet));

		if (bytesWritten == sizeof(packet)) {
			printf("���� ��� ��ȸ ��û ���� �Ϸ� (CMD=50002)\n");
		}
	}

	return 0;
}

long long sendPacket(unsigned char* _pPacket, unsigned long long _sizePacket)
{
	DWORD bytesWritten = 0;
	if (g_hSerial != INVALID_HANDLE_VALUE)
	{
		EnterCriticalSection(&g_txCS);
		BOOL ok = WriteFile(g_hSerial, _pPacket, _sizePacket, &bytesWritten, NULL);
		LeaveCriticalSection(&g_txCS);

		if (ok == FALSE) {
			DWORD err = GetLastError();
			printf("WriteFile failed: %u, wrote %u/%u\n", err, bytesWritten, _sizePacket);

			// ��Ʈ �ݰ�
			EnterCriticalSection(&g_txCS);
			if (g_hSerial != INVALID_HANDLE_VALUE)
			{
				CloseHandle(g_hSerial);
				g_hSerial = INVALID_HANDLE_VALUE;
			}
			LeaveCriticalSection(&g_txCS);
			return (long long)-1;
		}

	}

	return (long long)bytesWritten;
}

// ȣ��� ����
void request_CurrentMode(void)
{
	HANDLE hThread = (HANDLE)_beginthreadex(
		NULL, 0,
		sendGetModeThread,
		NULL, 0, NULL
	);
	CloseHandle(hThread);
}


// ���� ����
void request_ConfigMode(void)
{
	// Ÿ�̸Ӹ� �����ؼ� ������ �ޱ� ������ ��� ������

	// ���� ��������
	//if (InterlockedCompareExchange(&g_curMode, 0, 0) == MODE_MEASURE)
	{
		if (g_mmTimerID == 0)
		{
			startTimerRequestConfigurationMode();
		}
		else
		{
			printf("Ÿ�̸� �̹� ���� �� �Դϴ�.\n");
		}
	}
	//else
	//{
	//	printf("�̹� ���� ��� �Դϴ�\n");
	//}
}

// ������� ��û Ÿ�̸�
static void CALLBACK TimerCallbackProc_REQ_CONFIGUREMODE(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	// �ִ� Ƚ�� �ʰ� �� �ڵ� ����
	if (InterlockedIncrement(&g_cfgSendCount) > MAX_CFG_SEND_COUNT)
	{
		stopTimerRequestConfigurationMode();
		printf("���� ��� ���� ��� ������ %dȸ ���� �� �ߴ��߽��ϴ�.\n", MAX_CFG_SEND_COUNT);
	}
	else
	{
		//if (InterlockedCompareExchange(&g_curMode, 0, 0) == MODE_CONFIG)
		//{
		//	// ��������
		//	stopTimerRequestConfigurationMode();
		//	printf("������� �Դϴ�.\n");
		//}
		//else
		{
			// ���� ��������
			const unsigned char a5 = 0xA5;

			DWORD bytesWritten = 0;
			//EnterCriticalSection(&g_txCS);
			//WriteFile(g_hSerial, &a5, 1, &bytesWritten, NULL);
			//LeaveCriticalSection(&g_txCS);
			bytesWritten = sendPacket(&a5, sizeof(unsigned char));
		}
	}
}

void startTimerRequestConfigurationMode(void)
{
	if (g_mmTimerID == 0)
	{
		InterlockedExchange(&g_cfgSendCount, 0);					// ���� ī���� �ʱ�ȭ(100ȸ����)
		unsigned int interval = 50;			// 50ms
		timeBeginPeriod(1);

		g_mmTimerID = timeSetEvent(interval, 0, TimerCallbackProc_REQ_CONFIGUREMODE, 0, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
	}
}

void stopTimerRequestConfigurationMode(void)
{
	if (g_mmTimerID != 0)
	{
		timeKillEvent(g_mmTimerID);
		g_mmTimerID = 0;
		timeEndPeriod(1);
	}
}

// GETRAWDATACONFIG ��û ������ ���� ������
DWORD WINAPI sendGetConfigThread(LPVOID lpParam)
{
	// SYNC(4) + SIZE(2) + CMD(2) + CHK(2) = 10 bytes
	//while (g_running_end == 0)
	if (g_running_end == 0)
	{
		if (InterlockedCompareExchange(&g_curMode, 0, 0) == MODE_CONFIG)
		{
			// ���� ����϶��� �������� ��û�Ѵ�
			unsigned char packet[10] = { 0, };
			uint32_t sync = HEADER_SYNC_VAL;      // 0xFFFEFDFC
			uint16_t size = sizeof(uint16_t);    // CMD�� �����Ƿ� 2
			uint16_t cmd = 50004;               // GETRAWDATACONFIG
			uint16_t chk = (cmd & 0xFF) + (cmd >> 8); // üũ�� = LSB+MSB

			memcpy(&packet[0], &sync, 4);
			memcpy(&packet[4], &size, 2);
			memcpy(&packet[6], &cmd, 2);
			memcpy(&packet[8], &chk, 2);

			DWORD bytesWritten = 0;
			//EnterCriticalSection(&g_txCS);
			//WriteFile(g_hSerial, packet, sizeof(packet), &bytesWritten, NULL);
			//LeaveCriticalSection(&g_txCS);
			bytesWritten = sendPacket(packet, sizeof(packet));

			if (bytesWritten == sizeof(packet))
			{
				printf("������ ��ȸ ��û ���� �Ϸ� (CMD=50004)\n");
			}
			//Sleep(500);
		}
		else {
			// ������忡���� ��û���� �ʴ´�
			//Sleep(1000);
		}
	}
	return 0;
}

void request_changeSetting(int D10_11_number_distance_values, int D12_13_starting_spot, int D26_27_max_distance_range_SW, int D16_apd_distance_range)
{
	ST_SETRAWDATACONFIG_50003 stData;
	setConfigData(&stData);

	if (D10_11_number_distance_values != -1) {
		stData.D10_11_number_distance_values = D10_11_number_distance_values;
	}

	if (D12_13_starting_spot != -1) {
		stData.D12_13_starting_spot = D12_13_starting_spot;
	}

	if (D26_27_max_distance_range_SW != -1) {
		stData.D26_27_max_distance_range_SW = D26_27_max_distance_range_SW;
	}

	if (D16_apd_distance_range != -1) {
		stData.D16_apd_distance_range = D16_apd_distance_range;
	}

	unsigned short cmd = 50003;
	unsigned short data_len = sizeof(stData);
	unsigned short msg_len = sizeof(cmd) + data_len;

	// SYNC(4) + SIZE(2) + MSG + CHK(2)
	unsigned long long frame_len = sizeof(unsigned int) + sizeof(unsigned short) + msg_len + sizeof(unsigned short);
	unsigned char* frame = malloc(frame_len);
	if (!frame)return;

	unsigned long long copy_idx = 0;

	// SYNC
	unsigned int sync = HEADER_SYNC_VAL;
	memcpy(frame + copy_idx, &sync, sizeof(sync));
	copy_idx += sizeof(sync);

	// MSG_SIZE
	unsigned short size_len = msg_len;
	memcpy(frame + copy_idx, &size_len, sizeof(size_len));
	copy_idx += sizeof(size_len);

	// CMD
	memcpy(frame + copy_idx, &cmd, sizeof(cmd));
	copy_idx += sizeof(cmd);

	// DATA
	memcpy(frame + copy_idx, &stData, data_len);
	copy_idx += data_len;

	// CHK
	unsigned short chk = 0;
	for (int i = 4 + 2; i < 4 + 2 + msg_len; ++i)
	{
		chk += frame[i];
	}
	memcpy(frame + copy_idx, &chk, sizeof(chk));
	copy_idx += sizeof(chk);

	long long bw = sendPacket(frame, frame_len);
	if (bw == (long long)frame_len) {
		printf("���� ���� ��û ���� �Ϸ� (CMD=50003)\n");
	}

	free(frame);
}

// �ܺο��� ȣ���� ����
void request_GetConfig(void)
{
	// ����/���� ���� ������� ������ ��û ����
	HANDLE hThread = (HANDLE)_beginthreadex(
		NULL, 0,
		sendGetConfigThread,
		NULL, 0, NULL
	);
	CloseHandle(hThread);
}

void request_RestoreSetting(void)
{
	unsigned short cmd = 50007;    // SETRAWDATACONFIGRESTORE
	unsigned short msg_len = sizeof(cmd);      // CMD�� �����Ƿ� 2
	unsigned short frame_len = 4 + 2 + msg_len + 2; // SYNC(4)+SIZE(2)+MSG+CHK(2)

	unsigned char frame[4 + 2 + 2 + 2] = { 0 };
	size_t idx = 0;

	// 1) SYNC
	unsigned int sync = HEADER_SYNC_VAL; // 0xFFFEFDFC
	memcpy(frame + idx, &sync, 4);
	idx += 4;

	// 2) SIZE (MSG ����)
	memcpy(frame + idx, &msg_len, 2);
	idx += 2;

	// 3) CMD
	memcpy(frame + idx, &cmd, 2);
	idx += 2;

	// 4) CHK (CMD ����Ʈ ��)
	unsigned short chk = (cmd & 0xFF) + (cmd >> 8);
	memcpy(frame + idx, &chk, 2);
	idx += 2;

	// 5) ����
	long bw = sendPacket(frame, frame_len);
	if (bw != frame_len) {
		printf("���� �ʱ�ȭ ��� ���� ����: %ld/%u\n", bw, frame_len);
	}
	else {
		printf("���� �ʱ�ȭ ��û ���� �Ϸ� (CMD=50007)\n");
	}
}

void request_saveConfig_EEPROM(void)
{
	// SETRAWDATACONFIGSTORE
	// 1) �Ķ���� ����
	const unsigned int SYNC = HEADER_SYNC_VAL; // 0xFFFEFDFC
	const unsigned short CMD = 50005;           // SETRAWDATACONFIGSTORE
	const unsigned short SIZE = sizeof(CMD);     // DATA �����Ƿ� CMD(2����Ʈ)��
	const unsigned short CHK = (CMD & 0xFF) + (CMD >> 8);

	// 2) ������ ���� �Ҵ� (SYNC(4) + SIZE(2) + CMD(2) + CHK(2) = 10����Ʈ)
	unsigned char packet[10];
	size_t idx = 0;

	// 3) SYNC (LSB first)
	memcpy(packet + idx, &SYNC, sizeof(SYNC));
	idx += sizeof(SYNC);

	// 4) SIZE
	memcpy(packet + idx, &SIZE, sizeof(SIZE));
	idx += sizeof(SIZE);

	// 5) CMD
	memcpy(packet + idx, &CMD, sizeof(CMD));
	idx += sizeof(CMD);

	// 6) CHK
	memcpy(packet + idx, &CHK, sizeof(CHK));
	idx += sizeof(CHK);

	// 7) ����
	long long written = sendPacket(packet, sizeof(packet));
	if (written == sizeof(packet)) {
		printf("���� ���� ��û ���� �Ϸ� (CMD=50005)\n");
	}
	else {
		printf("���� ���� ��û ���� ����: %lld/%zu\n", written, sizeof(packet));
	}
}

void request_SETRAWDATAREDLASER(void)
{
	// SETRAWDATAREDLASER
	// 1) �Ķ���� ����
	const unsigned int SYNC = HEADER_SYNC_VAL; // 0xFFFEFDFC
	const unsigned short CMD = 50009;           // SETRAWDATAREDLASER
	const unsigned char DATA = 1;					// ON
	const unsigned short SIZE = sizeof(CMD) + sizeof(DATA);
	const unsigned short CHK = (CMD & 0xFF) + (CMD >> 8) + DATA;

	// 2) ������ ���� �Ҵ� (SYNC(4) + SIZE(2) + CMD(2) + CHK(2) = 10����Ʈ)
	unsigned char packet[11] = { 0 };
	size_t idx = 0;

	// 3) SYNC (LSB first)
	memcpy(packet + idx, &SYNC, sizeof(SYNC));
	idx += sizeof(SYNC);

	// 4) SIZE
	memcpy(packet + idx, &SIZE, sizeof(SIZE));
	idx += sizeof(SIZE);

	// 5) CMD
	memcpy(packet + idx, &CMD, sizeof(CMD));
	idx += sizeof(CMD);

	// DATA
	packet[idx++] = DATA;

	// 6) CHK
	memcpy(packet + idx, &CHK, sizeof(CHK));
	idx += sizeof(CHK);

	// 7) ����
	long long written = sendPacket(packet, sizeof(packet));
	if (written == sizeof(packet)) {
		printf("���巹����ON (CMD=50009)\n");
	}
	else {
		printf("���巹����ON ���� ����: %lld/%zu\n", written, sizeof(packet));
	}
}

void initConfigData(ST_DataConfig* p_stData)
{
	p_stData->D0_status0 = 0;
	p_stData->D1_status1 = 0;
	p_stData->D2_status2 = 0;
	p_stData->D3_status3 = 0;
	p_stData->D4D5_comm_charge = 0;
	p_stData->D6_baud_rate = 3;
	p_stData->D7_nodata0 = 0;
	p_stData->D8_LZR_information = 1;
	p_stData->D9_Red_laser_timeout = 1;
	p_stData->D10_test_frame_enable = 0;
	p_stData->D11_plane0_enable = 1;
	p_stData->D12_plane1_enable = 1;
	p_stData->D13_plane2_enable = 1;
	p_stData->D14_plane3_enable = 1;
	p_stData->D15_pulse_width_enable = 0;
	p_stData->D1617_number_distance_values = 274;
	p_stData->D1819_starting_spot = 0;
	p_stData->D2021_gap_between_spots = 1;
	p_stData->D22_apd_distance_range = 1;
	p_stData->D23_can_id_frame_counter_enable = 1;
	p_stData->D24_diode_lifetime_management_enable = 1;
	p_stData->D25_polarity_input1 = 1;
	p_stData->D26_heartbeat_delay_second = 5;
	p_stData->D27_led1_enable = 1;
	p_stData->D28_led2_enable = 1;
	p_stData->D29_led_blue_enable = 1;
	p_stData->D30_led_error_enable = 1;
	p_stData->D31_led_boot_duration = 255;
	p_stData->D3233_max_distance_range_SW = 65000;
	p_stData->D34_plane_number_inside_frame_enable = 1;
	p_stData->D35_immunity_level = 1;
	p_stData->D3637_hot_reset_timer_second = 1200;
	p_stData->D38_hot_reset_counter = 3;
}

#endif