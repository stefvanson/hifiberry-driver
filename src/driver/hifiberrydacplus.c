/*! \file
 * Hifiberry DAC+ source. */

#include "hifiberrydacplus.h"

#include "i2s.h"
#include "i2c1.h"
#include "test/waveform_luts.h"

//! The I2C slave address of the Hifiberry
#define HIFIBERRY_SLAVE_ADDR         (0x9A >> 1)

/*! \brief Initializes the hifiberry driver (but does not start
 *         anything yet). */
void hbd_init(void) {
    i2s_init();
    i2c1_init();
    i2c1_set_target_address(HIFIBERRY_SLAVE_ADDR);
}

/*! \brief Configures the registers of the PCM5122 with some default values
 *         that make it play hearable sound. */
void hbd_configure(void) {
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

/*! \brief Reads the value of a single register of the PCM5122.
 *  \param[in] reg The register that is set.
 *  \todo Useless because it doesn't return the read value yet. Make it return
 *  values. */
void hbd_read_register(hbd_register_t reg) {
    unsigned dlen = 1;
    
    i2c1_write(&reg, 1);
    i2c1_read(dlen);
    while (!i2c1_ready()) i2c1_handler();
}

/*! \brief Sets the value of a register of the PCM5122.
 *  \param[in] reg The register that is set.
 *  \param[in] value The value that is written to the register. */
void hbd_write_register(hbd_register_t reg, uint8_t value) {
    w_data[0] = reg;
    w_data[1] = value;
    i2c1_write(&w_data[0], 2);
    while (!i2c1_ready()) i2c1_handler();
}

/*! \brief Some function that generates a hearable sine on the output. */
void hbd_run(void) {
    unsigned int i = 0;
    
    // Writes some values to the buffer before the actual start to avoid bit
    // flags to be set when starting.
    for (i = 0; i < 20; i++) {
        while (i2s_write(sineLUT[i]) != PCM_RET_OK);
        while (i2s_write(sineLUT[i]) != PCM_RET_OK);
    }

    hbd_configure();
    i2s_start();
    while(1) {
        // Write the value for both channels
        while (i2s_write(sineLUT[i]) != PCM_RET_OK);
        while (i2s_write(sineLUT[i]) != PCM_RET_OK);
    
        // Update the index
        if (++i >= LUT_SIZE) {
            i = 0;
        }
    }
}
