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

#pragma comment(lib, "winmm.lib")

/*
	0xA5 : 설정 모드 진입 [필]
	50001 : 측정 모드 진입 [필]
	50002 : 모드상태조회(REQ) + 모드 변경 응답 (RES) [필]
	50003 : 설정 변경 [필]
	50004 : 설정값 조회(REQ) + 설정값 응답 (RES) [필]
	50005 : RAM설정을 EEPROM에 저장
	50006 : 로그 및 카운터 초기화
	50007 : RAM 설정 초기화
	50008 : 상태정보 일괄 조회
	50009 : 레이저 3개 ON/OFF
	50010 : 레드 레이저 상태 조회(REQ) + 응답(RES)
	50011 : MDI 반환
	50012 : 하트비트 ON/OFF
	50013 : 하트비트 상태 조회(REQ) + 응답(RES)
	50014 : SETRAWDATAFRAMECOUNTERRESET
	50015 : X

*/

#define MODE_UNKNOWN				 0
#define MODE_MEASURE				 1
#define MODE_CONFIG					 2

#define MAX_CFG_SEND_COUNT			100			// 설정모드 요청 최대 횟수

// static으로 접근 범위를 이 파일로 한정시킨다
static HANDLE g_hSerial = INVALID_HANDLE_VALUE;
static const char* g_portName = "\\\\.\\COM9";
static const int g_baudRate = 460800;
static ST_RingBuffer ring1;
static HANDLE g_hRecvThread;
static HANDLE g_hProcessThread;
static ST_DataConfig g_sensorConfig;

static UINT g_mmTimerID = 0;
static volatile LONG g_curMode = MODE_MEASURE;      // 1: 측정, 2: 설정, 0: 알수없음 (기본값을 측정모드로 해놓고 모드요청을 해본다)
static CRITICAL_SECTION g_txCS;

static CRITICAL_SECTION g_cs_end;
static CONDITION_VARIABLE g_cv_end;
static int g_running_end = 0;			// [0] 종료X [1] 종료O
static volatile LONG g_cfgSendCount = 0;      // 설정모드 A5 전송 횟수 카운트


// 수신부
static DWORD WINAPI recvThreadSerial(void* lpParam)
{

	while (g_running_end == 0)
	{
		unsigned int bytesRead = 0;
		unsigned char tempBuf[16384] = { 0, };
		// ReadFile 로 수신받는다
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
	}

	return 0;
}

