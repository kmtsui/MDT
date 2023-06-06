#include "WCRootData.h"
#include "Configuration.h"

#include "TBranch.h"

WCRootData::WCRootData()
{
    fDetCentreY = 0.;
    fWCGeom = 0;
    fWCSimC = 0;
    fWCSimT = 0;
    fSpEvt.clear();
    fOutFileName = "";

    int mult_flag = 1;
    fHitTimeOffset = 0.;

    Configuration *Conf = Configuration::GetInstance();
    Conf->GetValue<float>("TimeOffset", fHitTimeOffset);
    Conf->GetValue<int>("FlagMultDigits", mult_flag);

    fMultDigiHits = bool(mult_flag);

    fWCSimDigiWFT = 0;
    fWCSimDigiPulls = 0;
    fPullQ = -99.;
    fPullT = -99.;
    fTrueQ = -99.;
    fTrueT = -99.;
}

WCRootData::~WCRootData()
{
    if( fWCGeom ){ delete fWCGeom; fWCGeom = 0; }
    if( fWCSimT ){ delete fWCSimT; fWCSimT = 0; }
    if( fWCSimC ){ delete fWCSimC; fWCSimC = 0; }
    if( fWCSimDigiWFT ){ delete fWCSimDigiWFT; fWCSimDigiWFT = 0; }
    if( fWCSimDigiPulls ){ delete fWCSimDigiPulls; fWCSimDigiPulls = 0; }
    fSpEvt.clear();
    fSpEvt.shrink_to_fit();
}


void WCRootData::GetGeometryInfo(const char *filename)
{
    TFile *f=TFile::Open(filename);
    TTree *t=(TTree*)f->Get("wcsimGeoT");
    t->SetBranchAddress("wcsimrootgeom", &fWCGeom);
    t->GetEntry( 0 );
    fDetCentreY = fWCGeom->GetWCOffset(1);
    cout<<" DetCentreY: " << fDetCentreY <<endl;
    f->Close();
}


void WCRootData::AddTrueHitsToMDT(MDTManager *mdt)
{
    HitTubeCollection *hc = mdt->GetHitTubeCollection();
    this->AddTrueHitsToMDT(hc, mdt->GetPMTResponse(), 0., 0);
    hc = 0;
}


void WCRootData::AddTrueHitsToMDT(HitTubeCollection *hc, PMTResponse *pr, float intTime, int iPMT)
{
	// Set all tubes no matter how many of true hits, anyway
	// Position and orientation are also set to each tube
	this->SetTubes(hc, iPMT);

    const WCSimRootTrigger *aEvt = fSpEvt[iPMT]->GetTrigger(0);
    const int nCkovHits = aEvt->GetNcherenkovhits();
	//std::cout<<" nCkovHits: " << nCkovHits <<std::endl;
	float pmt_position[3];
	float pmt_orientation[3];
    TClonesArray *hitTimeArray = aEvt->GetCherenkovHitTimes();
    for(int iHit=0; iHit<nCkovHits; iHit++)
    {
        WCSimRootCherenkovHit *aHit=
                dynamic_cast<WCSimRootCherenkovHit*>((aEvt->GetCherenkovHits())->At(iHit));
    
        int timeArrayIndex = aHit->GetTotalPe(0);
        int peForTube = aHit->GetTotalPe(1);   
        int tubeID = aHit->GetTubeID();
        const int maxPE = timeArrayIndex + peForTube;
        for(int jPE=timeArrayIndex; jPE<maxPE; jPE++)
        {
            WCSimRootCherenkovHitTime *aHitTime=
                dynamic_cast<WCSimRootCherenkovHitTime*>(hitTimeArray->At(jPE));
    
            float truetime = aHitTime->GetTruetime();
            if( truetime<0. ){ continue; }
            if( aHitTime->GetParentID()<0 ){ continue; }

            TrueHit *th = new TrueHit(truetime, aHitTime->GetParentID());
#ifdef HYBRIDWCSIM
            for(int k=0; k<3; k++){ th->SetPosition(k, aHitTime->GetPhotonEndPos(k)); }
            for(int k=0; k<3; k++){ th->SetDirection(k, aHitTime->GetPhotonEndDir(k)); }
            for(int k=0; k<3; k++){ th->SetStartDirection(k, aHitTime->GetPhotonStartDir(k)); }
#endif
            th->SetStartTime(aHitTime->GetPhotonStartTime());
            for(int k=0; k<3; k++){ th->SetStartPosition(k, aHitTime->GetPhotonStartPos(k)); }
            if( !pr->ApplyDE(th) ){ continue; }

            (&(*hc)[tubeID])->AddRawPE(th);
        }
    }
}

