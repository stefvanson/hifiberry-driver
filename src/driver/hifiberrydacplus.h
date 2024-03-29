#ifndef _HIFIBERRY_DRIVER_H_
#define _HIFIBERRY_DRIVER_H_

/*! \file
 * Hifiberry DAC+ library main header file. */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>


//! Type for the PCM5122 registers.
typedef enum {
    HBD_REG_PAGE_SELECT                     =   0x80,
    HBD_REG_RESET                           =   0x81,
    HBD_REG_MODE_REQUEST                    =   0x82,
    HBD_REG_OUTPUT_ENABLE                   =   0x88,
    HBD_REG_PLL_REFERENCE                   =   0x8D,
    HBD_REG_IGNORE_SETTINGS                 =   0xA5,
    HBD_REG_VOLUME_EMERGENCY_RAMP_SETTING   =   0xC0,
    HBD_REG_GPIO3_OUTPUT_SELECT             =   0xD2,
    HBD_REG_GPIO4_OUTPUT_SELECT             =   0xD3,
    HBD_REG_GPIO6_OUTPUT_SELECT             =   0xD5,
    HBD_REG_GPIO_SET                        =   0xD6,
    HBD_REG_SCK_DETECTED                    =   0xDB,
    HBD_REG_STATUS1                         =   0xDE,
    HBD_REG_AUTO_MUTE_TIME                  =   0xBC,
    HBD_REG_DIGITAL_VOLUME_LEFT             =   0xBD,
    HBD_REG_DIGITAL_VOLUME_RIGHT            =   0xBE,
    HBD_REG_AUTO_MUTE_CONTROL               =   0xC1,
} hbd_register_t;

/*! Return values for this driver's operations. */
typedef enum {
    HBD_RET_OK = 0,   //!< The operation was executed successfully.
    HBD_RET_NOK,      //!< The operation was unsuccessful.
} hbd_return_t;

/*! Type for single channel audio values.
 * \note Only the 24 least significant bits are relevant and processed. */
typedef int32_t audio_val_t;

/*! \brief Initializes the hifiberry driver (but does not start
 *         anything yet). */
void hbd_init(bool i2s_slave_mode);

/*! \brief Reads the value of a single register of the PCM5122.
 *  \param[in] reg The register that is set.
 *  \todo Useless because it doesn't return the read value yet. Make it return
 *  values. */
void hbd_read_register(hbd_register_t reg);

/*! \brief Sets the value of a register of the PCM5122.
 *  \param[in] reg The register that is set.
 *  \param[in] value The value that is written to the register. */
void hbd_write_register(hbd_register_t reg, uint8_t value);

/*! \brief Starts hifiberry audio in- and output. */
void hbd_start(void);

/*! \brief Call this periodically from the main loop to update the driver's state. */
void hbd_handle(void);

/*! \brief Writes the next (single channel) audio value to the output.
    \param[in] v The next audio value.
    \note Write twice for writing the left and right channel value. */
hbd_return_t hbd_write_value(audio_val_t v);

/*! \brief Reads the next (single channel) audio value.
    \param[in] v Pointer to where the next input audio value is going to be stored.
    \note Read twice for getting the left and right channel value. */
hbd_return_t hbd_read_value(audio_val_t* v);

#ifdef __cplusplus
}
#endif

#endif /* _HIFIBERRY_DRIVER_H_ */
