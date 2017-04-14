#include "BmnMwpcRaw2Digit.h"

BmnMwpcRaw2Digit::BmnMwpcRaw2Digit(TString mapName) {

    printf("Reading MWPC mapping file ...\n");
    fMapFileName = TString(getenv("VMCWORKDIR")) + TString("/input/") + mapName;
    //========== read mapping file            ==========//
    fMapFile.open((fMapFileName).Data());
    if (!fMapFile.is_open()) {
        cout << "Error opening map-file (" << fMapFileName << ")!" << endl;
    }

    TString dummy;
    UInt_t ser;
    UInt_t plane;

    fMapFile >> dummy >> dummy;
    fMapFile >> dummy;
    while (!fMapFile.eof()) {
        fMapFile >> dec >> plane >> hex >> ser;
        if (!fMapFile.good()) break;
        mapping.insert(pair<UInt_t, UInt_t>(ser, plane));
    }
    fMapFile.close();
    //==================================================//

}

void BmnMwpcRaw2Digit::FillEvent(TClonesArray *hrb, TClonesArray *mwpc) {

    for (Int_t iDig = 0; iDig < hrb->GetEntriesFast(); ++iDig) {        
        BmnHRBDigit *dig = (BmnHRBDigit*) hrb->At(iDig);
        map<UInt_t, UInt_t>::iterator it = mapping.find(dig->GetSerial());
        if (it == mapping.end()) continue;
        new((*mwpc)[mwpc->GetEntriesFast()]) BmnMwpcDigit(it->second, dig->GetChannel(), dig->GetSample() * 8); // dig->GetSample() * 8 -- convert to ns
    }
}

ClassImp(BmnMwpcRaw2Digit)

