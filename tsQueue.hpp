#ifndef MESSAGEQUEUE
#define MESSAGEQUEUE
#include <mutex>
#include <queue>
#include <condition_variable>
//This class is a thread safe message queue
template <class T>
class MessageQueue
{
public:
	void push(const T & value);
	bool empty() const;
	bool try_pop(T & popped_value);
	void wait_and_pop(T & popped_value);
private:
	std::queue<T> m_queue;
	mutable std::mutex m_mutex;
	std::condition_variable the_condition_variable;
};
#include "tsQueue.tpp"
#endif 