void WCRootData::ReadFile(const char *filename, const vector<string> &list)
{
    fWCSimC = new TChain("wcsimT");
    fWCSimC->Add(filename);

	const char *filename0 = NULL;
	filename0 = fWCSimC->GetFile()->GetName();
	if( filename0!=NULL )
	{
		this->GetGeometryInfo(filename0);
	}

    if( list.size()==0 ) // default
    {
        fSpEvt.push_back( 0 );
        fWCSimC->SetBranchAddress("wcsimrootevent", &fSpEvt[0]);
    }
    else
    {
        fSpEvt.clear();
        fSpEvt = vector<WCSimRootEvent*>(list.size(),0);
        for(unsigned int i=0; i<list.size(); i++)
        {
            fWCSimC->SetBranchAddress(list[i].c_str(), &fSpEvt[i]);
        }
    }
    fWCSimC->SetAutoDelete();
}

void WCRootData::CloseFile()
{
    delete fWCSimC; fWCSimC = 0;
    fSpEvt.clear();
    fSpEvt.shrink_to_fit();
}

int WCRootData::GetEntries() 
{
    return fWCSimC->GetEntries();
}

void WCRootData::GetEntry(int i)
{
    fWCSimC->GetEntry(i);
}


void WCRootData::CreateTree(const char *filename, const vector<string> &list)
{
    fOutFileName = TString(filename);
    TFile *fout = new TFile(fOutFileName, "recreate");
    fout->SetCompressionLevel(2);

    Int_t branchStyle = 1;
    Int_t bufferSize = 64000;
    TString bName = "wcsimrootevent";
    TString bAddress = "WCSimRootEvent";

    fWCSimT = new TTree("wcsimT", "Processed WCSim Tree for pile-up");
    if( list.size()==0 )
    {
        fSpEvt.push_back( new WCSimRootEvent() );
        fSpEvt[0]->Initialize();
        TTree::SetBranchStyle(branchStyle);
        TBranch *branch = fWCSimT->Branch(bName, bAddress, &fSpEvt[0], bufferSize, 2);
    }
    else  
    {
        fSpEvt.clear();
        fSpEvt = vector<WCSimRootEvent*>(list.size(), 0);
        for(unsigned int i=0; i<list.size(); i++)
        {
//            fSpEvt.push_back( new WCSimRootEvent() );
            fSpEvt[i] = new WCSimRootEvent();
            fSpEvt[i]->Initialize();
            TTree::SetBranchStyle(branchStyle);
            TBranch *branch = fWCSimT->Branch(list[i].c_str(), bAddress, &fSpEvt[i], bufferSize, 2);
        }
    }

    fWCSimDigiWFT = new TTree("wcsimDigiWFTree","Digitized waveform for each PMT");
    fDigiWF = new TClonesArray("TH1F");
    fWCSimDigiWFT->Branch("wcsimDigiWF",&fDigiWF);
#ifdef HYBRIDWCSIM
    fDigiWF2 = new TClonesArray("TH1F");
    fWCSimDigiWFT->Branch("wcsimDigiWF2",&fDigiWF2);
#endif
    fWCSimDigiPulls = new TTree("WCSimDigiPulls","Time and charge pulls of digitized hits");
    fWCSimDigiPulls->Branch("PullQ",&fPullQ);
    fWCSimDigiPulls->Branch("PullT",&fPullT);
    fWCSimDigiPulls->Branch("TrueQ",&fTrueQ);
    fWCSimDigiPulls->Branch("TrueT",&fTrueT);
}

void WCRootData::AddDigiHits(MDTManager *mdt, int eventID, int iPMT)
{
    HitTubeCollection *hc = mdt->GetHitTubeCollection();
    TriggerInfo *ti = mdt->GetTriggerInfo();
    this->AddDigiHits(hc, ti, eventID, iPMT);
}

