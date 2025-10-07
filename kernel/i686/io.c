#include "include/io.h"

void i686_iowait()
{
    outb(PORT_UNUSED, 0);
}