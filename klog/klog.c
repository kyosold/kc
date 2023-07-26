#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include "klog.h"

static int klog_level = KLOG_INFO;
static char klog_sid[128] = {0};
static char klog_ident[512] = {0};
static char *klog_priority[] = {
    "EMERG",
    "ALERT",
    "CRIT",
    "ERROR",
    "WARNING",
    "NOTICE",
    "INFO",
    "DEBUG"};

static int klog_facility = LOG_USER;
static int klog_opt = LOG_PID;

// --- Use syslog of linux ---
void _klog_write(int level, const char *file, int line,
                 const char *fun, const char *fmt, ...)
{
    if (level > klog_level)
        return;

    int n;
    char new_fmt[KLOG_MAX_LINE];
    const char *head_fmt = "[%s] %s:%d:%s %s %s";
    snprintf(new_fmt, sizeof(new_fmt), head_fmt, klog_priority[level], file, line, fun, klog_sid, fmt);

    va_list ap;
    va_start(ap, fmt);
    openlog(klog_ident, klog_opt, klog_facility);
    vsyslog(level, new_fmt, ap);
    closelog();
    va_end(ap);
}

/**
 * @brief open log
 *
 * @param ident     the name of program
 * @param facility  LOG_MAIL/LOG_USER
 * @param level     KLOG_DEBUG/KLOG_INFO ...
 * @param sid       set sid for session
 * @example klog_open("test_klog", LOG_MAIL, KLOG_DEBUG, "FFFFFF");
 */
void klog_open(const char *ident, int facility, int level, char *sid)
{
    snprintf(klog_ident, sizeof(klog_ident), "%s", ident);
    klog_facility = facility;
    klog_level = level;
    snprintf(klog_sid, sizeof(klog_sid), "%s", sid);
    klog_type = 0;
}

// --- Use write to syslog file ---
static char klog_file[1024] = {0};
static char klog_hostname[1024];

/**
 * @brief open log file
 *
 * @param file      write log to file name
 * @param ident     the name of program
 * @param level     KLOG_DEBUG/KLOG_INFO ...
 * @param sid       set sid for session
 * @example klog_open_file(argv[1], "test_klog", KLOG_DEBUG, "FF00FF");
 */
void klog_open_file(const char *file, const char *ident, int level, char *sid)
{
    snprintf(klog_file, sizeof(klog_file), "%s", file);
    snprintf(klog_ident, sizeof(klog_ident), "%s", ident);
    klog_level = level;
    snprintf(klog_sid, sizeof(klog_sid), "%s", sid);
    gethostname(klog_hostname, sizeof(klog_hostname));
    char *end = (char *)memchr(klog_hostname, '.', strlen(klog_hostname));
    if (end)
        *end = 0;
    klog_type = 1;
}

void _klog_write_file(int level, const char *file, int line,
                      const char *fun, const char *fmt, ...)
{
    if (level > klog_level)
        return;

    // get datetime. e.g: Jul 13 15:06:01
    char log_date[128];
    time_t now = time(NULL);
    struct tm *local_tm = localtime(&now);
    strftime(log_date, sizeof(log_date), "%b %d %H:%M:%S", local_tm);

    // make log line prefix. e.g: Jul 13 16:38:45 songjian test[6857]: [INFO] test.c:35:main
    char pref[KLOG_MAX_LINE];
    const char *pref_fmt = "%s %s %s[%d]: [%s] %s:%d:%s %s";
    snprintf(pref, sizeof(pref), pref_fmt, log_date, klog_hostname, klog_ident, getpid(), klog_priority[level], file, line, fun, klog_sid);

    // make log muli-arguments
    char log_line[KLOG_MAX_LINE];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(log_line, sizeof(log_line), fmt, ap);
    va_end(ap);

    FILE *fp = fopen(klog_file, "a+");
    if (fp != NULL)
    {
        fprintf(fp, "%s %s\n", pref, log_line);
        fclose(fp);
    }

    return;
}

/**
 * @brief set log level
 *
 * @param level     KLOG_DEBUG/KLOG_INFO ...
 */
void klog_set_level(int level)
{
    klog_level = level;
}
/**
 * @brief set log session id
 *
 * @param sid       using tag session
 */
void klog_set_sid(char *sid)
{
    snprintf(klog_sid, sizeof(klog_sid), "%s", sid);
}
