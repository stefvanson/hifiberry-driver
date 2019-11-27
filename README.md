# Hifiberry DAC+ driver
Project for using the Hifiberry DAC+ in Raspberry Pi (3B+) bare metal projects.

Note that it currently is just a start with which sound can be generated. It
needs quite some work and documentation to be a proper driver.

This is code is run on the target and working as intended.

## Structure
All driver code is located in src/driver.

Some code for testing the library on the target is located in src/test.

## Pins for connecting an ADC
I'm using [this I2S ADC](https://www.audiophonics.fr/en/diy-dac/adc-analog-to-digital-converter-akm5720-i2s-24bit-96khz-p-13351.html) as the input device. Connect it in the following way (ADC pin -> RPi pin (function), find the Raspberry Pi pins [here](https://pinout.xyz/pinout/pcm)):
- VCC -> 2 (5V)
- GND -> 34 (GND)
- LRCK -> 35 (FS)
- DATA -> 38 (DIN)
- BICK -> 12 (CLK)
- MCLK -> not used, keep the jumper on.

To build it when this I2S ADC is hooked up to the pins above, change test/main.cpp to `i2s_slave_mode = true` and `i2s_input_present = true`.

*Note that this ADC outputs a clock at 96kHz (audio rate), which is different from the 48kHz that we configure if the Raspberry Pi is the I2S clock master. Therefore the tone of the sine can be higher when configured as an I2S slave with the external I2S device dictating the clock at a higher rate than that.*

## TODO
- \todo Finish I2C. Currently you can use it to send/receive I2C messages and check it by attaching a logic analyzer/scope, but no data is actually returned and it's not handled in a non-block way.
