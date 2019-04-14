#include "gpio.h"

#define GPIO_NUM_PINS       (54)

/*! \brief Sets the function of a pin.
 *  \param[in] p The pin number of the pin that must be configured.
 *  \param[in] f The function, e.g. GPFSEL_ALT3 */
void gpio_config(gpio_pin_t p, gpio_func_t f) {
    // Configure pin as an output
    if (p < GPIO_NUM_PINS) {
        int register_number = p / 10;
        int bit_index = (p - 10*register_number) * 3;

        volatile uint32_t* reg_ptr = GPFSEL0 + register_number;
        // Clear the old function bits
        *reg_ptr &= ~(0x07 << bit_index);
        // Set the actual function bits
        *reg_ptr |= f << bit_index;
    }
}

/*! \brief Sets the pin output value to high.
 *  \param[in] p The pin number of the pin that must be set. */
void gpio_set_high(gpio_pin_t p) {
    if (p < GPIO_NUM_PINS) {
        int register_number = p / 32;
        volatile uint32_t* reg_ptr = GPSET0 + register_number;
        *reg_ptr = 1 << (p % 32);
    }
}

/*! \brief Sets the pin output value to low.
 *  \param[in] p The pin number of the pin that must be set. */
void gpio_set_low(gpio_pin_t p) {
    if (p < GPIO_NUM_PINS) {
        int register_number = p / 32;
        volatile uint32_t* reg_ptr = GPCLR0 + register_number;
        *reg_ptr = 1 << (p % 32);
    }
}
