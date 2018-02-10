#include "BmnGemDigitizerQAHistograms.h"

BmnGemGeo* geo = new BmnGemGeo();

BmnGemDigitizerQAHistograms::BmnGemDigitizerQAHistograms() :
_hX_local(0), _hY_local(0), _hZ_local(0),
_hXY_local(0), _hYZ_local(0), _hXZ_local(0), _hADCX(0), _hADCY(0), 
_hDigiAdcX(0), _hDigiAdcY(0), _hDigiX(0), _hDigiY(0)
//_hRZ_global(0), _hYZ_local(0),
//_hSect_dig(0), _hADC_dig(0),
//_hX_dig(0), _hY_dig(0), _hZ_dig(0),
//_h3D_dig(0), _h3D_el(0),
//_hDiffuseXY(0), _hDistortXY(0),
//_hX_global(0), _hY_global(0), _hZ_global(0), _hXY_global(0),
//_hX_local(0), _hY_local(0), _hZ_local(0), _hXY_local(0),
//_hXT_dig_1(0), _hXT_dig_20(0), _hXT_dig_5(0), _hXT_dig_10(0), _hXT_dig_40(0), _hXT_dig_60(0) 
{
    for (Int_t i = 0; i < geo->GetNumOfStations(); i++) {
        X[i] = NULL;
        Y[i] = NULL;
        XY[i] = NULL;
        ADCX[i] = NULL;
        ADCY[i] = NULL;
    }
}

BmnGemDigitizerQAHistograms::BmnGemDigitizerQAHistograms(const std::string& suffix) :
_hX_local(0), _hY_local(0), _hZ_local(0),
_hXY_local(0), _hYZ_local(0), _hXZ_local(0), _hADCX(0), _hADCY(0), 
_hDigiAdcX(0), _hDigiAdcY(0), _hDigiX(0), _hDigiY(0)
//_hRZ_global(0), _hYZ_local(0),
//_hSect_dig(0),  _hADC_dig(0),
//_hX_dig(0), _hY_dig(0), _hZ_dig(0),
//_h3D_dig(0),
//_hDiffuseXY(0), _hDistortXY(0),
//_hX_global(0), _hY_global(0), _hZ_global(0), _hXY_global(0),
//_hX_local(0), _hY_local(0), _hZ_local(0), _hXY_local(0),
//_hXT_dig_1(0), _hXT_dig_20(0), _hXT_dig_5(0), _hXT_dig_10(0), _hXT_dig_40(0), _hXT_dig_60(0) 
{
    for (Int_t i = 0; i < geo->GetNumOfStations(); i++) {
        X[i] = NULL;
        Y[i] = NULL;
        XY[i] = NULL;
        ADCX[i] = NULL;
        ADCY[i] = NULL;
        DIGIX[i] = NULL;
        DIGIADCX[i] = NULL;
    }
}

BmnGemDigitizerQAHistograms::~BmnGemDigitizerQAHistograms() {
     
    delete _hYZ_local;
    delete _hX_local;
    delete _hY_local;
    delete _hZ_local;
    delete _hXY_local;
    delete _hXZ_local;
    delete _hADCX;
    delete _hADCY;
    delete _hDigiX;
    delete _hDigiY;
    delete _hDigiAdcX;
    delete _hDigiAdcY;
  
    for (Int_t i = 0; i < geo->GetNumOfStations(); i++) { 
        delete X[i];
        delete Y[i];
        delete XY[i];
        delete ADCX[i];
        delete ADCY[i];
        delete DIGIX[i];
        delete DIGIY[i];
        delete DIGIADCX[i];
        delete DIGIADCY[i];
    }
    delete geo;
}

