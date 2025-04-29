#ifdef _WIN32
#include "RingBuffer1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <Windows.h>

static CRITICAL_SECTION g_cs[256];
static CONDITION_VARIABLE g_cv[256];
static int g_using[256] = { 0, };                       // [0] ���X [1] �����

void RingBuffer_init(ST_RingBuffer* rb, const unsigned long long max_buf_size, unsigned char _id)
{
	rb->data = NULL;
	rb->capacity = 1024 * 1024;						// �⺻ 1MB
	rb->head = 0;
	rb->tail = 0;
	rb->id = _id;

	if (max_buf_size > 0) {
		rb->capacity = max_buf_size;				// �ִ� ���� ���� (�Ѿ�� ���)
	}

	if (rb->capacity > 0) {
		rb->data = malloc(rb->capacity);			// �����ۿ� �޸� �Ҵ�
	}

	// id�� ���� ����ȭ
	InitializeCriticalSection(&g_cs[rb->id]);
	InitializeConditionVariable(&g_cv[rb->id]);
	g_using[rb->id] = 1;				// ��� ������ ����
}

// ���۰� �������� ����� (����� ū ũ�⸦ �Ҵ��ؼ� ����� ��)
void RingBuffer_write(ST_RingBuffer* rb, const unsigned char* data, const unsigned long long data_size)
{
	EnterCriticalSection(&g_cs[rb->id]);

	for (unsigned long long idx = 0; idx < data_size; idx++)
	{
		rb->data[rb->tail] = data[idx];
		unsigned long long next_tail = (rb->tail + 1) % rb->capacity;

		// �����: ���� ���� tail�� head�� ������ ���۰� �� �� �����̹Ƿ� head�� �������� ���
		if (next_tail == rb->head)
		{
			rb->head = (rb->head + 1) % rb->capacity;
		}
		rb->tail = next_tail;
	}

	// ������ �߰��Ǿ����Ƿ� ��� ���� �б� �����带 �����.
	WakeConditionVariable(&g_cv[rb->id]);
	LeaveCriticalSection(&g_cs[rb->id]);
}

// �������� ���۸� ��� �д´� (return ��������Ʈ��, data*�� ���� ������ ����)
unsigned long long RingBuffer_read(ST_RingBuffer* rb, unsigned char** data)
{
	unsigned long long count = 0;
	EnterCriticalSection(&g_cs[rb->id]);

	// �����Ͱ� ������
	if (rb->head != rb->tail)
	{
		// ���� �����ͷ��� ����� ��
		if(rb->tail > rb->head)
		{
			count = rb->tail - rb->head;		// ���� �����ͷ�
		}
		else
		{
			count = rb->capacity - rb->head + rb->tail;	// ���� �����ͷ�
		}

		// data�� �Ҵ��ϰ�

		*data = (unsigned char*)malloc(count * sizeof(unsigned char));

		if (*data)
		{
			// �����͸� ī���Ѵ�
			if (rb->tail > rb->head)
			{
				memcpy(*data, rb->data + rb->head, count);			// ���� �����ͷ��� head�� tail���̿� ���� ��
			}
			else
			{
				unsigned long long first_part = rb->capacity - rb->head;	// ù��° �κ�
				unsigned long long second_part = rb->tail;				// �ι�° �κ�
				memcpy(*data, rb->data + rb->head, first_part);			// ù��° �κ�
				memcpy(*data + first_part, rb->data, second_part);		// �ι�° �κ�
			}
			// ���� �� head�� tail�� ���缭 �� ���� ���·� �����
			rb->head = rb->tail;
		}
		else {
			// �Ҵ� ���� �� count = 0
			count = 0;
		}
	}

	LeaveCriticalSection(&g_cs[rb->id]);
	return count;				// ���� ũ��
}

unsigned long long RingBuffer_wait_read(ST_RingBuffer* rb, unsigned char** data)
{
	unsigned long long count = 0;
	EnterCriticalSection(&g_cs[rb->id]);

	// �����Ͱ� ������ ���
	while ((rb->head == rb->tail) && (g_using[rb->id] == 1))
	{
		SleepConditionVariableCS(&g_cv[rb->id], &g_cs[rb->id], INFINITE);
	}

	if (g_using[rb->id] == 1)				// ��� ���� ���� (�����÷���)
	{
		// �����Ͱ� ������ ��� �о�ͼ� data�� �Ҵ��Ų��
		if (rb->tail > rb->head)
		{
			count = rb->tail - rb->head;		// ���� �����ͷ�
		}
		else
		{
			count = rb->capacity - rb->head + rb->tail;	// ���� �����ͷ�
		}

		*data = (unsigned char*)malloc(count * sizeof(unsigned char));
		if (*data)
		{
			if (rb->tail > rb->head)
			{
				memcpy(*data, rb->data + rb->head, count);			// ���� �����ͷ��� head�� tail���̿� ���� ��
			}
			else
			{
				unsigned long long first_part = rb->capacity - rb->head;
				unsigned long long second_part = rb->tail;
				memcpy(*data, rb->data + rb->head, first_part);
				memcpy(*data + first_part, rb->data, second_part);
			}
			// ���� �� head�� tail�� ���缭 �����͸� �Һ��� ������ ó���Ѵ�
			rb->head = rb->tail;
		}
		else
		{
			count = 0;			// �Ҵ� ����
		}
	}

	LeaveCriticalSection(&g_cs[rb->id]);
	return count;				// ���� ũ��
}

