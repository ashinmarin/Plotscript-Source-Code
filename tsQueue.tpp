template<class T>
void MessageQueue<T>::push(const T & value)
{
 	std::unique_lock<std::mutex> lock(m_mutex);
	m_queue.push(value); lock.unlock();
	the_condition_variable.notify_one();
 }

template<class T>
bool MessageQueue<T>::empty() const
{
 	std::lock_guard<std::mutex> lock(m_mutex); 
	
	return m_queue.empty();
 }

template<class T>
bool MessageQueue<T>::try_pop(T & popped_value)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_queue.empty())
	{ 
		return false; 
	}
 
	popped_value = m_queue.front();
	m_queue.pop();
	return true;
 }

template<class T>
void MessageQueue<T>::wait_and_pop(T & popped_value)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	while (m_queue.empty())
	{
		the_condition_variable.wait(lock);
	}
 	popped_value = m_queue.front(); 
	m_queue.pop();
 } 