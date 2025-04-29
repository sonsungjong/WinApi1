#ifndef RING_BUFFER_1_H_
#define RING_BUFFER_1_H_

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct STRingBuffer 
	{
		unsigned char* data;			// 요소들을 저장할 메모리 영역 (바이트 배열)
		unsigned long long capacity;
		unsigned long long head;
		unsigned long long tail;
		unsigned char id;				// 동기화 ID (0~255)
	} ST_RingBuffer;

	void RingBuffer_init(ST_RingBuffer* rb, const unsigned long long max_buf_size, unsigned char _id);
	void RingBuffer_write(ST_RingBuffer* rb, const unsigned char* data, const unsigned long long data_size);
	unsigned long long RingBuffer_read(ST_RingBuffer* rb, unsigned char** data);
	unsigned long long RingBuffer_wait_read(ST_RingBuffer* rb, unsigned char** data);
	unsigned long long RingBuffer_cur_size(ST_RingBuffer* rb);
	unsigned char RingBuffer_readbyte(ST_RingBuffer* rb);			// 추가로 1바이트 읽기
	unsigned long long RingBuffer_readbytes(ST_RingBuffer* rb, unsigned char** data, unsigned long long read_size);			// 원하는 만큼만 읽기

	void RingBuffer_reset(ST_RingBuffer* rb);
	void RingBuffer_destroy(ST_RingBuffer* rb);

#ifdef __cplusplus
}
#endif

#endif			RING_BUFFER_1_H_