#include "SpillAnalyzer.h"

#include "BeamTiming.h"
#include "WCRootDataPileUpSpill.h"
#include "WCSimRootEvent.hh"

SpillAnalyzer::SpillAnalyzer()
{
}

SpillAnalyzer::~SpillAnalyzer()
{
}

bool SpillAnalyzer::IsSingleIDIneractionAtBunch(const WCRootDataPileUpSpill *input, const int bunch_id, int &nuIntIdx)
{
    nuIntIdx = -1;

    const PileUpSpill_t *pup = input->GetPileUpSpill();
    float EdepoODTot[kNBunches] = {0.};
    int nIDNuInt = 0;
    int nBeamBkg = 0;

    for(int i=0; i<pup->NNuIntTot; i++)
    {
        EdepoODTot[pup->NuIntBunch[i]] += pup->EdepoOD[i];
        if( pup->NuIntBunch[i]==bunch_id )
        {
            if( pup->NuIntType[i]==0 ){ nIDNuInt += 1; nuIntIdx = i; }
            else if( pup->NuIntType[i]==1 ){ nBeamBkg += 1; }

        }
    }

    if( !(EdepoODTot[bunch_id]<50. && nIDNuInt==1) ){ return false; }

    //cout<<" NNuIntTot:" << pup->NNuIntTot 
    //    <<" EdepoODTot["<< bunch_id <<"]: " << EdepoODTot[bunch_id]
    //    <<" nIDNuInt: " << nIDNuInt
    //    <<" nBeamBkg: " << nBeamBkg
    //    <<endl;
    return true;
}


bool SpillAnalyzer::HasMichelEFromOtherInteractionAtBunch(const WCRootDataPileUpSpill *input, 
                                                          const int nuIntIdx,
                                                          const float time_low,
                                                          const float time_up)
{
    const PileUpSpill_t *pup = input->GetPileUpSpill();
    const int bunch_id = pup->NuIntBunch[nuIntIdx];
     
    const WCSimRootTrigger *aEvt = input->GetTrigger(0);
    TClonesArray *tracks = aEvt->GetTracks();
    const int nTracks = tracks->GetEntries();
    if( nTracks!=pup->NTracks )
    {
        cout<<" Number of tracks are different " <<endl;
        cout<<"  - WCSim tree: " << nTracks <<endl;
        cout<<"  - PileUpSpill tree: " << pup->NTracks <<endl;
    }

    int nSIEleFromOtherNuInt = 0;
    for(int i=0; i<nTracks; i++)
    {
        if( pup->TrackNuIntIdx[i]==nuIntIdx )
        {
            // Tracks associated with interaction of interest
            continue;
        }

        WCSimRootTrack *aTrack = (WCSimRootTrack*)tracks->At(i);
        Int_t     ipnu = aTrack->GetIpnu();
        if( TMath::Abs( ipnu )!=11 ){ continue; }

        Float_t   time = aTrack->GetTime();
        if( time>time_low && time<time_up )
        {
            cout<<" Found " << ipnu 
                <<" at t=" << time 
                <<" time_low:" << time_low 
                <<" time_up: " << time_up 
                <<" TrackNuIntIdx: "<< pup->TrackNuIntIdx[i]
                <<" nuIntIdx: " << nuIntIdx
                <<endl;
            nSIEleFromOtherNuInt += 1;
        } 
    }

    if( nSIEleFromOtherNuInt==0 ){ return false; }
    //cout<<" nSIEleFromOtherNuInt: " << nSIEleFromOtherNuInt <<endl;
    return true;
}
