#pragma once

#include <vector>
#include <algorithm>

using std::vector;

class MTRandom
{
    public:
        MTRandom(int s){ SetSeed(s); }
        virtual ~MTRandom(){};
        double Rndm();
        double Gaus(double m=0., double s=1.0);
        int Poisson(double);
        unsigned int Integer(unsigned int imax){ return (unsigned int)(imax*Rndm()); }
        double Uniform(double x1, double x2){ return x1 + (x2-x1)*Rndm(); }
        double Histogram(vector<double>&, vector<double>&, vector<double>&);

    private :
        void SetSeed(int);
        unsigned int fMt[624];
        int fCount624;
        int fSeed;

        static const int kM;
        static const int kN;
        static const unsigned int kTemperingMaskB;
        static const unsigned int kTemperingMaskC;
        static const unsigned int kUpperMask;
        static const unsigned int kLowerMask;
        static const unsigned int kMatrixA;

        static const double kC1;
        static const double kC2;
        static const double kC3;
        static const double kD1;
        static const double kD2;
        static const double kD3;
        static const double kHm;
        static const double kZm;
        static const double kHp;
        static const double kZp;
        static const double kPhln;
        static const double kHm1;
        static const double kHp1;
        static const double kHzm;
        static const double kHzmp;
   /*zhm 0.967882898*/
        static const double kAs;
        static const double kBs;
        static const double kCs;
        static const double kB ;
        static const double kX0;
        static const double kYm;
        static const double kS ;
        static const double kT ;
};
