#ifndef _I2S_H_
#define _I2S_H_

/*! \file
 * I2S driver header file. */

#include <stdint.h>
#include <stdbool.h>

#include "common.h"

/*! Type for single channel audio values.
 * \note Only the 24 least significant bits are relevant and processed. */
typedef int_fast32_t i2s_audio_val_t;

/*! Return values for I2S driver operations. */
typedef enum {
    PCM_RET_OK = 0,         //!< The operation was executed successfully.
    PCM_RET_NOK_FIFO_FULL,  //!< The operation was unsuccessful because the FIFO was already full.
    PCM_RET_NOK_FIFO_EMPTY, //!< The operation was unsuccessful because the FIFO was empty.
} i2s_return_t;

void i2s_init(bool slave_mode);
void i2s_start(void);
i2s_return_t i2s_write(i2s_audio_val_t v);
i2s_return_t i2s_read(i2s_audio_val_t* v);

#endif /* _I2S_H_ */
