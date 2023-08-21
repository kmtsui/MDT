#include "MTRandom.h"
#include <cmath>
#include <iostream>

const double kPI                             = 3.14159265358979323846;  // TMath::Pi
// Taken from $ROOTSYS/math/mathcore/TRadom.cxx and TRandom3.cxx
const int MTRandom::kM                       = 397;
const int MTRandom::kN                       = 624;
const unsigned int MTRandom::kTemperingMaskB = 0x9d2c5680;
const unsigned int MTRandom::kTemperingMaskC = 0xefc60000;
const unsigned int MTRandom::kUpperMask      = 0x80000000;
const unsigned int MTRandom::kLowerMask      = 0x7fffffff;
const unsigned int MTRandom::kMatrixA        = 0x9908b0df;

const double MTRandom::kC1   = 1.448242853;
const double MTRandom::kC2   = 3.307147487;
const double MTRandom::kC3   = 1.46754004;
const double MTRandom::kD1   = 1.036467755;
const double MTRandom::kD2   = 5.295844968;
const double MTRandom::kD3   = 3.631288474;
const double MTRandom::kHm   = 0.483941449;
const double MTRandom::kZm   = 0.107981933;
const double MTRandom::kHp   = 4.132731354;
const double MTRandom::kZp   = 18.52161694;
const double MTRandom::kPhln = 0.4515827053;
const double MTRandom::kHm1  = 0.516058551;
const double MTRandom::kHp1  = 3.132731354;
const double MTRandom::kHzm  = 0.375959516;
const double MTRandom::kHzmp = 0.591923442;
/*zhm 0.967882898*/

const double MTRandom::kAs = 0.8853395638;
const double MTRandom::kBs = 0.2452635696;
const double MTRandom::kCs = 0.2770276848;
const double MTRandom::kB  = 0.5029324303;
const double MTRandom::kX0 = 0.4571828819;
const double MTRandom::kYm = 0.187308492;
const double MTRandom::kS  = 0.7270572718;
const double MTRandom::kT  = 0.03895759111;

void MTRandom::SetSeed(int s) {
	fMt[0]    = s;
	fCount624 = 624;
	// use multipliers from  Knuth's "Art of Computer Programming" Vol. 2, 3rd Ed. p.106
	for(int i = 1; i < 624; i++) {
		fMt[i] = (1812433253 * (fMt[i - 1] ^ (fMt[i - 1] >> 30)) + i);
	}
}

double MTRandom::Gaus(double mean, double sigma) {
	double result;
	double rn, x, y, z;
	do {
		y = Rndm();

		if(y > kHm1) {
			result = kHp * y - kHp1;
			break;
		}

		else if(y < kZm) {
			rn     = kZp * y - 1;
			result = (rn > 0) ? (1 + rn) : (-1 + rn);
			break;
		}

		else if(y < kHm) {
			rn = Rndm();
			rn = rn - 1 + rn;
			z  = (rn > 0) ? 2 - rn : -2 - rn;
			if((kC1 - y) * (kC3 + std::abs(z)) < kC2) {
				result = z;
				break;
			}
			else {
				x = rn * rn;
				if((y + kD1) * (kD3 + x) < kD2) {
					result = rn;
					break;
				}
				else if(kHzmp - y < exp(-(z * z + kPhln) / 2)) {
					result = z;
					break;
				}
				else if(y + kHzm < exp(-(x + kPhln) / 2)) {
					result = rn;
					break;
				}
			}
		}

		while(1) {
			x = Rndm();
			y = kYm * Rndm();
			z = kX0 - kS * x - y;
			if(z > 0)
				rn = 2 + y / x;
			else {
				x  = 1 - x;
				y  = kYm - y;
				rn = -(2 + y / x);
			}
			if((y - kAs + x) * (kCs + x) + kBs < 0) {
				result = rn;
				break;
			}
			else if(y < x + kT)
				if(rn * rn < 4 * (kB - log(x))) {
					result = rn;
					break;
				}
		}
	}
	while(0);
	return mean + sigma * result;
}

// inline double MTRandom::Rndm()
double MTRandom::Rndm() {
	unsigned int y;
	if(fCount624 >= kN) {
		int i;

		for(i = 0; i < kN - kM; i++) {
			y      = (fMt[i] & kUpperMask) | (fMt[i + 1] & kLowerMask);
			fMt[i] = fMt[i + kM] ^ (y >> 1) ^ ((y & 0x1) ? kMatrixA : 0x0);
		}

		for(; i < kN - 1; i++) {
			y      = (fMt[i] & kUpperMask) | (fMt[i + 1] & kLowerMask);
			fMt[i] = fMt[i + kM - kN] ^ (y >> 1) ^ ((y & 0x1) ? kMatrixA : 0x0);
		}

		y           = (fMt[kN - 1] & kUpperMask) | (fMt[0] & kLowerMask);
		fMt[kN - 1] = fMt[kM - 1] ^ (y >> 1) ^ ((y & 0x1) ? kMatrixA : 0x0);
		fCount624   = 0;
	}
	y = fMt[fCount624++];
	y ^= (y >> 11);
	y ^= ((y << 7) & kTemperingMaskB);
	y ^= ((y << 15) & kTemperingMaskC);
	y ^= (y >> 18);

	// 2.3283064365386963e-10 == 1./(max<UINt_t>+1)  -> then returned value cannot be = 1.0
	if(y)
		return { ((double)y * 2.3283064365386963e-10) };  // * Power(2,-32)
	return Rndm();
}

int MTRandom::Poisson(double mean) {
	int n;
	if(mean <= 0)
		return 0;
	if(mean < 25) {
		double expmean = exp(-mean);
		double pir     = 1;
		n              = -1;
		while(1) {
			n++;
			pir *= Rndm();
			if(pir <= expmean)
				break;
		}
		return n;
	}
	// for large value we use inversion method
	else if(mean < 1E9) {
		double em, t, y;

		double sq   = sqrt(2.0 * mean);
		double alxm = log(mean);
		double g    = mean * alxm - lgamma(mean + 1.0);
		do {
			do {
				y  = tan(kPI * Rndm());
				em = sq * y + mean;
			}
			while(em < 0.0);

			em = floor(em);
			t  = 0.9 * (1.0 + y * y) * exp(em * alxm - lgamma(em + 1.0) - g);
		}
		while(Rndm() > t);
		return static_cast<int>(em);
	}
	else {
		// use Gaussian approximation vor very large values
		return int(Gaus(0., 1.) * sqrt(mean) + mean + 0.5);
	}
}

// Generate random number that follows given 1D histogram (CDF)
double MTRandom::Histogram(vector<double>& edgeLow, vector<double>& binCenter, vector<double>& cdf) {
	double value = this->Rndm();
	auto low     = lower_bound(cdf.begin(), cdf.end(), value);
	int index    = low - cdf.begin();
	if(*low != value) {
		index -= 1;
	}
	double dx = binCenter[index + 1] - binCenter[index];
	return binCenter[index] + dx * ((value - cdf[index]) / (cdf[index + 1] - cdf[index]));
}
