#include "PMTResponse3inchR12199_02.h"

#include "Configuration.h"

#include <map>
#include <math.h> 

using std::map;

PMTResponse3inchR12199_02::PMTResponse3inchR12199_02(int seed, const string &pmtname)
{
    this->Initialize(seed, pmtname);
}

PMTResponse3inchR12199_02::PMTResponse3inchR12199_02()
{
}

PMTResponse3inchR12199_02::~PMTResponse3inchR12199_02()
{
}

void PMTResponse3inchR12199_02::Initialize(int seed, const string &pmtname)
{
    fPMTType = pmtname;
    fRand = new MTRandom(seed);

    map<string, string> s;
    s["TimingResConstant"] = "TimingResConstant";
    s["TimingResMinimum"] = "TimingResMinimum"; 
    s["ScalFactorTTS"] = "ScalFactorTTS";
    s["SPECDFFile"] = "SPECDFFile";
    s["PMTDE"] = "PMTDE";
    if( fPMTType!="" )
    {
        map<string, string>::iterator i;
        for(i=s.begin(); i!=s.end(); i++)
        {
            i->second += "_" + fPMTType;
        }
    }
    Configuration *Conf = Configuration::GetInstance();
    Conf->GetValue<float>(s["TimingResConstant"], fTResConstant);
    Conf->GetValue<float>(s["TimingResMinimum"], fTResMinimum);
    Conf->GetValue<float>(s["ScalFactorTTS"], fSclFacTTS);
    Conf->GetValue<string>(s["SPECDFFile"], fTxtFileSPECDF);
    Conf->GetValue<string>(s["PMTDE"], fPMTDEFile);
    this->LoadCDFOfSPE(fTxtFileSPECDF);
    this->LoadPMTDE(fPMTDEFile);
}

void PMTResponse3inchR12199_02::LoadPMTDE(const string &filename)
{
    fLoadDE = false;
    fDE.clear();
    ifstream ifs(filename.c_str());
    if (!ifs)
    {
        cout<<" PMTResponse3inchR12199_02::LoadPMTDE" <<endl;
        cout<<"  - No PMT QE file: " << filename <<endl;
        cout<<"  - Do not apply DE " << endl;
    }
    string aLine;
    while( std::getline(ifs, aLine) )
    {
        if( aLine[0] == '#' ){ continue; }
        stringstream ssline(aLine);
        string item;
        while (getline(ssline, item, ssline.widen(' ')))
        {
            fDE.push_back( atof(item.c_str()) );
        }
    }
    ifs.close();

    if (fDE.size()>0)
    {
        fLoadDE = true;
        cout<<" PMTResponse3inchR12199_02::LoadPMTDE" <<endl;
        cout<<"  - Load PMT QE file: " << filename <<endl;
        cout<<"  - # Entries = " << fDE.size() << endl;
    }
}

bool PMTResponse3inchR12199_02::ApplyDE(const TrueHit* th, const HitTube *ht)
{
    if (fLoadDE && ht)
    {
        long unsigned int tubeID = ht->GetTubeID();
        if (tubeID>=fDE.size())
        {
            cout<<" PMTResponse3inchR12199_02::ApplyDE" <<endl;
            cout<<"  - tubeID = " << tubeID << " >= fDE.size() = " << fDE.size() << endl;
            cout<<"  -> EXIT" <<endl;
            exit(-1);
        }
        return fRand->Rndm() < fDE[tubeID];
    }

    return true;
}

float PMTResponse3inchR12199_02::HitTimeSmearing(float Q)
{
    Q = (Q > 0.5) ? Q : 0.5;
    float timingResolution = 0.5*fSclFacTTS*(0.33 + sqrt(fTResConstant/Q));
    if( timingResolution<fTResMinimum ){ timingResolution = fTResMinimum; }
    return fRand->Gaus(0.0,timingResolution);
}
