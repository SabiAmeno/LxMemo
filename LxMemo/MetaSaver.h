#pragma once

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include "Meta.h"

template<typename T>
class BlockQueue
{
public:
	void Push(T&& data);
	T Pop();
	bool Empty();

private:
	std::queue<T> queue_{};

	std::mutex mtx_{};
	std::condition_variable cv_{};
};

template<typename T>
inline void BlockQueue<T>::Push(T&& data)
{
	queue_.push(data);
	cv_.notify_one();
}

template<typename T>
inline T BlockQueue<T>::Pop()
{
	std::unique_lock<std::mutex> lck(mtx_);
	cv_.wait(lck, [&] {return !queue_.empty(); });

	if (queue_.empty())
		return nullptr;

	T top = queue_.front();
	queue_.pop();

	return top;
}

template<typename T>
inline bool BlockQueue<T>::Empty()
{
	return queue_.empty();
}

struct Event
{
	SharedMeta meta;
	QByteArray contents;

	QString time;
	QString snapshot;
};

class DB;
class MetaSaver
{
public:
	MetaSaver(DB* db);

	void Push(Event* event);
private:
	void Run();
private:
	DB* db_{ nullptr };
	BlockQueue<Event*> bq_{};

	std::thread* thread_{ nullptr };
};