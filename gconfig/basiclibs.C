// Macro for loading basic libraries used with both Geant3 and Geant4

Bool_t isLibrary(const char* libName)
{
  if (TString(gSystem->DynamicPathName(libName, kTRUE)) != TString(""))
    return kTRUE;
  else
    return kFALSE;
}

void basiclibs()
{
  gSystem->Load("libRIO");
  gSystem->Load("libGeom");
  gSystem->Load("libGeomPainter");
  gSystem->Load("libVMC");
  gSystem->Load("libEG");
  gSystem->Load("libEGPythia6");
  if (isLibrary("libpythia6.so"))
    gSystem->Load("libpythia6.so");
  else if (isLibrary("libPythia6.so")) // Old FairSoft
    gSystem->Load("libPythia6.so");
  gSystem->Load("libPhysics");
  gSystem->Load("libNet");
  gSystem->Load("libTree");
  gSystem->Load("libMinuit");
  gSystem->Load("libMathMore");

  gSystem->Load("libProof");
  gSystem->Load("libProofPlayer");
  gSystem->Load("libGX11TTF");
  gSystem->Load("libGX11");
  gSystem->Load("libboost_regex");

  gSystem->Load("libGdml");
  gSystem->Load("libXMLParser");
}
