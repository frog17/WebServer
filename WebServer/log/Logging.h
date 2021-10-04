#ifndef WEBSERVER_LOG_LOGGING_H
#define WEBSERVER_LOG_LOGGING_H

#include <assert.h>
#include <ctime>
#include <memory>
#include <thread>
#include <stdio.h>
#include <string>

#include "../base/LogUtil.h"
//#include "../base/config.h"

const long kRollSize = 500 * 1000 * 1000;

class Logger
{
public:

	enum class LogLevel
	{
		DEBUG,
		INFO,
		WARN,
		ERROR,
	};
	static const int LOG_LEVEL_NUM = 4;

	Logger(LogLevel);
	virtual ~Logger();

	static LogLevel getLogLevel();

	static void setLogLevel(LogLevel);

	void output(const char*, int);

	void append(const char* format, ...);

	void flush();

private:

	void formatTime();
	void printThreadID();
	void printLogLevel(LogLevel);

	FixedBuffer<kSmallBuffer> m_buffer;
	LogLevel m_level;
};

#define LOG_BASE(level, format, ...)\
    do {\
        std::unique_ptr<Logger> log(std::make_unique<Logger>(level));\
        if (log->getLogLevel() <= level) {\
		    log->append(format, ##__VA_ARGS__);\
            log->flush();\
		}\
	} while (0);

#define LOG_DEBUG(format, ...) do {LOG_BASE(Logger::LogLevel::DEBUG, format, ##__VA_ARGS__)} while (0);
#define LOG_INFO(format, ...) do {LOG_BASE(Logger::LogLevel::INFO, format, ##__VA_ARGS__)} while (0);
#define LOG_WARN(format, ...) do {LOG_BASE(Logger::LogLevel::WARN, format, ##__VA_ARGS__)} while (0);
#define LOG_ERROR(format, ...) do {LOG_BASE(Logger::LogLevel::ERROR, format, ##__VA_ARGS__)} while (0);

#endif // WEBSERVER_LOG_LOGGING_H
