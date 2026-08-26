#include "encoders.h"
#include "encoder.h"

namespace
{
    // Tracks all four encoders
    Encoder encoders[ENCODER_COUNT];
}

void Encoders_Init()
{
    for (int i = 0; i < ENCODER_COUNT; ++i)
    {
        encoders[i].Reset();
    }
}

void Encoders_Update(const uint8_t encoderPinValues)
{
    for (int i = 0; i < ENCODER_COUNT; ++i)
    {
        bool pinA = (encoderPinValues & (0x01 << (i * 2))) != 0;
        bool pinB = (encoderPinValues & (0x01 << (i * 2 + 1))) != 0;
        encoders[i].Update(pinA, pinB);
    }
}

int32_t Encoders_GetTurns(const uint8_t index)
{
    if (index < 0 || index >= ENCODER_COUNT)
    {
        return 0;
    }
    return encoders[index].GetTurns();
}
