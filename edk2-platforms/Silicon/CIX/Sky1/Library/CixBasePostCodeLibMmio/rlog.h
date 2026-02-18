#ifndef __RAM_LOG_PRINTF__
#define __RAM_LOG_PRINTF__
//  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
typedef void (*LOCKFN)(void);
typedef unsigned long long  (*GETTICK)(void);
enum LOG_LEVEL
{
    LOGLEVEL_MIN = 1,
    LOGLEVEL_ERR = LOGLEVEL_MIN,	/* error conditions */
    LOGLEVEL_WARNING,	/* warning conditions */
    LOGLEVEL_INFO,	/* informational */
    LOGLEVEL_DEBUG,	/* debug-level messages */
    LOGLEVEL_MAX
};
void rlog_init_printf(char* p, unsigned int size);
void rlog_set_sys_tick(void* get_tick);
void rlog_set_lock_unlock(void* lock, void* unlock);
void rlog_set_log_level(enum LOG_LEVEL level);
void rlog_printf(enum LOG_LEVEL level, const char* fmt, ...);
#endif
