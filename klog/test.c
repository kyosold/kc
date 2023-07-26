#include <stdio.h>
#include "klog.h"

int main(int argc, char **argv)
{
    klog_open("test_klog", LOG_MAIL, KLOG_DEBUG, "FFFFFF");
    klog_debug("name:%s", "kyosold");

    klog_set_sid("AF99991");
    klog_set_level(KLOG_INFO);
    klog_debug("show not this debug:%d", 1);
    klog_info("show this info:%s", "succ");
    klog_error("show this error:%s", "succ");

    // --- file ---
    if (argc > 1)
    {
        klog_open_file(argv[1], "test_klog", KLOG_DEBUG, "FF00FF");
        klog_debug("name:%s", "kyosold");

        klog_set_sid("F000001");
        klog_set_level(KLOG_INFO);
        klog_debug("show not this debug:%d", 1);
        klog_info("show this info:%s", "succ");
        klog_error("show this error:%s", "succ");
    }

    return 0;
}
