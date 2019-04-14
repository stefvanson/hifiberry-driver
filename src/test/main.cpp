#define DEBUG

#include "debug.h"
#include "driver/hifiberrydacplus.h"

int main(void) {
    DEBUG_INIT();
    DEBUG_STR("Program started...\n");

    hbd_init();
    hbd_run();

    while(1);

    return(0);
}
