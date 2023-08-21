#pragma once

#include <iostream>
#include <map>
#include <string>

#include "HitTube.h"

using std::cout;
using std::endl;
using std::map;
using std::string;

class HitTubeCollection {
	public:

		HitTubeCollection();
		virtual ~HitTubeCollection();

		bool HasTube(const int id) { return fPHs.count(id) != 0 ? true : false; }

		void AddHitTube(const int);
		void AddTrueHit(const int, const float, const int);
		void AddDigiHit(const int, const float, const float);

		int GetNumOfHitTubes() const { return fPHs.size(); }

		int GetTotalNumOfTrueHits();
		int GetTotalNumOfDigiHits();
		void Clear();

		void Begin() { fItr = fPHs.begin(); }

		bool IsEnd() { return fItr == fPHs.end() ? true : false; }

		void Next() { fItr++; }

		HitTube* GetHitTube(int id = -1) {
			if(id < 0) {
				return &(*fPHs[fItr->first]);
			}
			return &(*fPHs[id]);
		}

		HitTube& operator()() { return *fPHs[fItr->first]; }

		HitTube& operator[](int id) { return *fPHs[id]; }

		const vector<int>& GetHitTubeIDList() const { return fTubeList; }

	private:

		map<int, HitTube*> fPHs;
		map<int, HitTube*>::iterator fItr;
		vector<int> fTubeList;
		int fNTrueHitsTotal;
		int fNDigiHitsTotal;
};