void BmnGemDigitizerQAHistograms::Initialize() {
    
    Float_t Xmin = geo->GetX0();
    Float_t Ymin = geo->GetY0();
    Float_t Zmin = geo->GetZ0();
    Float_t Zmax = Zmin + geo->GetDistAlongZ() * geo->GetNumOfStations();

//    UInt_t nTimeBin = 512;
//    TpcSector* sector = new TpcSector();
//    UInt_t nSectors = sector->GetNSectors();
//    UInt_t nRows = sector->GetNumRows();
////    UInt_t nInRows = sector->GetNumInnerRows();
//    UInt_t nPads = sector->GetNumPadsInRow(nRows - 1) * 2;
//    Float_t zMax = sector->GetLength();
//    Float_t maxX = sector->GetMaxX();
//    Float_t maxY = sector->GetSectHeight() + 1.0;
//    Float_t Rmax = sector->GetRmin() + sector->GetSectHeight();
//        
//    _h3D_el = CreateHistogram3("3D distribution of electrons in TPC", "X (cm)", "Y (cm)", "Z (cm)", 200, -150, 150, 200, -150, 150, 200, -zMax, zMax);
        
    const Int_t dim = 500;
    
    _hX_local = CreateHistogram1("local X distribution of electrons in all GEM-stations", _suffix, "X, cm", 100, Xmin, -Xmin);
    _hY_local = CreateHistogram1("local Y distribution of electrons in all GEM-stations", _suffix, "Y, cm", 100, Ymin, -Ymin);
    _hZ_local = CreateHistogram1("local Z distribution of electrons in all GEM-stations", _suffix, "Z, cm", 100, Zmin, Zmax);
    
    Char_t buffX[dim] = " ", buffY[dim] = " ", buffZ[dim] = " ";
    
    for (Int_t i = 0; i < geo->GetNumOfStations(); i++) {
        
        sprintf(buffX, "local X distribution of electrons in GEM%d", i);
        sprintf(buffY, "local Y distribution of electrons in GEM%d", i);
                
        X[i] = CreateHistogram1(buffX, _suffix, "X, cm", 100, Xmin, -Xmin);
        Y[i] = CreateHistogram1(buffY, _suffix, "Y, cm", 100, Ymin, -Ymin);
    }
     
    _hXY_local = CreateHistogram2("local XY distribution of electrons in all GEM-stations", _suffix, "X, cm", "Y, cm", 500, Xmin, -Xmin, 500, Ymin, -Ymin);   
    _hYZ_local = CreateHistogram2("local YZ distribution of electrons in all GEM-stations", _suffix, "Y, cm", "Z, cm", 500,  Ymin, -Ymin, 500, Zmin, Zmax);
    _hXZ_local = CreateHistogram2("local XZ distribution of electrons in all GEM-stations", _suffix, "X, cm", "Z, cm", 500, Xmin, -Xmin, 500, Zmin, Zmax);
    
    for (Int_t i = 0; i < geo->GetNumOfStations(); i++) {
        
        sprintf(buffX, "local XY distribution of electrons in GEM%d", i);
        sprintf(buffY, "local YZ distribution of electrons in GEM%d", i);
        sprintf(buffZ, "local XZ distribution of electrons in GEM%d", i);
                
        XY[i] = CreateHistogram2(buffX, _suffix, "X, cm", "Y, cm", 500, Xmin, -Xmin, 500, Ymin, -Ymin); 
    }
    
    _hADCX = CreateHistogram1("X_ADC distribution in all GEM-stations", _suffix, "ADC, x-strips", 100, 0., 10000.);
    _hADCY = CreateHistogram1("Y_ADC distribution in all GEM-stations", _suffix, "ADC, y-strips", 100, 0., 10000.);
    
    for (Int_t i = 0; i < geo->GetNumOfStations(); i++) {
     
        sprintf(buffX, "X_ADC distribution in GEM%d", i);
        sprintf(buffY, "Y_ADC distribution in GEM%d", i);
        
        ADCX[i] = CreateHistogram1(buffX, _suffix, "ADC, x-strips", 100, 0., 10000.);
        ADCY[i] = CreateHistogram1(buffY, _suffix, "ADC, y-strips", 100, 0., 10000.);
    }
    
    _hDigiX = CreateHistogram1("X_DIGIT distribution in all GEM-stations", _suffix, "DIGITS, x-strips", 100, 0., 2000.);
    _hDigiY = CreateHistogram1("Y_DIGIT distribution in all GEM-stations", _suffix, "DIGITS, y-strips", 100, 0., 2000.);
    
    for (Int_t i = 0; i < geo->GetNumOfStations(); i++) {
     
        sprintf(buffX, "X_DIGIT distribution in GEM%d", i);
        sprintf(buffY, "Y_DIGIT distribution in GEM%d", i);
        
        DIGIX[i] = CreateHistogram1(buffX, _suffix, "DIGITS, x-strips", 100, 0., 2000.);
        DIGIY[i] = CreateHistogram1(buffY, _suffix, "DIGITS, y-strips", 100, 0., 2000.);
    }
    
    _hDigiAdcX = CreateHistogram1("X_DIGIT distribution weighted to ADC in all GEM-stations", _suffix, "DIGITS, x-strips", 100, 0., 2000.);
    _hDigiAdcY = CreateHistogram1("Y_DIGIT distribution weighted to ADC in all GEM-stations", _suffix, "DIGITS, y-strips", 100, 0., 2000.);
    
    for (Int_t i = 0; i < geo->GetNumOfStations(); i++) {
     
        sprintf(buffX, "X_DIGIT weighted to ADC distribution in GEM%d", i);
        sprintf(buffY, "Y_DIGIT weighted to ADC distribution in GEM%d", i);
        
        DIGIADCX[i] = CreateHistogram1(buffX, _suffix, "DIGITS weighted to ADC, x-strips", 100, 0., 2000.);
        DIGIADCY[i] = CreateHistogram1(buffY, _suffix, "DIGITS weighted to ADC, y-strips", 100, 0., 2000.);
    }
//
//    _hX_global = CreateHistogram1("global X distribution of electrons in TPC", _suffix, "X, cm", 200, -150, 150);
//    _hY_global = CreateHistogram1("global Y distribution of electrons in TPC", _suffix, "Y, cm", 200, -150, 150);
//    _hZ_global = CreateHistogram1("global Z distribution of electrons in TPC", _suffix, "Z, cm", 200, -zMax, zMax);
//    _hXY_global = CreateHistogram2("global XY distribution of electrons in TPC", _suffix, "X, cm", "Y, cm", 800, -150, 150, 800, -150, 150);
//    _hRZ_global = CreateHistogram2("global ZR distribution of electrons in TPC", _suffix, "Z, cm", "R, cm", 800, -zMax, zMax, 800, -Rmax, Rmax);
//
//    _hSect_dig = CreateHistogram1("Sector distribution of TPC digits", _suffix, "number of sector", nSectors, 0, nSectors);
//    _hADC_dig = CreateHistogram1("Charge distribution of TPC digits", _suffix, "charge", 200, 900, 2000000);
//    _hX_dig = CreateHistogram1("X distribution of TPC digits", _suffix, "X, pads", nPads, 0, nPads);
//    _hY_dig = CreateHistogram1("Y distribution of TPC digits", _suffix, "Y, pad rows", nRows, 0, nRows);
//    _hZ_dig = CreateHistogram1("Z distribution of TPC digits", _suffix, "Z, time bins", nTimeBin, 0, nTimeBin);
//    _hXY_dig = CreateHistogram2("XY distribution of TPC digits", _suffix, "X, pads", "Y, pad rows", nPads, 0, nPads, nRows, 0, nRows);
//    _h3D_dig = CreateHistogram3("XYZ distribution of TPC digits", _suffix, "X, pads", "Y, pad rows", "Z, time bins", nPads, 0, nPads, nRows, 0, nRows, nTimeBin, 0, nTimeBin);
//    _hDiffuseXY = CreateHistogram2("XY diffuse of electrons", _suffix, "X (cm)", "Y (cm)", 400, -2.0, 2.0, 400, -2.0, 2.0);
//    _hDistortXY = CreateHistogram2("XY distortion of electrons", _suffix, "X (cm)", "Y (cm)", 400, -2.0, 2.0, 400, -2.0, 2.0);
//
//    _hXT_dig_1 = CreateHistogram2("XT distribution of TPC digits on PadRow #1", _suffix, "X, pads", "T, time bins", nPads, 0, nPads, nTimeBin, 0, nTimeBin);
//    _hXT_dig_5 = CreateHistogram2("XT distribution of TPC digits on PadRow #5", _suffix, "X, pads", "T, time bins", nPads, 0, nPads, nTimeBin, 0, nTimeBin);
//    _hXT_dig_10 = CreateHistogram2("XT distribution of TPC digits on PadRow #10", _suffix, "X, pads", "T, time bins", nPads, 0, nPads, nTimeBin, 0, nTimeBin);
//    _hXT_dig_20 = CreateHistogram2("XT distribution of TPC digits on PadRow #20", _suffix, "X, pads", "T, time bins", nPads, 0, nPads, nTimeBin, 0, nTimeBin);
//    _hXT_dig_40 = CreateHistogram2("XT distribution of TPC digits on PadRow #40", _suffix, "X, pads", "T, time bins", nPads, 0, nPads, nTimeBin, 0, nTimeBin);
//    _hXT_dig_60 = CreateHistogram2("XT distribution of TPC digits on PadRow #60", _suffix, "X, pads", "T, time bins", nPads, 0, nPads, nTimeBin, 0, nTimeBin);
}

