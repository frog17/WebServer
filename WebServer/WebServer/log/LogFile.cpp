#include "LogFile.h"

#include <ctime>
#include <unistd.h>

LogFile::LogFile(const std::string& basename, long rollSize,
	bool threadSafe, int flushInterval, int checkEveryN)
	: m_basename(basename),
	  m_rollSize(rollSize),
	  m_flushInterval(flushInterval),
	  m_checkEveryN(checkEveryN),
	  m_count(0),
	  m_mutex(threadSafe ? new std::mutex : nullptr),
	  m_startOfPeriod(0),
	  m_lastRoll(0),
	  m_lastFlush(0)
{
	assert(basename.find('/') == std::string::npos);
	rollFile();
}

void LogFile::append(const char* logline, int len)
{
	if (m_mutex)
	{
		std::lock_guard<std::mutex> guard(*m_mutex);
		append_unlocked(logline, len);
	} else 
	{
		append_unlocked(logline, len);
	}
}

void LogFile::flush()
{
	if (m_mutex)
	{
		std::lock_guard<std::mutex> guard(*m_mutex);
		m_file->flush();
	} 
	else
	{
		m_file->flush();
	}
}

bool LogFile::rollFile()
{
	time_t now = 0;
	std::string filenme = getLogFileName(m_basename, &now);
	time_t start = now / kRollPerSeconds * kRollPerSeconds;

	if (now > m_lastRoll)
	{
		m_lastFlush = now;
		m_lastRoll = now;
		m_startOfPeriod = start;
		m_file.reset(new AppendFile(filenme.c_str()));
		return true;
	}
	return false;
}

void LogFile::append_unlocked(const char* logline, int len)
{
	m_file->append(logline, len);
	if (m_file->writtenBytes() > m_rollSize)
	{
		rollFile();
	}
	else
	{
		++m_count;
		if (m_count >= m_checkEveryN)
		{
			m_count = 0;
			time_t now = time(nullptr);
			time_t thisPeriod = now / kRollPerSeconds * kRollPerSeconds;
			if (thisPeriod != m_startOfPeriod)
			{
				rollFile();
			}
			else if (now - m_lastFlush > m_flushInterval)
			{
				m_lastFlush = now;
				m_file->flush();
			}
		}
	}
}

std::string LogFile::getLogFileName(const std::string& basename, time_t* now)
{
	std::string filename;
	filename.reserve(basename.size() + 64);

	char timebuf[32];
	*now = time(nullptr);
	struct tm* t = localtime(now);
	strftime(timebuf, sizeof(timebuf), ".%Y%m%d-%H%M%S.", t);
	filename += timebuf;

	char pidbuf[32];
	snprintf(pidbuf, sizeof(pidbuf), "%d", getpid());
	filename += pidbuf;

	filename += ".log";
	return filename;
}

