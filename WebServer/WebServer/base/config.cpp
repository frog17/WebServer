#include "config.h"

#include <stdlib.h>

Config& Config::getInstance(const std::string& path)
{
	static Config instance(path);
	return instance;
}

int Config::getInt(const std::string& section, const std::string& search, int defaultVal = 0)
{
	return m_reader->GetInteger(section, search, defaultVal);
}

size_t Config::getSize(const std::string& section, const std::string& search, int defaultVal = 0)
{
	return static_cast<size_t>(getInt(section, search, defaultVal));
}

bool Config::getBool(const std::string& section, const std::string& search, bool defaultVal = true)
{
	return m_reader->GetBoolean(section, search, defaultVal);
}

double Config::getDouble(const std::string& section, const std::string& search, double defaultVal = 0.0)
{
	return m_reader->GetDouble(section, search, defaultVal);
}

std::string Config::getString(const std::string& section, const std::string& search, std::string defaultVal = "")
{
	return m_reader->Get(section, search, defaultVal);
}

Config::Config(const std::string& path)
	: m_relatvePath(path)
{
	std::string confPath = getenv("HOME");
	confPath += m_relatvePath;
	printf("config file is: %s\n", confPath.c_str());
	m_reader = std::make_unique<INIReader>(confPath);
	if (m_reader->ParseError() != 0) {
		printf("Can't load config file at: {}\n", confPath);
	}
}