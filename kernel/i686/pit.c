#include "include/pit.h"
#include "include/io.h"
#include "include/irq.h"
#include "../../libc/include/stdio.h"

#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40
#define PIT_FREQ 1193182U

void i686_pit_init(uint32_t hz) {
    uint32_t divisor = PIT_FREQ / hz;
    i686_outb(PIT_COMMAND, 0x36);            /* channel 0, lobyte/hibyte, mode 3 */
    i686_outb(PIT_CHANNEL0, divisor & 0xFF);
    i686_outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}

static void pit_callback(registers_t* regs) {
    static uint32_t ticks = 0;
    ticks++;
    if ((ticks % 18) == 0) printf("."); /* diagnostic */
}

void timer_init(void) {
    i686_pit_init(18);               /* ~18.2 Hz */
    i686_irq_registerhandler(0, pit_callback); /* IRQ0 */
}