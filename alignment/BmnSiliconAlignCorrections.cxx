#include "BmnSiliconAlignCorrections.h"

BmnSiliconAlignCorrections::BmnSiliconAlignCorrections(TVector3 corrs) :
BmnAlignCorrections(corrs) {

}

BmnSiliconAlignCorrections::BmnSiliconAlignCorrections(Int_t stat, Int_t mod, TVector3 corrs) : 
BmnAlignCorrections(corrs) {
    fStation = stat;
    fModule = mod;
}
