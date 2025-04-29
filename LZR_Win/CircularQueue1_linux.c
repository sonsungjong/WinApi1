#ifdef __linux
#include "CircularQueue1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include <pthread.h>

static pthread_mutex_t g_mtx[256];
static pthread_cond_t  g_cv[256];
static int g_using[256] = { 0, };  // [0] not using, [1] using

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
    pthread_mutex_init(&g_mtx[q->id], NULL);
    pthread_cond_init(&g_cv[q->id], NULL);
    g_using[q->id] = 1;
}

int size_CircularQueue(ST_CircularQueue* q) {
    int nResult = 0;
    pthread_mutex_lock(&g_mtx[q->id]);
    if (q->front != -1)                 // ������� ������
    {
        if (q->rear >= q->front) {
            nResult = q->rear - q->front + 1;
        }
        else {
            nResult = q->max_count - q->front + q->rear + 1;

        }
    }
    pthread_mutex_unlock(&g_mtx[q->id]);
    return nResult;
}

// ť�� ���� á���� 1(true), �ƴϸ� 0(false) ��ȯ
int isFull_CircularQueue(ST_CircularQueue* q)
{
    // 1. front�� 0�̰� rear�� ������ �ε����� ��� 
    // 2. front�� rear�� �ٷ� ���� �ε����� ��� (��ȯ ����)
    int nResult = 0;
    pthread_mutex_lock(&g_mtx[q->id]);
    if (((q->front == 0) && (q->rear == q->max_count - 1)) || (q->front == q->rear + 1))
    {
        nResult = 1;
    }
    pthread_mutex_unlock(&g_mtx[q->id]);
    return nResult;
}
int isEmpty_CircularQueue(ST_CircularQueue* q)
{
    int nResult = 0;
    pthread_mutex_lock(&g_mtx[q->id]);
    if (q->front == -1)                 // ���������
    {
        nResult = 1;
    }
    pthread_mutex_unlock(&g_mtx[q->id]);

    return nResult;
}

void enqueue_CircularQueue(ST_CircularQueue* q, const void* new_element)
{
    pthread_mutex_lock(&g_mtx[q->id]);
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
    pthread_cond_signal(&g_cv[q->id]);                // condition_variable �����
    pthread_mutex_unlock(&g_mtx[q->id]);
}

// ��������� ���� ��ȯ�ϸ鼭 �����Ѵ�
void* dequeue_CircularQueue(ST_CircularQueue* q)
{
    void* deep_copy = NULL;
    pthread_mutex_lock(&g_mtx[q->id]);
    if (q->front != -1)                     // ������� ������
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
    pthread_mutex_unlock(&g_mtx[q->id]);

    return deep_copy;
}

// ���� ����� �տ��� Ȯ���Ѵ�
void* front_CircularQueue(ST_CircularQueue* q)
{
    void* leak_copy = NULL;
    pthread_mutex_lock(&g_mtx[q->id]);
    if (q->front != -1)             // ������� ������
    {
        leak_copy = (char*)q->data + q->front * q->element_size;
    }
    pthread_mutex_unlock(&g_mtx[q->id]);
    return leak_copy;
}

void* wait_pop_CircularQueue(ST_CircularQueue* q)
{
    void* deep_copy = NULL;

    pthread_mutex_lock(&g_mtx[q->id]);
    while ((q->front == -1) && (g_using[q->id] == 1))
    {
        pthread_cond_wait(&g_cv[q->id], &g_mtx[q->id]);           // ť�� ����ִ� ���� ���
    }

    if (g_using[q->id] == 1)
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

    pthread_mutex_unlock(&g_mtx[q->id]);

    return deep_copy;
}

void pop_CircularQueue(ST_CircularQueue* q)
{
    pthread_mutex_lock(&g_mtx[q->id]);    // ���
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
    pthread_mutex_unlock(&g_mtx[q->id]);  // ��� ����
}

/*
    ���� �����͸� ��� �����ϰ� ���� �����ϰ� ���� ��
*/
void reset_CircularQueue(ST_CircularQueue* q)
{
    pthread_mutex_lock(&g_mtx[q->id]);
    q->front = -1;
    q->rear = -1;
    pthread_mutex_unlock(&g_mtx[q->id]);
}

/*
    ���̻� ť�� ������� �ʾƼ� �����Ϸ��� ��
*/
void destroy_CircularQueue(ST_CircularQueue* q)
{
    if (g_using[q->id] == 1)
    {
        pthread_mutex_lock(&g_mtx[q->id]);            
        g_using[q->id] = 0;
        pthread_cond_broadcast(&g_cv[q->id]);               // ��� ���� ������ ��� �����

        q->front = -1;
        q->rear = -1;
        if (q->data != NULL) {
            free(q->data);
            q->data = NULL;
        }
        pthread_mutex_unlock(&g_mtx[q->id]);

        // ����
        pthread_mutex_destroy(&g_mtx[q->id]);
        pthread_cond_destroy(&g_cv[q->id]);
    }
}





void testDisplay_CircularQueue(ST_CircularQueue* q)
{
    int i;
    pthread_mutex_lock(&g_mtx[q->id]);
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

    
    pthread_mutex_unlock(&g_mtx[q->id]);
}


#endif          __linux