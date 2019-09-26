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

    // Get period, exp. setup, start and finish run id, set of detectors and triggers ...
    TString buff = "";
    for (Int_t iLine = 0; iLine < 3; iLine++) {
        getline(f, line);
        Int_t period, start, finish, Ndets, Ntrigs;
        TString setup, detector, trigger;
        f >> period >> setup >> start >> finish >> buff >> Ndets;
        fBorderRuns[make_pair(period, setup)] = make_pair(start, finish);
        
        vector <TString> detectors;
        
        for (Int_t iDet = 0; iDet < Ndets; iDet++) {
            f >> detector;
            detectors.push_back(detector);       
        }
        fDetectors[make_pair(period, setup)] = detectors;
        
        vector <TString> triggers;
        f >> buff >> Ntrigs;
        for (Int_t iTrigger = 0; iTrigger < Ntrigs; iTrigger++) { 
            f >> trigger;
            triggers.push_back(trigger);            
        }
        fTriggers[make_pair(period, setup)] = triggers;
    }
}

