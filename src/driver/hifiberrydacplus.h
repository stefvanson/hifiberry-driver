#ifndef _HIFIBERRY_DRIVER_H_
#define _HIFIBERRY_DRIVER_H_

/*! \file
 * Hifiberry DAC+ library main header file. */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "common.h"

typedef uint8_t hbd_register_t;
#define HBD_REG_PAGE_SELECT             (0x80)
#define HBD_REG_RESET                   (0x81)
#define HBD_REG_MODE_REQUEST            (0x82)
#define HBD_REG_OUTPUT_ENABLE           (0x88)
#define HBD_REG_PLL_REFERENCE           (0x8D)
#define HBD_REG_IGNORE_SETTINGS         (0xA5)
#define HBD_REG_VOLUME_EMERGENCY_RAMP_SETTING     (0xC0)
#define HBD_REG_GPIO3_OUTPUT_SELECT     (0xD2)
#define HBD_REG_GPIO4_OUTPUT_SELECT     (0xD3)
#define HBD_REG_GPIO6_OUTPUT_SELECT     (0xD5)
#define HBD_REG_GPIO_SET                (0xD6)
#define HBD_REG_SCK_DETECTED            (0xDB)
#define HBD_REG_STATUS1                 (0xDE)
#define HBD_REG_AUTO_MUTE_TIME          (0xBC)
#define HBD_REG_DIGITAL_VOLUME_LEFT     (0xBD)
#define HBD_REG_DIGITAL_VOLUME_RIGHT    (0xBE)
#define HBD_REG_AUTO_MUTE_CONTROL       (0xC1)


void hbd_init(void);
void hbd_read_register(hbd_register_t reg);
void hbd_write_register(hbd_register_t reg, uint8_t value);
void hbd_run(void);

#ifdef __cplusplus
}
#endif

#endif /* _HIFIBERRY_DRIVER_H_ */
