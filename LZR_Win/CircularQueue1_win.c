#ifdef _WIN32
#include "CircularQueue1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include <Windows.h>

static CRITICAL_SECTION g_cs[256];
static CONDITION_VARIABLE g_cv[256];
static int g_using[256] = { 0, };                       // [0] ���X [1] �����

void init_CircularQueue(ST_CircularQueue* q, int _max_count, int _element_size, unsigned char _id)
{
    q->data = NULL;
    q->max_count = 100;
    q->element_size = (int)(sizeof(unsigned char));
    q->front = -1;                // ť�� ��������� ��Ÿ���� �ε����� ����
    q->rear = -1;                // ť�� ��������� ��Ÿ���� �ε����� ����
    q->id = _id;                // ť�� ��������� ��Ÿ���� �ε����� ����

    if (_max_count > 0) {
        q->max_count = _max_count;                // �ִ� ���� ���� (�Ѿ�� ���)
    }
    if (_element_size > 0) {
        q->element_size = _element_size;           // ��� �ڷ����� ũ��
    }

    // �޸� �Ҵ�
    q->data = malloc(q->max_count * q->element_size);

    // ID ������� ����ȭ
    InitializeCriticalSection(&g_cs[q->id]);
    InitializeConditionVariable(&g_cv[q->id]);
    g_using[q->id] = 1;
}

int size_CircularQueue(ST_CircularQueue* q) {
    int nResult = 0;
    EnterCriticalSection(&g_cs[q->id]);
    if (q->front != -1)                 // ������� ������
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

// ť�� ���� á���� 1(true), �ƴϸ� 0(false) ��ȯ
int isFull_CircularQueue(ST_CircularQueue* q)
{
    // 1. front�� 0�̰� rear�� ������ �ε����� ��� 
    // 2. front�� rear�� �ٷ� ���� �ε����� ��� (��ȯ ����)
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
    if (q->front == -1)                 // ���������
    {
        nResult = 1;
    }
    LeaveCriticalSection(&g_cs[q->id]);

    return nResult;
}

void enqueue_CircularQueue(ST_CircularQueue* q, const void* new_element)
{
    EnterCriticalSection(&g_cs[q->id]);             // ũ��Ƽ�� ���� ����
    if (!((q->front == 0) && (q->rear == q->max_count - 1)) || (q->front == q->rear + 1)) 
    {
        if (q->front == -1)                 // ������� ��, ���� �����̸�
        {
            q->front = 0;
            q->rear = 0;
        }
        else {
            if (q->rear == q->max_count - 1)            // ��ȯ ó�� : rear�� �������̸� 0 ����
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
        // ���� ����� �������� else �� �ڵ带 ��� �ּ�ó��
        if (q->front == q->max_count - 1) {
            q->front = 0;

        }
        else {
            q->front++;

        }
        // rear ��ġ�� ����� ����� ��ġ�� �ȴ�.
        if (q->rear == q->max_count - 1) {
            q->rear = 0;

        }
        else {
            q->rear++;

        }
        memcpy((char*)q->data + q->rear * q->element_size, new_element, q->element_size);
    }
    WakeConditionVariable(&g_cv[q->id]);                // condition_variable �����
    LeaveCriticalSection(&g_cs[q->id]);                     // ũ��Ƽ�� ���� ����
}

// ��������� ���� ��ȯ�ϸ鼭 �����Ѵ�
void* dequeue_CircularQueue(ST_CircularQueue* q)
{
    void* deep_copy = NULL;
    EnterCriticalSection(&g_cs[q->id]);
    if (q->front != -1)                     // ������� ������
    {
        deep_copy = malloc(q->element_size);

        memcpy(deep_copy,  (char*)q->data + q->front * q->element_size, q->element_size);

        // ť�� ��Ұ� �� �� ���� ���� ���
        if (q->front == q->rear) {
            q->front = -1;
            q->rear = -1;
        }
        else {
            // ��ȯ ó��: front�� ������ �ε����̸� 0����
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

// ���� ����� �տ��� Ȯ���Ѵ�
void* front_CircularQueue(ST_CircularQueue* q)
{
    void* leak_copy = NULL;
    EnterCriticalSection(&g_cs[q->id]);
    if (q->front != -1)             // ������� ������
    {
        leak_copy = (char*)q->data + q->front * q->element_size;
    }
    LeaveCriticalSection(&g_cs[q->id]);
    return leak_copy;
}

void* wait_pop_CircularQueue(ST_CircularQueue* q)
{
    void* deep_copy = NULL;

    EnterCriticalSection(&g_cs[q->id]);             // ũ��Ƽ�� ���� ����
    while((q->front == -1) && (g_using[q->id] == 1))
    {
        SleepConditionVariableCS(&g_cv[q->id], &g_cs[q->id], INFINITE);             // ť�� ����ִ� ���� ���
    }

    if (g_using[q->id] == 1)                // ��� �� �϶��� ť�� ��´� (���� ����)
    {
        deep_copy = malloc(q->element_size);

        memcpy(deep_copy, (char*)q->data + q->front * q->element_size, q->element_size);

        // ť�� ��Ұ� �� �� ���� ���� ���
        if (q->front == q->rear) {
            q->front = -1;
            q->rear = -1;
        }
        else {
            // ��ȯ ó��: front�� ������ �ε����̸� 0����
            if (q->front == q->max_count - 1) {
                q->front = 0;
            }
            else {
                q->front = q->front + 1;
            }
        }
    }
    
    LeaveCriticalSection(&g_cs[q->id]);             // ũ��Ƽ�� ���� ����

    return deep_copy;
}

void pop_CircularQueue(ST_CircularQueue* q)
{
    EnterCriticalSection(&g_cs[q->id]);
    if (q->front != -1)             // ������� ������
    {
        if (q->front == q->rear)                // ť�� ��Ұ� �� �Ѱ��� ���� ���
        {
            q->front = -1;
            q->rear = -1;
        }
        else {
            // ��ȯ ó��: front�� ������ �ε����̸� 0����
            if (q->front == q->max_count - 1)               // ��ȯ ó�� : front�� ������ �ε����� 0����
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
    ���� �����͸� ��� �����ϰ� ���� �����ϰ� ���� ��
*/
void reset_CircularQueue(ST_CircularQueue* q)
{
    EnterCriticalSection(&g_cs[q->id]);
    q->front = -1;
    q->rear = -1;
    LeaveCriticalSection(&g_cs[q->id]);
}

/*
    ���̻� ť�� ������� �ʾƼ� �����Ϸ��� ��
*/
void destroy_CircularQueue(ST_CircularQueue* q)
{
    if (g_using[q->id] == 1)
    {
        EnterCriticalSection(&g_cs[q->id]);             // ũ��Ƽ�� ���� ����
        g_using[q->id] = 0;             // ��� ���� ó��
        WakeAllConditionVariable(&g_cv[q->id]);				// ��� ���� ������ ��� �����

        q->front = -1;
        q->rear = -1;
        if (q->data != NULL) {
            free(q->data);
            q->data = NULL;
        }
        LeaveCriticalSection(&g_cs[q->id]);             // ũ��Ƽ�� ���� ����
        Sleep(10);
        DeleteCriticalSection(&g_cs[q->id]);            // ũ��Ƽ�� ���� ����
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
                    // q->data�� void*�̹Ƿ� int*�� ĳ���� �� �� ��� ���
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
                    // q->data�� void*�̹Ƿ� int*�� ĳ���� �� �� ��� ���
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
                    // q->data�� void*�̹Ƿ� unsgiend char*�� ĳ���� �� �� ��� ���
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