#ifndef SAFE_QUEUE_H_
#define SAFE_QUEUE_H_

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>
#include <queue>


template<typename T>
class CSafeQueue
{
private:
	std::mutex m_mtx;
	std::condition_variable m_cv;
	std::queue<T> m_queue;
	std::atomic<bool> m_stopFlag;				// 파괴자에서 호출할 wait_pop 종료플래그

public:
	CSafeQueue();
	virtual ~CSafeQueue();
	CSafeQueue(CSafeQueue const& other_queue);
	CSafeQueue& operator=(CSafeQueue const& other_queue) = delete;
	CSafeQueue(CSafeQueue&& other_queue) noexcept;
	CSafeQueue& operator=(CSafeQueue&& other_queue) noexcept = delete;

	bool empty();				// 비어있으면 true, 들어있으면 false
	size_t size();				// 큐 사이즈
	void clear();				// 큐 비우기

	void push(const T& lvalue);							// 원본을 복사하여 push (일반, pop해서 사용 후 메모리해제 수동)
	T wait_pop();
	bool wait_pop(T& ref);
	bool pop(T& ref);

	void move_push(T&& rvalue);						// std::move로 push (원본은 더이상 사용안할때, 최적성능)

	// 동적할당 해제를 사용하는 쪽에서 하지 않으려할때 사용하는 버전
	void smart_push(const T& lvalue);						// 동적할당에 대한 메모리해제를 신경쓰고 싶지 않을때 (간편)
	std::shared_ptr<T> smart_wait_pop();				// 리턴으로 받는다 (대기)
	bool smart_wait_pop(T& ref);								// 레퍼런스로 받는다 (대기)
	bool smart_pop(T& ref);										// 레퍼런스로 받는다 (즉시)
};

template<typename T>
inline CSafeQueue<T>::CSafeQueue()
{
	m_stopFlag = false;
}

template<typename T>
inline CSafeQueue<T>::~CSafeQueue()
{
	// 모든 wait_pop을 해제한다
	{
		std::lock_guard<std::mutex> lg(m_mtx);
		m_stopFlag = true;
	}
	m_cv.notify_all();
}

template<typename T>
inline CSafeQueue<T>::CSafeQueue(CSafeQueue const& other_queue)
{
	std::lock_guard<std::mutex> lg(other_queue.m_mtx);
	m_queue = other_queue.m_queue;
}

template<typename T>
inline CSafeQueue<T>::CSafeQueue(CSafeQueue&& other_queue) noexcept
{
	std::lock_guard<std::mutex> lg(other_queue.m_mtx);
	m_queue = std::move(other_queue.m_queue);
}

template<typename T>
inline bool CSafeQueue<T>::empty()
{
	std::lock_guard<std::mutex> lg(m_mtx);
	bool bResult = m_queue.empty();

	return bResult;
}

template<typename T>
inline size_t CSafeQueue<T>::size()
{
	std::lock_guard<std::mutex> lg(m_mtx);
	size_t ullResult = m_queue.size();
	return ullResult;
}

template<typename T>
inline void CSafeQueue<T>::clear()
{
	std::lock_guard<std::mutex> lg(m_mtx);
	std::queue<T> queue_empty;
	std::swap(m_queue, queue_empty);
}

template<typename T>
inline void CSafeQueue<T>::push(const T& lvalue)
{
	std::lock_guard<std::mutex> lg(m_mtx);
	m_queue.push(lvalue);
	m_cv.notify_one();
}

template<typename T>
inline T CSafeQueue<T>::wait_pop()
{
	std::unique_lock<std::mutex> lg(m_mtx);
	m_cv.wait(lg, [this] {
		return ((!m_queue.empty()) || m_stopFlag);			// 기다린다
		});

	T ref{};
	if (!m_stopFlag)			// 종료플래그 활성화?
	{
		ref = m_queue.front();
		m_queue.pop();
	}

	return ref;			// 결과를 리턴한다
}

template<typename T>
inline bool CSafeQueue<T>::wait_pop(T& ref)
{
	std::unique_lock<std::mutex> lg(m_mtx);
	m_cv.wait(lg, [this] {
		return ((!m_queue.empty()) || m_stopFlag);				// 기다린다
		});

	if ((!m_stopFlag) && !m_queue.empty()) {
		ref = m_queue.front();				// 결과를 레퍼런스에 넣는다
		m_queue.pop();
	}

	return true;
}

template<typename T>
inline bool CSafeQueue<T>::pop(T& ref)
{
	std::lock_guard<std::mutex> lg(m_mtx);
	bool bExist = !m_queue.empty();
	if (bExist) {
		ref = m_queue.front();
		m_queue.pop();
	}

	return bExist;
}

template<typename T>
inline void CSafeQueue<T>::move_push(T&& rvalue)
{
	std::lock_guard<std::mutex> lg(m_mtx);
	m_queue.push(std::move(rvalue));
	m_cv.notify_one();
}

template<typename T>
inline void CSafeQueue<T>::smart_push(const T& lvalue)
{
	std::lock_guard<std::mutex> lg(m_mtx);
	m_queue.push(std::make_shared<T>(lvalue));
	m_cv.notify_one();
}

template<typename T>
inline std::shared_ptr<T> CSafeQueue<T>::smart_wait_pop()
{
	std::unique_lock<std::mutex> lg(m_mtx);
	m_cv.wait(lg, [this] {
		return ((!m_queue.empty()) || m_stopFlag);			// 채워지거나 종료신호를 기다린다
		});

	std::shared_ptr<T> ref = nullptr;

	if (!m_stopFlag) {
		ref = m_queue.front();
		m_queue.pop();
	}

	return ref;
}

template<typename T>
inline bool CSafeQueue<T>::smart_wait_pop(T& ref)
{
	std::unique_lock<std::mutex> lg(m_mtx);
	m_cv.wait(lg, [this] {
		return ((!m_queue.empty()) || m_stopFlag);				// 기다린다
		});

	if (!m_stopFlag)
	{
		ref = *(m_queue.front());
		m_queue.pop();
	}
	return true;
}

template<typename T>
inline bool CSafeQueue<T>::smart_pop(T& ref)
{
	std::lock_guard<std::mutex> lg(m_mtx);
	bool bExist = !m_queue.empty();
	if (bExist)
	{
		ref = *(m_queue.front());
		m_queue.pop();
	}

	return bExist;
}



#endif