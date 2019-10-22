/*! \file
 * I2S driver source file. */

#include "i2s.h"

#include <stdbool.h>

#include "gpio.h"
#include "rpi3bplus_common.h"


/*****************************
 * PCM Clock manager
 ****************************/

#define CM_PCMCTL       ((volatile uint32_t*) (MMIO_BASE + 0x00101098))
#define CM_PCMDIV       ((volatile uint32_t*) (MMIO_BASE + 0x0010109c))

/* CM PCMCTL fields */
#define CM_PCMCTL_SRC_19_2_XTAL (1) // 19.2MHz crystal (most accurate source)
#define CM_PCMCTL_ENAB          (1 << 4)
#define CM_PCMCTL_PASSWORD      (0x5a000000)

/* CM PCMDIV fields */
#define CM_PCMDIV_DIVF_OFFSET   (0)
#define CM_PCMDIV_DIVI_OFFSET   (12)
#define CM_PCMDIV_PASSWORD      (0x5a000000)

#define I2S_BIT_DEPTH           (24)        // 24-bit audio
/* The most accurate clock is the 19.2 MHz crystal */
#define I2S_CLK_SRC             (0x01)
/* A single bit is sent per tick of the PCM clock.
 * If we want a sample rate of 44.1 kHz then we will need to transmit:
 *      44100 Hz * 24 bits * 2 channels = 2116800 bits per second
 * The divider value must then be:
 *      19.2Mhz / 2116800 = 9.070
 * The value that is closest to it is a fraction part of 288/2^12. */
#define I2S_CLK_DIVI            (9 << CM_PCMDIV_DIVI_OFFSET)
#define I2S_CLK_DIVF            (288)


/*********************
 * PCM registers
 ********************/

#define PCM_CS_A        ((volatile uint32_t*) (MMIO_BASE + 0x00203000))
#define PCM_FIFO_A      ((volatile uint32_t*) (MMIO_BASE + 0x00203004))
#define PCM_MODE_A      ((volatile uint32_t*) (MMIO_BASE + 0x00203008))
#define PCM_RXC_A       ((volatile uint32_t*) (MMIO_BASE + 0x0020300C))
#define PCM_TXC_A       ((volatile uint32_t*) (MMIO_BASE + 0x00203010))
#define PCM_DREQ_A      ((volatile uint32_t*) (MMIO_BASE + 0x00203014))
#define PCM_INTEN_A     ((volatile uint32_t*) (MMIO_BASE + 0x00203018))
#define PCM_INTSTC_A    ((volatile uint32_t*) (MMIO_BASE + 0x0020301C))
#define PCM_GRAY        ((volatile uint32_t*) (MMIO_BASE + 0x00203020))

/* PCM CS_A register bits */
#define PCM_CS_EN               (1)
#define PCM_CS_RXON             (1 << 1)
#define PCM_CS_TXON             (1 << 2)
#define PCM_CS_TXCLR            (1 << 3)
#define PCM_CS_RXCLR            (1 << 4)
#define PCM_CS_TXTHR_OFFSET     (5)
#define PCM_CS_RXTHR_OFFSET     (7)
#define PCM_CS_DMAEN            (1 << 9)
#define PCM_CS_TXSYNC           (1 << 13)
#define PCM_CS_RXSYNC           (1 << 14)
#define PCM_CS_TXERR            (1 << 15)
#define PCM_CS_RXERR            (1 << 16)
#define PCM_CS_TXW              (1 << 17)
#define PCM_CS_RXR              (1 << 18)
#define PCM_CS_TXD              (1 << 19)
#define PCM_CS_RXD              (1 << 20)
#define PCM_CS_TXE              (1 << 21)
#define PCM_CS_RXF              (1 << 22)
#define PCM_CS_RXSEC            (1 << 23)
#define PCM_CS_SYNC             (1 << 24)
#define PCM_CS_STBY             (1 << 25)

/* PCM MODE_A register bits */
#define PCM_MODE_FSLEN_OFFSET   (0)
#define PCM_MODE_FLEN_OFFSET    (10)
#define PCM_MODE_FSI            (1 << 20)
#define PCM_MODE_FSM            (1 << 21)
#define PCM_MODE_CLKI           (1 << 22)
#define PCM_MODE_CLKM           (1 << 23)
#define PCM_MODE_FTXP           (1 << 24)
#define PCM_MODE_FRXP           (1 << 25)
#define PCM_MODE_PDME           (1 << 26)
#define PCM_MODE_PDMN           (1 << 27)
#define PCM_MODE_CLKDIS         (1 << 28)

/* PCM RXC_A register bits */
#define PCM_RXC_CH2WID_OFFSET   (0)
#define PCM_RXC_CH2POS_OFFSET   (4)
#define PCM_RXC_CH2EN           (1 << 14)
#define PCM_RXC_CH2WEX          (1 << 15)
#define PCM_RXC_CH1WID_OFFSET   (16)
#define PCM_RXC_CH1POS_OFFSET   (20)
#define PCM_RXC_CH1EN           (1 << 30)
#define PCM_RXC_CH1WEX          (1 << 31)

