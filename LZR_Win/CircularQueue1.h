#ifndef CIRCULAR_QUEUE_1_H_
#define CIRCULAR_QUEUE_1_H_

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct STCircularQueue 
    {
        void* data;                         // ��ҵ��� ������ �޸� ���� (����Ʈ �迭)
        int max_count;                       // ���� ������ �ִ� ��� ����
        int element_size;                 // ������ �ڷ����� ũ��(int ==> sizeof(int), double ==> sizeof(double))
        int front;
        int rear;
        unsigned char id;                     // ����ȭ�ϴ� ���� ���̵�
    } ST_CircularQueue;

    void init_CircularQueue(ST_CircularQueue* q, int _max_count, int _element_size, unsigned char _id);			// ��ȯť �ʱ�ȭ
    int size_CircularQueue(ST_CircularQueue* q);									// ť�� ����ũ��
    int isFull_CircularQueue(ST_CircularQueue* q);									// ť�� ���� á���� �Ǵ�
    int isEmpty_CircularQueue(ST_CircularQueue* q);								// ť�� ������� �Ǵ�
    void enqueue_CircularQueue(ST_CircularQueue* q, void* new_element);							// ť�� �����͸� �߰�
    void* dequeue_CircularQueue(ST_CircularQueue* q);										// ť���� �����͸� �����ϸ鼭 ��ȯ
    void* front_CircularQueue(ST_CircularQueue* q);										// ���������ʰ� �����͸� Ȯ�θ��Ѵ�
    void pop_CircularQueue(ST_CircularQueue* q);										// ���� �պκ��� ���Ÿ� �Ѵ� (�������� ����)
    void reset_CircularQueue(ST_CircularQueue* q);									// ť�� �ʱ�ȭ
    void destroy_CircularQueue(ST_CircularQueue* q);						// ť�� ����
    void testDisplay_CircularQueue(ST_CircularQueue* q);			// �׽�Ʈ��
    void* wait_pop_CircularQueue(ST_CircularQueue* q);                  // wait_pop

#ifdef __cplusplus
}
#endif

#endif			CIRCULAR_QUEUE_1_H_