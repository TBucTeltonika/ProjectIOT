#ifndef LOGGER_H
#define LOGGER_H
#include <sys/syslog.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#define LOGNAME "ProjectIot"

#define LOGTO_LOGREAD

#ifdef LOGTO_LOGREAD
#define TRACE_LOG(pri,fmt,args...) logwritef(pri, fmt, ##args)
#endif
#ifndef LOGTO_LOGREAD
//#define TRACE_LOG(pri,fmt,...) printf((fmt), ##__VA_ARGS__)
#define TRACE_LOG(pri,format, args...) \    
    fprintf(stderr, format , ## args); \
    fprintf(stderr, "\n"); 
#endif

int logwritepri(int pri, const char* text);
int logwrite(const char* text);
int logwritef(int pri, const char* fmt,...);
#endif