#include "TriggerAlgo.h"
#include "Configuration.h"
#include "HitTube.h"

TriggerAlgo::TriggerAlgo() :
    fNDigitsWindow(200.), fNDigitsStepSize(5.), fNDigitsThreshold(25), fTriggerTimeForFailure(100.) {
	fPreTriggerWindow[TriggerType::eNDigits] = -400.;
	fPreTriggerWindow[TriggerType::eFailure] = -400.;

	fPostTriggerWindow[TriggerType::eNDigits] = 950.;
	fPostTriggerWindow[TriggerType::eFailure] = 100000.;

	Configuration* Conf = Configuration::GetInstance();
	Conf->GetValue<float>("NDigitsWindow", fNDigitsWindow);
	Conf->GetValue<float>("NDigitsStepSize", fNDigitsStepSize);
	Conf->GetValue<int>("NDigitsThreshold", fNDigitsThreshold);
	Conf->GetValue<float>("FailureTime", fTriggerTimeForFailure);
	Conf->GetValue<float>("NDigitsPreTriggerWindow", fPreTriggerWindow[TriggerType::eNDigits]);
	Conf->GetValue<float>("NDigitsPostTriggerWindow", fPostTriggerWindow[TriggerType::eNDigits]);
	Conf->GetValue<float>("FailurePreTriggerWindow", fPreTriggerWindow[TriggerType::eFailure]);
	Conf->GetValue<float>("FailurePostTriggerWindow", fPostTriggerWindow[TriggerType::eFailure]);
}

// This is based on WCSimWCTriggerBase::AlgNDigits in WCSimWCTrigger.cc
void TriggerAlgo::NDigits(HitTubeCollection* hc, TriggerInfo* ti) {
	ti->Clear();
	const int nTotalDigiHits = hc->GetTotalNumOfDigiHits();
	int nTriggers            = 0;
	float trigTime           = 0.;

	if(nTotalDigiHits > 0) {
		vector<float> times;
		times.reserve(nTotalDigiHits);
		for(hc->Begin(); !hc->IsEnd(); hc->Next()) {
			HitTube* aPH = &(*hc)();
			for(int i = 0; i < aPH->GetNDigiHits(); i++) {
				times.push_back(aPH->GetTimeDigi(i));
			}
			aPH = NULL;
		}
		std::sort(times.begin(), times.end());

		float tFirstHit = times[0];
		float tLastHit  = times[nTotalDigiHits - 1];

		const double stepSize   = fNDigitsStepSize;                          // in ns
		const double tWindowMax = std::max(0.f, tLastHit - fNDigitsWindow);  // in ns

		double tWindowUp  = 0.;
		double tWindowLow = 0.;

		// - Slide the time window with a width of "fNDigitsWindow"
		//   from "tWindowLow" (assumed to be 0 initially) to "tWindowMax"
		//   with a step size of "stepSize"
		//
		// - For each step, all the digitized hits falling the corresponding window
		//   are counted. If the number of those hits are greater than "fNDigitsThreshold"
		//   a new trigger is created
		tWindowUp = tWindowLow + fNDigitsWindow;
		int iHit  = 0;
		while(tWindowLow <= tWindowMax) {
			vector<float> Times;
			Times.clear();
			for(iHit = 0; iHit < nTotalDigiHits; iHit++) {
				float t = times[iHit];
				if(t >= tWindowLow && t <= tWindowUp) {
					Times.push_back(t);
				}
			}

			bool isTriggerFound = false;
			if((int)Times.size() > fNDigitsThreshold) {
				trigTime          = Times[fNDigitsThreshold];
				trigTime          -= (int)trigTime % 5;
				float trigTimeLow = trigTime + fPreTriggerWindow[TriggerType::eNDigits];
				float trigTimeUp  = trigTime + fPostTriggerWindow[TriggerType::eNDigits];

				// Avoid overlapping with previous trigger window
				if(nTriggers >= 1) {
					float trigTimeUpPrevious = ti->GetUpEdge(nTriggers - 1);
					if(trigTimeUpPrevious > trigTimeLow) {
						trigTimeLow = trigTimeUpPrevious;
					}
				}
				ti->AddTrigger(trigTime,
				               trigTimeLow,
				               trigTimeUp,
				               (int)Times.size(),
				               (int)TriggerType::eNDigits);
				cout << " Found trigger at: " << trigTime << " nHits: " << Times.size()
				     << " trigger window: [" << trigTimeLow << ", " << trigTimeUp << "] ns " << endl;
				isTriggerFound = true;
				nTriggers      += 1;
			}

			if(isTriggerFound) {
				tWindowLow = trigTime + fPostTriggerWindow[TriggerType::eNDigits];
			}
			else {
				tWindowLow += stepSize;
			}
			tWindowUp = tWindowLow + fNDigitsWindow;
		}
	}

	// Check to see if there is at least one trigger created
	if(nTriggers == 0) {
		// No trigger was created
		// Add a failure tigger
		trigTime = fTriggerTimeForFailure;
		// float trigTimeLow = trigTime - tFirstHit;
		// float trigTimeUp = trigTime + tLastHit;

		float trigTimeLow = trigTime + fPreTriggerWindow[TriggerType::eFailure];
		float trigTimeUp  = trigTime + fPostTriggerWindow[TriggerType::eFailure];

		ti->AddTrigger(trigTime, trigTimeLow, trigTimeUp, -1, (int)TriggerType::eFailure);
		// cout<<" No trigger found " <<endl;
	}
}
