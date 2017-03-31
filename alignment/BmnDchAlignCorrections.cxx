#include "BmnDchAlignCorrections.h"

BmnDchAlignCorrections::BmnDchAlignCorrections(Int_t dchID, TVector3 corrs) : 
BmnAlignCorrections(corrs) {
    fDchId = dchID;
}


