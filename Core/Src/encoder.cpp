#include "encoder.h"

// See the first page of the Bourns PEC11 datasheet for the quadrature output
// table.  When going clockwise, the A/B pin states progress through the following 
// sequence:
// --------------------------------------------------------------------------
// Pin A | Pin B | Value | Description
// --------------------------------------------------------------------------
//   0   |   0   | 0x0   | At rest
//   1   |   0   | 0x1   | Started turning clockwise
//   1   |   1   | 0x3   | Continuing turning clockwise
//   0   |   1   | 0x2   | Continuing turning clockwise
//   0   |   0   | 0x0   | Again at rest
// --------------------------------------------------------------------------
//
// Counter-clockwise is the reverse of this sequence.  The Encoder class tracks:
// --------------------------------------------------------------------------
// Pin A | Pin B | Value | Description
// --------------------------------------------------------------------------
//   0   |   0   | 0x0   | At rest
//   0   |   1   | 0x2   | Started turning counter-clockwise
//   1   |   1   | 0x3   | Continuing turning counter-clockwise
//   1   |   0   | 0x1   | Continuing turning counter-clockwise
//   0   |   0   | 0x0   | Again at rest
// --------------------------------------------------------------------------

namespace
{
    const uint8_t AT_REST = 0;
    const uint8_t A_HIGH_B_LOW = 1;
    const uint8_t A_LOW_B_HIGH = 2;
    const uint8_t A_HIGH_B_HIGH = 3;
}

void Encoder::Update(bool pinA, bool pinB)
{
    auto currentState  = static_cast<uint8_t>(pinA | (pinB << 1));

    // If no change, just return
    if(currentState == prevState_)
        return;

    if(direction_ == DIR_NONE && prevState_ == AT_REST && currentState != AT_REST)
    {
        direction_ = (currentState == 1) ? DIR_CW : DIR_CCW;
    }
    else if(direction_ == DIR_CW)
    {
        if((prevState_ == A_HIGH_B_LOW && currentState == A_HIGH_B_HIGH) || 
           (prevState_ == A_HIGH_B_HIGH && currentState == A_LOW_B_HIGH))
        {
            // Do nothing, we're still turning clockwise.
        }
        else if(prevState_ == A_LOW_B_HIGH && currentState == AT_REST)
        {
            // If we're here, we've completed a clockwise detent.
            ++turns_;
            direction_ = DIR_NONE;
        }
        else
        {
            // If we're here, the encoder has changed direction or is glitching so consider the move no longer in progress.
            direction_ = DIR_NONE;
        }
    }
    else if(direction_ == DIR_CCW)
    {
        if((prevState_ == A_LOW_B_HIGH && currentState == A_HIGH_B_HIGH) || 
           (prevState_ == A_HIGH_B_HIGH && currentState == A_HIGH_B_LOW))
        {
            // Do nothing, we're still turning counter-clockwise.
        }
        else if(prevState_ == A_HIGH_B_LOW && currentState == AT_REST)
        {
            // If we're here, we've completed a counter-clockwise detent.
            --turns_;
            direction_ = DIR_NONE;
        }
        else
        {
            // If we're here, the encoder has changed direction or is glitching so consider the move no longer in progress.
            direction_ = DIR_NONE;
        }
    }

    prevState_ = currentState;
}

int32_t Encoder::GetTurns() const
{
    return turns_;
}

void Encoder::Reset()
{
    turns_ = 0;
}
