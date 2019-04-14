#include "uart.h"


/*! \brief Initializes the UART peripheral and used pins. */
void uart_init(void) {
    *AUX_ENABLES = 1;
    *AUX_MU_IER_REG = 0;
    *AUX_MU_CNTL_REG = 0;
    *AUX_MU_LCR_REG = 3;
    *AUX_MU_MCR_REG = 0;
    *AUX_MU_IER_REG = 0;
    *AUX_MU_IIR_REG = 0xC6;
    *AUX_MU_BAUD_REG = 270;
    gpio_config(14, GPFSEL_ALT5);
    gpio_config(15, GPFSEL_ALT5);
    *AUX_MU_CNTL_REG = 3;
}

/*! \brief Deinitializes the UART peripheral (and associated pins). */
void uart_deinit(void) {
    uart_flush();

    // Reset to their default state
    gpio_config(14, GPFSEL_INPUT);
    gpio_config(15, GPFSEL_INPUT);

    // Reset the registers to their default values.
    *AUX_MU_BAUD_REG = 0;
    *AUX_MU_IIR_REG  = 0;
    *AUX_MU_CNTL_REG = 3;
    *AUX_MU_IER_REG  = 0x06;
    *AUX_MU_MCR_REG  = 0;
    *AUX_MU_LCR_REG  = 0;
    *AUX_ENABLES     = 0;
}

/*! \brief Sends a single byte over UART.
 *  \param[in] c The value to be sent. */
void uart_send(unsigned c) {
    while(1) {
        // Wait for the transmit FIFO to accept bytes
        if(*AUX_MU_LSR_REG & 0x20) {
            break;
        }
    }
    // Write the byte into the FIFO
    *AUX_MU_IO_REG = c;
}

/*! \brief Transmits a string over UART. */
void uart_send_string(const char s[]) {
    int i = 0;
    while (s[i] != '\0') {
        uart_send(s[i++]);
    }
}

/*! \brief Transmits a hexadecimal value as ASCII including a 0x prefix.
 *  \param[in] value The value to transmitted.
 *  \param[in] num_digits The number of hexadecimal digits to be printed.
 *  \param[in] print_newline End printing with CRLF or not. */
void uart_hexstring(unsigned value, unsigned num_digits, bool print_newline) {
    uint_fast8_t bits_left = num_digits * 4;
    uart_send('0');
    uart_send('x');
    // Print data
    while (1) {
        uint_fast8_t cur_val = (value >> (bits_left - 4)) & 0xF;
        if (cur_val > 9) {
            // If it is a number add the offset for letters...
            cur_val += '0';
        } else {
            // ... otherwise for numbers
            cur_val += 'A';
        }
        uart_send(cur_val);
        bits_left -= 4;

        if (bits_left == 0) {
            break;
        }
    }

    if (print_newline) {
        uart_send('\n');
    }
}

/*! \brief Receive a character over UART.
 *  \returns The received byte. */
uart_ret_t uart_get_char(char* c) {
    // Check if there is new data in the FIFO register
    if(*AUX_MU_LSR_REG & 0x01) {
        *c = (*AUX_MU_IO_REG & 0xFF);
        return UART_RET_OK;
    } else {
        return UART_RET_NO_DATA;
    }
}

/*! \brief Flushes the UART buffer (waits for the transmit FIFO to be empty. */
void uart_flush(void) {
    while(1) {
        // Check if the transmit FIFO is empty
        if(*AUX_MU_LSR_REG & 0x40) {
            break;
        }
    }
}
