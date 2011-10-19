#ifndef NET02_LOG_H
#define NET02_LOG_H

#include <cstdio>

#ifdef NETO2_DEBUG_LOG
#define NET02_LOG(fmt, ...) printf(fmt, ## __VA_ARGS__ 
#else
#define NET02_LOG(fmt, ...) ;
#endif

#endif /* NET02_LOG_H */