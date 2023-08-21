#include "TriggerInfo.h"

TriggerInfo::TriggerInfo()
{
    this->Clear();
}

TriggerInfo::~TriggerInfo()
{
    this->Clear();
}

void TriggerInfo::AddTrigger(float timeTrig, 
                             float timeLow, 
                             float timeUp, 
                             int nHits,
                             int type)
{
    fTrigTime.push_back(timeTrig); 
    fTimeLow.push_back(timeLow);
    fTimeUp.push_back(timeUp);
    fNHits.push_back(nHits);
    fTrigType.push_back(type);
}

void TriggerInfo::Clear()
{
    fTrigTime.clear();
    fTimeLow.clear();
    fTimeUp.clear();
    fNHits.clear();
    fTrigType.clear();
    vector<float>().swap(fTrigTime);
    vector<float>().swap(fTimeLow);
    vector<float>().swap(fTimeUp);
    vector<int>().swap(fNHits);
    vector<int>().swap(fTrigType);
}
