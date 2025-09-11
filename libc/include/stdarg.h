#ifndef _STDARG_H
#define _STDARG_H

typedef char* va_list;
#define _VA_ALIGN(t) (((sizeof(t) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))
#define va_start(ap, last_named) (ap = (va_list)&last_named + _VA_ALIGN(last_named))
#define va_arg(ap, type) (*(type *)((ap += _VA_ALIGN(type)) - _VA_ALIGN(type)))
#define va_end(ap) (ap = (va_list)0)

#endif