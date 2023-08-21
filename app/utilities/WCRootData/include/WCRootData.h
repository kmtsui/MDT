#ifndef WCROOTDATA_H
#define WCROOTDATA_H

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "TBranch.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TFile.h"
#include "TTree.h"

#include "TNRooTrackerVtx.hh"
#include "WCSimRootEvent.hh"
#include "WCSimRootGeom.hh"

#include "MDTManager.h"

using std::cout;
using std::endl;
using std::map;
using std::pair;
using std::string;
using std::vector;

class WCRootData {
	public:

		WCRootData();
		virtual ~WCRootData();

		void ReadFile(const char*, const vector<string>& v = vector<string>());
		void CloseFile();

		void CreateTree(const char*, const vector<string>& v = vector<string>());
		virtual void FillTree();
		virtual void WriteTree();

		void GetGeometryInfo(const char*);
		void CopyTree(const char*, const char*, const vector<bool>& v = vector<bool>());

		void SetOutFileName(const char* f) { fOutFileName = TString(f); }

		int GetEntries();
		void GetEntry(int);

		WCSimRootTrigger* GetTrigger(int i, int iPMT = 0) const { return fSpEvt[iPMT]->GetTrigger(i); }

		void AddTrueHitsToMDT(MDTManager*);
		void AddTrueHitsToMDT(HitTubeCollection*, PMTResponse*, float offset_time = 0., int iPMT = 0);
		void AddDigiHits(MDTManager*, int event_id = 0, int iPMT = 0);
		void AddDigiHits(HitTubeCollection* hc, TriggerInfo* ti, int even_it = 0, int iPMT = 0);
		void AddTracks(const WCSimRootTrigger*, float offset_time = 0., int iPMT = 0);

		const char* GetCurrentInputFileName() const { return fWCSimC->GetFile()->GetName(); }

	protected:

		TChain* fWCSimC;
		TTree* fWCSimT;
		vector<WCSimRootEvent*> fSpEvt;
		WCSimRootGeom* fWCGeom;

		Float_t fDetCentreY;

		float fHitTimeOffset;
		bool fMultDigiHits;

	private:

		void SetTubes(HitTubeCollection*, const int);
		TString fOutFileName;
};

#endif
