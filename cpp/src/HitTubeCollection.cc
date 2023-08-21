#include "HitTubeCollection.h"

HitTubeCollection::HitTubeCollection() : fNTrueHitsTotal(0), fNDigiHitsTotal(0) {
	fPHs.clear();
	fTubeList.clear();
}

HitTubeCollection::~HitTubeCollection() {
	this->Clear();
}

void HitTubeCollection::Clear() {
	map<int, HitTube*>::iterator itr;
	for(itr = fPHs.begin(); itr != fPHs.end(); ++itr) {
		delete itr->second;
		itr->second = NULL;
	}
	fPHs.clear();
	vector<int>().swap(fTubeList);
}

void HitTubeCollection::AddHitTube(const int id) {
	if(fPHs.count(id) == 0) {
		fPHs[id] = new HitTube(id);
		fTubeList.push_back(id);
	}
}

void HitTubeCollection::AddTrueHit(const int tube_id, const float time, const int parent_id) {
	this->AddHitTube(tube_id);
	fPHs[tube_id]->AddRawPE(time, parent_id);
}

void HitTubeCollection::AddDigiHit(const int id, const float time, const float charge) {
	this->AddHitTube(id);
	fPHs[id]->AddDigiHit(time, charge);
}

int HitTubeCollection::GetTotalNumOfTrueHits() {
	int n = 0;
	for(fItr = fPHs.begin(); fItr != fPHs.end(); fItr++) {
		n += fItr->second->GetNRawPE();
	}
	return n;
}

int HitTubeCollection::GetTotalNumOfDigiHits() {
	int n = 0;
	for(fItr = fPHs.begin(); fItr != fPHs.end(); fItr++) {
		n += fItr->second->GetNDigiHits();
	}
	return n;
}
