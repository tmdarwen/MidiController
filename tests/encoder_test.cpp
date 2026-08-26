#include "encoder.h"

#include <gtest/gtest.h>

namespace
{
    // Feeds the same pin reading `count` times immitating uage where bit shifter is read 
    // periodically and the same value would likely be read multiple times.
    void Feed(Encoder& encoder, bool pinA, bool pinB, int repeated)
    {
        for (int i = 0; i < repeated; ++i)
        {
            encoder.Update(pinA, pinB);
            printf("repeatCount=%d, pinA=%d, pinB=%d, turns=%d\n", i, pinA, pinB, encoder.GetTurns());
        }
    }

    // Drives one full clockwise detent (rest -> ... -> rest) through a
    // debounced encoder, holding each intermediate pin state for
    // `repeated` samples so every step gets confirmed.
    void FeedClockwiseDetent(Encoder& encoder, int repeated)
    {
        Feed(encoder, false, false, repeated);
        Feed(encoder, true, false, repeated); 
        Feed(encoder, true, true, repeated); 
        Feed(encoder, false, true, repeated);
        Feed(encoder, false, false, repeated);
    }

    // Mirror image of FeedClockwiseDetent for a counter-clockwise detent.
    void FeedCounterClockwiseDetent(Encoder& encoder, int repeated)
    {
        Feed(encoder, false, false, repeated);
        Feed(encoder, false, true, repeated);
        Feed(encoder, true, true, repeated);
        Feed(encoder, true, false, repeated); 
        Feed(encoder, false, false, repeated); 
    }
}

TEST(Encoder, InitialTurnsIsZero)
{
    Encoder encoder;
    EXPECT_EQ(encoder.GetTurns(), 0);
}

TEST(Encoder, SingleClockwiseDetentIncrementsTurns)
{
    Encoder encoder;
    FeedClockwiseDetent(encoder, 3);
    EXPECT_EQ(encoder.GetTurns(), 1);
}

TEST(Encoder, SingleCounterClockwiseDetentDecrementsTurns)
{
    Encoder encoder;
    FeedCounterClockwiseDetent(encoder, 3);
    EXPECT_EQ(encoder.GetTurns(), -1);
}

TEST(Encoder, MultipleClockwiseDetentsAccumulate)
{
    Encoder encoder;
    FeedClockwiseDetent(encoder, 3);
    FeedClockwiseDetent(encoder, 3);
    FeedClockwiseDetent(encoder, 3);
    EXPECT_EQ(encoder.GetTurns(), 3);
}

TEST(Encoder, ClockwiseThenCounterClockwiseCancelOut)
{
    Encoder encoder;
    FeedClockwiseDetent(encoder, 3);
    FeedCounterClockwiseDetent(encoder, 3);
    EXPECT_EQ(encoder.GetTurns(), 0);
}

TEST(Encoder, ClockwiseTwiceThenCounterClockwiseOnce)
{
    Encoder encoder;
    FeedClockwiseDetent(encoder, 3);
    EXPECT_EQ(encoder.GetTurns(), 1);
    FeedClockwiseDetent(encoder, 3);
    EXPECT_EQ(encoder.GetTurns(), 2);
    FeedCounterClockwiseDetent(encoder, 3);
    EXPECT_EQ(encoder.GetTurns(), 1);
}

TEST(Encoder, ReversingMidDetentDoesNotCount)
{
    Encoder encoder;
    // Get partway into a clockwise detent, then reverse back to rest
    // without ever completing it.
    Feed(encoder, false, true, 3);  // A=0,B=1 (kCwBegin)
    Feed(encoder, false, false, 3); // A=0,B=0 (kCwNext)
    Feed(encoder, true, true, 3);   // back to rest without the kCwFinal step
    EXPECT_EQ(encoder.GetTurns(), 0);
}

TEST(Encoder, IncompleteDetentStoppedAtFinalStepDoesNotCount)
{
    Encoder encoder;
    Feed(encoder, false, true, 3);
    Feed(encoder, false, false, 3);
    Feed(encoder, true, false, 3); // reaches kCwFinal, but never returns to rest
    EXPECT_EQ(encoder.GetTurns(), 0);
}

TEST(Encoder, BriefGlitchBelowDebounceThresholdIsIgnored)
{
    Encoder encoder;
    // A single stray reading (contact bounce) that reverts before holding
    // for repeated consecutive samples must not move the state
    // machine or the turn count.
    encoder.Update(false, true); // one glitchy sample toward A=0,B=1
    encoder.Update(true, true);  // bounces straight back to rest
    encoder.Update(true, true);
    EXPECT_EQ(encoder.GetTurns(), 0);

    // The encoder should still be able to complete a clean detent afterward.
    FeedClockwiseDetent(encoder, 3);
    EXPECT_EQ(encoder.GetTurns(), 1);
}

TEST(Encoder, ReadingHeldForFewerThanDebounceSamplesIsNotConfirmed)
{
    Encoder encoder;
    Feed(encoder, false, true, 2); // held for 2 of 3 required samples
    Feed(encoder, true, true, 3);  // reverts to rest before ever confirming
    EXPECT_EQ(encoder.GetTurns(), 0);
}

TEST(Encoder, RepeatedRestReadingsDoNotAccumulateTurns)
{
    Encoder encoder;
    Feed(encoder, true, true, 20);
    EXPECT_EQ(encoder.GetTurns(), 0);
}

TEST(Encoder, ResetClearsTurnsCount)
{
    Encoder encoder;
    FeedClockwiseDetent(encoder, 3);
    ASSERT_EQ(encoder.GetTurns(), 1);

    encoder.Reset();
    EXPECT_EQ(encoder.GetTurns(), 0);
}

TEST(Encoder, ResetDoesNotDisruptInFlightDetent)
{
    Encoder encoder;
    // Get partway into a clockwise detent.
    Feed(encoder, false, false, 3);
    Feed(encoder, true, false, 3);
    Feed(encoder, true, true, 3);

    encoder.Reset();

    // Finishing the same detent after Reset() should still register,
    // since Reset() only clears the turn counter, not the debounce or
    // quadrature state machine.
    Feed(encoder, false, true, 3);
    Feed(encoder, false, false, 3);
    EXPECT_EQ(encoder.GetTurns(), 1);
}

TEST(Encoder, ZeroDebounceSamplesIsTreatedAsOne)
{
    Encoder encoder;
    FeedClockwiseDetent(encoder, 1);
    EXPECT_EQ(encoder.GetTurns(), 1);
}

TEST(Encoder, NoDebouncingAcceptsEverySample)
{
    Encoder encoder;
    FeedClockwiseDetent(encoder, 1);
    EXPECT_EQ(encoder.GetTurns(), 1);
}
