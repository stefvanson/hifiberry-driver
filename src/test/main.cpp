#define DEBUG

#include "debug.h"
#include "driver/hifiberrydacplus.h"
#include "waveform_luts.h"

int main(void) {
    // Whether the RPi is I2S clock master or slave
    bool i2s_slave_mode = false;
    // Whether an input device is connected on the I2s bus
    bool i2s_input_present = false;

    DEBUG_INIT();
    DEBUG_STR("Program started...\n");

    hbd_init(i2s_slave_mode);
    hbd_start();

    if (i2s_input_present == false) {
        // If no input is present, output a sine ("round" tone)
        unsigned i = 0;
        while(1) {
            while (hbd_write_value(sineLUT[i]) != HBD_RET_OK);
            while (hbd_write_value(sineLUT[i]) != HBD_RET_OK);
            // Update the index
            if (++i >= LUT_SIZE) {
                i = 0;
            }
        }
    } else {
        // If input is present, simply forward input to output
        while(1) {
          audio_val_t av = 0;
          while(hbd_read_value(&av) != HBD_RET_OK);
          while (hbd_write_value(av) != HBD_RET_OK);
        }
    }

    while(1);

    return(0);
}