void WCRootData::AddDigiHits(HitTubeCollection *hc, TriggerInfo *ti, int eventID, int iPMT)
{
    WCSimRootTrigger* anEvent = fSpEvt[iPMT]->GetTrigger(0);
    const int nTriggers = ti->GetNumOfTrigger(); 
    for(int iTrig=0; iTrig<nTriggers; iTrig++) 
    {
        TriggerType_t trigType = kTriggerNDigits;
        if( ti->GetType(iTrig)==(int)TriggerType::eFailure )
        {
            trigType = kTriggerFailure; 
        }

        if( iTrig>=1 )
        {
            fSpEvt[iPMT]->AddSubEvent();
            anEvent = fSpEvt[iPMT]->GetTrigger(iTrig);
            anEvent->SetHeader(eventID, 0, 0, iTrig+1);
            anEvent->SetMode(0);
        }

#ifdef HYBRIDWCSIM
        vector<Double_t> info(1, ti->GetNHits(iTrig));
#else
        vector<Float_t> info(1, ti->GetNHits(iTrig));
#endif
        anEvent->SetTriggerInfo(trigType, info);

        const float triggerTime = ti->GetTriggerTime(iTrig);
        const float tWinLowEdge = ti->GetLowEdge(iTrig);
        const float tWinUpEdge = ti->GetUpEdge(iTrig);

        int nHits = 0;
        float sumQ = 0.;
        map<int, bool> tubeCount;
        for(hc->Begin(); !hc->IsEnd(); hc->Next())
        {
            HitTube *aPH = &(*hc)();
            int tubeID = aPH->GetTubeID();
            int nCount = 0;
            for(int i=0; i<aPH->GetNDigiHits(); i++)
            {
                float t = aPH->GetTimeDigi(i);

                // Need to be updated 
                int mPMT_module_id = 0;
                int mPMT_pmt_id = 0;
                if( t>=tWinLowEdge && t<=tWinUpEdge )
                {
                    bool doFill = false;
                 
                    // The first hit is always allowed to be saved.
                    // Later hits depend on your setting
                    if( tubeCount.count(tubeID)==0 ){ doFill = true; } 
                    else { doFill = fMultDigiHits ? true : false; }  

                    if( doFill ) 
                    {
                        vector<int> true_pe_comp = aPH->GetParentCompositionDigi(i);
                        t = t +  fHitTimeOffset - triggerTime;
                        float q = aPH->GetChargeDigi(i);
                        anEvent->AddCherenkovDigiHit(q,
                                                     t,
                                                     tubeID,
                                                     mPMT_module_id,
                                                     mPMT_pmt_id,
                                                     true_pe_comp);
                        nHits += 1;
                        sumQ += q;
                        nCount+=1;
                        tubeCount[tubeID] = 1;
                    }
                }
            } // digi hit loop
            aPH = NULL;
        } // PMT loop
        anEvent->SetNumDigitizedTubes(nHits); 
        anEvent->SetSumQ(sumQ);
        WCSimRootEventHeader *eh = anEvent->GetHeader();
        eh->SetDate( int(triggerTime) );
    }
#ifdef HYBRIDWCSIM
    TClonesArray &fDigiWFarray;
    if (!bool(iPMT)) fDigiWFarray = *fDigiWF;
    else fDigiWFarray = *fDigiWF2;
#else
    TClonesArray &fDigiWFarray = *fDigiWF;
#endif

    for(hc->Begin(); !hc->IsEnd(); hc->Next())
    {
        HitTube *aPH = &(*hc)();
        if (aPH->GetDigiWF())
            new(fDigiWFarray[aPH->GetTubeID()-1]) TH1F(*(aPH->GetDigiWF()));
        else new(fDigiWFarray[aPH->GetTubeID()-1]) TH1F();

        fPullQ = aPH->GetPullQ();
        fPullT = aPH->GetPullT();
        fTrueQ = aPH->GetTrueQ();
        fTrueT = aPH->GetTrueT();
        fWCSimDigiPulls->Fill();

        aPH = NULL;
    } // PMT loop
}

