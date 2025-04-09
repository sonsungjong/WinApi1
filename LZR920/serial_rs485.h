#ifndef SERIAL_RS485_H_
#define SERIAL_RS485_H_

#ifdef __cplusplus
extern "C" {
#endif

void pushMessage(void* msg);
void* popMessage();
int openSerialPort(char* portName, int baudRate);
void recvSerialPort();
void closeSerialPort(void);
//int sendSerial(const char* data);
//int readPacketSerial();
//


#ifdef __cplusplus
}
#endif

#endif