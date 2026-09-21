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

void Encoders_Update(const uint16_t encoderPinValues)
{
    for (int i = 0; i < ENCODER_COUNT; ++i)
    {
        bool pinA = (encoderPinValues & (0x01 << (i * 2))) != 0;
        bool pinB = (encoderPinValues & (0x01 << (i * 2 + 1))) != 0;
        bool button = (encoderPinValues & (0x01 << (i + 8))) == 0; // Active low button
        encoders[i].Update(pinA, pinB, button);
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

bool Encoders_GetButtonState(const uint8_t index)
{
    if (index < 0 || index >= ENCODER_COUNT)
    {
        return false;
    }
    return encoders[index].GetButtonState();
}
