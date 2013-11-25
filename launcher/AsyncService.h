#pragma once

#include <functional>
#include <thread>
#include <atomic>

template <typename CommandType, typename ResultType>
class CAsyncService
{
public:
	typedef std::function<void (const ResultType&)> ContinuationFunction;

	CAsyncService()
	{
		m_threadActive = true;
		m_thread = std::thread([&] () { ThreadProc(); });
	}

	virtual ~CAsyncService()
	{
		m_threadActive = false;
		m_thread.join();
	}

	bool IsActive() const
	{
		return m_commandActive;
	}

protected:
	virtual ResultType Execute(const CommandType&) = 0;

	void LaunchCommand(const CommandType& command, const ContinuationFunction& continuation)
	{
		assert(!m_commandActive);
		m_continuation = continuation;
		m_command = command;
		m_commandActive = true;
	}

private:
	void ThreadProc()
	{
		while(m_threadActive)
		{
			if(m_commandActive)
			{
				auto continuation = m_continuation;
				auto result = Execute(m_command);
				m_commandActive = false;
				if(continuation)
				{
					continuation(result);
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
	
	std::thread				m_thread;
	bool					m_threadActive;
	CommandType				m_command;
	ContinuationFunction	m_continuation;
	std::atomic<bool>		m_commandActive = false;
};