/* PCM TXC_A register bits */
#define PCM_TXC_CH2WID_OFFSET   (0)
#define PCM_TXC_CH2POS_OFFSET   (4)
#define PCM_TXC_CH2EN           (1 << 14)
#define PCM_TXC_CH2WEX          (1 << 15)
#define PCM_TXC_CH1WID_OFFSET   (16)
#define PCM_TXC_CH1POS_OFFSET   (20)
#define PCM_TXC_CH1EN           (1 << 30)
#define PCM_TXC_CH1WEX          (1 << 31)

/*! \todo If needed add PCM DREQ_A, INTEN_A, INTSTC_A and GRAY register bits */


/*! \brief Initializes the I2S interface, (but does not start communication!).
 *         If it was already initialized before, it will delete the old instance
 *         and create a new one.
 *  \note If we are going to record, also the RX channel(s) must be configured. */
void i2s_init(void) {
    // Configure the I2S pins
    gpio_config(18, GPFSEL_ALT0);
    gpio_config(19, GPFSEL_ALT0);
    gpio_config(20, GPFSEL_ALT0);
    gpio_config(21, GPFSEL_ALT0);

    // Configure the clock for the I2S interface and start it
    *CM_PCMCTL = CM_PCMCTL_PASSWORD | CM_PCMCTL_SRC_19_2_XTAL;
    *CM_PCMDIV = CM_PCMDIV_PASSWORD | I2S_CLK_DIVI | I2S_CLK_DIVF;
    *CM_PCMCTL = CM_PCMCTL_PASSWORD | CM_PCMCTL_SRC_19_2_XTAL | CM_PCMCTL_ENAB;
    //! \todo MASH filtering needed for the clock?

    // Configure the I2S interface for transmission
    // Enable the peripheral
    *PCM_CS_A   |= PCM_CS_EN;
    // Set the frame settings
    *PCM_MODE_A = (PCM_MODE_CLKI) | (32 << PCM_MODE_FSLEN_OFFSET) | ((64 - 1) << PCM_MODE_FLEN_OFFSET) | PCM_MODE_CLKM;

    // Set the TX channel settings
    *PCM_TXC_A  = (33 << PCM_TXC_CH2POS_OFFSET) | PCM_TXC_CH2EN | PCM_TXC_CH2WEX | // Enable both channels as 24-bits
                  (1  << PCM_TXC_CH1POS_OFFSET) | PCM_TXC_CH1EN | PCM_TXC_CH1WEX;  // Channel 1 data comes first

    // Set the RX channel settings
    *PCM_RXC_A  = (33 << PCM_RXC_CH2POS_OFFSET) | PCM_RXC_CH2EN | PCM_RXC_CH2WEX | // Enable both channels as 24-bits
                  (1  << PCM_RXC_CH1POS_OFFSET) | PCM_RXC_CH1EN | PCM_RXC_CH1WEX;  // Channel 1 data comes first

    // Reset FIFO
    *PCM_CS_A   |= PCM_CS_RXCLR | PCM_CS_TXCLR | PCM_CS_SYNC;
    while(!(*PCM_CS_A & PCM_CS_SYNC));            // Wait for the SYNC bit to be set (2 PCM cycles have passed).
}

/*! \brief Starts transmission of audio values. */
void i2s_start(void) {
    // Completely fill the buffer with zero's before starting to avoid
    // immediate buffer underrun.
    while(*PCM_CS_A & PCM_CS_TXD) {
        *PCM_FIFO_A = 0;
    }
    // Start transmission and reception
    *PCM_CS_A |= PCM_CS_TXON | PCM_CS_RXON;
}

/*! \brief Writes an audio value (for a single channel) to the buffer.
 *  \param[in] val The 24-bit audio value.
 *  \returns PCM_RET_OK if the message was written into the FIFO or
 *           PCM_RET_NOK_FIFO_FULL if it couldn't. */
i2s_return_t i2s_write(audio_val_t val) {
    if(*PCM_CS_A & PCM_CS_TXD) {
        *PCM_FIFO_A = val;// & 0x00FFFFFF;
        return PCM_RET_OK;
    } else {
        return PCM_RET_NOK_FIFO_FULL;
    }
}

/*! \brief Reads an audio value (for a single channel) from the buffer.
 *  \param[in] val Pointer to where the 24-bit audio value is stored.
 *  \returns PCM_RET_OK if the message was read successfully or
 *           PCM_RET_NOK_FIFO_EMPTY if it couldn't. */
i2s_return_t i2s_read(audio_val_t* val) {
  // TODO
    if(*PCM_CS_A & PCM_CS_RXD) {
        *val = *PCM_FIFO_A;// & 0x00FFFFFF;
        return PCM_RET_OK;
    } else {
        return PCM_RET_NOK_FIFO_EMPTY;
    }
}
