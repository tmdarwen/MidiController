#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ENCODER_COUNT 4

// C-callable wrapper around four Encoder instances (see encoder.h)
void Encoders_Init();

// Takes a reading from the SN74HC165 shift register as input.  There's one bit 
// for each encoder pin, two pins per encoder, so 8 bits total:
// ------------------------------------------------------
// | Encoder | PEC11 Pin | SN74HC165 Pin | Bit Position |
// ------------------------------------------------------
// |    0    |     A     |     A         |      0       |
// |    0    |     B     |     B         |      1       |
// |    1    |     A     |     C         |      2       |
// |    1    |     B     |     D         |      3       |
// |    2    |     A     |     E         |      4       |
// |    2    |     B     |     F         |      5       |
// |    3    |     A     |     G         |      6       |
// |    3    |     B     |     H         |      7       |
// ------------------------------------------------------
void Encoders_Update(const uint8_t encoderPinValues);

// Returns the current position of the encoder
int32_t Encoders_GetTurns(const uint8_t index);

#ifdef __cplusplus
}
#endif
