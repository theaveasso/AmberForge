#pragma once

#include "af_defines.h"

#include <time.h>
#include <stdio.h>

/**
 * https://github.com/rxi/log.c.git
 */
typedef struct AFlog_event AFlog_event;
struct AFlog_event {
    va_list ap;
    const char *fmt;
    const char *file;
    struct tm time;
    void *data;
    int line;
    int level;
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

const char *log_level_string(int level);
void log_set_lock(AFlock_fn fn, void *data);
void log_set_level(int level);
void log_set_quite(bool enable);
int log_add_callback(AFlog_fn fn, void *data, int level);
int log_add_fp(FILE *fp, int level);

AFAPI void cel_log(int level, const char *file, int line, const char *fmt, ...);

// clang-format off
#define AF_TRACE(...)  cel_log(LOG_TRACE,  __FILE__, __LINE__, __VA_ARGS__)
#define AF_DEBUG(...)  cel_log(LOG_DEBUG,  __FILE__, __LINE__, __VA_ARGS__)
#define AF_INFO(...)   cel_log(LOG_INFO,   __FILE__, __LINE__, __VA_ARGS__)
#define AF_WARN(...)   cel_log(LOG_WARN,   __FILE__, __LINE__, __VA_ARGS__)
#define AF_ERROR(...)  cel_log(LOG_ERROR,  __FILE__, __LINE__, __VA_ARGS__)
#define AF_FATAL(...)  cel_log(LOG_FATAL,  __FILE__, __LINE__, __VA_ARGS__)
// clang-format on