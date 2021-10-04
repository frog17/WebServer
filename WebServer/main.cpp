#include <iostream>
#include "server/WebServer.h"
#include "log/Logging.h"

int main()
{
	std::cout << "Hello CMake." << std::endl;
	LOG_INFO("hello worid!");
	return 0;
}