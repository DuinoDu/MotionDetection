#ifndef LOG_H
#define LOG_H

#include "windows.h"
#include <string>
void Log(const char * szLog);
void LogString(const std::string& msg);
void LogString2(const std::string& key, int value);

#endif // LOG_H
