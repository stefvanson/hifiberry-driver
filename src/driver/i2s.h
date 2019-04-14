#ifndef _I2S_H_
#define _I2S_H_

/*! \file
 * I2S driver header file. */

#include <stdint.h>

#include "common.h"

/*! Type for single channel audio values.
 * \note Only the 24 least significant bits are relevant and processed. */
typedef uint_fast32_t audio_val_t;

/*! Return values for I2S driver operations. */
typedef enum {
    PCM_RET_OK = 0,         //!< The operation was executed successfully.
    PCM_RET_NOK_FIFO_FULL,  //!< The operation was unsuccessful because the FIFO was already full.
} i2s_return_t;

void i2s_init(void);
void i2s_start(void);
i2s_return_t i2s_write(audio_val_t v);

#endif /* _I2S_H_ */