void WCRootData::FillTree()
{
    int tmp = (int)fSpEvt.size();
    TFile *f = fWCSimT->GetCurrentFile();
    f->cd();
    fWCSimT->Fill();     
    fWCSimT->Write("",TObject::kOverwrite);
    for(unsigned int i=0; i<fSpEvt.size(); i++)
    {
        fSpEvt[i]->ReInitialize();
    }
    fWCSimDigiWFT->Fill();
    fDigiWF->Clear();
#ifdef HYBRIDWCSIM
    fDigiWF2->Clear();
#endif
}


void WCRootData::AddTracks(const WCSimRootTrigger *aEvtIn, float offset_time, int iPMT)
{
    WCSimRootTrigger *aEvtOut = fSpEvt[iPMT]->GetTrigger(0);
    TClonesArray *tracks = aEvtIn->GetTracks();
    const int ntrack = tracks->GetEntries();
    for(int i=0; i<ntrack; i++)
    {
        WCSimRootTrack *aTrack = (WCSimRootTrack*)tracks->At(i);
        Int_t     ipnu = aTrack->GetIpnu();
        Int_t     flag = aTrack->GetFlag();
        Int_t     startvol = aTrack->GetStartvol();
        Int_t     stopvol = aTrack->GetStopvol();
        Int_t     parenttype = aTrack->GetParenttype();
        Int_t     id = aTrack->GetId();
#ifdef HYBRIDWCSIM
        Int_t     idPrnt = aTrack->GetParentId();
#endif


#ifdef HYBRIDWCSIM
        Double_t   dir[3];
        Double_t   pdir[3];
        Double_t   stop[3];
        Double_t   start[3];
        Double_t   m = aTrack->GetM();
        Double_t   p = aTrack->GetP();
        Double_t   E = aTrack->GetE();
        Double_t   time = aTrack->GetTime() + offset_time;
#else 
        Float_t   dir[3];
        Float_t   pdir[3];
        Float_t   stop[3];
        Float_t   start[3];
        Float_t   m = aTrack->GetM();
        Float_t   p = aTrack->GetP();
        Float_t   E = aTrack->GetE();
        Float_t   time = aTrack->GetTime() + offset_time;
#endif
        for(int j=0; j<3; j++)
        {
            dir[j] = aTrack->GetDir(j); 
            pdir[j] = aTrack->GetPdir(j);
            stop[j] = aTrack->GetStop(j);
            start[j] = aTrack->GetStart(j);
        }

#ifdef HYBRIDWCSIM
        aEvtOut->AddTrack(ipnu, 
			  flag, 
			  m, 
			  p, 
			  E, 
			  startvol, 
			  stopvol, 
			  dir, 
			  pdir, 
			  stop,
			  start,
			  parenttype,
			  time,
			  id,
			  idPrnt);
#else
        aEvtOut->AddTrack(ipnu, 
			  flag, 
			  m, 
			  p, 
			  E, 
			  startvol, 
			  stopvol, 
			  dir, 
			  pdir, 
			  stop,
			  start,
			  parenttype,
			  time,
			  id);
#endif

  WCSimRootTrack         *AddTrack(Int_t ipnu, 
				   Int_t flag, 
				   Float_t m, 
				   Float_t p, 
				   Float_t E, 
				   Int_t startvol, 
				   Int_t stopvol, 
				   Float_t dir[3], 
				   Float_t pdir[3], 
				   Float_t stop[3],
				   Float_t start[3],
				   Int_t parenttype,
				   Float_t time,
				   Int_t id);


    }
}

void WCRootData::WriteTree()
{
    TFile *f = fWCSimT->GetCurrentFile();
    f->cd();
    fWCSimT->Write();
    fWCSimDigiWFT->Write();
    fWCSimDigiPulls->Write();
    f->Close();
}


