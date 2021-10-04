#include "FileUtil.h"

#include <errno.h>
#include <string.h>

AppendFile::AppendFile(const char* filename)
	: m_fp(fopen(filename, "ae")),
	  m_writtenBytes(0)
{
	assert(m_fp);
	setbuffer(m_fp, m_buffer, sizeof(m_buffer));
}

AppendFile::~AppendFile()
{
	fclose(m_fp);
}

void AppendFile::append(const char* logline, size_t len)
{
	size_t written = 0;
	while (written != len)
	{
		size_t remain = len - written;
		size_t n = write(logline + written, remain);
		if (n != remain)
		{
			int err = ferror(m_fp);
			if (err)
			{
				fprintf(stderr, "AppendFile::append() failed %s\n", strerror(err));
				break;
			}
		}
		written += n;
	}

	m_writtenBytes += written;
}

void AppendFile::flush()
{
	fflush(m_fp);
}

size_t AppendFile::write(const char* logline, size_t len)
{
	return fwrite_unlocked(logline, 1, len, m_fp);
}