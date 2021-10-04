#include "Logging.h"
#include "AsyncLogging.h"

#include <stdarg.h>
#include <sstream>

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;

// Efficient Integer to String Conversions, by Matthew Wilson.
/*
template <typename T>
size_t convert(char buf[], T value)
{
	T i = value;
	char* p = buf;

	do {
		int lsd = static_cast<int>(i % 10);
		i /= 10;
		*p++ = zero[lsd];
	} while (i != 0);

	if (value < 0) {
		*p++ = '-';
	}
	*p = '\0';
	std::reverse(buf, p);

	return p - buf;
}
*/

Logger::LogLevel initLogLevel()
{
	/*std::string conf = Config::getInstance().getString("log", "level", "");
	Logger::LogLevel level;

	switch (level) {
	case "LOG_LEVEL_INFO" :
		level = Logger::INFO;
		break;
	case "LOG_LEVEL_DEBUG":
		level = Logger::DEBUG;
		break;
	default:
		level = Logger::INFO;
	}
	return level;*/
	return Logger::LogLevel::INFO;
}

const char* LogLevelName[Logger::LOG_LEVEL_NUM] = {
	"DEBUG ", "INFO", "WARN", "ERROR",
};

Logger::LogLevel g_logLevel = initLogLevel();

Logger::Logger(LogLevel level) : m_level(level)
{
	formatTime();
	printThreadID();
	printLogLevel(level);
}

Logger::~Logger()
{
	m_buffer.append("\n\0", 2);
	output(m_buffer.getData(), m_buffer.size());
}

Logger::LogLevel Logger::getLogLevel()
{
	return g_logLevel;
}

void Logger::setLogLevel(Logger::LogLevel level)
{
	g_logLevel = level;
}

void Logger::output(const char* msg, int len)
{
	AsyncLogging* asyncLog = AsyncLogging::getInstance("logcat", kRollSize);
	asyncLog->append(msg, len);
}

void Logger::append(const char* format, ...)
{
	va_list vaList;
	va_start(vaList, format);
	int m = vsnprintf(m_buffer.current(), m_buffer.avail(), format, vaList);
	m_buffer.add(m);
	va_end(vaList);
}

void Logger::flush()
{

}

/* 时间格式：20210925 16:04:28 */
void Logger::formatTime()
{
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	std::strftime(m_buffer.current(), m_buffer.avail(), "%Y%m%d %H:%M:%S", std::gmtime(&ts.tv_sec));
	m_buffer.add(17);

	snprintf(m_buffer.current(), m_buffer.avail(), ".%06ld ", ts.tv_nsec);
	m_buffer.add(8);
}

void Logger::printThreadID()
{
	std::thread::id tid = std::this_thread::get_id();
	std::ostringstream ss;
	ss << tid;
	std::string idstr = ss.str();
	m_buffer.append(idstr.c_str(), idstr.size());
}

void Logger::printLogLevel(LogLevel level)
{
	int index = (int)level;
	const char* log = LogLevelName[index];
	m_buffer.append(log, strlen(log));
}

