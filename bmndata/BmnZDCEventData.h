/* 
 * File:   BmnZDCEventData.h
 * Author: pnaleks
 *
 * Created on 5 июня 2020 г., 13:43
 */

#ifndef BMNZDCEVENTDATA_H
#define BMNZDCEVENTDATA_H

#include <TNamed.h>
#include <TClonesArray.h>

#include "BmnZDCDigit.h"

class BmnZDCEventData : public TNamed {
public:
    BmnZDCEventData();
    virtual ~BmnZDCEventData();
    
    /// 
    /// Calculate all parameters values from BmnZDCDigits passed
    //
    /// \param pArrayOfBmnZDCDigits - array of BmnZDCDigit 
    /// \param pModuleScale - Array of coeffs for each ZDC module
    ///                            (starting from 1 to 104, index 0 is not used)
    /// \param pModuleThreshold - Array of thresholds for each ZDC module 
    ///                            (starting from 1 to 104, index 0 is not used)
    ///
    void Set(TClonesArray * pArrayOfBmnZDCDigits, Float_t * pModuleScale = 0, Float_t * pModuleThreshold = 0);
    
    /// 
    /// Get ZDC energy
    ///
    /// \param op - 'c' - central, 'p' - protons side, 'n' - nutrons side 
    ///             or full energy by default 
    /// \return - ZDC energy in GeV
    ///
    Float_t GetEnergy(char op);
    Float_t GetEnergy() { return fE; }

    /// 
    /// Get number of hits
    ///
    /// \param op - 'c' - central, 'p' - protons side, 'n' - nutrons side 
    ///             or full energy by default 
    /// \return - ZDC multiplicity
    ///
    Int_t GetNHits(char op);
    Int_t GetNHits() { return fH; }
    
    Float_t GetX() { return fX; }
    Float_t GetY() { return fY; }
    
    Float_t GetAsymmetry() { return fAsymmetry; }
    Float_t GetMoment() { return fMoment; }
    
    virtual void Print(Option_t *option="") const;
protected:
    Float_t fE;    // Full energy, GeV
    Float_t fEc;   // Central part energy (small modules, A/Z ~ 0.5), GeV
    Float_t fEp;   // Protons side (12 large modules), GeV
    Float_t fEn;   // Nutrons side (12 large modules), GeV
    
    UChar_t fH;       // Number of hits (modules triggered)
    UChar_t fHc;      // Central hits
    UChar_t fHp;      // Proton-side hits
    UChar_t fHn;      // Nutron-side hits

    Float_t fX;
    Float_t fY;
    Float_t fAsymmetry;
    Float_t fMoment;
    
    ClassDef(BmnZDCEventData, 2);
};

#endif /* BMNZDCEVENTDATA_H */

