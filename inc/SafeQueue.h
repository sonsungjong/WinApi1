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
	std::atomic<bool> m_stopFlag;				// �ı��ڿ��� ȣ���� wait_pop �����÷���

public:
	CSafeQueue();
	virtual ~CSafeQueue();
	CSafeQueue(CSafeQueue const& other_queue);
	CSafeQueue& operator=(CSafeQueue const& other_queue) = delete;
	CSafeQueue(CSafeQueue&& other_queue) noexcept;
	CSafeQueue& operator=(CSafeQueue&& other_queue) noexcept = delete;

	bool empty();				// ��������� true, ��������� false
	size_t size();				// ť ������
	void clear();				// ť ����

	void push(const T& lvalue);							// ������ �����Ͽ� push (�Ϲ�, pop�ؼ� ��� �� �޸����� ����)
	T wait_pop();
	bool wait_pop(T& ref);
	bool pop(T& ref);

	void move_push(T&& rvalue);						// std::move�� push (������ ���̻� �����Ҷ�, ��������)

	// �����Ҵ� ������ ����ϴ� �ʿ��� ���� �������Ҷ� ����ϴ� ����
	void smart_push(const T& lvalue);						// �����Ҵ翡 ���� �޸������� �Ű澲�� ���� ������ (����)
	std::shared_ptr<T> smart_wait_pop();				// �������� �޴´� (���)
	bool smart_wait_pop(T& ref);								// ���۷����� �޴´� (���)
	bool smart_pop(T& ref);										// ���۷����� �޴´� (���)
};

template<typename T>
inline CSafeQueue<T>::CSafeQueue()
{
	m_stopFlag = false;
}

template<typename T>
inline CSafeQueue<T>::~CSafeQueue()
{
	// ��� wait_pop�� �����Ѵ�
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
		return ((!m_queue.empty()) || m_stopFlag);			// ��ٸ���
		});

	T ref{};
	if (!m_stopFlag)			// �����÷��� Ȱ��ȭ?
	{
		ref = m_queue.front();
		m_queue.pop();
	}

	return ref;			// ����� �����Ѵ�
}

template<typename T>
inline bool CSafeQueue<T>::wait_pop(T& ref)
{
	std::unique_lock<std::mutex> lg(m_mtx);
	m_cv.wait(lg, [this] {
		return ((!m_queue.empty()) || m_stopFlag);				// ��ٸ���
		});

	if ((!m_stopFlag) && !m_queue.empty()) {
		ref = m_queue.front();				// ����� ���۷����� �ִ´�
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
		return ((!m_queue.empty()) || m_stopFlag);			// ä�����ų� �����ȣ�� ��ٸ���
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
		return ((!m_queue.empty()) || m_stopFlag);				// ��ٸ���
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