#ifndef LZR920_STRUCT_H_
#define LZR920_STRUCT_H_

typedef struct LZR920
{
	unsigned int header_sync;					// 0xFFFEFDFC
	unsigned short header_size;				// message의 크기
	unsigned short message_cmd;				// 메시지 아이디
	unsigned char* message_data;			// 데이터
	unsigned short footer_chk;
} ST_LZR920;

// 메시지 큐 노드
typedef struct MessageNode
{
	ST_LZR920* msg;
	struct ST_MessageNode* next;
} ST_MessageNode;

#endif