#include "AsyncLogging.h"

#include <assert.h>
#include <chrono>

//void setThreadName(std::thread* t, const char* name)
//{
//	auto handle = t->native_handle();
//	pthread_setname_np(handle, name);
//}

AsyncLogging* AsyncLogging::getInstance(const std::string& basename,
	long rollSize, int flushInterval)
{
	static AsyncLogging log(basename, rollSize);
	return &log;
}

AsyncLogging::AsyncLogging(const std::string& basename, long rollSize, int flushInterval)
	: m_basename(basename),
	  m_rollSize(rollSize),
	  m_flushInterval(flushInterval),
	  m_running(false),
	  m_currBuffer(new Buffer),
	  m_nextBuffer(new Buffer),
	  m_buffers()
{
	m_currBuffer->bzero();
	m_nextBuffer->bzero();
	m_buffers.reserve(16);

	start();
}

AsyncLogging::~AsyncLogging()
{
	if (m_running)
	{
		stop();
	}
}

void AsyncLogging::start()
{
	m_running = true;
	m_thread = std::thread(&AsyncLogging::threadFunc, this);
	//setThreadName(&m_thread, "Logging");
}

void AsyncLogging::stop()
{
	m_running = false;
	m_cond.notify_one();
	m_thread.join();
}

void AsyncLogging::append(const char* logline, int len)
{
	std::lock_guard<std::mutex> guard(m_mutex);
	if (m_currBuffer->avail() > len)
	{
		m_currBuffer->append(logline, len);
	} else
	{
		m_buffers.push_back(std::move(m_currBuffer));

		if (m_nextBuffer)
		{
			m_currBuffer = std::move(m_nextBuffer);
		}
		else
		{
			m_currBuffer.reset(new Buffer);
		}

		m_currBuffer->append(logline, len);
		m_cond.notify_one();
	}
}

void AsyncLogging::threadFunc()
{
	assert(m_running == true);
	LogFile output(m_basename, m_rollSize, false);
	BufferPtr newBuffer1(new Buffer);
	BufferPtr newBuffer2(new Buffer);
	newBuffer1->bzero();
	newBuffer2->bzero();
	BufferVector buffersToWrite;
	buffersToWrite.reserve(16);
	while (m_running)
	{
		assert(newBuffer1 && newBuffer1->size() == 0);
		assert(newBuffer2 && newBuffer2->size() == 0);
		assert(buffersToWrite.empty());

		{
			std::unique_lock<std::mutex> guard(m_mutex);
			if (m_buffers.empty())
			{
				m_cond.wait_for(guard, std::chrono::seconds(m_flushInterval));
			}
			m_buffers.push_back(std::move(m_currBuffer));
			m_currBuffer = std::move(newBuffer1);
			buffersToWrite.swap(m_buffers);
			if (!m_nextBuffer)
			{
				m_nextBuffer = std::move(newBuffer2);
			}
		}

		assert(!buffersToWrite.empty());

		// todo: skip to much buffer

		for (const auto& buffer : buffersToWrite)
		{
			output.append(buffer->getData(), buffer->size());
		}

		if (buffersToWrite.size() > 2)
		{
			buffersToWrite.resize(2);
		}

		if (!newBuffer1)
		{
			assert(!buffersToWrite.size());
			newBuffer1 = std::move(buffersToWrite.back());
			buffersToWrite.pop_back();
			newBuffer1->reset();
		}

		if (!newBuffer2)
		{
			assert(!buffersToWrite.size());
			newBuffer2 = std::move(buffersToWrite.back());
			buffersToWrite.pop_back();
			newBuffer2->reset();
		}

		buffersToWrite.clear();
		output.flush();
	}
	output.flush();
}
