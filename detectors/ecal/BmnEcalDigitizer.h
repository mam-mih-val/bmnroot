/* 
 * File:   BmnEcalDigitizer.h
 * Author: Petr Alekseev
 *
 * Created on 05.08.2020, 19:31
 */

#ifndef BMNECALDIGITIZER_H
#define BMNECALDIGITIZER_H

#include "BmnECALDigit.h"

#include <FairTask.h>
#include <TClonesArray.h>


class BmnEcalDigitizer : public FairTask {
public:
    BmnEcalDigitizer();
    virtual ~BmnEcalDigitizer();  
    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Print(Option_t *option="") const;
    
    /// 
    /// Read coordinates on ECAL cells from provided gepmetry file
    /// If this file nod set try will be made to obtain it from FairGeoLoader
    ///
    /// \param name - ECAL geometry file name
    ///
    void SetGeometryFileName(const char * name) {
        fEcalGeometryFileName = name;
    }
    
    /// 
    /// By default cell interaction point supposed to be in the center of the 
    /// cell volume. This param allows it to be shifted along of Z axis of the 
    /// volume.
    /// Default value is 0
    ///
    /// \param cm - Depth shift in cm
    ///
    void SetInteractionDepthShift(Double_t cm) {
        fDepthShift = cm;
    }

    /// 
    /// If cell energy is less than threshold it wont be stored.
    /// Default value is 10 MeV
    ///
    /// \param MeV - cell threshold, MeV
    ///
    void SetThreshold(Float_t MeV) { fThreshold = MeV; }

    
    /// 
    /// BmnEcalPoints with grater time will be ignored.
    /// Default value is 1000 ns. Set 0 for no limit
    ///
    /// \param ns - max allowed time in nanoseconds, 0 - no limit
    ///
    void SetMaxEcalPointTime(Float_t ns) {
        fMaxPointTime = ns;
    }
    
    /// 
    /// Cell start time calculated as weighted average of all BmnEcalPoints 
    /// inside it. Cell peak time is just the start time with constant delay.
    /// Default value is 5 ns
    ///
    /// \param ns - delay between start and peak time
    ///
    void SetPeakTimeDelay(Float_t ns) {
        fPeakTimeDelay = ns;
    }
    
    /// 
    /// Allows to take into account slowing of light propagation inside of 
    /// WLS fibers
    ///
    /// \param sol - light propagation speed in cm/ns, 0 - don't apply
    /// \param length - length of the WLS fiber (52 cm)
    /// \param moduleLayerThickness - thikness of the layer in the module (0.195 cm)
    ///
    void SetFiber(Float_t sol, Float_t length = 52., Float_t moduleLayerThickness = .195) {
        fFiberSOL = sol;
        fFiberLength = length;
        fLayerThickness = moduleLayerThickness;
    }

private:
    
    int LoadGeometry();
    
    const char * fEcalGeometryFileName = nullptr;
    Float_t fDepthShift = 0.;
    Float_t fThreshold = 10.;
    Float_t fMaxPointTime = 1000.;
    Float_t fPeakTimeDelay = 5.;
    
    Float_t fFiberSOL = 0.; // cm/ns (not used if zero)
    Float_t fFiberLength = 52.; //cm
    Float_t fLayerThickness = 0.195; //cm
    
    BmnECALDigit fCells[1024];
    const Int_t fCellsSize = sizeof(fCells)/sizeof(BmnECALDigit);
    
    TClonesArray * fArrayOfEcalPoints; // input
    TClonesArray * fArrayOfEcalDigits; // output

    ClassDef(BmnEcalDigitizer,0);
};

#endif /* BMNECALDIGITIZER_H */
