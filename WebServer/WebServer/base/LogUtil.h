#ifndef WEBSERVER_BASE_LOGUTIL_H
#define WEBSERVER_BASE_LOGUTIL_H

#include <string.h>

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;
const int kMaxNumericSize = 48;

template <int SIZE>
class FixedBuffer{
public:
    FixedBuffer() : m_curPos(m_buffer) {}
    ~FixedBuffer() {}

    void append(const char* buf, size_t len)
    {
        if (static_cast<size_t>(avail()) > len + 1) {
            memcpy(m_curPos, buf, len);
            m_curPos += len;
        }
        *m_curPos++ = ' ';
    }

    const char* getData() const { return m_buffer; }
    int size() const { return static_cast<int>(m_curPos - m_buffer); }

    char* current() { return m_curPos; }
    int avail() const { return static_cast<int>(end() - m_curPos); }
    void add(size_t len) { m_curPos += len; }

    void reset() { m_curPos = m_buffer; }
    void bzero() { memset(m_buffer, 0, sizeof(m_buffer)); }
private:
    const char* end() const { return m_buffer + sizeof(m_buffer); }

    char m_buffer[SIZE];
    char* m_curPos;
};

#endif // !WEBSERVER_BASE_LOGUTIL_H