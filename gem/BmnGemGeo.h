#ifndef MPD_TPC_SECTOR_H
#define MPD_TPC_SECTOR_H

#include <math.h>
#include <iostream>

#include <TObject.h>
#include "TMath.h"
using namespace std;

class BmnGemGeo : public TObject {
public:

    BmnGemGeo();
    virtual ~BmnGemGeo();

    Int_t GetNumOfStations() const {
        return fNumStations;
    }

    Float_t GetDistAlongZ() const {
        return fDistance;
    }

    Float_t GetStationNum(Float_t z) const {
        return (z - fZ0) / fDistance + 0.5;
    }

    /// GEM Characteristics

    Float_t GetDriftArea() const {
        return fDriftArea;
    }

    Float_t GetFirstTransArea() const {
        return fFirstTransArea;
    }

    Float_t GetSecondTransArea() const {
        return fSecondTransArea;
    }

    Float_t GetInductArea() const {
        return fInductArea;
    }

    Float_t GetHoneyCombDist() const {
        return fHoneyCombDist;
    }

    /// Strip Characteristics   

    Float_t GetXstripWidth() const {
        return fStripXWidth;
    }

    Float_t GetYstripWidth() const {
        return fStripYWidth;
    }

    Float_t GetXstripLength() const {
        return fStripXLength;
    }

    Float_t GetYstripLength() const {
        return fStripYLength;
    }

    Int_t GetXstripNum() const {
        return 2 * TMath::Abs(fX0) / fStripXWidth;
    }

    Int_t GetYstripNum() const {
        return 2 * TMath::Abs(fY0) / fStripYWidth;
    }

    Float_t GetXstripPitch() const {
        return fXstripPitch;
    }

    Float_t GetYstripPitch() const {
        return fYstripPitch;
    }

    Float_t GetX0() const {
        return fX0;
    }

    Float_t GetY0() const {
        return fY0;
    }
    
    Float_t GetZ0() const {
        return fZ0;
    }

private:

    /// GEM Characteristics
    Int_t fStation; /// Number of the current GEM-station
      
    Int_t fNumStations; /// Total Number of the GEM-stations 
    Float_t fDistance; /// Distance between the GEM-stations along z-axis

    Float_t fDriftArea;
    Float_t fFirstTransArea;
    Float_t fSecondTransArea;
    Float_t fInductArea;
    Float_t fHoneyCombDist;

    /// Strip Characteristics

    Float_t fX0; /// Minimal coordinate of the strip plane along the X-axis 
    Float_t fY0; /// Minimal coordinate of the strip plane along the Y-axis
    Float_t fZ0; /// Z-coordinate of the first GEM station

    Float_t fXstripPitch; /// X-pitch value of strips  
    Float_t fYstripPitch; /// X-pitch value of strips

    Float_t fStripXWidth; /// Width of X-strips 
    Float_t fStripYWidth; /// Width of Y-strips

    Float_t fStripXLength; /// Length of X-strips 
    Float_t fStripYLength; /// Length of Y-strips

    ClassDef(BmnGemGeo, 1);

};

#endif
