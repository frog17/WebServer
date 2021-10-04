#ifndef WEBSERVER_LOG_LOGFILE_H
#define WEBSERVER_LOG_LOGFILE_H

#include <string>
#include <memory>
#include <mutex>

#include "../base/FileUtil.h"

class LogFile
{
public:
	LogFile(const std::string& basename, long rollSize,
		bool threadSafe = true,
		int flushInterval = 3, int checkEveryN = 1024);
	~LogFile() = default;

	void append(const char* logline, int len);
	void flush();
	bool rollFile();

private:
	void append_unlocked(const char* logline, int len);

	static std::string getLogFileName(const std::string& basename, time_t* now);

	const std::string m_basename;
	const long m_rollSize;
	const int m_flushInterval;
	const int m_checkEveryN;

	int m_count;

	std::unique_ptr<std::mutex> m_mutex;
	time_t m_startOfPeriod;
	time_t m_lastRoll;
	time_t m_lastFlush;
	std::unique_ptr<AppendFile> m_file;

	const static int kRollPerSeconds = 60 * 60 * 24;
};

#endif // !WEBSERVER_LOG_LOGFILE_H
