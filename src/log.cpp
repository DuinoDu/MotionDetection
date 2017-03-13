#include "../include/log.h"

void Log(const char * szLog)
{
#if defined(DEBUG_OUTPUT_LOG)
    OutputDebugStringA(szLog);
    OutputDebugStringA("\n");
#else
    return;
#endif
    /*
    SYSTEMTIME st;
    GetLocalTime(&st);
    FILE *fp;
    fp = fopen("mlog.txt", "at");
    fprintf(fp, "MyLogInfo: %d:%d:%d:%d ", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    fprintf(fp, szLog);
    fclose(fp);
    */
}
void LogString(const std::string& msg)
{
    Log(msg.c_str());
}
void LogString2(const std::string& key, int value)
{
#if defined(DEBUG_OUTPUT_LOG)
    char str[100];
    sprintf(str, "%d", value);
    std::string value_string(str);
    std::string msg = key;
    msg += ' = ';
    msg += value_string;
    Log(msg.c_str());
#endif
}
