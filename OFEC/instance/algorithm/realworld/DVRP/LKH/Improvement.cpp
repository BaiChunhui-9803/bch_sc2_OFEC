#include "./INCLUDE/Segment.h"
#include "./INCLUDE/LKH.h"

/* The Improvement function is used to check whether a done move
 * has improved the current best tour. 
 * If the tour has been improved, the function computes the penalty gain
 * and returns 1. Otherwise, the move is undone, and the function returns 0.
 */

int LKH::LKHAlg::Improvement(GainType * Gain, Node * t1, Node * SUCt1)
{
    GainType NewPenalty;

    if (!Penalty) {
        if (*Gain > 0)
            return 1;
        RestoreTour();
        if (SUC(t1) != SUCt1)
            Reversed ^= 1;
        *Gain = PenaltyGain = 0;
        return 0;
    }
    CurrentGain = *Gain;
    NewPenalty = (this->*Penalty)();
    if (NewPenalty <= CurrentPenalty) {
        if (TSPTW_Makespan)
            *Gain =
                (TSPTW_CurrentMakespanCost -
                 TSPTW_MakespanCost()) * Precision;
        if (NewPenalty < CurrentPenalty || *Gain > 0) {
            PenaltyGain = CurrentPenalty - NewPenalty;
            return 1;
        }
    }
    RestoreTour();
    if (SUC(t1) != SUCt1)
        Reversed ^= 1;
    *Gain = PenaltyGain = 0;
    return 0;
}
