#pragma once

#include "amber_forge.h"
#include <stdio.h>
#include <time.h>

typedef struct AFlog_event AFlog_event;
struct AFlog_event {
    va_list ap;
    const char *fmt;
    const char *file;
    struct tm time;
    int line;
    int level;
    void *data;
};

typedef void (*AFlog_fn)(AFlog_event *event);
typedef void (*AFlock_fn)(bool lock, void *data);

typedef enum AFlog_level
{
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL,
    MAX_LOG_COUNT
} AFlog_level;

void log_set_lock(AFlock_fn fn, void *data);
void afLogSetLevel(int level);
void log_set_quite(bool enable);

int log_add_callback(AFlog_fn fn, void *data, int level);
int log_add_fp(FILE *fp, int level);

AFAPI void afLog(int level, const char *file, int line, const char *fmt, ...);

// clang-format off
#define AFTRACE(...)  afLog(LOG_TRACE,  __FILE__, __LINE__, __VA_ARGS__)
#define AFDEBUG(...)  afLog(LOG_DEBUG,  __FILE__, __LINE__, __VA_ARGS__)
#define AFINFO(...)   afLog(LOG_INFO,   __FILE__, __LINE__, __VA_ARGS__)
#define AFWARN(...)   afLog(LOG_WARN,   __FILE__, __LINE__, __VA_ARGS__)
#define AFERROR(...)  afLog(LOG_ERROR,  __FILE__, __LINE__, __VA_ARGS__)
#define AFFATAL(...)  afLog(LOG_FATAL,  __FILE__, __LINE__, __VA_ARGS__)
// clang-format on
