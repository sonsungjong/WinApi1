#ifdef _WIN32
#include "RingBuffer1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <Windows.h>

static CRITICAL_SECTION g_cs[256];
static CONDITION_VARIABLE g_cv[256];
static int g_using[256] = { 0, };                       // [0] 사용X [1] 사용중

void RingBuffer_init(ST_RingBuffer* rb, const unsigned long long max_buf_size, unsigned char _id)
{
	rb->data = NULL;
	rb->capacity = 1024 * 1024;						// 기본 1MB
	rb->head = 0;
	rb->tail = 0;
	rb->id = _id;

	if (max_buf_size > 0) {
		rb->capacity = max_buf_size;				// 최대 저장 개수 (넘어가면 덮어씀)
	}

	if (rb->capacity > 0) {
		rb->data = malloc(rb->capacity);			// 링버퍼에 메모리 할당
	}

	// id를 통해 동기화
	InitializeCriticalSection(&g_cs[rb->id]);
	InitializeConditionVariable(&g_cv[rb->id]);
	g_using[rb->id] = 1;				// 사용 중으로 변경
}

// 버퍼가 가득차면 덮어쓴다 (충분히 큰 크기를 할당해서 사용할 것)
void RingBuffer_write(ST_RingBuffer* rb, const unsigned char* data, const unsigned long long data_size)
{
	EnterCriticalSection(&g_cs[rb->id]);

	for (unsigned long long idx = 0; idx < data_size; idx++)
	{
		rb->data[rb->tail] = data[idx];
		unsigned long long next_tail = (rb->tail + 1) % rb->capacity;

		// 덮어쓰기: 만약 다음 tail이 head와 같으면 버퍼가 꽉 찬 상태이므로 head를 전진시켜 덮어씀
		if (next_tail == rb->head)
		{
			rb->head = (rb->head + 1) % rb->capacity;
		}
		rb->tail = next_tail;
	}

	// 데이터 추가되었으므로 대기 중인 읽기 스레드를 깨운다.
	WakeConditionVariable(&g_cv[rb->id]);
	LeaveCriticalSection(&g_cs[rb->id]);
}

// 읽지않은 버퍼를 모두 읽는다 (return 읽은바이트수, data*에 읽은 데이터 저장)
unsigned long long RingBuffer_read(ST_RingBuffer* rb, unsigned char** data)
{
	unsigned long long count = 0;
	EnterCriticalSection(&g_cs[rb->id]);

	// 데이터가 있으면
	if (rb->head != rb->tail)
	{
		// 읽을 데이터량을 계산한 후
		if(rb->tail > rb->head)
		{
			count = rb->tail - rb->head;		// 읽을 데이터량
		}
		else
		{
			count = rb->capacity - rb->head + rb->tail;	// 읽을 데이터량
		}

		// data에 할당하고

		*data = (unsigned char*)malloc(count * sizeof(unsigned char));

		if (*data)
		{
			// 데이터를 카피한다
			if (rb->tail > rb->head)
			{
				memcpy(*data, rb->data + rb->head, count);			// 읽을 데이터량이 head와 tail사이에 있을 때
			}
			else
			{
				unsigned long long first_part = rb->capacity - rb->head;	// 첫번째 부분
				unsigned long long second_part = rb->tail;				// 두번째 부분
				memcpy(*data, rb->data + rb->head, first_part);			// 첫번째 부분
				memcpy(*data + first_part, rb->data, second_part);		// 두번째 부분
			}
			// 읽은 후 head를 tail로 맞춰서 빈 버퍼 상태로 만든다
			rb->head = rb->tail;
		}
		else {
			// 할당 실패 시 count = 0
			count = 0;
		}
	}

	LeaveCriticalSection(&g_cs[rb->id]);
	return count;				// 읽은 크기
}

