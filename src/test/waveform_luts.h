#ifndef _WAVEFORM_LUTS_H_
#define _WAVEFORM_LUTS_H_

#include <stdint.h>

typedef int32_t AudioData;

#define LUT_SIZE			(441)

extern AudioData sineLUT[441];
extern AudioData sawtoothLUT[441];
extern AudioData squareLUT[441];
extern AudioData triangleLUT[441];

#endif /* _WAVEFORM_LUTS_H_ */
