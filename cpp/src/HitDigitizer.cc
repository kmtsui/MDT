#include "HitDigitizer.h"
#include "Configuration.h"
        
HitDigitizer::HitDigitizer(int seed) :
fPrecisionCharge(0.), 
fPrecisionTiming(0.1),
fEfficiency( 0.985 ),
fIntegWindow( 200. ),
fApplyEff( 1 )
{
    fRand = new MTRandom( seed );
    Configuration *Conf = Configuration::GetInstance();
    Conf->GetValue<float>("DigiHitIntegrationWindow", fIntegWindow);
    Conf->GetValue<float>("PrecisionTiming", fPrecisionTiming);
    Conf->GetValue<float>("PrecisionCharge", fPrecisionCharge);
    Conf->GetValue<int>("ApplyDAQEfficiency", fApplyEff);
}

HitDigitizer::~HitDigitizer()
{
    if( fRand ){ delete fRand; fRand = NULL; }
}

void HitDigitizer::Digitize(HitTubeCollection *hc, PMTResponse *pr)
{
    for(hc->Begin(); !hc->IsEnd(); hc->Next())
    {
        if( (&(*hc)())->GetNRawPE()>0 )
        {
            this->DigitizeTube(&(*hc)(), pr);
        }
    }
}

// Based on WCSimWCDigitizerSKI::DigitizeHits in WCSimWCDigitizer.cc
void HitDigitizer::DigitizeTube(HitTube *aHT, PMTResponse *pr)
{
    aHT->SortTrueHits();
    const int NPE = aHT->GetNRawPE();
    const vector<TrueHit*> PEs = aHT->GetPhotoElectrons();

    // Taken from WCSimWCDigitizerSKI::DigitizeHits
    double sumSPE = 0.;
    double tSmeared = -9999.;
    bool isAlive = false;
    double digiT = 0.;
    double digiQ = 0.;

    double intgr_srt = (double)PEs[0]->GetTime();
    double intgr_end = intgr_srt+fIntegWindow;
    vector<int> parent_composition;  
    parent_composition.clear();

    for(int iPE=0; iPE<NPE; iPE++)
    {
        if( PEs[iPE]->GetTime()>=intgr_srt && PEs[iPE]->GetTime()<intgr_end )
        {
            sumSPE += pr->GetRawSPE(PEs[iPE], aHT);
            parent_composition.push_back( PEs[iPE]->GetParentId() );
        }
        else
        {
            digiT = intgr_srt;
            digiQ = sumSPE;

            isAlive = true;
            if (fApplyEff ){ this->ApplyThreshold(digiQ, isAlive); }
            if( isAlive ) 
            {
                tSmeared = pr->HitTimeSmearing(digiQ);
                digiQ *= fEfficiency;
                digiT += tSmeared;
                digiQ = this->DoTruncate(digiQ, fPrecisionCharge);
                digiT = this->DoTruncate(digiT, fPrecisionTiming);
                aHT->AddDigiHit(digiT, digiQ, parent_composition);
            }
            sumSPE = 0.;
            parent_composition.clear(); 

            intgr_srt = PEs[iPE]->GetTime();
            intgr_end = intgr_srt+fIntegWindow;
            sumSPE += pr->GetRawSPE(PEs[iPE], aHT);
            parent_composition.push_back( PEs[iPE]->GetParentId() );
        }
    }

    digiT = intgr_srt;
    digiQ = sumSPE;
    tSmeared = pr->HitTimeSmearing(digiQ);
    isAlive = true;
    if (fApplyEff ){ this->ApplyThreshold(digiQ, isAlive); }
    if( isAlive )
    {
        digiQ *= fEfficiency;
        digiT += tSmeared ;
        digiQ = this->DoTruncate(digiQ, fPrecisionCharge);
        digiT = this->DoTruncate(digiT, fPrecisionTiming);
        aHT->AddDigiHit(digiT, digiQ, parent_composition);
    }
}


void HitDigitizer::ApplyThreshold(double& pe, bool& pass)
{
// Apply DAQ efficiency
// Taken from WCSimWCDigitizerSKI::Threshold 
// in WCSimWCDigitizer.hh
//
// NOTE: the input charge, pe, will be altered in this function
    pass=true;
    double x=pe+0.1;
    double thr=0.;
    if( x<1.1 )
    {
      thr = std::min(1.0,
		     -0.06374+x*(3.748+x*(-63.23+x*(452.0+x*(-1449.0+x*(2513.0
									+x*(-2529.+x*(1472.0+x*(-452.2+x*(51.34+x*2.370))))))))));
    } 
    else 
    {
      thr = 1.0;
    }

    if( thr<fRand->Rndm() )
    {
        pe=0.;
        pass=false;
    }
    else
    {
        pe += fRand->Gaus(0., 0.03);
    }
}

double HitDigitizer::DoTruncate(const double x, const double precision)
{
// Based on WCSimWCDigitizerBase::AddNewDigit in WCSimWCDigitizer.hh
// digitised hit information does not have infinite precision
// so need to round the charge and time information
//
// The following is based on WCSimWCDigitizerBas::Truncate
// in WCSimWCDigitizer.hh
    if(precision < 1E-10){ return x; }
    return precision * (int)(x/precision); 
}
