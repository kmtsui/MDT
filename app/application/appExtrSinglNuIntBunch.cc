#include "BeamTiming.h"
#include "SpillAnalyzer.h"
#include "WCRootDataPileUpSpill.h"

// Will dump digitized hits associated with give bunch id number
int main(int argc, char** argv) {
	TString InFileName    = argv[1];
	TString OutFileName   = argv[2];
	string MDTParFileName = argv[3];
	int bunch_id          = atoi(argv[4]);
	int seed              = atoi(argv[5]);

	Configuration* Conf = Configuration::GetInstance();
	Conf->ReadParameter(MDTParFileName);
	Conf->PrintParameters();
	Conf = NULL;

	// Mange digitzed hits and add afterpulse (optinal)
	MDTManager* MDT = new MDTManager(seed);

	// Mange input pile-up spill files
	WCRootDataPileUpSpill* input = new WCRootDataPileUpSpill();
	input->ReadFile(InFileName);

	// Manage selected ""bunch events""
	WCRootData* output = new WCRootData();
	output->CreateTree(OutFileName);

	// Manage spill selection
	SpillAnalyzer* sa = new SpillAnalyzer();

	// Peak time of the bunch
	const float bunch_mean_time = (float)kBunchInterval * (bunch_id - 1);
	;
	const float TimeMichelELow = bunch_mean_time - 3. * kBunchSigma;
	const float TimeMichelEUp  = bunch_mean_time + 3. * kBunchSigma;

	// Only digitized hits that belong to the time window will be extracted
	const float TimeSelDigiLow = TimeMichelELow;
	float TimeSelDigiUp        = (float)kBunchInterval * bunch_id - 3. * kBunchSigma;
	if(bunch_id == kNBunches) {
		TimeSelDigiUp = (float)kBunchInterval * (bunch_id - 1) + 3. * kBunchSigma + 10000.;
	}

	cout << " Target bunch id: " << bunch_id << " in [0, " << kNBunches << "]" << endl;
	cout << "  - True secondary electron search window: [" << TimeMichelELow << ", " << TimeMichelEUp
	     << " (ns)" << endl;
	cout << "  - Digi hit saving time window: [" << TimeSelDigiLow << ", " << TimeSelDigiUp << " (ns)"
	     << endl;

	int nuIntIdx       = -1;
	const int nEntries = input->GetEntries();
	cout << " Start processing " << input->GetEntries() << " pile-up spills" << endl;
	int nPassed = 0;

	// Will copy a TTree object to the output file with selected spills
	vector<bool> flagToSave(nEntries, false);
	for(int iEntry = 0; iEntry < nEntries; iEntry++) {
		input->GetEntry(iEntry);

		// Check that the bunch of interest has just one ID interaction with true visible energy deposition<50
		// MeV
		if(!sa->IsSingleIDIneractionAtBunch(input, 4 - 1, nuIntIdx)) {
			continue;
		}

		// Check whether or not there is no secondary electrons (e.g. Michel-e) steming from earlier ID/beam
		// bkg interations around the bunch of interest
		if(sa->HasMichelEFromOtherInteractionAtBunch(input, nuIntIdx, TimeMichelELow, TimeMichelEUp)) {
			continue;
		}

		// Extract digitized hits being in time window [TimeSelDigiLow, TimeSelDigiUp]
		// The offset time, TimeSelDigiUp, will be subtracted from the extracted time just for late use
		input->AddDigiHitsToMDT(MDT, TimeSelDigiLow, TimeSelDigiUp, bunch_mean_time);

		MDT->DoTrigger();
		output->AddDigiHits(MDT);
		output->FillTree();
		MDT->DoInitialize();

		flagToSave[iEntry] = true;
		nPassed            += 1;
	}
	cout << " Number of passed entries: " << nPassed << "/" << nEntries << endl;

	output->WriteTree();
	output->CopyTree(InFileName, "pupspillT", flagToSave);

	Conf = Configuration::GetInstance();
	Conf->Finalize();
}
