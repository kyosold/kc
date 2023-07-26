#ifndef __KLOG_H__
#define __KLOG_H__

#include <syslog.h>
#include <signal.h>

#define KLOG_DEBUG 7
#define KLOG_INFO 6
#define KLOG_NOTICE 5
#define KLOG_WARNING 4
#define KLOG_ERROR 3
#define KLOG_CRIT 2
#define KLOG_ALERT 1
#define KLOG_EMERG 0

#define KLOG_MAX_LINE 4096

volatile __sig_atomic_t klog_type; // 0:_klog_write 1:_klog_write_file

void klog_set_level(int level);
void klog_set_sid(char *sid);

// --- Use syslog of linux ---
void klog_open(const char *ident, int facility, int level, char *sid);
// -- Use custom log file ---
void klog_open_file(const char *file, const char *ident, int level, char *sid);

#define klog_debug(fmt, args...)                                                     \
    if (klog_type == 0)                                                              \
    {                                                                                \
        _klog_write(KLOG_DEBUG, __FILE__, __LINE__, __FUNCTION__, fmt, ##args);      \
    }                                                                                \
    else                                                                             \
    {                                                                                \
        _klog_write_file(KLOG_DEBUG, __FILE__, __LINE__, __FUNCTION__, fmt, ##args); \
    }

#define klog_info(fmt, args...)                                                     \
    if (klog_type == 0)                                                             \
    {                                                                               \
        _klog_write(KLOG_INFO, __FILE__, __LINE__, __FUNCTION__, fmt, ##args);      \
    }                                                                               \
    else                                                                            \
    {                                                                               \
        _klog_write_file(KLOG_INFO, __FILE__, __LINE__, __FUNCTION__, fmt, ##args); \
    }

#define klog_notice(fmt, args...)                                                     \
    if (klog_type == 0)                                                               \
    {                                                                                 \
        _klog_write(KLOG_NOTICE, __FILE__, __LINE__, __FUNCTION__, fmt, ##args);      \
    }                                                                                 \
    else                                                                              \
    {                                                                                 \
        _klog_write_file(KLOG_NOTICE, __FILE__, __LINE__, __FUNCTION__, fmt, ##args); \
    }

#define klog_warning(fmt, args...)                                                     \
    if (klog_type == 0)                                                                \
    {                                                                                  \
        _klog_write(KLOG_WARNING, __FILE__, __LINE__, __FUNCTION__, fmt, ##args);      \
    }                                                                                  \
    else                                                                               \
    {                                                                                  \
        _klog_write_file(KLOG_WARNING, __FILE__, __LINE__, __FUNCTION__, fmt, ##args); \
    }

#define klog_error(fmt, args...)                                                     \
    if (klog_type == 0)                                                              \
    {                                                                                \
        _klog_write(KLOG_ERROR, __FILE__, __LINE__, __FUNCTION__, fmt, ##args);      \
    }                                                                                \
    else                                                                             \
    {                                                                                \
        _klog_write_file(KLOG_ERROR, __FILE__, __LINE__, __FUNCTION__, fmt, ##args); \
    }

#define klog_crit(fmt, args...)                                                     \
    if (klog_type == 0)                                                             \
    {                                                                               \
        _klog_write(KLOG_CRIT, __FILE__, __LINE__, __FUNCTION__, fmt, ##args);      \
    }                                                                               \
    else                                                                            \
    {                                                                               \
        _klog_write_file(KLOG_CRIT, __FILE__, __LINE__, __FUNCTION__, fmt, ##args); \
    }

#define klog_alert(fmt, args...)                                                     \
    if (klog_type == 0)                                                              \
    {                                                                                \
        _klog_write(KLOG_ALERT, __FILE__, __LINE__, __FUNCTION__, fmt, ##args);      \
    }                                                                                \
    else                                                                             \
    {                                                                                \
        _klog_write_file(KLOG_ALERT, __FILE__, __LINE__, __FUNCTION__, fmt, ##args); \
    }

#define klog_emerg(fmt, args...)                                                     \
    if (klog_type == 0)                                                              \
    {                                                                                \
        _klog_write(KLOG_EMERG, __FILE__, __LINE__, __FUNCTION__, fmt, ##args);      \
    }                                                                                \
    else                                                                             \
    {                                                                                \
        _klog_write_file(KLOG_EMERG, __FILE__, __LINE__, __FUNCTION__, fmt, ##args); \
    }

#endif
