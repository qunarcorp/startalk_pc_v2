#ifndef _QTQUEUE_H_
#define _QTQUEUE_H_

#include <QMutexLocker>
#include <QQueue>
#include <QWaitCondition>

template <class T>
class QtQueue
{
public:

	QtQueue()
	{
		m_queue.reserve(100000);
	}

	inline T dequeue()
	{
		QMutexLocker locker(&m_mutex);
		while (m_queue.size() == 0)
		{
			m_wait.wait(&m_mutex);
		}
		return m_queue.dequeue();
	}

	inline void enqueue(const T& t)
	{
		QMutexLocker locker(&m_mutex);

		m_queue.enqueue(t);

		m_wait.wakeAll();
	}

	inline void clear()
	{
		QMutexLocker locker(&m_mutex);

		m_queue.clear();
		m_queue.reserve(100000);

		m_wait.wakeAll();
	}

	inline int GetSize()
	{
		QMutexLocker locker(&m_mutex);

		return m_queue.size();
	}

protected:
	QMutex           m_mutex;
	QQueue<T>        m_queue;
	QWaitCondition   m_wait;
};

#endif// _QTQUEUE_H_