#include "BeamTiming.h"

BeamTiming::BeamTiming(int seed)
{
    fRnd = new TRandom3(seed);
}

BeamTiming::~BeamTiming()
{
    delete fRnd; fRnd = 0;
}

void BeamTiming::DrawInteractionTime(float &time, int &iBunch)
{
    iBunch = fRnd->Integer(kNBunches);    
    float bunchTime = fRnd->Gaus(0., kBunchSigma);
    time =(float)kBunchInterval*iBunch + bunchTime;
}
