#ifndef LZR920_STRUCT_H_
#define LZR920_STRUCT_H_

typedef struct LZR920
{
	unsigned int header_sync;					// 0xFFFEFDFC
	unsigned short header_size;				// message�� ũ��
	unsigned short message_cmd;				// �޽��� ���̵�
	unsigned char* message_data;			// ������
	unsigned short footer_chk;
} ST_LZR920;

// �޽��� ť ���
typedef struct MessageNode
{
	ST_LZR920* msg;
	struct ST_MessageNode* next;
} ST_MessageNode;

#endif