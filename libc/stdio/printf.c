#include "../include/limits.h"
#include "../include/stdarg.h"
#include "../include/stdbool.h"
#include "../include/string.h"
#include "../include/stdio.h"
#include "../../kernel/i686/include/io.h"

static char stdio_putc(const char c)
{
    while (!((i686_inb(0x3f8 + 5)) & 0x20));
    i686_outb(0x3f8, c);
    return c;
}
static bool print(const char* data) {
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < strlen(data); i++)
		if (stdio_putc(bytes[i]) == EOF)
			return false;
	return true;
}

const char g_HexChars[] = "0123456789abcdef";

void printf_unsigned(uint32_t number, int radix)
{
    char buffer[32];
    int pos = 0;

    // convert number to ASCII
    do 
    {
        
        uint32_t rem = number % radix;
        number /= radix;
        buffer[pos++] = g_HexChars[rem];
        
    } while (number > 0);

    // print number in reverse order
    while (--pos >= 0)
        stdio_putc(buffer[pos]);
}

void printf_signed(long long number, int radix)
{
    if (number < 0)
    {
        stdio_putc('-');
        printf_unsigned(-number, radix);
    }
    else printf_unsigned(number, radix);
}

#define PRINTF_STATE_NORMAL         0
#define PRINTF_STATE_LENGTH         1
#define PRINTF_STATE_LENGTH_SHORT   2
#define PRINTF_STATE_LENGTH_LONG    3
#define PRINTF_STATE_SPEC           4

#define PRINTF_LENGTH_DEFAULT       0
#define PRINTF_LENGTH_SHORT_SHORT   1
#define PRINTF_LENGTH_SHORT         2
#define PRINTF_LENGTH_LONG          3
#define PRINTF_LENGTH_LONG_LONG     4

void printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int state = PRINTF_STATE_NORMAL;
    int length = PRINTF_LENGTH_DEFAULT;
    int radix = 10;
    bool sign = false;
    bool number = false;

    while (*fmt)
    {
        switch (state)
        {
            case PRINTF_STATE_NORMAL:
                switch (*fmt)
                {
                    case '%':   state = PRINTF_STATE_LENGTH;
                                break;
                    default:    stdio_putc(*fmt);
                                break;
                }
                break;

            case PRINTF_STATE_LENGTH:
                switch (*fmt)
                {
                    case 'h':   length = PRINTF_LENGTH_SHORT;
                                state = PRINTF_STATE_LENGTH_SHORT;
                                break;
                    case 'l':   length = PRINTF_LENGTH_LONG;
                                state = PRINTF_STATE_LENGTH_LONG;
                                break;
                    default:    goto PRINTF_STATE_SPEC_;
                }
                break;

            case PRINTF_STATE_LENGTH_SHORT:
                if (*fmt == 'h')
                {
                    length = PRINTF_LENGTH_SHORT_SHORT;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;

            case PRINTF_STATE_LENGTH_LONG:
                if (*fmt == 'l')
                {
                    length = PRINTF_LENGTH_LONG_LONG;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;

            case PRINTF_STATE_SPEC:
            PRINTF_STATE_SPEC_:
                switch (*fmt)
                {
                    case 'c':   stdio_putc((char)va_arg(args, int));
                                break;

                    case 's':   
                                print(va_arg(args, const char*));
                                break;

                    case '%':   stdio_putc('%');
                                break;

                    case 'd':
                    case 'i':   radix = 10; sign = true; number = true;
                                break;

                    case 'u':   radix = 10; sign = false; number = true;
                                break;

                    case 'X':
                    case 'x':
                    case 'p':   radix = 16; sign = false; number = true;
                                break;

                    case 'o':   radix = 8; sign = false; number = true;
                                break;

                    // ignore invalid spec
                    default:    break;
                }

                if (number)
                {
                    if (sign)
                    {
                        switch (length)
                        {
                        case PRINTF_LENGTH_SHORT_SHORT:
                        case PRINTF_LENGTH_SHORT:
                        case PRINTF_LENGTH_DEFAULT:     printf_signed(va_arg(args, int), radix);
                                                        break;

                        case PRINTF_LENGTH_LONG:        printf_signed(va_arg(args, long), radix);
                                                        break;

                        case PRINTF_LENGTH_LONG_LONG:   printf_signed(va_arg(args, long long), radix);
                                                        break;
                        }
                    }
                    else
                    {
                        switch (length)
                        {
                        case PRINTF_LENGTH_SHORT_SHORT:
                        case PRINTF_LENGTH_SHORT:
                        case PRINTF_LENGTH_DEFAULT:     printf_unsigned(va_arg(args, unsigned int), radix);
                                                        break;
                                                        
                        case PRINTF_LENGTH_LONG:        printf_unsigned(va_arg(args, unsigned  long), radix);
                                                        break;

                        case PRINTF_LENGTH_LONG_LONG:   printf_unsigned(va_arg(args, unsigned  long long), radix);
                                                        break;
                        }
                    }
                }

                // reset state
                state = PRINTF_STATE_NORMAL;
                length = PRINTF_LENGTH_DEFAULT;
                radix = 10;
                sign = false;
                number = false;
                break;
        }

        fmt++;
    }

    va_end(args);
}

void print_buffer(const char* msg, const void* buffer, uint32_t count)
{
    const uint8_t* u8Buffer = (const uint8_t*)buffer;
    
    print(msg);
    for (uint16_t i = 0; i < count; i++)
    {
        stdio_putc(g_HexChars[u8Buffer[i] >> 4]);
        stdio_putc(g_HexChars[u8Buffer[i] & 0xF]);
    }
    print("\n");
}

// int printf(const char* restrict format, ...) {
// 	va_list parameters;
// 	va_start(parameters, format);

// 	int written = 0;

// 	while (*format != '\0') {
// 		size_t maxrem = INT_MAX - written;

// 		if (format[0] != '%' || format[1] == '%') {
// 			if (format[0] == '%')
// 				format++;
// 			size_t amount = 1;
// 			while (format[amount] && format[amount] != '%')
// 				amount++;
// 			if (maxrem < amount) {
// 				// TODO: Set errno to EOVERFLOW.
// 				return -1;
// 			}
// 			if (!print(format, amount))
// 				return -1;
// 			format += amount;
// 			written += amount;
// 			continue;
// 		}

// 		const char* format_begun_at = format++;

// 		if (*format == 'c') {
// 			format++;
// 			char c = (char) va_arg(parameters, int /* char promotes to int */);
// 			if (!maxrem) {
// 				// TODO: Set errno to EOVERFLOW.
// 				return -1;
// 			}
// 			if (!print(&c, sizeof(c)))
// 				return -1;
// 			written++;
// 		} else if (*format == 's') {
// 			format++;
// 			const char* str = va_arg(parameters, const char*);
// 			size_t len = strlen(str);
// 			if (maxrem < len) {
// 				// TODO: Set errno to EOVERFLOW.
// 				return -1;
// 			}
// 			if (!print(str, len))
// 				return -1;
// 			written += len;
// 		} else if (*format == 'd') {
// 			format++;
// 			int value = va_arg(parameters, int);

// 			char buffer[32] = {0};  // enough for 32-bit int
// 			char* ptr = buffer + sizeof(buffer) - 1;
// 			*ptr = '\0';

// 			bool negative = value < 0;
// 			unsigned int uval = negative ? -value : value;

// 			do {
// 				*--ptr = '0' + (uval % 10);
// 				uval /= 10;
// 			} while (uval > 0);

// 			if (negative)
// 				*--ptr = '-';

// 			size_t len = (buffer + sizeof(buffer) - 1) - ptr;
// 			if (maxrem < len)
// 				return -1;

// 			if (!print(ptr, len))
// 				return -1;

// 			written += len;
// 		} else {
// 			format = format_begun_at;
// 			size_t len = strlen(format);
// 			if (maxrem < len) {
// 				// TODO: Set errno to EOVERFLOW.
// 				return -1;
// 			}
// 			if (!print(format, len))
// 				return -1;
// 			written += len;
// 			format += len;
// 		}
// 	}

// 	va_end(parameters);
// 	return written;
// }
