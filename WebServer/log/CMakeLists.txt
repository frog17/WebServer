set(LOG_SRCS
	AsyncLogging.cpp
	LogFile.cpp
	Logging.cpp
)

add_library(xlog ${LOG_SRCS})
target_link_libraries(xlog pthread)
