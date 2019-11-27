/*! \file
 * Hifiberry DAC+ source. */

#include "hifiberrydacplus.h"

#include "i2s.h"
#include "i2c1.h"


//! The I2C slave address of the Hifiberry
#define HIFIBERRY_SLAVE_ADDR         (0x9A >> 1)

void hbd_configure(void);

void hbd_init(bool i2s_slave_mode) {
    i2s_init(i2s_slave_mode);
    i2c1_init();
    i2c1_set_target_address(HIFIBERRY_SLAVE_ADDR);

    hbd_configure();
}

/*! \brief Configures the registers of the PCM5122 with some default values
 *         that make it play hearable sound. */
void hbd_configure() {
    hbd_write_register(HBD_REG_PAGE_SELECT,         0x00);
    hbd_write_register(HBD_REG_RESET,               0x11);
    hbd_read_register(HBD_REG_PAGE_SELECT);
    hbd_write_register(HBD_REG_RESET,               0x00);
    hbd_write_register(HBD_REG_MODE_REQUEST,        0x10);
    hbd_write_register(HBD_REG_MODE_REQUEST,        0x11);

    hbd_write_register(HBD_REG_OUTPUT_ENABLE,       0x24);
    hbd_read_register(HBD_REG_OUTPUT_ENABLE);
    hbd_write_register(HBD_REG_GPIO3_OUTPUT_SELECT, 0x02);
    hbd_read_register(HBD_REG_GPIO3_OUTPUT_SELECT);
    hbd_write_register(HBD_REG_GPIO6_OUTPUT_SELECT, 0x02);
    hbd_read_register(HBD_REG_GPIO6_OUTPUT_SELECT);
    hbd_write_register(HBD_REG_GPIO_SET,            0x20);

    hbd_read_register(HBD_REG_STATUS1);
    hbd_write_register(HBD_REG_GPIO_SET,            0x00);
    hbd_read_register(HBD_REG_STATUS1);
    hbd_write_register(HBD_REG_GPIO_SET,            0x04);
    hbd_read_register(HBD_REG_STATUS1);

    hbd_write_register(HBD_REG_OUTPUT_ENABLE,       0x2C);
    hbd_write_register(HBD_REG_GPIO4_OUTPUT_SELECT, 0x02);
    hbd_write_register(HBD_REG_GPIO_SET,            0x0C);
    hbd_write_register(HBD_REG_VOLUME_EMERGENCY_RAMP_SETTING, 0x30);

    hbd_write_register(HBD_REG_AUTO_MUTE_TIME,          0x77);
    hbd_write_register(HBD_REG_AUTO_MUTE_CONTROL,       0x00);
    //hbd_write_register(HBD_REG_DIGITAL_VOLUME_LEFT,     0x00);
    //hbd_write_register(HBD_REG_DIGITAL_VOLUME_RIGHT,    0x00);

    hbd_write_register(HBD_REG_MODE_REQUEST,        0x10);
    hbd_write_register(HBD_REG_IGNORE_SETTINGS,     0x08);
    hbd_write_register(HBD_REG_PLL_REFERENCE,       0x10);
    hbd_write_register(HBD_REG_MODE_REQUEST,        0x00);
    hbd_write_register(HBD_REG_GPIO_SET,            0x2C);
}

/*! Temporary buffer for read-register data
 * \todo Replace by something appropriate. */
uint8_t r_data[20] = {0};
/*! Temporary buffer for write-register data
 * \todo Replace by something appropriate. */
uint8_t w_data[20];

void hbd_read_register(hbd_register_t reg) {
    unsigned dlen = 1;
    uint8_t d;

    d = (uint8_t) reg;
    i2c1_write(&d, 1);
    i2c1_read(dlen);
    while (!i2c1_ready()) i2c1_handler();
}

void hbd_write_register(hbd_register_t reg, uint8_t value) {
    w_data[0] = reg;
    w_data[1] = value;
    i2c1_write(&w_data[0], 2);
    while (!i2c1_ready()) i2c1_handler();
}

void hbd_start(void) {
    i2s_start();
}

void hbd_handle(void) {
    i2c1_handler();
}

hbd_return_t hbd_write_value(audio_val_t v) {
    if (i2s_write(v) == PCM_RET_OK) {
        return HBD_RET_OK;
    } else {
        return HBD_RET_NOK;
    }
}

hbd_return_t hbd_read_value(audio_val_t* v) {
    i2s_audio_val_t i2s_val = 0;
    if (i2s_read(&i2s_val) == PCM_RET_OK) {
        *v = (audio_val_t) i2s_val;
        return HBD_RET_OK;
    } else {
        return HBD_RET_NOK;
    }
}
