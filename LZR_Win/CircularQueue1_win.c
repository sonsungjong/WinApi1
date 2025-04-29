#ifdef _WIN32
#include "CircularQueue1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include <Windows.h>

static CRITICAL_SECTION g_cs[256];
static CONDITION_VARIABLE g_cv[256];
static int g_using[256] = { 0, };                       // [0] 사용X [1] 사용중

void init_CircularQueue(ST_CircularQueue* q, int _max_count, int _element_size, unsigned char _id)
{
    q->data = NULL;
    q->max_count = 100;
    q->element_size = (int)(sizeof(unsigned char));
    q->front = -1;                // 큐가 비어있음을 나타내는 인덱스로 설정
    q->rear = -1;                // 큐가 비어있음을 나타내는 인덱스로 설정
    q->id = _id;                // 큐가 비어있음을 나타내는 인덱스로 설정

    if (_max_count > 0) {
        q->max_count = _max_count;                // 최대 저장 개수 (넘어가면 덮어씀)
    }
    if (_element_size > 0) {
        q->element_size = _element_size;           // 대상 자료형의 크기
    }

    // 메모리 할당
    q->data = malloc(q->max_count * q->element_size);

    // ID 기반으로 동기화
    InitializeCriticalSection(&g_cs[q->id]);
    InitializeConditionVariable(&g_cv[q->id]);
    g_using[q->id] = 1;
}

int size_CircularQueue(ST_CircularQueue* q) {
    int nResult = 0;
    EnterCriticalSection(&g_cs[q->id]);
    if (q->front != -1)                 // 비어있지 않으면
    {
        if (q->rear >= q->front) {
            nResult = q->rear - q->front + 1;
        }
        else {
            nResult = q->max_count - q->front + q->rear + 1;

        }
    }
    LeaveCriticalSection(&g_cs[q->id]);
    return nResult;
}

// 큐가 가득 찼으면 1(true), 아니면 0(false) 반환
int isFull_CircularQueue(ST_CircularQueue* q)
{
    // 1. front가 0이고 rear가 마지막 인덱스인 경우 
    // 2. front가 rear의 바로 다음 인덱스인 경우 (순환 구조)
    int nResult = 0;
    EnterCriticalSection(&g_cs[q->id]);
    if (((q->front == 0) && (q->rear == q->max_count - 1)) || (q->front == q->rear + 1))
    {
        nResult = 1;
    }
    LeaveCriticalSection(&g_cs[q->id]);
    return nResult;
}
int isEmpty_CircularQueue(ST_CircularQueue* q)
{
    int nResult = 0;
    EnterCriticalSection(&g_cs[q->id]);
    if (q->front == -1)                 // 비어있으면
    {
        nResult = 1;
    }
    LeaveCriticalSection(&g_cs[q->id]);

    return nResult;
}

void enqueue_CircularQueue(ST_CircularQueue* q, const void* new_element)
{
    EnterCriticalSection(&g_cs[q->id]);             // 크리티컬 섹션 시작
    if (!((q->front == 0) && (q->rear == q->max_count - 1)) || (q->front == q->rear + 1)) 
    {
        if (q->front == -1)                 // 비어있을 때, 최초 삽입이면
        {
            q->front = 0;
            q->rear = 0;
        }
        else {
            if (q->rear == q->max_count - 1)            // 순환 처리 : rear가 마지막이면 0 으로
            {
                q->rear = 0;
            }
            else {
                q->rear = q->rear + 1;
            }
        }
        memcpy((char*)q->data + q->rear * q->element_size, new_element, q->element_size);
    }
    else {
        // 만약 덮어쓰지 않으려면 else 의 코드를 모두 주석처리
        if (q->front == q->max_count - 1) {
            q->front = 0;

        }
        else {
            q->front++;

        }
        // rear 위치는 덮어쓰기 대상의 위치가 된다.
        if (q->rear == q->max_count - 1) {
            q->rear = 0;

        }
        else {
            q->rear++;

        }
        memcpy((char*)q->data + q->rear * q->element_size, new_element, q->element_size);
    }
    WakeConditionVariable(&g_cv[q->id]);                // condition_variable 깨운다
    LeaveCriticalSection(&g_cs[q->id]);                     // 크리티컬 섹션 종료
}

// 깊은복사로 값을 반환하면서 제거한다
void* dequeue_CircularQueue(ST_CircularQueue* q)
{
    void* deep_copy = NULL;
    EnterCriticalSection(&g_cs[q->id]);
    if (q->front != -1)                     // 비어있지 않으면
    {
        deep_copy = malloc(q->element_size);

        memcpy(deep_copy,  (char*)q->data + q->front * q->element_size, q->element_size);

        // 큐에 요소가 단 한 개만 있을 경우
        if (q->front == q->rear) {
            q->front = -1;
            q->rear = -1;
        }
        else {
            // 순환 처리: front가 마지막 인덱스이면 0으로
            if (q->front == q->max_count - 1) {
                q->front = 0;
            }
            else {
                q->front = q->front + 1;
            }
        }
    }
    LeaveCriticalSection(&g_cs[q->id]);

    return deep_copy;
}

// 얕은 복사로 앞에만 확인한다
void* front_CircularQueue(ST_CircularQueue* q)
{
    void* leak_copy = NULL;
    EnterCriticalSection(&g_cs[q->id]);
    if (q->front != -1)             // 비어있지 않으면
    {
        leak_copy = (char*)q->data + q->front * q->element_size;
    }
    LeaveCriticalSection(&g_cs[q->id]);
    return leak_copy;
}

