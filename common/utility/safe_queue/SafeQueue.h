#ifndef SLINK_SAFE_QUEUE_H
#define SLINK_SAFE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

template<typename T>
class SafeQueue
{
public:
	SafeQueue() = default;

	SafeQueue(const SafeQueue& other) = delete;

	SafeQueue& operator=(const SafeQueue& other) = delete;

	void push(const T& val)
	{
		{
			std::lock_guard lock(m_Mutex);

			m_Queue.push(val);
		}
		m_CondVar.notify_one();
	}

	void push(T&& val)
	{
		{
			std::lock_guard lock(m_Mutex);

			m_Queue.push(std::move(val));
		}
		m_CondVar.notify_one();
	}

	std::optional<T> tryPop()
	{
		std::lock_guard lock(m_Mutex);

		if (m_Queue.empty())
			return std::nullopt;

		auto value = std::move(m_Queue.front());

		m_Queue.pop();

		return value;
	}

private:
	std::queue<T> m_Queue;

	std::mutex m_Mutex;

	std::condition_variable m_CondVar;
};

#endif