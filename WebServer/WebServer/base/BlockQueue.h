#ifndef WEBSERVER_BASE_BLOCKQUEUE_H
#define WEBSERVER_BASE_BLOCKQUEUE_H

#include <deque>
#include <mutex>
#include <condition_variable>

template <typename T>
class BlockQueue
{
public:
	BlockQueue(size_t) {}

	~BlockQueue() {}

	void stop();

	void clear();

	bool empty();

	bool full();

	void flush();

	T front();

	T back();

	size_t size();

	size_t capacity();

	bool push(const T&);

	bool pop(T&);
private:
	// 保护m_queue队列
	std::mutex m_mtx;
	std::condition_variable m_cond;

	std::deque<T> m_queue;
	int m_capacity;
	
	bool m_stop;
};

template <typename T>
BlockQueue<T>::BlockQueue(size_t capacity) : m_capacity(capacity)
{
	assert(m_capacity > 0);
	m_stop = false;
}

template <typename T>
BlockQueue<T>::~BlockQueue()
{
	stop();
}

template <typename T>
BlockQueue<T>::stop()
{
	{
		std::lock_guard<std::mutex> gurad(m_mtx);
		m_queue.clear();
		m_stop = true;
	}
	m_cond.notify_all();
}

template <typename T>
BlockQueue<T>::clear()
{
	std::lock_guard<std::mutex> guard(m_mtx);
	m_queue.clear();
}

template <typename T>
bool BlockQueue<T>::empty()
{
	std::lock_guard<std::mutex> guard(m_mtx);
	return m_queue.empty();
}

template <typename T>
bool BlockQueue<T>::full()
{
	std::lock_guard<std::mutex> guard(m_mtx);
	return m_queue.size() >= m_capacity;
}

template <typename T>
void BlockQueue<T>::flush()
{
	m_cond.notify_one();
}

template <typename T>
T BlockQueue<T>::front()
{
	std::lock_guard<std::mutex> guard(m_mtx);
	return m_queue.front();
}

template <typename T>
T BlockQueue<T>::back()
{
	std::lock_guard<std::mutex> guard(m_mtx);
	return m_queue.back();
}

template <typename T>
size_t BlockQueue<T>::size()
{
	std::lock_guard<std::mutex> guard(m_mtx);
	return m_queue.size();
}

template <typename T>
size_t BlockQueue<T>::capacity()
{
	std::lock_guard<std::mutex> guard(m_mtx);
	return m_capacity;
}

template <typename T>
bool BlockQueue<T>::push(const T& item)
{
	std::lock_guard<std::mutex> guard(m_mtx);
	if (m_queue.size() >= m_capacity) {
		m_cond.notify_one();
		return false;
	}
	m_queue.push_back(item);
	m_cond.notify_one();
	return true;
}

template <typename T>
bool BlockQueue<T>::pop(T& item)
{
	std::unique_lock<std::mutex> guard(m_mtx);
	while (m_queue.empty()) {
		if (m_stop) {
			return false;
		}
		m_cond.wait(guard);
	}
	item = m_queue.front();
	m_queue.pop_front();
	return true;
}

#endif // !WEBSERVER_BASE_BLOCKQUEUE_H
