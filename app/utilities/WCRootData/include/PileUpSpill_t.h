#pragma once

const int kMaxNuInt = 200;
const int kMaxTracks = 5000;

struct PileUpSpill_t
{
    unsigned int NNuIntTot;
    unsigned int NIDNuInt;
    unsigned int NBeamBkg;
    unsigned int NuIntType[kMaxNuInt];
    float NuIntTime[kMaxNuInt];  
    unsigned int NuIntBunch[kMaxNuInt];
    int Mode[kMaxNuInt];
    int NuPDG[kMaxNuInt];
    float NuEne[kMaxNuInt];
    float NuPos[kMaxNuInt][3];
    float NuDir[kMaxNuInt][3];
    float EdepoOD[kMaxNuInt];
    unsigned int FileIdNum[kMaxNuInt];
    unsigned int EventIdNum[kMaxNuInt];
    
    unsigned int NTracks;
    unsigned int TrackNuIntIdx[kMaxTracks];

    void Clear()
    {
        NNuIntTot = 0;
        NIDNuInt = 0;
        NBeamBkg = 0;
        for(int i=0; i<kMaxNuInt; i++)
        {
            NuIntType[i] = 99999;
            NuIntTime[i] = -99999.;
            NuIntBunch[i] = 99999;
            EdepoOD[i] = -99999.;
            Mode[i] = -99999;
            NuPDG[i] = -99999;
            NuEne[i] = -99999.; 
            for(int j=0; j<3; j++)
            {
                NuPos[i][j] = -99999.;
                NuDir[i][j] = -99999.;
            }
            FileIdNum[i] = 99999;
            EventIdNum[i] = 99999;
        } 
        NTracks = 0;
        for(int i=0; i<kMaxTracks; i++)
        {
            TrackNuIntIdx[i] = 99999;
        }
    }

    void CreateBranch(TTree *t)
    {
        t->Branch("NNuIntTot",      &NNuIntTot,     "NNuIntTot/i");
        t->Branch("NIDNuInt",       &NIDNuInt,      "NIDNuInt/i");
        t->Branch("NBeamBkg",       &NBeamBkg,      "NBeamBkg/i");
        t->Branch("NuIntType",      NuIntType,      "NuIntType[NNuIntTot]/i");
        t->Branch("NuIntTime",      NuIntTime,      "NuIntTime[NNuIntTot]/F");
        t->Branch("NuIntBunch",     NuIntBunch,     "NuIntBunch[NNuIntTot]/i");
        t->Branch("NuPDG",     NuPDG,     "NuPDG[NNuIntTot]/I");
        t->Branch("NuEne",     NuEne,     "NuEne[NNuIntTot]/F");
        t->Branch("NuPos",     NuPos,     "NuPos[NNuIntTot][3]/F");
        t->Branch("NuDir",     NuDir,     "NuDir[NNuIntTot][3]/F");
        t->Branch("EdepoOD",        EdepoOD,        "EdepoOD[NNuIntTot]/F");
        t->Branch("FileIdNum",      FileIdNum,      "FileIdNum[NNuIntTot]/i");
        t->Branch("EventIdNum",     EventIdNum,     "EventIdNum[NNuIntTot]/i");
        t->Branch("NTracks",        &NTracks,       "NTracks/i");
        t->Branch("TrackNuIntIdx",  TrackNuIntIdx,  "TrackNuIntIdx[NTracks]/i");
    }

    void SetBranch(TTree *t)
    {
        t->SetBranchAddress("NNuIntTot",        &NNuIntTot);
        t->SetBranchAddress("NIDNuInt",         &NIDNuInt);
        t->SetBranchAddress("NBeamBkg",         &NBeamBkg);
        t->SetBranchAddress("NuIntType",        NuIntType);
        t->SetBranchAddress("NuIntTime",        NuIntTime);
        t->SetBranchAddress("NuIntBunch",       NuIntBunch);
        t->SetBranchAddress("Mode",             Mode);
        t->SetBranchAddress("NuPDG",            NuPDG);
        t->SetBranchAddress("NuEne",            NuEne);
        t->SetBranchAddress("NuPos",            NuPos);
        t->SetBranchAddress("NuDir",            NuDir);
        t->SetBranchAddress("EdepoOD",          EdepoOD);
        t->SetBranchAddress("FileIdNum",        FileIdNum);
        t->SetBranchAddress("EventIdNum",       EventIdNum);
        t->SetBranchAddress("NTracks",          &NTracks);
        t->SetBranchAddress("TrackNuIntIdx",    TrackNuIntIdx);
    }
};
