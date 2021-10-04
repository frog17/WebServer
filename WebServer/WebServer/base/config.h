#ifndef WEBSERVER_BASE_CONFIG_H
#define WEBSERVER_BASE_CONFIG_H

#include <string>
#include <memory>

#include "ini/ini.h"

class Config
{
public:
	static Config& getInstance(const std::string& path = "/.xxd/config/conf.ini");

	int getInt(const std::string& section, const std::string& search, int defaultVal = 0);
	size_t getSize(const std::string& section, const std::string& search, int defaultVal = 0);
	bool getBool(const std::string& section, const std::string& search, bool defaultVal = true);
	double getDouble(const std::string& section, const std::string& search, double defaultVal = 0.0);
	std::string getString(const std::string& section, const std::string& search, std::string defaultVal = "");
private:
	Config(const std::string& path);
	~Config() = default;
	Config(const Config&) = delete;
	Config& operator=(const Config&) = delete;

	std::string m_relatvePath;
	std::unique_ptr<INIReader> m_reader;
};

#endif // !WEBSERVER_BASE_CONFIG_H