void BmnGemDigitizerQAHistograms::Write() {

    _hYZ_local->Write(0, kOverwrite);
    _hX_local->Write(0, kOverwrite);
    _hY_local->Write(0, kOverwrite);
    _hZ_local->Write(0, kOverwrite);
    _hXY_local->Write(0, kOverwrite);
    _hXZ_local->Write(0, kOverwrite);
    _hADCX->Write(0, kOverwrite);
    _hADCY->Write(0, kOverwrite);
    _hDigiX->Write(0, kOverwrite);
    _hDigiY->Write(0, kOverwrite);
    _hDigiAdcX->Write(0, kOverwrite);
    _hDigiAdcY->Write(0, kOverwrite);

    for (Int_t i = 0; i < geo->GetNumOfStations(); i++) {
        X[i]->Write(0, kOverwrite);
        Y[i]->Write(0, kOverwrite);
        XY[i]->Write(0, kOverwrite);
        ADCX[i]->Write(0, kOverwrite);
        ADCY[i]->Write(0, kOverwrite);
        DIGIX[i]->Write(0, kOverwrite);
        DIGIY[i]->Write(0, kOverwrite);
        DIGIADCX[i]->Write(0, kOverwrite);
        DIGIADCY[i]->Write(0, kOverwrite);
    }
}


ClassImp(BmnGemDigitizerQAHistograms);
