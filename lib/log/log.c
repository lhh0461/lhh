/*
 * 实现日志封装
 * TODO：可以改为直接根据模块名写文件
 *       不使用系统的接口
 */
#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>

#include "log.h"

void syslog_err(const char *format, ...)
{
    va_list arg;
    char buf[1024] = {'\0'};
    int n; 
    va_start(arg, format);
    n = vsnprintf(buf, sizeof(buf), format, arg);
    va_end(arg);

    if (n < 0) {
        syslog(LOG_ERR, "syslog_err vsnprintf error!!!\n");
        return;
    }
    syslog(LOG_ERR, buf);
}

void syslog_debug(const char *format, ...)
{
    va_list arg;
    char buf[1024] = {'\0'};
    int n; 
    va_start(arg, format);
    n = vsnprintf(buf, sizeof(buf), format, arg);
    va_end(arg);

    if (n < 0) {
        syslog(LOG_ERR, "syslog_debug vsnprintf error!!!\n");
        return;
    }
    syslog(LOG_DEBUG, buf);
}

void syslog_info(const char *format, ...)
{
    va_list arg;
    char buf[1024] = {'\0'};
    int n; 
    va_start(arg, format);
    n = vsnprintf(buf, sizeof(buf), format, arg);
    va_end(arg);

    if (n < 0) {
        syslog(LOG_ERR, "syslog_info vsnprintf error!!!\n");
        return;
    }
    syslog(LOG_INFO, buf);
}

void log_init(const char *program_name, int facility)
{
    openlog(program_name, LOG_PID | LOG_CONS, facility);
}

void log_release(void)
{
    closelog();
}

//int main()
//{
//    log_init("test", LOG_LOCAL0);
//    //log_init("test", 0);
//    syslog_info("info ");
//    syslog_debug("debug");
//    syslog_err("error");
//    log_release();
//    return 0;
//}
