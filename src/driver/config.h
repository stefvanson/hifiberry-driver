#ifndef _CONFIG_H_
#define _CONFIG_H_

#define HBD_MAX_READ_RIZE               (20)
#define HBD_MAX_WRITE_SIZE              (20)

// Configure I2C1 for usage with the Hifiberrydriver
// Note: This assumes that I2C1 is not used for other devices as well.
#define I2C1_MAX_READ_SIZE              (HBD_MAX_READ_RIZE)
#define I2C1_MAX_WRITE_SIZE             (HBD_MAX_WRITE_SIZE)
#define I2C1_SDA_PIN                    (2)
#define I2C1_SDA_PIN_FUNC               (GPFSEL_ALT0)
#define I2C1_SCL_PIN                    (3)
#define I2C1_SCL_PIN_FUNC               (GPFSEL_ALT0)
#define I2C1_DIV_VALUE                  (0x000005dc) // Default value for 100kHz

#endif /* _CONFIG_H_ */
