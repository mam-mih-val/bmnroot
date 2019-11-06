#include "BmnECALDigit.h"

Bool_t BmnECALMapElement::Scan(ifstream& in) {
    in >> std::hex >> fAdcId >> std::dec >> fAdcChan >> fChan >> fX >> fY;
    if (in.good()) {
        fX *= 10.;
        fY *= 10.;
        fIX = (Short_t) (fX / 40.);
        fIY = (Short_t) (fY / 40.);
        //Print();
        return kTRUE;
    }
    fChan = -1;
    return in.eof();
}

void BmnECALMapElement::Print() {
    cout << "0x" << std::hex << fAdcId << std::dec 
            << "\t" << fAdcChan 
            << "\t" << fChan 
            << "\t" << fX 
            << "\t" << fY << "\n";
}

Bool_t BmnECALMap::Load() {
    TString dir = getenv("VMCWORKDIR");
    TString mapFile = dir + "/input/ECAL_map_period_7.txt";
    
    ifstream in;
    in.open(mapFile.Data());
    if (!in.is_open())
    {
	printf("Loading ECAL Map from file: %s - file open error!\n", mapFile.Data());
	return kFALSE;
    }
    printf("Loading ECAL Map from file: %s\n", mapFile.Data());
    
    TString dummy;
    in >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy;

    BmnECALMapElement e;
    Int_t line = 2;
    while (!in.eof()) {
        line++;
        if (!e.Scan(in)) {
            printf("Loading ECAL Map: scan error at line %d!\n", line);
            in.close();
            return kFALSE;
        }
        if (!in.eof() && e.GetChan()) {
            fMap[e.GetChan()] = e;
        }
    }
    in.close();
    printf("Loading ECAL map: success\n");
    return kTRUE;
}

BmnECALDigit::BmnECALDigit(){
    fChannel = 0;
    fX = 0;
    fY = 0;
    fAmp = 0;
    fPeakAmp = 0;
    fPeakTime = 0;
    fStartTime = 0;
    fLabX = fLabY = fLabZ = 0;
}

BmnECALDigit::BmnECALDigit(Float_t x,Float_t y,Short_t ch,Float_t amp){
    fChannel = ch;
    fX = x;
    fY = y;
    fAmp = amp;
    fPeakAmp = 0;
    fPeakTime = 0;
    fStartTime = 0;
    fLabX = fLabY = fLabZ = 0;
}

BmnECALDigit::BmnECALDigit(BmnECALMapElement* e, Float_t amp) {
    fChannel = e->GetChan();
    fX = e->GetX();
    fY = e->GetY();
    fAmp = amp;
    fPeakAmp = 0;
    fPeakTime = 0;
    fStartTime = 0;
    fLabX = fLabY = fLabZ = 0;
}

ClassImp(BmnECALDigit)
