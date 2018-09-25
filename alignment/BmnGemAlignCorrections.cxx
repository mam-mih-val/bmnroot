#include "BmnGemAlignCorrections.h"

BmnGemAlignCorrections::BmnGemAlignCorrections() : 
BmnAlignCorrections() {

}

BmnGemAlignCorrections::BmnGemAlignCorrections(Int_t stat, Int_t mod, TVector3 corrs) : 
BmnAlignCorrections(corrs) {
    fStation = stat;
    fModule = mod;
}


