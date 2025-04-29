#ifndef CIRCULAR_QUEUE_1_H_
#define CIRCULAR_QUEUE_1_H_

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct STCircularQueue 
    {
        void* data;                         // 요소들을 저장할 메모리 영역 (바이트 배열)
        int max_count;                       // 저장 가능한 최대 요소 개수
        int element_size;                 // 저장할 자료형의 크기(int ==> sizeof(int), double ==> sizeof(double))
        int front;
        int rear;
        unsigned char id;                     // 동기화하는 기준 아이디
    } ST_CircularQueue;

    void init_CircularQueue(ST_CircularQueue* q, int _max_count, int _element_size, unsigned char _id);			// 순환큐 초기화
    int size_CircularQueue(ST_CircularQueue* q);									// 큐의 현재크기
    int isFull_CircularQueue(ST_CircularQueue* q);									// 큐가 가득 찼는지 판단
    int isEmpty_CircularQueue(ST_CircularQueue* q);								// 큐가 비었는지 판단
    void enqueue_CircularQueue(ST_CircularQueue* q, void* new_element);							// 큐에 데이터를 추가
    void* dequeue_CircularQueue(ST_CircularQueue* q);										// 큐에서 데이터를 삭제하면서 반환
    void* front_CircularQueue(ST_CircularQueue* q);										// 삭제하지않고 데이터를 확인만한다
    void pop_CircularQueue(ST_CircularQueue* q);										// 가장 앞부분을 제거만 한다 (깊은복사 안함)
    void reset_CircularQueue(ST_CircularQueue* q);									// 큐를 초기화
    void destroy_CircularQueue(ST_CircularQueue* q);						// 큐를 없앰
    void testDisplay_CircularQueue(ST_CircularQueue* q);			// 테스트용
    void* wait_pop_CircularQueue(ST_CircularQueue* q);                  // wait_pop

#ifdef __cplusplus
}
#endif

#endif			CIRCULAR_QUEUE_1_H_