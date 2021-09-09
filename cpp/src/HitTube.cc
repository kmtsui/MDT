#include "HitTube.h"

HitTube::HitTube(int id)
{
    fNRawPE=0;
    fPhotoElectrons.clear();

    fNDigiHits=0;
    fTimeDigi.clear();
    fChargeDigi.clear();
    fParentCompDigi.clear();
    fParentCompDigi2.clear();

    fTubeID = id;
}


HitTube::~HitTube()
{
    vector<TrueHit*>().swap(fPhotoElectrons );

    vector<float>().swap( fTimeDigi );
    vector<float>().swap( fChargeDigi );
    vector<vector<int>>().swap( fParentCompDigi );
    vector<vector<TrueHit*>>().swap( fParentCompDigi2 );
}
