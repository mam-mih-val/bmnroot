/* 
 * File:   BmnZdcAnalyzer.h
 * Author: pnaleks
 *
 * Created on 5 июня 2020 г., 18:58
 */

#ifndef BMNZDCANALYZER_H
#define BMNZDCANALYZER_H

#include <FairTask.h>
#include <TClonesArray.h>

#include "BmnZDCEventData.h"

class BmnZdcAnalyzer : public FairTask
{
  public:
    BmnZdcAnalyzer();
    virtual ~BmnZdcAnalyzer();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    /// 
    /// Set array of module coeffs for each channel to be used in 
    /// BmnZDCEventData::Set
    ///
    /// \param array - coeffs, indices from 1 to 104, index 0 is not used
    ///
    void SetModuleScale(Float_t * array) { fModuleScale = array; }
    /// 
    /// Set array of module thresholds for each channel to be used in 
    /// BmnZDCEventData::Set
    ///
    /// \param array - thresholds, indices from 1 to 104, index 0 is not used
    ///
    void SetModuleThreshold(Float_t * array) { fModuleThreshold = array; }

  private:

    TClonesArray * fArrayOfZdcDigits; // input
    BmnZDCEventData * fBmnZDCEventData; // output
    
    Float_t* fModuleScale;         //!
    Float_t* fModuleThreshold;     //!

    ClassDef(BmnZdcAnalyzer,1);
};

#endif /* BMNZDCANALYZER_H */
