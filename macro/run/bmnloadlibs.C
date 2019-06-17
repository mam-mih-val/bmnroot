R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "gconfig/basiclibs.C"

void bmnloadlibs()
{
  // Load basic libraries
  basiclibs();
  // Load other libraries
  gSystem->Load("libBase");
  gSystem->Load("libFairTools");
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBmnField");
  gSystem->Load("libPassive");
  gSystem->Load("libGen");
  gSystem->Load("libTrkBase");
  gSystem->Load("libBmnData");
  gSystem->Load("libMpdGen");

  gSystem->Load("libTOF");
  gSystem->Load("libPsd");
  gSystem->Load("libSts");
  gSystem->Load("libRecoil");
  gSystem->Load("libZdc");
  gSystem->Load("libEcal");
  gSystem->Load("libBd");
  gSystem->Load("libBmnSteering");
  gSystem->Load("libGem");
  gSystem->Load("libSilicon");
  gSystem->Load("libBmnSsd");
  gSystem->Load("libCSC");

  gSystem->Load("libTOF1");
  gSystem->Load("libDch");
  gSystem->Load("libMwpc");
  gSystem->Load("libBmnQa");

  gSystem->Load("libGeane");
  gSystem->Load("libKF");
  gSystem->Load("libCat");

  gSystem->Load("libUniDb");
  gSystem->Load("libBmnAlignment");
  gSystem->Load("libGlobalTracking");
  gSystem->Load("libzmq");
  gSystem->Load("libBmnParticles");
  gSystem->Load("libDecoder");
  gSystem->Load("libBmnMisc");
}
