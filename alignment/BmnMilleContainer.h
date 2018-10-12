#ifndef BMNMILLECONTAINER_H
#define BMNMILLECONTAINER_H

#include <TNamed.h>
#include <TVector2.h>

using namespace std;

class BmnMilleContainer : public TNamed {
public:

    /** Default constructor **/
    BmnMilleContainer();
    // BmnMilleContainer(TString);

    void SetStation(Int_t stat) {
        fStation = stat;
    }

    Int_t GetStation() {
        return fStation;
    }

    void SetModule(Int_t mod) {
        fModule = mod;
    }

    Int_t GetModule() {
        return fModule;
    }

    void SetLocDers(vector <Double_t> vecX, vector <Double_t> vecY) {
        fLocDerX = vecX;
        fLocDerY = vecY;
    }

    vector <Double_t> GetLocDers(TString lay) {
        return lay.Contains("X") ? fLocDerX : lay.Contains("Y") ? fLocDerY : throw;
    }

    void SetGlobDers(vector <Double_t> vecX, vector <Double_t> vecY) {
        fGlobDerX = vecX;
        fGlobDerY = vecY;
    }

    vector <Double_t> GetGlobDers(TString lay) {
        return lay.Contains("X") ? fGlobDerX : lay.Contains("Y") ? fGlobDerY : throw;
    }

    void SetMeasures(Double_t measX, Double_t measY) {
        fMeasX = measX;
        fMeasY = measY;
    }
    
    TVector2 GetMeasures() {
        return TVector2(fMeasX, fMeasY);
    }
    
    void SetDMeasures(Double_t dMeasX, Double_t dMeasY) {
        fdMeasX = dMeasX;
        fdMeasY = dMeasY;
    }
    
    TVector2 GetDMeasures() {
        return TVector2(fdMeasX, fdMeasY);
    }

    /** Destructor **/
    virtual ~BmnMilleContainer() {
    }

private:
    Int_t fStation;
    Int_t fModule;

    vector <Double_t> fLocDerX;
    vector <Double_t> fLocDerY;
    vector <Double_t> fGlobDerX;
    vector <Double_t> fGlobDerY;

    Double_t fMeasX;
    Double_t fMeasY;
    Double_t fdMeasX;
    Double_t fdMeasY;

    ClassDef(BmnMilleContainer, 1);
};

#endif

