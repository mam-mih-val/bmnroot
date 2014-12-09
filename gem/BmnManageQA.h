#ifndef BmnManageQA_HH
#define BmnManageQA_HH

#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TGeoManager.h>
#include <iostream>

TH1F* CreateHistogram1(const char* name, const char* xtitle, int nbin, Float_t firstBin, Float_t lastBin);
TH2F* CreateHistogram2(const char* name, const char* xtitle, const char* ytitle, 
			int nbinx, Float_t firstBinX, Float_t lastBinX, int nbiny, Float_t firstBinY, Float_t lastBinY);
TH3F* CreateHistogram3(const char* name, const char* xtitle, const char* ytitle, const char* ztitle,
			int nbinx, Float_t firstBinX, Float_t lastBinX, int nbiny, Float_t firstBinY, Float_t lastBinY,
			int nbinz, Float_t firstBinZ, Float_t lastBinZ);

// The same with suffixes
TH1F* CreateHistogram1(const char* name, const std::string&, const char* xtitle, int nbin, Float_t firstBin, Float_t lastBin);
TH2F* CreateHistogram2(const char* name, const std::string&, const char* xtitle, const char* ytitle, 
			int nbinx, Float_t firstBinX, Float_t lastBinX, int nbiny, Float_t firstBinY, Float_t lastBinY);
TH3F* CreateHistogram3(const char* name,const std::string&,  const char* xtitle, const char* ytitle, const char* ztitle,
			int nbinx, Float_t firstBinX, Float_t lastBinX, int nbiny, Float_t firstBinY, Float_t lastBinY,
			int nbinz, Float_t firstBinZ, Float_t lastBinZ);

void toDirectory(const char*);

const TGeoShape* getShape(const TGeoManager *g, const char* name);

#define ENTRY_POINT(x)                                        \
     std::cout << "-=TPC=- " << __FUNCTION__ << ": " << x \
                    << " (" << __FILE__                                \
                    << ", " << __LINE__ << ")" << std::endl

#endif

