#ifndef  BMNGEMDIGITIZERQAHISTOGRAMS_H
#define  BMNGEMDIGITIZERQAHISTOGRAMS_H

// Base Class Headers ----------------
#include <TObject.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include "BmnManageQA.h"
#include "BmnGemGeo.h"

class BmnGemDigitizerQAHistograms : public TObject {
private:
 std::string _suffix;
public:

   BmnGemDigitizerQAHistograms();
   BmnGemDigitizerQAHistograms(const std::string& suffix);
  ~BmnGemDigitizerQAHistograms();

  void Initialize();
  void Write();
  
//  TH2F* _hRZ_global;
//  TH2F* _hYZ_local;
//  
//  TH1F *_hY_global;
//  TH1F *_hX_global;
//  TH1F *_hZ_global;
//  TH1F *_hZ_local;
//  TH1F *_hY_local;
//  TH1F *_hX_local;
//  TH2F *_hXY_local;
//  TH2F *_hXY_global;
//  TH1F *_hSect_dig;
//  TH1F *_hADC_dig;
//  TH1F *_hZ_dig;
//  TH1F *_hY_dig;
//  TH1F *_hX_dig;
//  TH2F *_hXY_dig;
//  TH3F *_h3D_dig;
//  TH2F *_hDiffuseXY;
//  TH2F *_hDistortXY;
//  TH3F *_h3D_el;
//  
//  TH2F *_hXT_dig_1;
//  TH2F *_hXT_dig_5;
//  TH2F *_hXT_dig_10;
//  TH2F *_hXT_dig_20;
//  TH2F *_hXT_dig_40;
//  TH2F *_hXT_dig_60;
  
  // All GEM-stations
  TH1F* _hX_local;     // distribution of electrons over X-coordinate
  TH1F* _hY_local;     // distribution of electrons over Y-coordinate
  TH1F* _hZ_local;     // distribution of electrons over Z-coordinate
  
  // distributions of electrons over X-coordinate for each GEM-station
  TH1F* X[8];
  
  // distributions of electrons over Y-coordinate for each GEM-station
  TH1F* Y[8];
     
  // All GEM-stations
  TH2F* _hXY_local;  // distribution of electrons in XY-plane
  TH2F* _hYZ_local;  // distribution of electrons in YZ-plane
  TH2F* _hXZ_local;  // distribution of electrons in XZ-plane
  
  // distributions of electrons in XY-plane for each GEM-station
  TH2F* XY[8];
  
  // distribution of ADCs for all GEM-stations, X-strips
  TH1F* _hADCX;
  
  // distribution of ADCs for all GEM-stations, Y-strips
  TH1F* _hADCY;
  
  // distribution of ADCs for each GEM-station, X-strips
  TH1F* ADCX[8];
  
  // distribution of ADCs for each GEM-station, Y-strips
  TH1F* ADCY[8];
  
  // distribution of digits for all GEM-stations, X-strips
  TH1F* _hDigiX;
  
  // distribution of digits for all GEM-stations, Y-strips
  TH1F* _hDigiY;
  
  // distribution of digits for each GEM-station, X-strips
  TH1F* DIGIX[8];
  
  // distribution of digits for each GEM-station, Y-strips
  TH1F* DIGIY[8];
  
  // distribution of digits for all GEM-stations weighted to ADC, X-strips
  TH1F* _hDigiAdcX;
  
  // distribution of digits for all GEM-stations weighted to ADC, Y-strips
  TH1F* _hDigiAdcY;
  
  // distribution of digits for each GEM-station weighted to ADC, X-strips
  TH1F* DIGIADCX[8];
  
  // distribution of digits for each GEM-station weighted to ADC, Y-strips
  TH1F* DIGIADCY[8];
   
     
public:
  ClassDef(BmnGemDigitizerQAHistograms,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
