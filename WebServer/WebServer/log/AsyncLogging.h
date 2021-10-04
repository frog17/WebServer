#ifndef WEBSERVER_LOG_ASYNCLOGGING_H
#define WEBSERVER_LOG_ASYNCLOGGING_H

#include <condition_variable>
#include <vector>
#include <functional>
#include <thread>

#include "../base/LogUtil.h"
#include "LogFile.h"

class AsyncLogging
{
public:
	static AsyncLogging* getInstance(const std::string& basename, 
		long rollSize, int flushInterval = 3);

	void start();
	void stop();

	void append(const char* logline, int len);
private:
	AsyncLogging(const std::string& basename,
		long rollSize, int flushInterval = 3);
	~AsyncLogging();

	void threadFunc();

	const std::string m_basename;
	const long m_rollSize;
	const int m_flushInterval;
	bool m_running;

	std::thread m_thread;
	std::mutex m_mutex;
	std::condition_variable m_cond;

	using Buffer = FixedBuffer<kLargeBuffer>;
	using BufferVector = std::vector<std::unique_ptr<Buffer>>;
	using BufferPtr = BufferVector::value_type;

	BufferPtr m_currBuffer;
	BufferPtr m_nextBuffer;
	BufferVector m_buffers;
};

#endif // !WEBSERVER_LOG_ASYNCLOGGING_H
