#include "TString.h"
#include "BmnEnums.h"
#include "BmnTTBDigit.h"
#include "BmnTDCDigit.h"
#include "BmnADCDigit.h"
#include "BmnADC32Digit.h"
#include "BmnSyncDigit.h"
#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include <iostream>
#include <vector>

class BmnRawDataDecoder {
  
public:
  BmnRawDataDecoder(TString file);
  BmnRawDataDecoder();
  virtual ~BmnRawDataDecoder();

  
  BmnStatus ConvertRawToRoot();
  BmnStatus DecodeDataToDigi();
  
  UInt_t GetRunId() const {
      return fRunId;
  }
  
  UInt_t GetNevents() const {
      return fNevents;
  }
  
   	
private:
  
  UInt_t fRunId;
  UInt_t fEventId;
  UInt_t fNevents;

  TTree *fOutTree;
  TString fRootFileName;
  TString fRawFileName;
  TFile *fRootFile;
  FILE  *fRawFile;
  
  TClonesArray *sync;
  TClonesArray *adc;
  TClonesArray *tdc;
  TClonesArray *msc;
  
  UInt_t data[1000000];

  BmnStatus ProcessEvent(UInt_t *data, UInt_t len);
  
  BmnStatus Process_ADC64VE(UInt_t *data, UInt_t len, UInt_t serial); 
  BmnStatus Process_FVME(UInt_t *data, UInt_t len, UInt_t serial);
  
  BmnStatus FillTDC(UInt_t *d, UInt_t serial, UInt_t slot, UInt_t modId, UInt_t &idx);
  BmnStatus FillTRIG(UInt_t *d, UInt_t serial, UInt_t &idx);
  BmnStatus FillMSC(UInt_t *d, UInt_t serial, UInt_t &idx);
   
};
