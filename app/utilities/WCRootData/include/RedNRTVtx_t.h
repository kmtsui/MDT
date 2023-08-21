#pragma once

#include "TTree.h"

const int kMaxIntVtx = 1000;

struct RedNRTVtx_t
{
    Int_t NIntVtx;
    Int_t Mode[kMaxIntVtx]; 
    Float_t NuEne[kMaxIntVtx];
    Int_t NuPDG[kMaxIntVtx];
    Float_t NuPos[kMaxIntVtx][3];
    Float_t NuDir[kMaxIntVtx][3];
    Float_t LepDir[kMaxIntVtx][3]; 
    Float_t LepMom[kMaxIntVtx];

    void Clear()
    {
        NIntVtx = 0;
        for(Int_t i=0; i<kMaxIntVtx; i++)
        {
            Mode[i] = -9999; 
            NuEne[i] = -9999.;
            NuPDG[i] = -9999;
            for(Int_t j=0; j<3; j++)
            {
                NuPos[i][j] = -9999.;
                NuDir[i][j] = -9999.;
                LepDir[i][j] = -9999.; 
            }
            LepMom[i] = -9999.;
        }
    }

    void AddVertex(const RedNRTVtx_t &r, const int i)
    {
        Mode[NIntVtx] = r.Mode[i]; 
        NuEne[NIntVtx] = r.NuEne[i];
        NuPDG[NIntVtx] = r.NuPDG[i];
        for(Int_t j=0; j<3; j++)
        {
            NuPos[NIntVtx][j] = r.NuPos[i][j];
            NuDir[NIntVtx][j] = r.NuDir[i][j];
            LepDir[NIntVtx][j] = r.LepDir[i][j]; 
        }
        LepMom[NIntVtx] = r.LepMom[i];
        NIntVtx += 1;
    }
    
    void CreateBranch(TTree *t)
    {
        t->Branch("NIntVtx", &NIntVtx, "NIntVtx/I");
        t->Branch("Mode",    Mode,     "Mode[NIntVtx]/I");
        t->Branch("NuPDG",   NuPDG,    "NuPDG[NIntVtx]/I");
        t->Branch("NuEne",   NuEne,    "NuEne[NIntVtx]/F");
        t->Branch("NuPos",   NuPos,    "NuPos[NIntVtx][3]/F");
        t->Branch("NuDir",   NuDir,    "NuDir[NIntVtx][3]/F");
        t->Branch("LepDir",  LepDir,   "LepDir[NIntVtx][3]/F");
        t->Branch("LepMom",  LepMom,   "LepMom[NIntVtx]/F");
    }

    void SetBranch(TTree *t)
    {
        t->SetBranchAddress("NIntVtx", &NIntVtx);
        t->SetBranchAddress("Mode",    Mode);
        t->SetBranchAddress("NuPDG",   NuPDG);
        t->SetBranchAddress("NuEne",   NuEne);
        t->SetBranchAddress("NuPos",   NuPos);
        t->SetBranchAddress("NuDir",   NuDir);
        t->SetBranchAddress("LepDir",  LepDir);
        t->SetBranchAddress("LepMom",  LepMom);
    }
};


