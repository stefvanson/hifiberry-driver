#define DEBUG

#include "debug.h"
#include "driver/hifiberrydacplus.h"
#include "waveform_luts.h"

#define PCM_TXC_A       ((volatile uint32_t*) (MMIO_BASE + 0x00203010))
#define PCM_CS_TXE              (1 << 21)


int main(void) {
    unsigned i = 0;
    DEBUG_INIT();
    DEBUG_STR("Program started...\n");

    hbd_init();
    hbd_start();

    //while(1) {
    //  while (hbd_write_value(sineLUT[i]) != HBD_RET_OK);
    //  while (hbd_write_value(sineLUT[i]) != HBD_RET_OK);
    //  // Update the index
    //  if (++i >= LUT_SIZE) {
    //      i = 0;
    //  }
    //}

    while(1) {
        hbd_return_t ret = HBD_RET_NOK;
        audio_val_t av = 0;
        // Write the read value for both channels
        ret = hbd_read_value(&av);
        if (ret == HBD_RET_OK) {
            while (hbd_write_value(av) != HBD_RET_OK);
        } else if (*PCM_TXC_A & PCM_CS_TXE) {
            hbd_write_value(0xF3F2F1);
        }
    }

    while(1);

    return(0);
}