unsigned long long RingBuffer_wait_read(ST_RingBuffer* rb, unsigned char** data)
{
	unsigned long long count = 0;
	EnterCriticalSection(&g_cs[rb->id]);

	// 데이터가 없으면 대기
	while ((rb->head == rb->tail) && (g_using[rb->id] == 1))
	{
		SleepConditionVariableCS(&g_cv[rb->id], &g_cs[rb->id], INFINITE);
	}

	if (g_using[rb->id] == 1)				// 사용 중일 때만 (종료플래그)
	{
		// 데이터가 있으면 모두 읽어와서 data에 할당시킨다
		if (rb->tail > rb->head)
		{
			count = rb->tail - rb->head;		// 읽을 데이터량
		}
		else
		{
			count = rb->capacity - rb->head + rb->tail;	// 읽을 데이터량
		}

		*data = (unsigned char*)malloc(count * sizeof(unsigned char));
		if (*data)
		{
			if (rb->tail > rb->head)
			{
				memcpy(*data, rb->data + rb->head, count);			// 읽을 데이터량이 head와 tail사이에 있을 때
			}
			else
			{
				unsigned long long first_part = rb->capacity - rb->head;
				unsigned long long second_part = rb->tail;
				memcpy(*data, rb->data + rb->head, first_part);
				memcpy(*data + first_part, rb->data, second_part);
			}
			// 읽은 후 head를 tail로 맞춰서 데이터를 소비한 것으로 처리한다
			rb->head = rb->tail;
		}
		else
		{
			count = 0;			// 할당 실패
		}
	}

	LeaveCriticalSection(&g_cs[rb->id]);
	return count;				// 읽은 크기
}

// 읽지않은 데이터 사이즈
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

// 링버퍼를 초기화하여 빈 상태로 만든다
void RingBuffer_reset(ST_RingBuffer* rb)
{
	EnterCriticalSection(&g_cs[rb->id]);
	rb->head = 0;
	rb->tail = 0;
	LeaveCriticalSection(&g_cs[rb->id]);
}

// 해당 링버퍼의 할당된 메모리와 동기화 객체를 정리한다.
void RingBuffer_destroy(ST_RingBuffer* rb)
{
	if (g_using[rb->id] == 1)
	{
		EnterCriticalSection(&g_cs[rb->id]);
		g_using[rb->id] = 0;				// 사용 안함 처리
		WakeAllConditionVariable(&g_cv[rb->id]);				// 대기 중인 스레드 모두 깨우기

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

unsigned char RingBuffer_readbyte(ST_RingBuffer* rb)			// 추가로 1바이트 읽기
{
	unsigned char val = 0;   // 버퍼가 비어 있으면 0 반환
	EnterCriticalSection(&g_cs[rb->id]);

	if (rb->head != rb->tail)               // 데이터가 하나 이상 있을 때
	{
		val = rb->data[rb->head];           // 1바이트 추출
		rb->head = (rb->head + 1) % rb->capacity;   // head 한 칸 전진
	}

	LeaveCriticalSection(&g_cs[rb->id]);
	return val;
}

unsigned long long RingBuffer_readbytes(ST_RingBuffer* rb, unsigned char** data, unsigned long long read_size)			// 원하는 만큼만 읽기
{
	unsigned long long count = 0;
	if (read_size > 0)
	{
		*data = NULL;

		EnterCriticalSection(&g_cs[rb->id]);

		/* 1) 버퍼에 현재 들어있는 데이터 양 구하기 */
		if (rb->head != rb->tail)
		{
			if (rb->tail > rb->head)
				count = rb->tail - rb->head;
			else
				count = rb->capacity - rb->head + rb->tail;

			/* 2) 요청량(read_size)보다 많으면, 그만큼만 읽기 */
			if (count > read_size)
				count = read_size;

			/* 3) 동적 할당 */
			*data = (unsigned char*)malloc(count);
			if (*data)
			{
				/* 4) 두 구간으로 나눠질 수 있음 → 복사 */
				if (rb->tail > rb->head ||                 /* 연속 */
					rb->head + count <= rb->capacity)      /* 첫 구간만으로 충분 */
				{
					memcpy(*data, rb->data + rb->head, count);
				}
				else
				{
					/* 앞쪽 + 뒤쪽 두 번 복사 */
					unsigned long long first_part = rb->capacity - rb->head;
					unsigned long long second_part = count - first_part;
					memcpy(*data, rb->data + rb->head, first_part);
					memcpy(*data + first_part, rb->data, second_part);
				}

				/* 5) head 전진 → 소비 */
				rb->head = (rb->head + count) % rb->capacity;
			}
			else
			{
				/* malloc 실패 시 count=0 로 반환 */
				count = 0;
			}
		}

		LeaveCriticalSection(&g_cs[rb->id]);
	}

	return count;
}

#endif		_WIN32