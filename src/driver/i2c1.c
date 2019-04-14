#include "i2c1.h"

#include "config.h"
#include "gpio.h"

typedef enum {
    I2C1_READY_ENTRY,
    I2C1_READY,
    I2C1_TRANSMITTING,
    I2C1_WAIT_FOR_END,
    I2C1_READ_PENDING,
    I2C1_RECEIVING,
} i2c_state_t;


// Global variables that keep the I2C1's state
i2c_state_t  i2c_cur_state;     // Current I2C1 state
uint_fast8_t i2c_wb_idx;        // Current read buffer index
uint_fast8_t i2c_w_len;         // Current data length of the read buffer
uint_fast8_t i2c_rb_idx;        // Current read buffer index
uint_fast8_t i2c_r_len;         // Current data length of the read buffer
bool i2c_read_pending;          // Whether there is a read-after-write pending

uint8_t i2c_write_buffer[I2C1_MAX_WRITE_SIZE];
uint8_t i2c_read_buffer[I2C1_MAX_READ_SIZE];


/*! \brief Returns the pointer to the buffer that's holding the read bytes.
 *  \returns Pointer to an array of uint8_t data. */
uint8_t* i2c1_get_rb_pointer() {
    return i2c_read_buffer;
}

/*! \brief Resets the I2C1 state variables. */
static void i2c1_state_reset(void) {
    i2c_cur_state = I2C1_READY;
    i2c_wb_idx = 0;
    i2c_w_len  = 0;
    i2c_rb_idx = 0;
    i2c_r_len  = 0;
    i2c_read_pending = false;
}

/*! \brief Initializes I2C1. */
void i2c1_init(void) {
    // Configure GPIO pins
    gpio_config(I2C1_SDA_PIN, I2C1_SDA_PIN_FUNC);
    gpio_config(I2C1_SCL_PIN, I2C1_SCL_PIN_FUNC);
	
	*I2C1_DIV = I2C1_DIV_VALUE;

	*I2C1_C |= I2C1_C_CLEAR | I2C1_C_I2CEN;
    i2c_cur_state = I2C1_READY_ENTRY;
    i2c1_handler();
}

/*! \brief Checks if the transfer is done.
 *  \returns True if it is, false otherwise. */
static inline bool i2c1_transfer_done(void) {
    return (*I2C1_S & I2C1_S_DONE);
}

/*! \brief Sets the (slave) address of the target device.
 *  \param[in] addr The 7-bits address (MSB is not taken into account). */
i2c1_ret_t i2c1_set_target_address(i2c_addr_t addr) {
    if (addr & 0x80) {
        return I2C1_NOK;
    }

    *I2C1_A = (addr & 0x7F);
    return I2C1_OK;
}

/*! \brief Starts an I2C1 read. If it is already writing it will trigger a read
 * after write with a repeated start condition.
 *  \param[in] dlen The number of bytes that is requested to be read. */
i2c1_ret_t i2c1_read(unsigned dlen) {
    // Check if the data length is within the limits
    if (dlen > I2C1_MAX_READ_SIZE) {
        DEBUG_STR("i2c1_read: too many bytes!\n");
        return I2C1_NOK_DATA;
    }
    
    i2c_rb_idx = 0;
    i2c_r_len = dlen;
    
    // Check if currently allowed to start a read
    if (i2c_cur_state == I2C1_TRANSMITTING ||
        i2c_cur_state == I2C1_WAIT_FOR_END) {
        i2c_read_pending = true;
    } else if (i2c_cur_state == I2C1_READY) {
        // Clear FIFO, status/error bits
        *I2C1_C |= I2C1_C_CLEAR;
	    *I2C1_S |= I2C1_S_DONE | I2C1_S_ERR | I2C1_S_CLKT;
        // Configure and start transmission
    	*I2C1_DLEN = i2c_r_len;
    	*I2C1_C |= I2C1_C_READ;     // Read operation
    	*I2C1_C |= I2C1_C_ST;       // Start transmission

        i2c_cur_state = I2C1_RECEIVING;
    } else {
        return I2C1_NOK_BUSY;
    }
   
    return I2C1_OK;
}

bool i2c1_ready(void) {
    return i2c_cur_state == I2C1_READY;
}

