#include "PMTAfterpulse.h"
#include "Configuration.h"

PMTAfterpulse::PMTAfterpulse() : fSlope(0.3523), fIntercept(0.5662), fDoAdd(0), fTxtFileCDF("") {
	fCDF_BinEdgeLow.clear();
	fCDF_BinEdgeUp.clear();
	fCDF_BinCenter.clear();
	fCDF.clear();

	Configuration* Conf = Configuration::GetInstance();
	Conf->GetValue<float>("APRateSlope", fSlope);
	Conf->GetValue<float>("APRateIntercept", fIntercept);
	Conf->GetValue<string>("APTimeCDFFile", fTxtFileCDF);
	Conf->GetValue<int>("APDoAdd", fDoAdd);

	if(fDoAdd == 1) {
		this->LoadTimingCDF(fTxtFileCDF);
	}
}

PMTAfterpulse::~PMTAfterpulse() {
	vector<double>().swap(fCDF_BinEdgeLow);
	vector<double>().swap(fCDF_BinEdgeUp);
	vector<double>().swap(fCDF_BinCenter);
	vector<double>().swap(fCDF);
}

bool PMTAfterpulse::GenerateAfterpulse(const float charge, const vector<int>& composition, MTRandom* r) {
	// Do not add afterpulsing by default
	if(fDoAdd == 0) {
		return false;
	}

	float rate      = fSlope * charge + fIntercept;
	bool afterpulse = false;
	if(composition.size() == 1 && composition[0] == -1) {
		return afterpulse;
	}

	if(rate > 1.0) {
		afterpulse = true;
	}
	else if(rate < 1.0) {
		afterpulse = r->Rndm() < rate ? true : false;
	}
	return afterpulse;
}

float PMTAfterpulse::GetAfterpulseTime(const float time, MTRandom* r) {
	float dt = r->Histogram(fCDF_BinEdgeLow, fCDF_BinCenter, fCDF);
	return time + dt * 1000.;
}

void PMTAfterpulse::LoadTimingCDF(const string& infiletxt) {
	string aline;
	string svalue;
	ifstream fin(infiletxt);
	if(!fin.is_open()) {
		cout << " [ERROR] PMTAfterpulse::LoadTimingCDF " << endl;
		cout << "  - Cannot opent file: " << infiletxt << endl;
		exit(-1);
	}

	int nRead = 0;
	while(std::getline(fin, aline)) {
		stringstream ss(aline);
		ss >> svalue;
		fCDF_BinEdgeLow.push_back(atof(svalue.c_str()));
		ss >> svalue;
		fCDF_BinCenter.push_back(atof(svalue.c_str()));
		ss >> svalue;
		fCDF_BinEdgeUp.push_back(atof(svalue.c_str()));
		ss >> svalue;
		fCDF.push_back(atof(svalue.c_str()));
		if(fCDF[nRead] == 1.0) {
			break;
		}
	}
	fin.close();
}
