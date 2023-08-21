#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "MTRandom.h"

using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using std::vector;

class PMTAfterpulse {
	public:

		PMTAfterpulse();
		virtual ~PMTAfterpulse();

		bool GenerateAfterpulse(const float, const vector<int>&, MTRandom*);
		float GetAfterpulseTime(const float, MTRandom*);

	private:

		void LoadTimingCDF(const string&);

		vector<double> fCDF_BinEdgeLow;
		vector<double> fCDF_BinEdgeUp;
		vector<double> fCDF_BinCenter;
		vector<double> fCDF;
		float fSlope;
		float fIntercept;

		int fDoAdd;
		string fTxtFileCDF;
};
