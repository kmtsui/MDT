#include "MDTManager.h"

MDTManager::MDTManager(int seed)
{
    fRndm = new MTRandom( seed );
    fTrigAlgo = new TriggerAlgo();
    
    Configuration *Conf = Configuration::GetInstance();
    int UsemPMTDigitizer = 0;
    Conf->GetValue<int>("UsemPMTDigitizer", UsemPMTDigitizer);
    if (!UsemPMTDigitizer) fDgtzr = new HitDigitizer( fRndm->Integer(1000000) );
    else fDgtzr = new HitDigitizer_mPMT( fRndm->Integer(1000000) );

    fPMTResp.clear();
    fDark.clear();
    fPHC.clear();
    fTrigInfo.clear();
}

MDTManager::~MDTManager()
{
    if( fTrigAlgo ){ delete fTrigAlgo; fTrigAlgo = NULL; }
    if( fRndm ){ delete fRndm; fRndm = NULL; }

    map<string, PMTResponse*>::iterator iPMTResp;
    for(iPMTResp=fPMTResp.begin(); iPMTResp!=fPMTResp.end(); iPMTResp++)
    {
        std::cout<<" Deleting" << iPMTResp->first <<std::endl;
        delete iPMTResp->second; iPMTResp->second = NULL;
    }
    fPMTResp.clear();

    map<string, PMTNoise*>::iterator iDark;
    for(iDark=fDark.begin(); iDark!=fDark.end(); iDark++)
    {
        delete iDark->second; iDark->second = NULL;
    }
    fDark.clear();

    map<string, HitTubeCollection*>::iterator iPHC; 
    for(iPHC=fPHC.begin(); iPHC!=fPHC.end(); iPHC++)
    {
        delete iPHC->second; iPHC->second = NULL;
    }
    fPHC.clear();

    map<string, TriggerInfo*>::iterator iTrigInfo;
    for(iTrigInfo=fTrigInfo.begin(); iTrigInfo!=fTrigInfo.end(); iTrigInfo++)
    {
        delete iTrigInfo->second; iTrigInfo->second = NULL;
    }
    fTrigInfo.clear();
}

void MDTManager::DoAddDark(const string &pmtname)
{
    if( this->HasThisPMTType(pmtname) )
    {
        fDark[pmtname]->AddPhotoElectrons(fPHC[pmtname]);
    }
}

void MDTManager::DoDigitize(const string &pmtname)
{
    if( this->HasThisPMTType(pmtname) )
    {
        fDgtzr->Digitize(fPHC[pmtname], fPMTResp[pmtname]);
        //cout<<" # true hits: " << fPHC[pmtname]->GetTotalNumOfTrueHits()
		//	<<" # digitized hits: " << fPHC[pmtname]->GetTotalNumOfDigiHits()
		//	<<endl;
    }
}

void MDTManager::DoTrigger(const string &pmtname)
{
    if( this->HasThisPMTType(pmtname) )
    {
        fTrigAlgo->NDigits(fPHC[pmtname], fTrigInfo[pmtname]);
    }
}

void MDTManager::DoAddAfterpulse(const string &pmtname)
{
    if( this->HasThisPMTType(pmtname) )
    {
        fDark[pmtname]->AddAfterpulse(fPHC[pmtname], fDgtzr, fPMTResp[pmtname]);
    }
}

void MDTManager::DoInitialize()
{
    map<string, TriggerInfo*>::iterator iTrigInfo;
    for(iTrigInfo=fTrigInfo.begin(); iTrigInfo!=fTrigInfo.end(); iTrigInfo++)
    {
        iTrigInfo->second->Clear();
    }

    map<string, HitTubeCollection*>::iterator iPHC; 
    for(iPHC=fPHC.begin(); iPHC!=fPHC.end(); iPHC++)
    {
        iPHC->second->Clear();
    }
}

void MDTManager::SetHitTubeCollection(HitTubeCollection *hc, const string &pmtname)
{
    if( this->HasThisPMTType(pmtname) )
    {
        if( fPHC[pmtname] ){ delete fPHC[pmtname]; fPHC[pmtname] = NULL; }
        fPHC[pmtname] = hc;
    }
}

void MDTManager::RegisterPMTType(const string &pmtname, PMTResponse *pmtResp)
{
    fTrigInfo[pmtname] = new TriggerInfo();
    fPHC[pmtname] = new HitTubeCollection();
    fDark[pmtname] = new PMTNoise(fRndm->Integer(1000000), pmtname);

    if( pmtResp==0 ){ fPMTResp[pmtname] = new GenericPMTResponse(); }
    else{ fPMTResp[pmtname] = pmtResp; }
    fPMTResp[pmtname]->Initialize(fRndm->Integer(10000000), pmtname);
}

bool MDTManager::HasThisPMTType(const string &pmtname)
{
    bool b = true;
    if( fPHC.count(pmtname)==0 )
    {   
        std::cout<<" [ERROR] MDTManager::HasPMTType " <<std::endl;
        std::cout<<" PMT type: " << pmtname <<" is not registered" <<endl;
        b = false;
    }
    return b;
}
