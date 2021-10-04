#ifndef WEBSERVER_BASE_FILEUTIL_H
#define WEBSERVER_BASE_FILEUTIL_H

#include <stdio.h>
#include <assert.h>

class AppendFile
{
public:
	explicit AppendFile(const char* filename);

	~AppendFile();

	void append(const char* logline, size_t len);

	void flush();

	long writtenBytes() const { return m_writtenBytes; }

private:
	size_t write(const char* logline, size_t len);

	FILE* m_fp;
	char m_buffer[64 * 1024];
	long m_writtenBytes;
};

#endif // !WEBSERVER_BASE_FILEUTIL_H