void WCRootData::CopyTree(const char *filename,
                          const char *treename,
                          const vector<bool> &savelist)
{

	TFile *fin = TFile::Open(filename);
	if( !fin->FindKey(treename) )
	{
		fin->Close();
		std::cout<<" [ERROR] WCRootData::CopyTree " <<std::endl;
		std::cout<<"  - No " << treename <<" tree is saved in : " << filename <<std::endl;
        exit(-1);
	}

    // Copy all the entries
    if( savelist.size()==0 )
    {
        if( treename!="Settings" )
        {
            TTree *tin = (TTree*)fin->Get(treename);

            TFile *fout = TFile::Open(fOutFileName, "update");
            fout->cd();
            TTree *tout = tin->CloneTree(-1, "fast");
            tout->Write();
            fout->Close();
        }
        else // This is a special treatment
        {
            TTree *tin = (TTree*)fin->Get(treename);

            TBranch *bWCXRotation = tin->GetBranch("WCXRotation");
            TBranch *bWCYRotation = tin->GetBranch("WCYRotation");
            TBranch *bWCZRotation = tin->GetBranch("WCZRotation");
            TBranch *bWCDetCentre = tin->GetBranch("WCDetCentre");
            TBranch *bWCDetRadius = tin->GetBranch("WCDetRadius");
            TBranch *bWCDetHeight = tin->GetBranch("WCDetHeight");

            Float_t WCXRotation[3] = {0.};
            Float_t WCYRotation[3] = {0.};
            Float_t WCZRotation[3] = {0.};
            Float_t WCDetCentre[3] = {0.};
            Float_t WCDetRadius = 0.;
            Float_t WCDetHeight = 0.;

            bWCXRotation->SetAddress(WCXRotation);
            bWCYRotation->SetAddress(WCYRotation);
            bWCZRotation->SetAddress(WCZRotation);
            bWCDetCentre->SetAddress(WCDetCentre);
            bWCDetRadius->SetAddress(&WCDetRadius);
            bWCDetHeight->SetAddress(&WCDetHeight);

            bWCXRotation->GetEntry(0);
            bWCYRotation->GetEntry(0);
            bWCZRotation->GetEntry(0);
            bWCDetCentre->GetEntry(0);
            bWCDetRadius->GetEntry(0);
            bWCDetHeight->GetEntry(0);

            TFile *fout = TFile::Open(fOutFileName, "update");
            fout->cd();
            TTree *tout = new TTree("Settings", "");
            tout->Branch("WCXRotation", WCXRotation, "WCXRotation[3]/F");
            tout->Branch("WCYRotation", WCYRotation, "WCYRotation[3]/F");
            tout->Branch("WCZRotation", WCZRotation, "WCZRotation[3]/F");
            tout->Branch("WCDetCentre", WCDetCentre, "WCDetCentre[3]/F");
            tout->Branch("WCDetRadius", &WCDetRadius, "WCDetRadius/F");
            tout->Branch("WCDetHeight", &WCDetHeight, "WCDetHeight/F");
            tout->Fill();
            tout->Write();
            fout->Close();
        }
    }
    else
    {
        TTree *tin = (TTree*)fin->Get(treename);

        TFile *fout = TFile::Open(fOutFileName, "update");
        fout->cd();
        TTree *tout = tin->CloneTree(0);

        const int nEntries = tin->GetEntries();
        for(int iEntry=0; iEntry<nEntries; iEntry++)
        {
            tin->GetEntry(iEntry);
            if( !savelist[iEntry] ){ continue; }
            fout->cd();
            tout->Fill();
        }
        fout->cd();
        tout->Write();
        fout->Close();
    }
    fin->Close();
}

void WCRootData::SetTubes(HitTubeCollection *hc, const int iPMT)
{
#ifdef HYBRIDWCSIM
	const int nTubes = fWCGeom->GetWCNumPMT(bool(iPMT));
	for(int i=0; i<nTubes; i++)
	{
		const WCSimRootPMT *tube = fWCGeom->GetPMTPtr(i, bool(iPMT));

		const int tubeID = tube->GetTubeNo();
		hc->AddHitTube(tubeID);
		for(int j=0; j<3; j++)
		{
			(&(*hc)[tubeID])->SetPosition(j, tube->GetPosition(j));
			(&(*hc)[tubeID])->SetOrientation(j, tube->GetOrientation(j));
		}
	}
#else
	const int nTubes = fWCGeom->GetWCNumPMT();
	for(int i=0; i<nTubes; i++)
	{
  		WCSimRootPMT tube = fWCGeom->GetPMT(i);
		const int tubeID = tube.GetTubeNo();
		hc->AddHitTube(tubeID);
		for(int j=0; j<3; j++)
		{
			(&(*hc)[tubeID])->SetPosition(j, tube.GetPosition(j));
			(&(*hc)[tubeID])->SetOrientation(j, tube.GetOrientation(j));
		}
	}
#endif
}
