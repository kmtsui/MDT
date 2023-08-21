#pragma once

#ifndef BEAMTIMING_H
#define BEAMTIMING_H

#include "TRandom3.h"

const int kNBunches = 8;
const double kBunchInterval = 560.;  // ns
const double kBunchWidth = 50.;   // ns (FWHM)
const double kBunchSigma = kBunchWidth/2.355;

class BeamTiming
{
    public:
        BeamTiming(int seed=5879);
        virtual ~BeamTiming();

        void DrawInteractionTime(float&, int&);
        
    private:
        TRandom3 *fRnd;
};

#endif