// �������� ������ ������
unsigned long long RingBuffer_cur_size(ST_RingBuffer* rb)
{
	unsigned long long size = 0;
	EnterCriticalSection(&g_cs[rb->id]);

	if (rb->head <= rb->tail)
	{
		size = rb->tail - rb->head;
	}
	else
	{
		size = rb->capacity - rb->head + rb->tail;
	}

	LeaveCriticalSection(&g_cs[rb->id]);
	return size;
}

// �����۸� �ʱ�ȭ�Ͽ� �� ���·� �����
void RingBuffer_reset(ST_RingBuffer* rb)
{
	EnterCriticalSection(&g_cs[rb->id]);
	rb->head = 0;
	rb->tail = 0;
	LeaveCriticalSection(&g_cs[rb->id]);
}

// �ش� �������� �Ҵ�� �޸𸮿� ����ȭ ��ü�� �����Ѵ�.
void RingBuffer_destroy(ST_RingBuffer* rb)
{
	if (g_using[rb->id] == 1)
	{
		EnterCriticalSection(&g_cs[rb->id]);
		g_using[rb->id] = 0;				// ��� ���� ó��
		WakeAllConditionVariable(&g_cv[rb->id]);				// ��� ���� ������ ��� �����

		rb->head = 0;
		rb->tail = 0;
		if (rb->data != NULL)
		{
			free(rb->data);
			rb->data = NULL;
		}
		LeaveCriticalSection(&g_cs[rb->id]);
		Sleep(10);
		DeleteCriticalSection(&g_cs[rb->id]);
	}
}

unsigned char RingBuffer_readbyte(ST_RingBuffer* rb)			// �߰��� 1����Ʈ �б�
{
	unsigned char val = 0;   // ���۰� ��� ������ 0 ��ȯ
	EnterCriticalSection(&g_cs[rb->id]);

	if (rb->head != rb->tail)               // �����Ͱ� �ϳ� �̻� ���� ��
	{
		val = rb->data[rb->head];           // 1����Ʈ ����
		rb->head = (rb->head + 1) % rb->capacity;   // head �� ĭ ����
	}

	LeaveCriticalSection(&g_cs[rb->id]);
	return val;
}

unsigned long long RingBuffer_readbytes(ST_RingBuffer* rb, unsigned char** data, unsigned long long read_size)			// ���ϴ� ��ŭ�� �б�
{
	unsigned long long count = 0;
	if (read_size > 0)
	{
		*data = NULL;

		EnterCriticalSection(&g_cs[rb->id]);

		/* 1) ���ۿ� ���� ����ִ� ������ �� ���ϱ� */
		if (rb->head != rb->tail)
		{
			if (rb->tail > rb->head)
				count = rb->tail - rb->head;
			else
				count = rb->capacity - rb->head + rb->tail;

			/* 2) ��û��(read_size)���� ������, �׸�ŭ�� �б� */
			if (count > read_size)
				count = read_size;

			/* 3) ���� �Ҵ� */
			*data = (unsigned char*)malloc(count);
			if (*data)
			{
				/* 4) �� �������� ������ �� ���� �� ���� */
				if (rb->tail > rb->head ||                 /* ���� */
					rb->head + count <= rb->capacity)      /* ù ���������� ��� */
				{
					memcpy(*data, rb->data + rb->head, count);
				}
				else
				{
					/* ���� + ���� �� �� ���� */
					unsigned long long first_part = rb->capacity - rb->head;
					unsigned long long second_part = count - first_part;
					memcpy(*data, rb->data + rb->head, first_part);
					memcpy(*data + first_part, rb->data, second_part);
				}

				/* 5) head ���� �� �Һ� */
				rb->head = (rb->head + count) % rb->capacity;
			}
			else
			{
				/* malloc ���� �� count=0 �� ��ȯ */
				count = 0;
			}
		}

		LeaveCriticalSection(&g_cs[rb->id]);
	}

	return count;
}

#endif		_WIN32