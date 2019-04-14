#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdint.h>

#include "common.h"
#include "rpi3bplus_common.h"

#define GPFSEL0         ((volatile uint32_t*) (MMIO_BASE + 0x00200000))
#define GPFSEL1         ((volatile uint32_t*) (MMIO_BASE + 0x00200004))
#define GPFSEL2         ((volatile uint32_t*) (MMIO_BASE + 0x00200008))
#define GPFSEL3         ((volatile uint32_t*) (MMIO_BASE + 0x0020000C))
#define GPFSEL4         ((volatile uint32_t*) (MMIO_BASE + 0x00200010))
#define GPFSEL5         ((volatile uint32_t*) (MMIO_BASE + 0x00200014))
#define GPSET0          ((volatile uint32_t*) (MMIO_BASE + 0x0020001C))
#define GPSET1          ((volatile uint32_t*) (MMIO_BASE + 0x00200020))
#define GPCLR0          ((volatile uint32_t*) (MMIO_BASE + 0x00200028))
#define GPCLR1          ((volatile uint32_t*) (MMIO_BASE + 0x0020002C))
#define GPLEV0          ((volatile uint32_t*) (MMIO_BASE + 0x00200034))
#define GPLEV1          ((volatile uint32_t*) (MMIO_BASE + 0x00200038))
#define GPEDS0          ((volatile uint32_t*) (MMIO_BASE + 0x00200040))
#define GPEDS1          ((volatile uint32_t*) (MMIO_BASE + 0x00200044))
#define GPREN0          ((volatile uint32_t*) (MMIO_BASE + 0x0020004C))
#define GPREN1          ((volatile uint32_t*) (MMIO_BASE + 0x00200050))
#define GPFEN0          ((volatile uint32_t*) (MMIO_BASE + 0x00200058))
#define GPFEN1          ((volatile uint32_t*) (MMIO_BASE + 0x0020005C))
#define GPHEN0          ((volatile uint32_t*) (MMIO_BASE + 0x00200064))
#define GPHEN1          ((volatile uint32_t*) (MMIO_BASE + 0x00200068))
#define GPLEN0          ((volatile uint32_t*) (MMIO_BASE + 0x00200070))
#define GPLEN1          ((volatile uint32_t*) (MMIO_BASE + 0x00200074))
#define GPAREN0         ((volatile uint32_t*) (MMIO_BASE + 0x0020007C))
#define GPAREN1         ((volatile uint32_t*) (MMIO_BASE + 0x00200080))
#define GPAFEN0         ((volatile uint32_t*) (MMIO_BASE + 0x00200088))
#define GPAFEN1         ((volatile uint32_t*) (MMIO_BASE + 0x0020008C))
#define GPPUD           ((volatile uint32_t*) (MMIO_BASE + 0x00200094))
#define GPPUDCLK0       ((volatile uint32_t*) (MMIO_BASE + 0x00200098))
#define GPPUDCLK1       ((volatile uint32_t*) (MMIO_BASE + 0x0020009C))

typedef enum {
    GPFSEL_INPUT  = 0b000, 
    GPFSEL_OUTPUT = 0b001,
    GPFSEL_ALT0   = 0b100,
    GPFSEL_ALT1   = 0b101,
    GPFSEL_ALT2   = 0b110,
    GPFSEL_ALT3   = 0b111,
    GPFSEL_ALT4   = 0b011,
    GPFSEL_ALT5   = 0b010,
} gpio_func_t;

typedef uint_fast16_t gpio_pin_t;

void gpio_config(gpio_pin_t p, gpio_func_t f);
void gpio_set_high(gpio_pin_t p);
void gpio_set_low(gpio_pin_t p);

#endif  /* _GPIO_H_ */  
