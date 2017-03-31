#include "BmnMwpcAlignCorrections.h"

BmnMwpcAlignCorrections::BmnMwpcAlignCorrections(Int_t mwpcID, TVector3 corrs) : 
BmnAlignCorrections(corrs) {
    fMwpcId = mwpcID;
}


