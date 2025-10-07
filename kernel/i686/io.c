#include "include/io.h"

void i686_iowait()
{
    i686_outb(PORT_UNUSED, 0);
}