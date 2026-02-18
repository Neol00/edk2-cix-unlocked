//#include <stdarg.h>
//#include <stddef.h>
//  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
#include <Library/rlog.h>
#include <Base.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#define size_t UINTN
#define va_start VA_START
#define va_end VA_END
#define va_list VA_LIST
#define va_arg VA_ARG
typedef struct {
  UINT32  g_ram_cur_pos;
  UINT32  Reserved0;
  UINT64  Reserved1;
  UINT64  Reserved2;
  UINT64  Reserved3;
} RAM_LOG_MAIL_BOX;
static char* g_ram_start_Addr = (char*)FixedPcdGet64 (PcdRamLogBaseAddress) + sizeof(RAM_LOG_MAIL_BOX);
static unsigned int g_ram_size = FixedPcdGet32 (PcdRamLogSize);
static unsigned int g_ram_cur_pos;
static unsigned char g_rlog_init_state = 1;
static LOCKFN g_lock_fn;
static LOCKFN g_unlock_fn;
static GETTICK g_get_tick_fn;
static enum LOG_LEVEL g_log_level = LOGLEVEL_INFO;
/*
 * Implementation
 */
struct param {
    char lz : 1;          /**<  Leading zeros */
    char alt : 1;         /**<  alternate form */
    char uc : 1;          /**<  Upper case (for base16 only) */
    char align_left : 1;  /**<  0 == align right (default), 1 == align left */
    unsigned int width; /**<  field width */
    char sign;          /**<  The sign to display (if any) */
    unsigned int base;  /**<  number base (e.g.: 8, 10, 16) */
    char* bf;           /**<  Buffer to output */
};
#ifdef PLAT_SUPPORT_64BIT_DIV
static void ulli2a(unsigned long long int num, struct param* p)
#else
static void ulli2a(unsigned long int num, struct param* p)
#endif
{
    int n = 0;
#ifdef PLAT_SUPPORT_64BIT_DIV
    unsigned long long int d = 1;
#else
    unsigned long int d = 1;
#endif
    char* bf = p->bf;
    while (num / d >= p->base)
        d *= p->base;
    while (d != 0) {
        int dgt = num / d;
        num %= d;
        d /= p->base;
        if (n || dgt > 0 || d == 0) {
            *bf++ = dgt + (dgt < 10 ? '0' : (p->uc ? 'A' : 'a') - 10);
            ++n;
        }
    }
    *bf = 0;
}
static void lli2a(long long int num, struct param* p)
{
    if (num < 0) {
        num = -num;
        p->sign = '-';
    }
    ulli2a(num, p);
}
static void uli2a(unsigned long int num, struct param* p)
{
    int n = 0;
    unsigned long int d = 1;
    char* bf = p->bf;
    while (num / d >= p->base)
        d *= p->base;
    while (d != 0) {
        int dgt = num / d;
        num %= d;
        d /= p->base;
        if (n || dgt > 0 || d == 0) {
            *bf++ = dgt + (dgt < 10 ? '0' : (p->uc ? 'A' : 'a') - 10);
            ++n;
        }
    }
    *bf = 0;
}
static void li2a(long num, struct param* p)
{
    if (num < 0) {
        num = -num;
        p->sign = '-';
    }
    uli2a(num, p);
}
static void ui2a(unsigned int num, struct param* p)
{
    int n = 0;
    unsigned int d = 1;
    char* bf = p->bf;
    while (num / d >= p->base)
        d *= p->base;
    while (d != 0) {
        int dgt = num / d;
        num %= d;
        d /= p->base;
        if (n || dgt > 0 || d == 0) {
            *bf++ = dgt + (dgt < 10 ? '0' : (p->uc ? 'A' : 'a') - 10);
            ++n;
        }
    }
    *bf = 0;
}
static void i2a(int num, struct param* p)
{
    if (num < 0) {
        num = -num;
        p->sign = '-';
    }
    ui2a(num, p);
}
static int a2d(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    else if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    else if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    else
        return -1;
}
static char a2u(char ch, const char** src, int base, unsigned int* nump)
{
    const char* p = *src;
    unsigned int num = 0;
    int digit;
    while ((digit = a2d(ch)) >= 0) {
        if (digit > base)
            break;
        num = num * base + digit;
        ch = *p++;
    }
    *src = p;
    *nump = num;
    return ch;
}
static void putc(char c)
{
    *(g_ram_start_Addr + g_ram_cur_pos++) = c;
    if (g_ram_cur_pos >= g_ram_size)
        g_ram_cur_pos = 0;
}
static void putchw(struct param* p)
{
    char ch;
    int n = p->width;
    char* bf = p->bf;
    /* Number of filling characters */
    while (*bf++ && n > 0)
        n--;
    if (p->sign)
        n--;
    if (p->alt && p->base == 16)
        n -= 2;
    else if (p->alt && p->base == 8)
        n--;
    /* Fill with space to align to the right, before alternate or sign */
    if (!p->lz && !p->align_left) {
        while (n-- > 0)
            putc(' ');
    }
    /* print sign */
    if (p->sign)
        putc(p->sign);
    /* Alternate */
    if (p->alt && p->base == 16) {
        putc('0');
        putc((p->uc ? 'X' : 'x'));
    }
    else if (p->alt && p->base == 8) {
        putc('0');
    }
    /* Fill with zeros, after alternate or sign */
    if (p->lz) {
        while (n-- > 0)
            putc('0');
    }
    /* Put actual buffer */
    bf = p->bf;
    while ((ch = *bf++))
        putc(ch);
    /* Fill with space to align to the left, after string */
    if (!p->lz && p->align_left) {
        while (n-- > 0)
            putc(' ');
    }
}
static void putstr(char* str)
{
    unsigned i = 0;
    while (str[i]) {
        putc(str[i]);
        i++;
    }
}
static void add_print_time()
{
    unsigned long long tick = 0;
    struct param p;
    char bf[23];
    p.bf = bf;
    if (g_get_tick_fn)
        tick = g_get_tick_fn();
    putstr("\"time\":\"");
    p.base = 16;
    p.alt = 1;
    p.lz = 0;
    p.width = 0;
    p.sign = 0;
    p.uc = 0;
    ulli2a(tick, &p);
    putchw(&p);
    putstr("\",");
}
static void add_print_level(unsigned char level)
{
    putstr("{\"level\":");
    switch (level)
    {
    case LOGLEVEL_ERR:
        putstr("\"ERR\",");
        break;
    case LOGLEVEL_WARNING:
        putstr("\"WARN\",");
        break;
    case LOGLEVEL_INFO:
        putstr("\"INFO\",");
        break;
    case LOGLEVEL_DEBUG:
        putstr("\"DBG\",");
        break;
    default:
        putstr("\"UNK\",");
        break;
    }
}
static void add_print_end()
{
    putc('}');
    putc(';');
}
static void rlog_format(const char* fmt, va_list va)
{
    struct param p;
    char bf[23];
    char ch;
    p.bf = bf;
    while ((ch = *(fmt++))) {
        if (ch != '%') {
            putc(ch);
        }
        else {
            char lng = 0;  /* 1 for long, 2 for long long */
            /* Init parameter struct */
            p.lz = 0;
            p.alt = 0;
            p.width = 0;
            p.align_left = 0;
            p.sign = 0;
            /* Flags */
            while ((ch = *(fmt++))) {
                switch (ch) {
                case '-':
                    p.align_left = 1;
                    continue;
                case '0':
                    p.lz = 1;
                    continue;
                case '#':
                    p.alt = 1;
                    continue;
                default:
                    break;
                }
                break;
            }
            /* Width */
            if (ch >= '0' && ch <= '9') {
                ch = a2u(ch, &fmt, 10, &(p.width));
            }
            /* We accept 'x.y' format but don't support it completely:
             * we ignore the 'y' digit => this ignores 0-fill
             * size and makes it == width (ie. 'x') */
            if (ch == '.') {
                p.lz = 1;  /* zero-padding */
                /* ignore actual 0-fill size: */
                do {
                    ch = *(fmt++);
                } while ((ch >= '0') && (ch <= '9'));
            }
            if (ch == 'z') {
                ch = *(fmt++);
                if (sizeof(size_t) == sizeof(unsigned long int))
                    lng = 1;
                else if (sizeof(size_t) == sizeof(unsigned long long int))
                    lng = 2;
            }
            else if (ch == 'l') {
                ch = *(fmt++);
                lng = 1;
                if (ch == 'l') {
                    ch = *(fmt++);
                    lng = 2;
                }
            }
            switch (ch) {
            case 0:
                goto exit1;
            case 'u':
                p.base = 10;
                if (2 == lng)
                    ulli2a(va_arg(va, unsigned long long int), &p);
                else if (1 == lng)
                    uli2a(va_arg(va, unsigned long int), &p);
                else
                    ui2a(va_arg(va, unsigned int), &p);
                putchw(&p);
                break;
            case 'd':
            case 'i':
                p.base = 10;
                if (2 == lng)
                    lli2a(va_arg(va, long long int), &p);
                else if (1 == lng)
                    li2a(va_arg(va, long int), &p);
                else
                    i2a(va_arg(va, int), &p);
                putchw(&p);
                break;
            case 'p':
                p.alt = 1;
                lng = 2;
            case 'x':
            case 'X':
                p.base = 16;
                p.uc = (ch == 'X') ? 1 : 0;
                if (2 == lng)
                    ulli2a(va_arg(va, unsigned long long int), &p);
                else if (1 == lng)
                    uli2a(va_arg(va, unsigned long int), &p);
                else
                    ui2a(va_arg(va, unsigned int), &p);
                putchw(&p);
                break;
            case 'o':
                p.base = 8;
                ui2a(va_arg(va, unsigned int), &p);
                putchw(&p);
                break;
            case 'c':
                putc((char)(va_arg(va, int)));
                break;
            case 's':
                p.bf = va_arg(va, char*);
                putchw(&p);
                p.bf = bf;
                break;
            case '%':
                putc(ch);
            default:
                break;
            }
        }
    }
exit1:;
}
void rlog_set_sys_tick(void* get_tick)
{
    g_get_tick_fn = get_tick;
}
void rlog_set_lock_unlock(void* lock, void* unlock)
{
    g_lock_fn = lock;
    g_unlock_fn = unlock;
}
void rlog_set_log_level(enum LOG_LEVEL level)
{
    g_log_level = level;
}
unsigned int GetCurPos()
{
    RAM_LOG_MAIL_BOX *pRamLogMailBox;
    pRamLogMailBox = (RAM_LOG_MAIL_BOX*)FixedPcdGet64 (PcdRamLogBaseAddress);
    return pRamLogMailBox->g_ram_cur_pos;
}
void SetCurPos(unsigned int g_ram_cur_pos)
{
    RAM_LOG_MAIL_BOX *pRamLogMailBox;
    pRamLogMailBox = (RAM_LOG_MAIL_BOX*)FixedPcdGet64 (PcdRamLogBaseAddress);
    pRamLogMailBox->g_ram_cur_pos = g_ram_cur_pos;
}
void rlog_init_printf(char* buf, unsigned int size)
{
    g_ram_start_Addr = buf + sizeof(RAM_LOG_MAIL_BOX);
    g_ram_size = size;
    g_ram_cur_pos = 0;
    g_get_tick_fn = NULL;
    g_lock_fn = NULL;
    g_unlock_fn = NULL;
    g_log_level = LOGLEVEL_INFO;
    g_rlog_init_state = 1;
    SetCurPos(0);
}
void rlog_printf(enum LOG_LEVEL level, const char* fmt, ...)
{
    va_list va;
    g_ram_cur_pos = GetCurPos();
    if (!g_rlog_init_state || g_log_level < level)
        return;
    va_start(va, fmt);
    if (g_lock_fn)
        g_lock_fn();
    add_print_level(level);
    add_print_time();
    putstr("\"log\":\"");
    rlog_format(fmt, va);
    add_print_end();
    if (g_unlock_fn)
        g_unlock_fn();
    va_end(va);
    SetCurPos(g_ram_cur_pos);
}