// 처리부
DWORD WINAPI processMessageThread(void* lpParam)
{
	while (g_running_end == 0)
	{
		unsigned char* data = NULL;
		size_t buf_size = RingBuffer_wait_read(&ring1, &data);
		if (data != NULL && buf_size > 0)
		{
			while (buf_size >= 6)					// 최소 헤더
			{
				ST_LZR920 stData;
				size_t idx = 0;

				// 1. SYNC
				memcpy(&stData.header_sync, data + idx, sizeof(stData.header_sync));
				idx += sizeof(stData.header_sync);

				if (stData.header_sync != HEADER_SYNC_VAL)
				{
					// 잘못된 SYNC이면 1바이트 버리고 재동기화
					memmove(data, data + 1, buf_size - 1);
					buf_size--;
					unsigned char add = RingBuffer_readbyte(&ring1);
					data[buf_size] = add;
					continue;
				}

				if (stData.header_sync == HEADER_SYNC_VAL)			// SYNC 검사 (FF FD FE FF)
				{
					memcpy(&stData.header_size, data + idx, sizeof(stData.header_size));
					idx += sizeof(stData.header_size);
					size_t total_len = sizeof(stData.header_sync) + sizeof(stData.header_size) + stData.header_size + sizeof(stData.footer_chk);

					// 4) 전체 프레임이 아직 다 안 왔으면 부족분만큼 더 읽기
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
						// 그래도 모자라면 루프 탈출해서 다음 차례에 다시 시도
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

							// CHK 검증
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
								if (size_body_msg == 2202)
								{
									// ID + Frame counter
									// Plane Number + MDI

									// 앞에 6바이트를 다시 떼어내고
									// 그 다음 Plane Number 1바이트를 읽고
									// 그 다음 548 바이트를 unsigned short로 각각 변환한다음
									// 다시 Plane Number 1바이트를 읽고
									// 다시 548 바이트를 unsigned short로 각각 변환하고... 총 4번
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
							else if (stData.message_cmd == 50002)			// 모드 변경 응답
							{
								unsigned char mode = stData.message_data[0];
								if (mode == 1)
								{
									printf("=============>측정 모드 응답\n");
									if (InterlockedCompareExchange(&g_curMode, 0, 0) != MODE_MEASURE) {
										stopTimerRequestConfigurationMode();
										InterlockedExchange(&g_curMode, MODE_MEASURE);
									}
									break;
								}
								else if (mode == 2)
								{
									printf("=============>설정 모드 응답\n");
									if (InterlockedCompareExchange(&g_curMode, 0, 0) != MODE_CONFIG) {
										stopTimerRequestConfigurationMode();
										InterlockedExchange(&g_curMode, MODE_CONFIG);
									}
									break;
								}
								else
								{
									printf("=============>알수 없는 모드\n");
									InterlockedExchange(&g_curMode, MODE_UNKNOWN);
									break;
								}
							}
							else if (stData.message_cmd == 50003)
							{
								printf("셋팅값 변경!!!!!!\n");
								unsigned char* p = stData.message_data;
								break;
							}
							else if (stData.message_cmd == 50004)			// GETRAWDATACONFIG 응답 (설정값)
							{
								// 1) status bits(D0-D3) 복사
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

								const char* baud_strs[] = { "57600", "115200", "230400", "460800", "921600" };
								const char* baud_rate = (g_sensorConfig.D6_baud_rate < 5 ? baud_strs[g_sensorConfig.D6_baud_rate] : "Unknown");

								// 출력
								printf("=== 현재 설정값 ===\n");
								printf("통신부하: %u%%\n", g_sensorConfig.D4D5_comm_charge);
								printf("전송속도: %s\n", baud_rate);
								printf("시작 Spot: %u\n", g_sensorConfig.D1819_starting_spot);
								printf("거리값 개수: %u\n", g_sensorConfig.D1617_number_distance_values);
								printf("Spot 간격: %u\n", g_sensorConfig.D2021_gap_between_spots);
								printf("APD 거리: %u\n", g_sensorConfig.D22_apd_distance_range);
								printf("Plane enable: P0=%s, P1=%s, P2=%s, P3=%s\n",
									g_sensorConfig.D11_plane0_enable ? "Y" : "N",
									g_sensorConfig.D12_plane1_enable ? "Y" : "N",
									g_sensorConfig.D13_plane2_enable ? "Y" : "N",
									g_sensorConfig.D14_plane3_enable ? "Y" : "N");
								break;
							}
							else if (stData.message_cmd == 50005)
							{
								unsigned char* p = stData.message_data;
								printf("EPPROM 현재 셋팅값 영구 저장 응답\n");
								break;
							}
							else if (stData.message_cmd == 50007)
							{
								unsigned char* p = stData.message_data;
								printf("공장 초기화 응답(RAM만)\n");
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
						// 헤더는 맞지만 크기가 안맞으면 좀더 읽기
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
					// 헤더가 안맞으면 앞에서 1바이트 버리고 1바이트만 추가로 읽어온 후 다시 시도한다
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
	InitializeCriticalSection(&g_cs_end);			// 큐 크리티컬 섹션 초기화
	InitializeCriticalSection(&g_txCS);

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

				// 타임아웃 설정
				COMMTIMEOUTS timeouts = {
					MAXDWORD,  // ReadIntervalTimeout: 무조건 논블록킹 인터벌 모드
					0,         // ReadTotalTimeoutMultiplier
					18,         // ReadTotalTimeoutConstant: 첫 바이트는 최대 15ms 대기 (11ms ~ 19ms)
					0,         // WriteTotalTimeoutMultiplier
					0          // WriteTotalTimeoutConstant
				};
				SetCommTimeouts(g_hSerial, &timeouts);

				// 링버퍼 생성
				int max_buffer = 1024 * 1024 * 4;           // 4MB
				int ring_id = 0;
				RingBuffer_init(&ring1, max_buffer, ring_id);

				// 처리 및 수신쓰레드 생성
				initConfigData(&g_sensorConfig);
				g_hProcessThread = CreateThread(NULL, 0, processMessageThread, NULL, 0, NULL);
				g_hRecvThread = CreateThread(NULL, 0, recvThreadSerial, NULL, 0, NULL);
				g_curMode = MODE_MEASURE;				// 처음은 측정모드로 초기화
				request_GetConfig();
				nResult = 1;
			}
		}
	}

	return nResult;
}

void initConfigData(ST_SETRAWDATACONFIG_50003* p_stData)
{
	// 포트번호, 통신부하, 전송 속도, 레이저0번, 레이저1번, 레이저2번, 레이저3번, 측정영역, 시작점, 측정거리
	p_stData->D0_baud_rate = 3;											// 출력
	p_stData->D1_reserved = 0;
	p_stData->D2_LZR_infos_enable = 0;
	p_stData->D3_red_laser_timeout = 1;
	p_stData->D4_test_frame_enable = 0;
	p_stData->D5_plane0_enable = 1;									// 출력
	p_stData->D6_plane1_enable = 1;									// 출력
	p_stData->D7_plane2_enable = 1;									// 출력
	p_stData->D8_plane3_enable = 1;									// 출력
	p_stData->D9_pulse_width_enable = 0;
	p_stData->D10_11_number_distance_values = 274;			// 출력
	p_stData->D12_13_starting_spot = 0;								// 출력
	p_stData->D14_15_gap_between_spots = 1;
	p_stData->D16_apd_distance_range = 1;							// 출력
	p_stData->D17_canid_fc_enable = 1;
	p_stData->D18_diode_lifetime_enable = 1;
	p_stData->D19_polarity_input1 = 1;
	p_stData->D20_heartbeat_delay = 5;
	p_stData->D21_led1_enable = 1;
	p_stData->D22_led2_enable = 1;
	p_stData->D23_led_blue_enable = 1;
	p_stData->D24_led_error_enable = 1;
	p_stData->D25_led_boot_duration = 255;
	p_stData->D26_27_max_distance_range_SW = 65000;
	p_stData->D28_plane_number_in_frame = 1;
	p_stData->D29_immunity_level = 1;
	p_stData->D30_31_hot_reset_timer = 1200;
	p_stData->D32_hot_reset_counter = 3;
}

void closeSerialPort(void)
{
	// 종료 전 정리
	g_running_end = 1;
	Sleep(100);
	CloseHandle(g_hSerial);
	RingBuffer_destroy(&ring1);
	DeleteCriticalSection(&g_txCS);
	DeleteCriticalSection(&g_cs_end);
}

// 측정 모드 요청
DWORD WINAPI sendMeasurementThread(LPVOID lpParam)
{
	// 프레임 구성: SYNC(4) + SIZE(2) + CMD(2) + DATA(1) + CHK(2) = 11 bytes
	for (int i = 0; i < 3; i++) {
		unsigned char packet[11] = { 0, };
		unsigned int sync = HEADER_SYNC_VAL;
		unsigned short size = 3;             // CMD(2) + DATA(1)
		unsigned short cmd = 50001;         // SETRAWDATAMODE
		unsigned char d0 = 1;             // 측정 모드 요청

		// CHK 계산
		unsigned short chk = 0;
		chk += cmd & 0xFF;        // LSB
		chk += (cmd >> 8);        // MSB
		chk += d0;

		// 패킷 작성 (LSB first)
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
			printf("측정 모드 진입 명령 전송 완료 (0x5001)\n");
		}
	}

	return 0;
}

// 측정모드로
void request_MeasurementMode(void)
{
	// 측정모드가 아닐때만 측정모드 요청 전송
	if (InterlockedCompareExchange(&g_curMode, 0, 0) != MODE_MEASURE)
	{
		HANDLE hThread = (HANDLE)_beginthreadex(
			NULL,       // 보안 속성
			0,          // 스택 크기 (0 = 기본)
			sendMeasurementThread, // 스레드 함수
			NULL,       // 파라미터
			0,          // 생성 후 바로 실행
			NULL
		);
		CloseHandle(hThread);
	}
	else {
		printf("이미 측정 모드\n");
	}
}

// 현재 모드 조회 요청
DWORD WINAPI sendGetModeThread(LPVOID lpParam)
{
	// 프레임 구성: SYNC(4) + SIZE(2) + CMD(2) + CHK(2) = 10 bytes
	for (int i = 0; i < 3; i++) {
		unsigned char packet[10] = { 0, };
		unsigned int sync = HEADER_SYNC_VAL;           // 0xFFFEFDFC
		unsigned short size = sizeof(unsigned short);         // CMD만 있으므로 2
		unsigned short cmd = 50002;                    // GETRAWDATAMODE
		unsigned short chk = (cmd & 0xFF) + (cmd >> 8); // 체크섬 = CMD LSB+MSB

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
			printf("현재 모드 조회 요청 전송 완료 (CMD=50002)\n");
		}
	}

	return 0;
}

long long sendPacket(unsigned char* _pPacket, unsigned long long _sizePacket)
{
	DWORD bytesWritten = 0;
	EnterCriticalSection(&g_txCS);
	BOOL ok = WriteFile(g_hSerial, _pPacket, _sizePacket, &bytesWritten, NULL);
	LeaveCriticalSection(&g_txCS);

	if (ok == FALSE) {
		DWORD err = GetLastError();
		printf("WriteFile failed: %u, wrote %u/%u\n", err, bytesWritten, _sizePacket);
		return (long long)-1;
	}

	return (long long)bytesWritten;
}

// 호출용 래퍼
void request_CurrentMode(void)
{
	HANDLE hThread = (HANDLE)_beginthreadex(
		NULL, 0,
		sendGetModeThread,
		NULL, 0, NULL
	);
	CloseHandle(hThread);
}


// 설정 모드로
void request_ConfigMode(void)
{
	// 타이머를 생성해서 응답을 받기 전까지 계속 보낸다

	// 현재 측정모드면
	//if (InterlockedCompareExchange(&g_curMode, 0, 0) == MODE_MEASURE)
	{
		if (g_mmTimerID == 0)
		{
			startTimerRequestConfigurationMode();
		}
		else
		{
			printf("타이머 이미 실행 중 입니다.\n");
		}
	}
	//else
	//{
	//	printf("이미 설정 모드 입니다\n");
	//}
}

// 설정모드 요청 타이머
static void CALLBACK TimerCallbackProc_REQ_CONFIGUREMODE(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	// 최대 횟수 초과 시 자동 멈춤
	if (InterlockedIncrement(&g_cfgSendCount) > MAX_CFG_SEND_COUNT)
	{
		stopTimerRequestConfigurationMode();
		printf("설정 모드 진입 명령 전송을 %d회 수행 후 중단했습니다.\n", MAX_CFG_SEND_COUNT);
	}
	else
	{
		//if (InterlockedCompareExchange(&g_curMode, 0, 0) == MODE_CONFIG)
		//{
		//	// 설정모드다
		//	stopTimerRequestConfigurationMode();
		//	printf("설정모드 입니다.\n");
		//}
		//else
		{
			// 현재 측정모드다
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
		InterlockedExchange(&g_cfgSendCount, 0);					// 제한 카운터 초기화(100회까지)
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

// GETRAWDATACONFIG 요청 프레임 전송 스레드
DWORD WINAPI sendGetConfigThread(LPVOID lpParam)
{
	// SYNC(4) + SIZE(2) + CMD(2) + CHK(2) = 10 bytes
	if (g_running_end == 0)
	{
		// 1초에 한번씩 동작
		if (InterlockedCompareExchange(&g_curMode, 0, 0) == MODE_CONFIG)
		{
			// 설정 모드일때는 설정값을 요청한다
			unsigned char packet[10] = { 0, };
			uint32_t sync = HEADER_SYNC_VAL;      // 0xFFFEFDFC
			uint16_t size = sizeof(uint16_t);    // CMD만 있으므로 2
			uint16_t cmd = 50004;               // GETRAWDATACONFIG
			uint16_t chk = (cmd & 0xFF) + (cmd >> 8); // 체크섬 = LSB+MSB

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
				printf("설정값 조회 요청 전송 완료 (CMD=50004)\n");
			}
			//Sleep(1000);
		}
		else {
			// 측정모드에서는 요청하지 않는다
			//Sleep(1000);
		}
	}
	return 0;
}

void request_changeSetting(ST_SETRAWDATACONFIG_50003 stData)
{
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
		printf("설정 변경 요청 전송 완료 (CMD=50003)\n");
	}

	free(frame);
}

// 외부에서 호출할 래퍼
void request_GetConfig(void)
{
	// 측정/설정 모드와 상관없이 언제든 요청 가능
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
	unsigned short msg_len = sizeof(cmd);      // CMD만 있으므로 2
	unsigned short frame_len = 4 + 2 + msg_len + 2; // SYNC(4)+SIZE(2)+MSG+CHK(2)

	unsigned char frame[4 + 2 + 2 + 2] = { 0 };
	size_t idx = 0;

	// 1) SYNC
	unsigned int sync = HEADER_SYNC_VAL; // 0xFFFEFDFC
	memcpy(frame + idx, &sync, 4);
	idx += 4;

	// 2) SIZE (MSG 길이)
	memcpy(frame + idx, &msg_len, 2);
	idx += 2;

	// 3) CMD
	memcpy(frame + idx, &cmd, 2);
	idx += 2;

	// 4) CHK (CMD 바이트 합)
	unsigned short chk = (cmd & 0xFF) + (cmd >> 8);
	memcpy(frame + idx, &chk, 2);
	idx += 2;

	// 5) 전송
	long bw = sendPacket(frame, frame_len);
	if (bw != frame_len) {
		printf("공장 초기화 명령 전송 실패: %ld/%u\n", bw, frame_len);
	}
	else {
		printf("공장 초기화 요청 전송 완료 (CMD=50007)\n");
	}
}

void request_saveConfig_EEPROM(void)
{
	// SETRAWDATACONFIGSTORE
	// 1) 파라미터 정의
	const unsigned int SYNC = HEADER_SYNC_VAL; // 0xFFFEFDFC
	const unsigned short CMD = 50005;           // SETRAWDATACONFIGSTORE
	const unsigned short SIZE = sizeof(CMD);     // DATA 없으므로 CMD(2바이트)만
	const unsigned short CHK = (CMD & 0xFF) + (CMD >> 8);

	// 2) 프레임 버퍼 할당 (SYNC(4) + SIZE(2) + CMD(2) + CHK(2) = 10바이트)
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

	// 7) 전송
	long long written = sendPacket(packet, sizeof(packet));
	if (written == sizeof(packet)) {
		printf("영구 저장 요청 전송 완료 (CMD=50005)\n");
	}
	else {
		printf("영구 저장 요청 전송 실패: %lld/%zu\n", written, sizeof(packet));
	}
}

#endif