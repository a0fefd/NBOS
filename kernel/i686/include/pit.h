#include "includestdint.h"

#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40
#define PIT_FREQ 1193182U

void i686_pit_init(uint32_t hz);
void timer_init(void);