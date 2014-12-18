/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <sc/ash/log.h>

#include <sc/ash/util.h>

#include <assert.h>
#include <stdio.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h> // getpid()

static const char* LABELS[] = {
    "CRIT",
    "ERROR",
    "WARN",
    "NOTICE",
    "INFO",
    "DEBUG"
};

static __thread struct {
    int level;
    ScLogger logger;
} log = {
#if SC_DEBUG_LEVEL >= 1
    .level = SC_LOG_DEBUG1,
#else  // SC_DEBUG_LEVEL < 1
    .level = SC_LOG_INFO,
#endif // SC_DEBUG_LEVEL < 1
    .logger = sc_log_clnt
};

SC_API const char*
sc_log_label(int level)
{
    return LABELS[level < SC_LOG_DEBUG1 ? level : SC_LOG_DEBUG1];
}

SC_API int
sc_log_level(void)
{
    return log.level;
}

SC_API ScLogger
sc_log_logger(void)
{
    return log.logger;
}

SC_API int
sc_log_setlevel(int level)
{
    assert(level >= 0);
    int prev = log.level;
    log.level = level;
    return prev;
}

SC_API ScLogger
sc_log_setlogger(ScLogger logger)
{
    assert(logger);
    ScLogger prev = log.logger;
    log.logger = logger;
    return prev;
}

SC_API void
sc_log_clnt(int level, const char* msg)
{
    FILE* stream = level > SC_LOG_WARN ? stdout : stderr;
    fprintf(stream, "%s\n", msg);
}

SC_API void
sc_log_serv(int level, const char* msg)
{
    const long ms = sc_millis();
    const time_t now = ms / 1000;

    struct tm tm;
    localtime_r(&now, &tm);

    char buf[sizeof("Mar 12 06:26:39")];
    strftime(buf, sizeof(buf), "%b %d %H:%M:%S", &tm);

    FILE* stream = level > SC_LOG_WARN ? stdout : stderr;
    fprintf(stream, "%s.%03d %-6s [%d]: %s\n", buf, (int)(ms % 1000), sc_log_label(level),
            (int)getpid(), msg);
}

SC_API void
sc_log_syslog(int level, const char* msg)
{
    int priority;
    switch (level) {
    case SC_LOG_CRIT:
        priority = LOG_CRIT;
        break;
    case SC_LOG_ERROR:
        priority = LOG_ERR;
        break;
    case SC_LOG_WARN:
        priority = LOG_WARNING;
        break;
    case SC_LOG_NOTICE:
        priority = LOG_NOTICE;
        break;
    case SC_LOG_INFO:
        priority = LOG_INFO;
        break;
    default:
        priority = LOG_DEBUG;
    }
    syslog(priority, "%s", msg);
}

SC_API void
sc_log_printf(int level, const char* format, ...)
{
    if (level > log.level)
        return;

    va_list args;
    va_start(args, format);
    char msg[SC_LOGMSG_MAX + 1];
    vsnprintf(msg, sizeof(msg), format, args);
    log.logger(level, msg);
    va_end(args);
}

SC_API void
sc_log_vprintf(int level, const char* format, va_list args)
{
    if (level > log.level)
        return;

    char msg[SC_LOGMSG_MAX + 1];
    vsnprintf(msg, sizeof(msg), format, args);
    log.logger(level, msg);
}
