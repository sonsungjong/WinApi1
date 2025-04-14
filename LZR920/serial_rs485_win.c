#ifdef _WIN32
#include "serial_rs485.h"
#include <Windows.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include "LZR920_Define.h"
#include "LZR920_Enum.h"
#include "LZR920_Struct.h"

// static���� ���� ������ �� ���Ϸ� ������Ų��
static const char* g_portName = "\\\\.\\COM9";
static const int g_baudRate = BAUD_RATE;
static ST_MessageNode* g_queueHead = NULL;
static ST_MessageNode* g_queueTail = NULL;
static CRITICAL_SECTION g_queueCS;
static HANDLE g_hRecvThread;
static HANDLE g_hProcessThread;

static CONDITION_VARIABLE g_cv;
volatile int g_running = 1;			// [0] ����, [1] ���� ��

// �ø��� �ڵ�
static HANDLE g_hSerial = INVALID_HANDLE_VALUE;

ST_LZR920* parseSerialMsg(const unsigned char* buf, int total_len)
{
	ST_LZR920* msg = NULL;
	if (total_len >= MIN_MSG_SIZE)
	{
		msg = (ST_LZR920*)malloc(sizeof(ST_LZR920));

		memcpy(&msg->header_sync, buf, sizeof(msg->header_sync));
		if (msg->header_sync == HEADER_SYNC_VAL)
		{
			memcpy(&msg->header_size, buf + sizeof(msg->header_sync), sizeof(msg->header_size));
			memcpy(&msg->message_cmd, buf + sizeof(msg->header_sync) + sizeof(msg->header_size), sizeof(msg->message_cmd));

			int data_length = msg->header_size - sizeof(msg->message_cmd);
			if (total_len == (sizeof(msg->header_sync) + sizeof(msg->header_size) + msg->header_size + FOOTER_CHK_SIZE))
			{
				msg->message_data = (unsigned char*)malloc(data_length);
				memcpy(msg->message_data, buf + sizeof(msg->header_sync) + sizeof(msg->header_size) + sizeof(msg->message_cmd), data_length);
				memcpy(&msg->footer_chk, buf + sizeof(msg->header_sync) + sizeof(msg->header_size) + sizeof(msg->message_cmd) + data_length, sizeof(msg->footer_chk));


			}
			else
			{
				free(msg);
				msg = NULL;
			}
		}
		else
		{
			free(msg);
			msg = NULL;
		}
	}

	return msg;
}

DWORD WINAPI recvThreadSerial(void* lpParam)
{
	// �ø�����Ʈ ���ž�����
	// ���۸� �Ҵ��ϰ�
	// �޽���üũ�� �ؼ� ť�� ���� �ִ´�
	unsigned char* tempBuf = (unsigned char*)malloc(BUFFER_SIZE);
	unsigned int bytesRead;

	unsigned char* recvBuffer = (unsigned char*)malloc(BUFFER_SIZE);
	int recv_len = 0;
	memset(recvBuffer, 0, BUFFER_SIZE);

	while (g_running)
	{
		memset(tempBuf, 0, BUFFER_SIZE);
		if (ReadFile(g_hSerial, tempBuf, BUFFER_SIZE, &bytesRead, NULL))
		{
			if (bytesRead > 0)
			{
				if (recv_len + bytesRead > BUFFER_SIZE)
				{
					recv_len = 0;			// �����÷ο� ����
				}

				memcpy(recvBuffer + recv_len, tempBuf, bytesRead);
				recv_len += bytesRead;

				// ���� ���ۿ��� ��Ŷ ����
				while (recv_len > MIN_MSG_SIZE)
				{
					unsigned int sync = 0;
					memcpy(&sync, recvBuffer, sizeof(sync));
					if (sync == HEADER_SYNC_VAL)
					{
						unsigned short body_size = 0;
						memcpy(&body_size, recvBuffer + sizeof(sync), sizeof(body_size));
						int total_msg_len = sizeof(sync) + sizeof(body_size) + body_size + FOOTER_CHK_SIZE;
						if (recv_len < total_msg_len)
						{
							break;				// ������ ��Ŷ �̼��� (�߰� ������ ���Ŵ��)
						}

						ST_LZR920* msg = parseSerialMsg(recvBuffer, total_msg_len);
						if (msg)
						{
							pushMessage((void*)msg);					// ť�� �״´�
						}
						// ó���� �κ� ����
						int remaining = recv_len - total_msg_len;
						memmove(recvBuffer, recvBuffer + total_msg_len, remaining);
						recv_len = remaining;
					}
					else
					{
						memmove(recvBuffer, recvBuffer + 1, recv_len - 1);
						recv_len -= 1;
						continue;
					}
				}
			}
			else
			{
				SleepConditionVariableCS(&g_cv, &g_queueCS, 50);
			}
		}
		else
		{
			SleepConditionVariableCS(&g_cv, &g_queueCS, 50);
		}
	}

	free(tempBuf);
	free(recvBuffer);
}

DWORD WINAPI processMessageThread(void* lpParam)
{

}

void pushMessage(void* msg)
{
	ST_MessageNode* node = (ST_MessageNode*)malloc(sizeof(ST_MessageNode));
	if (node)
	{
		node->msg = msg;
		node->next = NULL;

		EnterCriticalSection(&g_queueCS);			// ũ��Ƽ�� ���� ����ȭ



		LeaveCriticalSection(&g_queueCS);			// ũ��Ƽ�� ���� ����ȭ ����
	}


}

void* popMessage()
{


	return NULL;
}

int openSerialPort(char* portName, int baudRate)
{
	InitializeCriticalSection(&g_queueCS);			// ť ũ��Ƽ�� ���� �ʱ�ȭ
	int nResult = 0;

	if (portName == NULL)
	{
		portName = g_portName;
	}

	if (baudRate < BAUD_RATE)
	{
		baudRate = BAUD_RATE;
	}

	g_hSerial = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (g_hSerial != INVALID_HANDLE_VALUE)
	{
		// ���ž����� ����
		g_hRecvThread = CreateThread(NULL, 0, recvThreadSerial, NULL, 0, NULL);
		//g_hProcessThread = CreateThread(NULL, 0, processMessageThread, NULL, 0, NULL);
	}

	return nResult;
}



void closeSerialPort(void)
{
	// ���� �� ����
	DeleteCriticalSection(&g_queueCS);
	CloseHandle(g_hSerial);
}

#endif