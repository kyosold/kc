#ifndef __KLOG_H__
#define __KLOG_H__

#include <syslog.h>

#define KLOG_DEBUG 7
#define KLOG_INFO 6
#define KLOG_NOTICE 5
#define KLOG_WARNING 4
#define KLOG_ERROR 3
#define KLOG_CRIT 2
#define KLOG_ALERT 1
#define KLOG_EMERG 0

#define KLOG_MAX_LINE 4096

void klog_set_level(int level);
void klog_set_sid(char *sid);

// --- Use syslog of linux ---
void klog_open(const char *ident, int facility, int level, char *sid);
void _klog_write(int level, const char *file, int line, const char *fun, const char *fmt, ...);
#define klog_debug(fmt, args...) \
    _klog_write(KLOG_DEBUG, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#define klog_info(fmt, args...) \
    _klog_write(KLOG_INFO, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#define klog_notice(fmt, args...) \
    _klog_write(KLOG_NOTICE, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#define klog_warning(fmt, args...) \
    _klog_write(KLOG_WARNING, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#define klog_error(fmt, args...) \
    _klog_write(KLOG_ERROR, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#define klog_crit(fmt, args...) \
    _klog_write(KLOG_CRIT, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#define klog_alert(fmt, args...) \
    _klog_write(KLOG_ALERT, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#define klog_emerg(fmt, args...) \
    _klog_write(KLOG_EMERG, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)

// --- Use write to syslog file ---
void klog_open_plus(const char *ident, int facility, int level, char *sid);
void _klog_write_plus(int level, const char *file, int line,
                      const char *fun, const char *fmt, ...);
#define klog_debug_plus(fmt, args...) \
    _klog_write_file(KLOG_DEBUG, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#define klog_info_plus(fmt, args...) \
    _klog_write_file(KLOG_INFO, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#define klog_notice_plus(fmt, args...) \
    _klog_write_file(KLOG_NOTICE, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#define klog_warningv(fmt, args...) \
    _klog_write_file(KLOG_WARNING, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#define klog_error_plus(fmt, args...) \
    _klog_write_file(KLOG_ERROR, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#define klog_crit_plus(fmt, args...) \
    _klog_write_file(KLOG_CRIT, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#define klog_alert_plus(fmt, args...) \
    _klog_write_file(KLOG_ALERT, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#define klog_emerg_plus(fmt, args...) \
    _klog_write_file(KLOG_EMERG, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)

// -- Use custom log file ---
void klog_open_file(const char *file, const char *ident, int level, char *sid);
#define klog_debug_file(fmt, args...) \
    _klog_write_file(KLOG_DEBUG, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#define klog_info_file(fmt, args...) \
    _klog_write_file(KLOG_INFO, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#define klog_notice(fmt, args...) \
    _klog_write(KLOG_NOTICE, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#define klog_warning_file(fmt, args...) \
    _klog_write_file(KLOG_WARNING, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#define klog_error_file(fmt, args...) \
    _klog_write_file(KLOG_ERROR, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#define klog_crit_file(fmt, args...) \
    _klog_write_file(KLOG_CRIT, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#define klog_alert(fmt, args...) \
    _klog_write(KLOG_ALERT, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#define klog_emerg_file(fmt, args...) \
    _klog_write_file(KLOG_EMERG, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)

#endif