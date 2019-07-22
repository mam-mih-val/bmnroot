#include "BmnOfflineQaSteering.h"

BmnOfflineQaSteering::BmnOfflineQaSteering() :
fNdets(11) {
    fCanvNames.resize(0);
    // const Int_t nDets = 11;
    const Int_t nDims = 2;
    const Int_t nElems = 2;

    fDetCanvas = new Int_t**[fNdets];

    for (Int_t iDet = 0; iDet < fNdets; iDet++) {
        fDetCanvas[iDet] = new Int_t*[nDims];

        for (Int_t iDim = 0; iDim < nDims; iDim++) {
            fDetCanvas[iDet][iDim] = new Int_t[nElems];

            for (Int_t iEle = 0; iEle < nElems; iEle++)
                fDetCanvas[iDet][iDim][iEle] = -1;
        }
    }

    ParseSteerFile();
}

BmnOfflineQaSteering::~BmnOfflineQaSteering() {

}

void BmnOfflineQaSteering::ParseSteerFile(TString fileName) {
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathFull = gPathConfig + "/macro/steering/" + fileName;

    TString det = "";
    string line;
    ifstream f(gPathFull.Data(), ios::in);

    getline(f, line); // Line with comments

    for (Int_t iDet = 0; iDet < fNdets; iDet++) {
        for (Int_t iDim = 0; iDim < 2; iDim++) {
            getline(f, line);
            f >> det >> fDetCanvas[iDet][iDim][0] >> fDetCanvas[iDet][iDim][1];
            fCanvNames.push_back(det);
        }
    }
    
    // Get canvas ordering
    for (Int_t iDet = 0; iDet < fNdets; iDet++) {
       getline(f, line); 
       TString detBuff;
       Int_t pad1, pad2;
       f >> detBuff >> pad1 >> pad2;
       fCanvDetCorresp[detBuff] = pair <Int_t, Int_t> (pad1, pad2);  
    }
}


