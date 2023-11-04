#include "include/utils.h"
#include "include/type.h"
#include "include/io.h"
#include "include/variable_args.h"
#include "include/stdio.h"
#include "include/log.h"
#include "include/console.h"

// string utils ------
size_t length(const char* str) {
    size_t len = 0;
    while (str[len]) {
        len++;
    }
    return len;
}

void concat(char* s1, char* s2) {
    size_t len = length(s1);
    char* ptr = s1 + len;
    char* q = s2;
    while (q != NULL) {
        *ptr++ = *q++;
    }
}

int is_digit(char c) {
    return c >= '0' && c <= '9';
}
// ------ string utils end

// print utils ------
#define ZEROPAD 0x01 // 填充零
#define SIGN 0x02    // unsigned/signed long
#define PLUS 0x04    // 显示加
#define SPACE 0x08   // 如是加，则置空格
#define LEFT 0x10    // 左调整
#define SPECIAL 0x20 // 0x
#define SMALL 0x40   // 使用小写字母
#define DOUBLE 0x80  // 浮点数

// 将字符数字串转换成整数，并将指针前移
static int skip_atoi(const char **s)
{
    int i = 0;
    while (is_digit(**s))
        i = i * 10 + *((*s)++) - '0';
    return i;
}

static char *number(char *str, u32 *num, int base, int size, int precision, int flags)
{
    char pad, sign, tmp[36];
    const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int i;
    int index;
    char *ptr = str;

    if (flags & SMALL)
        digits = "0123456789abcdefghijklmnopqrstuvwxyz";

    if (flags & LEFT)
        flags &= ~ZEROPAD;

    if (base < 2 || base > 36)
        return 0;

    pad = (flags & ZEROPAD) ? '0' : ' ';

    if (flags & DOUBLE && (*(double *)(num)) < 0)
    {
        sign = '-';
        *(double *)(num) = -(*(double *)(num));
    }
    else if (flags & SIGN && !(flags & DOUBLE) && ((int)(*num)) < 0)
    {
        sign = '-';
        (*num) = -(int)(*num);
    }
    else
        sign = (flags & PLUS) ? '+' : ((flags & SPACE) ? ' ' : 0);

    if (sign)
        size--;

    if (flags & SPECIAL)
    {
        if (base == 16)
            size -= 2;
        else if (base == 8)
            size--;
    }

    i = 0;

    if (flags & DOUBLE)
    {
        u32 ival = (u32)(*(double *)num);
        u32 fval = (u32)(((*(double *)num) - ival) * 1000000);
        do
        {
            index = (fval) % base;
            (fval) /= base;
            tmp[i++] = digits[index];
        } while (fval);
        tmp[i++] = '.';

        do
        {
            index = (ival) % base;
            (ival) /= base;
            tmp[i++] = digits[index];
        } while (ival);
    }
    else if ((*num) == 0)
    {
        tmp[i++] = '0';
    }
    else
    {
        while ((*num) != 0)
        {
            index = (*num) % base;
            (*num) /= base;
            tmp[i++] = digits[index];
        }
    }

    if (i > precision)
        precision = i;

    size -= precision;

    if (!(flags & (ZEROPAD + LEFT)))
        while (size-- > 0)
            *str++ = ' ';

    if (sign)
        *str++ = sign;

    if (flags & SPECIAL)
    {
        if (base == 8)
            *str++ = '0';
        else if (base == 16)
        {
            *str++ = '0';
            *str++ = digits[33];
        }
    }
    if (!(flags & LEFT))
        while (size-- > 0)
            *str++ = pad;

    while (i < precision--)
        *str++ = '0';

    while (i-- > 0)
        *str++ = tmp[i];

    while (size-- > 0)
        *str++ = ' ';
    return str;
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
    int len;
    int i;

    char *str;
    char *s;
    int *ip;

    int flags;

    int field_width;
    int precision;
    int qualifier;
    u32 num;
    u8 *ptr;

    for (str = buf; *fmt; ++fmt)
    {
        if (*fmt != '%')
        {
            *str++ = *fmt;
            continue;
        }

        flags = 0;
    repeat:
        ++fmt;
        switch (*fmt)
        {
        case '-':
            flags |= LEFT;
            goto repeat;
        case '+':
            flags |= PLUS;
            goto repeat;
        case ' ':
            flags |= SPACE;
            goto repeat;
        case '#':
            flags |= SPECIAL;
            goto repeat;
        case '0':
            flags |= ZEROPAD;
            goto repeat;
        }

        field_width = -1;

        if (is_digit(*fmt))
            field_width = skip_atoi(&fmt);

        else if (*fmt == '*')
        {
            ++fmt;
            field_width = va_arg(args, int);

            if (field_width < 0)
            {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        precision = -1;

        if (*fmt == '.')
        {
            ++fmt;
            if (is_digit(*fmt))
                precision = skip_atoi(&fmt);

            else if (*fmt == '*')
            {
                precision = va_arg(args, int);
            }
            if (precision < 0)
                precision = 0;
        }

        // 下面这段代码分析长度修饰符，并将其存入 qualifer 变量
        qualifier = -1;
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L')
        {
            qualifier = *fmt;
            ++fmt;
        }

        switch (*fmt)
        {

        case 'c':
            if (!(flags & LEFT))
                while (--field_width > 0)
                    *str++ = ' ';
            *str++ = (unsigned char)va_arg(args, int);
            while (--field_width > 0)
                *str++ = ' ';
            break;

        case 's':
            s = va_arg(args, char *);
            len = length(s);
            if (precision < 0)
                precision = len;
            else if (len > precision)
                len = precision;

            if (!(flags & LEFT))
                while (len < field_width--)
                    *str++ = ' ';
            for (i = 0; i < len; ++i)
                *str++ = *s++;
            while (len < field_width--)
                *str++ = ' ';
            break;

        case 'o':
            num = va_arg(args, unsigned long);
            str = number(str, &num, 8, field_width, precision, flags);
            break;

        case 'p':
            if (field_width == -1)
            {
                field_width = 8;
                flags |= ZEROPAD;
            }
            num = va_arg(args, unsigned long);
            str = number(str, &num, 16, field_width, precision, flags);
            break;

        case 'x':
            flags |= SMALL;
        case 'X':
            num = va_arg(args, unsigned long);
            str = number(str, &num, 16, field_width, precision, flags);
            break;

        case 'd':
        case 'i':
            flags |= SIGN;
        case 'u':
            num = va_arg(args, unsigned long);
            str = number(str, &num, 10, field_width, precision, flags);
            break;

        case 'n':
            ip = va_arg(args, int *);
            *ip = (str - buf);
            break;
        case 'f':
            flags |= SIGN;
            flags |= DOUBLE;
            double dnum = va_arg(args, double);
            str = number(str, (u32 *)&dnum, 10, field_width, precision, flags);
            break;
        case 'b': // binary
            num = va_arg(args, unsigned long);
            str = number(str, &num, 2, field_width, precision, flags);
            break;
        case 'm': // mac address
            flags |= SMALL | ZEROPAD;
            ptr = va_arg(args, char *);
            for (size_t t = 0; t < 6; t++, ptr++)
            {
                int num = *ptr;
                str = number(str, &num, 16, 2, precision, flags);
                *str = ':';
                str++;
            }
            str--;
            break;
        case 'r': // ip address
            flags |= SMALL;
            ptr = va_arg(args, u8 *);
            for (size_t t = 0; t < 4; t++, ptr++)
            {
                int num = *ptr;
                str = number(str, &num, 10, field_width, precision, flags);
                *str = '.';
                str++;
            }
            str--;
            break;
        default:
            if (*fmt != '%')
                *str++ = '%';
            if (*fmt)
                *str++ = *fmt;
            else
                --fmt;
            break;
        }
    }
    *str = '\0';

    i = str - buf;
    assert(i < 1024);
    return i;
}

int sprintf(char *buf, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int i = vsprintf(buf, fmt, args);
    va_end(args);
    return i;
}
    
int printf(const char *fmt, ...)
{
    char buf[1024];
    va_list args;
    int i;

    va_start(args, fmt);

    i = vsprintf(buf, fmt, args);

    va_end(args);

    console_write(buf, i);

    return i;
}

int println(const char *fmt, ...)
{
    char buf[1024];
    va_list args;
    int i;

    va_start(args, fmt);

    i = vsprintf(buf, fmt, args);

    va_end(args);

    console_write(buf, i);
    console_write("\n", 1);

    return i;
}

int println_with_color(u8 color, const char *fmt, ...)
{
    char buf[1024];
    va_list args;
    int i;

    va_start(args, fmt);

    i = vsprintf(buf, fmt, args);

    va_end(args);

    console_write_with_color(buf, i, color);
    console_write("\n", 1);

    return i;
}

int print_with_color(u8 color, const char *fmt, ...)
{
    char buf[1024];
    va_list args;
    int i;

    va_start(args, fmt);

    i = vsprintf(buf, fmt, args);

    va_end(args);

    console_write_with_color(buf, i, color);

    return i;
}
// ------ print utils end

// assert utils ------
void assertion_failure(char *exp, char *file, char *base, int line)
{
    printf("Assertion: %s failed!, %s:%d\n", exp, file, line);

    while (1);
}

void panic(const char *fmt, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    int i = vsprintf(buf, fmt, args);
    va_end(args);

    printf("PANIC: %s \n", buf);
}
// ------ assert utils end

// log utils ------
void debug_info(const char *file, int line, const char *fmt, ...) {
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);

    println_with_color(CYAN, "[DEBUG] %s:%d %s", file, line, buf);
}

void trace_info(const char *file, int line, const char *fmt, ...) {
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);

    println_with_color(GRAY, "[TRACE] %s:%d %s", file, line, buf);
}

void info_info(const char *file, int line, const char *fmt, ...) {
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);

    println_with_color(GREEN, "[INFO] %s:%d %s", file, line, buf);
}

void warn_info(const char *file, int line, const char *fmt, ...) {
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);

    println_with_color(YELLOW, "[WARN] %s:%d %s", file, line, buf);
}

void error_info(const char *file, int line, const char *fmt, ...) {
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);

    println_with_color(RED, "[DEBUG] %s:%d %s", file, line, buf);
}
// ------ log utils end

void memcpy(void* dst, const void* src, size_t n) {
    u8* d = (u8*)dst;
    const u8* s = (const u8*)src;
    while (n--) {
        *d++ = *s++;
    }
}