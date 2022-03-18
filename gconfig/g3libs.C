/// \file g3libs.C
/// \brief Macro for loading Geant3 and Pythia 6 libraries

Bool_t isLibrary(const char* libName)
{
  if (TString(gSystem->DynamicPathName(libName, kTRUE)) != TString(""))
    return kTRUE;
  else  
    return kFALSE;
}    

void g3libs()
{
  cout << "Loading Geant3 libraries ..." << endl;

  if (isLibrary("libdummies.so"))
    gSystem->Load("libdummies.so");
                   // libdummies.so needed from geant3_+vmc version 0.5

  if (isLibrary("libpythia6.so"))
    gSystem->Load("libpythia6.so");
  else if (isLibrary("libPythia6.so")) // Old FairSoft
    gSystem->Load("libPythia6.so");
  gSystem->Load("libEGPythia6.so");
  gSystem->Load("libgeant321.so");

  cout << "Loading Geant3 libraries ... finished" << endl;
}
