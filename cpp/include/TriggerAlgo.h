// Apply event trigger to a given set of dititized PMT hits (map<int, HitTube*>)
// The resultant triggers are saved in a TrggerInfo class object
#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

#include "HitTubeCollection.h"
#include "TriggerInfo.h"

using std::cout;
using std::endl;
using std::map;
using std::vector;

class TriggerAlgo
{
    public:
        TriggerAlgo();
        virtual ~TriggerAlgo(){};

        void NDigits(HitTubeCollection*, TriggerInfo*);
        float GetTriggerTimeForFailure() const { return fTriggerTimeForFailure; }

    private:       
        map<TriggerType,float> fPreTriggerWindow;
        map<TriggerType,float> fPostTriggerWindow;
        float fTriggerOffsetTime; 
        float fTriggerTimeForFailure;
        float fNDigitsWindow;
        float fNDigitsStepSize;
        int   fNDigitsThreshold;
};
