#ifndef _I2C1_H_
#define _I2C1_H_

#include <stdbool.h>
#include <stdint.h>

#include "config.h"

// Check the required configuration macros
#ifndef I2C1_MAX_READ_SIZE
    #error "I2C1_MAX_READ_SIZE not defined!"
#endif
#ifndef I2C1_MAX_WRITE_SIZE
    #error "I2C1_MAX_WRITE_SIZE not defined!"
#endif
#ifndef I2C1_SDA_PIN
    #error "I2C1_SDA_PIN not defined!"
#endif
#ifndef I2C1_SDA_PIN_FUNC
    #error "I2C1_SDA_PIN_FUNC not defined!"
#endif
#ifndef I2C1_SCL_PIN
    #error "I2C1_SCL_PIN not defined!"
#endif
#ifndef I2C1_SCL_PIN_FUNC
    #error "I2C1_SCL_PIN_FUNC not defined!"
#endif
#ifndef I2C1_DIV_VALUE
    #error "I2C1_DIV_VALUE not defined!"
#endif

#define I2C1_C          ((volatile uint32_t*) (MMIO_BASE + 0x00804000))
#define I2C1_S          ((volatile uint32_t*) (MMIO_BASE + 0x00804004))
#define I2C1_DLEN       ((volatile uint32_t*) (MMIO_BASE + 0x00804008))
#define I2C1_A          ((volatile uint32_t*) (MMIO_BASE + 0x0080400C))
#define I2C1_FIFO       ((volatile uint32_t*) (MMIO_BASE + 0x00804010))
#define I2C1_DIV        ((volatile uint32_t*) (MMIO_BASE + 0x00804014))
#define I2C1_DEL        ((volatile uint32_t*) (MMIO_BASE + 0x00804018))
#define I2C1_CLKT       ((volatile uint32_t*) (MMIO_BASE + 0x0080401C))

#define I2C1_C_READ     (1)
#define I2C1_C_CLEAR    (1 << 4)
#define I2C1_C_ST       (1 << 7)
#define I2C1_C_INTD     (1 << 8)
#define I2C1_C_INTT     (1 << 9)
#define I2C1_C_INTR     (1 << 10)
#define I2C1_C_I2CEN    (1 << 15)

#define I2C1_S_TA       (1)
#define I2C1_S_DONE     (1 << 1)
#define I2C1_S_TXW      (1 << 2)
#define I2C1_S_RXR      (1 << 3)
#define I2C1_S_TXD      (1 << 4)
#define I2C1_S_RXD      (1 << 5)
#define I2C1_S_TXE      (1 << 6)
#define I2C1_S_RXF      (1 << 7)
#define I2C1_S_ERR      (1 << 8)
#define I2C1_S_CLKT     (1 << 9)

typedef enum {
    I2C1_OK,
    I2C1_NOK,
    I2C1_NOK_BUSY,
    I2C1_NOK_DATA,
} i2c1_ret_t;

typedef uint8_t i2c_addr_t;

void i2c1_init(void);
bool i2c1_ready(void);
i2c1_ret_t i2c1_set_target_address(i2c_addr_t addr);
i2c1_ret_t i2c1_read(unsigned dlen);
i2c1_ret_t i2c1_write(uint8_t* data, unsigned dlen);
void i2c1_handler(void);
uint8_t* i2c1_get_rb_pointer(void);

#endif /* _I2C1_H_ */
