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
    Short_t station;
    Short_t plane;

    fMapFile >> dummy >> dummy >> dummy;
    fMapFile >> dummy;
    while (!fMapFile.eof()) {
        fMapFile >> hex >> ser >> dec >> station >> plane;
        if (!fMapFile.good()) break;
        BmnMwpcMapping record;
        record.serial = ser;
        record.station = station;
        record.plane = plane;
        fMap.push_back(record);
    }
    fMapFile.close();
    //==================================================//

}

void BmnMwpcRaw2Digit::FillEvent(TClonesArray *hrb, TClonesArray *mwpc) {

    for (Int_t iDig = 0; iDig < hrb->GetEntriesFast(); ++iDig) {
        BmnHRBDigit *dig = (BmnHRBDigit*) hrb->At(iDig);
        for (Int_t iMap = 0; iMap < fMap.size(); ++iMap) {
            BmnMwpcMapping tM = fMap[iMap];
            if (dig->GetSerial() == tM.serial) {
                new((*mwpc)[mwpc->GetEntriesFast()]) BmnMwpcDigit(tM.station, tM.plane, dig->GetChannel(), dig->GetSample() * 8); // dig->GetSample() * 8 -- convert to ns
            }
        }
    }
}

ClassImp(BmnMwpcRaw2Digit)

