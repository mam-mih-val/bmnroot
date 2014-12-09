#include "BmnManageQA.h"
#include <TDirectory.h>
#include <TFile.h>
#include <TError.h>

std::vector<std::string> pathToList(const char *path);



TH1F* CreateHistogram1(const char* name, const char *xtitle, int nbin,
									    Float_t firstBin, Float_t lastBin)
{
 TH1F* h = new TH1F(name, name, nbin, firstBin, lastBin);
 h->SetXTitle(xtitle);
 return h;
}

TH2F* CreateHistogram2(const char* name, const char* xtitle, const char* ytitle, 
			int nbinx, Float_t firstBinX, Float_t lastBinX, int nbiny, Float_t firstBinY, Float_t lastBinY)
{
 TH2F *h = new TH2F(name, name, nbinx, firstBinX, lastBinX, nbiny, firstBinY, lastBinY);
 h->SetXTitle(xtitle);
 h->SetYTitle(ytitle);
 return h;
}

TH3F* CreateHistogram3(const char* name, const char* xtitle, const char* ytitle, const char* ztitle, 
			int nbinx, Float_t firstBinX, Float_t lastBinX, int nbiny, Float_t firstBinY, Float_t lastBinY,
			int nbinz, Float_t firstBinZ, Float_t lastBinZ)
{
 TH3F *h = new TH3F(name, name, nbinx, firstBinX, lastBinX, nbiny, firstBinY, lastBinY, nbinz, firstBinZ, lastBinZ);
 h->SetXTitle(xtitle);
 h->SetYTitle(ytitle);
 h->SetZTitle(ztitle);
 return h;
}

//go to given directory if directory dose not exists create it

void toDirectory(const char* dir)
{
 std::vector<std::string> splitedPath = pathToList(dir);
 TDirectory *d = (TDirectory*) gFile;
 for(int i = 0; i < splitedPath.size(); i++)
 {
  TDirectory *nextDir = (TDirectory*) d->FindObject( splitedPath[i].c_str() );
  if (! nextDir)
  {
      d->mkdir( splitedPath[i].c_str() );
      nextDir = (TDirectory*) d->GetDirectory( splitedPath[i].c_str() );
  }
  if ( !nextDir)
       Error("QAHelpers::toDir","Can't create/find directory");
  d = nextDir;
 }
 gFile->cd(dir);
}

//split path for components using '/' as delimeter
std::vector<std::string> pathToList(const char *path)
{
 std::vector<std::string> result;
 
 const char *begin = path;
 const char *index = path;
 
 while ( *index )
 {
   for( ; *index != 0 && *index != '/'; index++);
   if (begin != index)
   	result.push_back( std::string(begin, index - begin ) );
   for(; *index != 0 && *index == '/'; index++);                                   // if several delimeters then skip them all
   begin = index;
 }
 return result;
}

const TGeoShape* getShape(const TGeoManager *g, const char* name)
{
  if (! g)
  {
    Error("getShape", "Geo Manager not initialized");
    return 0;
  }
  TGeoVolume *vol = gGeoManager->GetVolume(name);
  if (! vol)
  {
    Error("getShape", "Volume %s not found", name);
    return 0;
  }
  
  return vol->GetShape();
}

//Suffixed versions of CreateHistograms...

TH1F* CreateHistogram1(const char* name, const std::string &suffix, const char *xtitle, int nbin,
									    Float_t firstBin, Float_t lastBin)
{
 std::string fullName = std::string(name) + suffix;
 return CreateHistogram1(fullName.c_str(), xtitle, nbin, firstBin, lastBin);
}
TH2F* CreateHistogram2(const char* name, const std::string &suffix, const char* xtitle, const char* ytitle, 
			int nbinx, Float_t firstBinX, Float_t lastBinX, int nbiny, Float_t firstBinY, Float_t lastBinY)
{
 std::string fullName = std::string(name) + suffix;
 return CreateHistogram2(fullName.c_str(), xtitle, ytitle, nbinx, firstBinX, lastBinX, nbiny, firstBinY, lastBinY);
}
TH3F* CreateHistogram3(const char* name, const std::string &suffix, const char* xtitle, const char* ytitle, 
			const char* ztitle, 
			int nbinx, Float_t firstBinX, Float_t lastBinX, int nbiny, Float_t firstBinY, Float_t lastBinY,
			int nbinz, Float_t firstBinZ, Float_t lastBinZ)
{
 std::string fullName = std::string(name) + suffix;
 return CreateHistogram3(fullName.c_str(), xtitle, ytitle, ztitle, nbinx, firstBinX, lastBinX, nbiny, firstBinY, lastBinY,
 			  nbinz, firstBinZ, lastBinZ);
}