void* wait_pop_CircularQueue(ST_CircularQueue* q)
{
    void* deep_copy = NULL;

    EnterCriticalSection(&g_cs[q->id]);             // 크리티컬 섹션 시작
    while((q->front == -1) && (g_using[q->id] == 1))
    {
        SleepConditionVariableCS(&g_cv[q->id], &g_cs[q->id], INFINITE);             // 큐가 비어있는 동안 대기
    }

    if (g_using[q->id] == 1)                // 사용 중 일때만 큐에 담는다 (안전 종료)
    {
        deep_copy = malloc(q->element_size);

        memcpy(deep_copy, (char*)q->data + q->front * q->element_size, q->element_size);

        // 큐에 요소가 단 한 개만 있을 경우
        if (q->front == q->rear) {
            q->front = -1;
            q->rear = -1;
        }
        else {
            // 순환 처리: front가 마지막 인덱스이면 0으로
            if (q->front == q->max_count - 1) {
                q->front = 0;
            }
            else {
                q->front = q->front + 1;
            }
        }
    }
    
    LeaveCriticalSection(&g_cs[q->id]);             // 크리티컬 섹션 종료

    return deep_copy;
}

void pop_CircularQueue(ST_CircularQueue* q)
{
    EnterCriticalSection(&g_cs[q->id]);
    if (q->front != -1)             // 비어있지 않으면
    {
        if (q->front == q->rear)                // 큐에 요소가 단 한개만 있을 경우
        {
            q->front = -1;
            q->rear = -1;
        }
        else {
            // 순환 처리: front가 마지막 인덱스이면 0으로
            if (q->front == q->max_count - 1)               // 순환 처리 : front가 마지막 인덱스면 0으로
            {
                q->front = 0;
            }
            else {
                q->front = q->front + 1;
            }
        }
    }
    LeaveCriticalSection(&g_cs[q->id]);
}

/*
    기존 데이터를 모두 무시하고 새로 시작하고 싶을 때
*/
void reset_CircularQueue(ST_CircularQueue* q)
{
    EnterCriticalSection(&g_cs[q->id]);
    q->front = -1;
    q->rear = -1;
    LeaveCriticalSection(&g_cs[q->id]);
}

/*
    더이상 큐를 사용하지 않아서 정리하려할 때
*/
void destroy_CircularQueue(ST_CircularQueue* q)
{
    if (g_using[q->id] == 1)
    {
        EnterCriticalSection(&g_cs[q->id]);             // 크리티컬 섹션 시작
        g_using[q->id] = 0;             // 사용 안함 처리
        WakeAllConditionVariable(&g_cv[q->id]);				// 대기 중인 스레드 모두 깨우기

        q->front = -1;
        q->rear = -1;
        if (q->data != NULL) {
            free(q->data);
            q->data = NULL;
        }
        LeaveCriticalSection(&g_cs[q->id]);             // 크리티컬 섹션 종료
        Sleep(10);
        DeleteCriticalSection(&g_cs[q->id]);            // 크리티컬 섹션 삭제
    }
}





void testDisplay_CircularQueue(ST_CircularQueue* q)
{
    int i;
    EnterCriticalSection(&g_cs[q->id]);
    printf("front = %d, rear = %d\n", q->front, q->rear);

    if (q->front == -1)
    {
        printf("Queue is empty\n");
    }
    else
    {
        if (q->element_size == sizeof(int))
        {
            printf("Queue<int> is :");

            i = q->front;
            if (q->front <= q->rear)
            {
                while (i <= q->rear)
                {
                    // q->data는 void*이므로 int*로 캐스팅 후 각 요소 출력
                    printf("%d ", ((int*)q->data)[i]);
                    i++;
                }
            }
            else
            {
                while (i < q->max_count)
                {
                    printf("%d ", ((int*)q->data)[i]);
                    i++;
                }
                i = 0;
                while (i <= q->rear)
                {
                    printf("%d ", ((int*)q->data)[i]);
                    i++;
                }
            }
            printf("\n");
        }
        else if (q->element_size == sizeof(double))
        {
            printf("Queue<double> is :");

            i = q->front;
            if (q->front <= q->rear)
            {
                while (i <= q->rear)
                {
                    // q->data는 void*이므로 int*로 캐스팅 후 각 요소 출력
                    printf("%lf ", ((double*)q->data)[i]);
                    i++;
                }
            }
            else
            {
                while (i < q->max_count)
                {
                    printf("%lf ", ((double*)q->data)[i]);
                    i++;
                }
                i = 0;
                while (i <= q->rear)
                {
                    printf("%lf ", ((double*)q->data)[i]);
                    i++;
                }
            }
            printf("\n");
        }
        else if (q->element_size == sizeof(unsigned char))
        {
            printf("Queue<unsigned char> is :");

            i = q->front;
            if (q->front <= q->rear)
            {
                while (i <= q->rear)
                {
                    // q->data는 void*이므로 unsgiend char*로 캐스팅 후 각 요소 출력
                    printf("%c ", ((unsigned char*)q->data)[i]);
                    i++;
                }
            }
            else
            {
                while (i < q->max_count)
                {
                    printf("%c ", ((unsigned char*)q->data)[i]);
                    i++;
                }
                i = 0;
                while (i <= q->rear)
                {
                    printf("%c ", ((unsigned char*)q->data)[i]);
                    i++;
                }
            }
            printf("\n");
        }
    }

    LeaveCriticalSection(&g_cs[q->id]);
}


#endif          _WIN32