static inline bool transmit_fifo_full(void) {
    return *I2C1_S & I2C1_S_TXD ? false : true;
}

/*! \brief Starts an I2C1 write (if allowed).
 *  \param[in] data Pointer to the array of data that must be transmitted.
 *  \param[in] dlen The number of bytes (data length) in the data array.
 *  \returns Return code which can be:
 *           - I2C1_OK = Write started.
 *           - I2C1_NOK_BUSY = the peripheral was not ready for start
 *           - I2C1_NOK_DATA = too many data bytes. */
i2c1_ret_t i2c1_write(uint8_t* data, unsigned dlen) {
    unsigned i;
    
    // Check if the data length is within the limits
    if (dlen > I2C1_MAX_WRITE_SIZE) {
        DEBUG_STR("i2c1_write: too many bytes!\n");
        return I2C1_NOK_DATA;
    }
    
    // Check if currently allowed to start a write
    if (i2c_cur_state != I2C1_READY) {
        return I2C1_NOK_BUSY;
    }

    // Copy the data to the write buffer
    for (i = 0; i < dlen; i++) {
        i2c_write_buffer[i] = data[i];
    }

    // Clear status/error bits
    *I2C1_C |= I2C1_C_CLEAR;
	*I2C1_S |= I2C1_S_DONE | I2C1_S_ERR | I2C1_S_CLKT;
	
    // Configure transmission
	*I2C1_C &= ~I2C1_C_READ;    // Write operation
	*I2C1_DLEN = dlen;

    // Fill FIFO
    i2c_wb_idx = 0;
    i2c_w_len  = dlen;
    while (i2c_wb_idx < i2c_w_len && !transmit_fifo_full()) {
        *I2C1_FIFO = i2c_write_buffer[i2c_wb_idx++];
    }
    
    // If the last byte is already in the fifo, directly go to wait for end,
    // otherwise go to the transmitting state.
    i2c_cur_state = i2c_wb_idx >= i2c_w_len ? I2C1_WAIT_FOR_END : I2C1_TRANSMITTING;
	
    // Start the transmission
	*I2C1_C |= I2C1_C_ST;

    return I2C1_OK;
}

/*! \brief Handles the state machine of I2C1. Call this function in the main
 *         loop. */
void i2c1_handler(void) {
    i2c_state_t next_state = i2c_cur_state;

    switch (i2c_cur_state) {
        case I2C1_READY_ENTRY:
            i2c1_state_reset();
            next_state = I2C1_READY;
            // Intended fall-through
        case I2C1_READY:
            break;

        case I2C1_TRANSMITTING:
            while (i2c_wb_idx < i2c_w_len && !transmit_fifo_full()) {
                *I2C1_FIFO = i2c_write_buffer[i2c_wb_idx++];
            }
            if (i2c_wb_idx >= i2c_w_len) {
                next_state = i2c_read_pending ? I2C1_READ_PENDING : I2C1_WAIT_FOR_END;
            }
            break;

        case I2C1_WAIT_FOR_END:
            if (i2c_read_pending) {
                next_state = I2C1_READ_PENDING;
            } else if (i2c1_transfer_done()) {
                next_state = I2C1_READY_ENTRY;
            }
            break;

        case I2C1_READ_PENDING:
            if (*I2C1_S & (I2C1_S_ERR | I2C1_S_CLKT)) {
                // If during the write an error or clock stretch occurred, do
                // not perform the read anymore.
                i2c_read_pending = 0;
                next_state = I2C1_WAIT_FOR_END;
            } else if (*I2C1_S & I2C1_S_TXE) {
                // Configure and start transmission
    	        *I2C1_DLEN = i2c_r_len;
    	        *I2C1_C |= I2C1_C_READ;     // Read operation
    	        *I2C1_C |= I2C1_C_ST;       // Start transmission
                next_state = I2C1_RECEIVING;
            }
           break;

        case I2C1_RECEIVING:
            while (i2c_rb_idx < i2c_r_len && *I2C1_S & I2C1_S_RXD) {
                i2c_read_buffer[i2c_rb_idx++] = (uint8_t) *I2C1_FIFO;
            }
            if (*I2C1_S & I2C1_S_DONE) {
                next_state = I2C1_READY_ENTRY;
            }
            break;

        default:
            break;
    }

    i2c_cur_state = next_state;
}
