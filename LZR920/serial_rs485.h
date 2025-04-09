#ifndef SERIAL_RS485_H_
#define SERIAL_RS485_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct STLZR920
{
	unsigned int header_sync;					// 0xFFFEFDFC
	unsigned short header_size;				// message�� ũ��
	unsigned short message_cmd;				// �޽��� ���̵�
	unsigned char* message_data;			// ������
	unsigned short footer_chk;
} ST_LZR920;

//int openSerialPort(const char* portName = "\\\\.\\COM9", int baudRate = 460800);
//void recvSerialPort();
//void closeSerialPort(void);
//int sendSerial(const char* data);
//int readPacketSerial();
//
//void* recvThreadSerial(void*);



#ifdef __cplusplus
}
#endif

#endif