#pragma once

#include <cstdint>

//======================================================================
// Tracks the net number of detents turned on a Bourns PEC11 quadrature
// rotary encoder.  See the Bourns PEC11 datasheet for more info.
//======================================================================
class Encoder
{
public:
    Encoder() = default;

    // Sends the current pin states to the encoder.
    void Update(const bool pinA, const bool pinB);

    // Returns the current number of detent turns.
    int32_t GetTurns() const;

    // Resets the turn counter to zero.
    void Reset();

private:
    int32_t turns_ = 0;      // Tracks current number of detents turns
    int8_t  prevState_ = 0;  // Previous pin A and pin B states

    enum DIRECTION
    {
        DIR_NONE,
        DIR_CW,
        DIR_CCW
    } direction_ = DIR_NONE; // Tracks the current direction of rotation
};
