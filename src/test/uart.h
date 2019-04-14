#ifndef _UART_H_
#define _UART_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "driver/common.h"
#include "driver/rpi3bplus_common.h"
#include "driver/gpio.h"

#define AUX_ENABLES         ((volatile uint32_t*) (MMIO_BASE + 0x00215004))
#define AUX_MU_IO_REG       ((volatile uint32_t*) (MMIO_BASE + 0x00215040))
#define AUX_MU_IER_REG      ((volatile uint32_t*) (MMIO_BASE + 0x00215044))
#define AUX_MU_IIR_REG      ((volatile uint32_t*) (MMIO_BASE + 0x00215048))
#define AUX_MU_LCR_REG      ((volatile uint32_t*) (MMIO_BASE + 0x0021504C))
#define AUX_MU_MCR_REG      ((volatile uint32_t*) (MMIO_BASE + 0x00215050))
#define AUX_MU_LSR_REG      ((volatile uint32_t*) (MMIO_BASE + 0x00215054))
#define AUX_MU_MSR_REG      ((volatile uint32_t*) (MMIO_BASE + 0x00215058))
#define AUX_MU_SCRATCH      ((volatile uint32_t*) (MMIO_BASE + 0x0021505C))
#define AUX_MU_CNTL_REG     ((volatile uint32_t*) (MMIO_BASE + 0x00215060))
#define AUX_MU_STAT_REG     ((volatile uint32_t*) (MMIO_BASE + 0x00215064))
#define AUX_MU_BAUD_REG     ((volatile uint32_t*) (MMIO_BASE + 0x00215068))

typedef enum {
    UART_RET_OK = 0,
    UART_RET_NO_DATA,
} uart_ret_t;

void uart_init(void);
void uart_deinit(void);
void uart_send(unsigned c);
void uart_send_string(const char s[]);
void uart_hexstring(unsigned value, unsigned num_digits, bool print_newline);
uart_ret_t uart_get_char(char* c);
void uart_flush(void);

#ifdef __cplusplus
}
#endif

#endif /* _UART_H_ */
