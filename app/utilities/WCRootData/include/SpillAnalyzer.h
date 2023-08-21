#ifndef SPILLANALYZER_H
#define SPILLANALYZER_H

#include <iostream>

using std::cout;
using std::endl;

class WCRootDataPileUpSpill;

class SpillAnalyzer
{
    public:
        SpillAnalyzer();
        virtual ~SpillAnalyzer();

        bool IsSingleIDIneractionAtBunch(const WCRootDataPileUpSpill*, const int, int&);
        bool HasMichelEFromOtherInteractionAtBunch(const WCRootDataPileUpSpill*, const int, const float, const float);
};

#endif
