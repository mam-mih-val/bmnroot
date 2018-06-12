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
}

TString find_path_to_URQMD_files ()
{
  TString hostname = gSystem->HostName();
  TString path_to_URQMD_files;

  if ((hostname=="nc2.jinr.ru")||(hostname=="nc3.jinr.ru") ||
      (hostname=="nc8.jinr.ru")||(hostname=="nc9.jinr.ru") || 
      (hostname=="nc10.jinr.ru")||(hostname=="nc11.jinr.ru")) {
    path_to_URQMD_files="/nica/mpd1/data4mpd/UrQMD/1.3/";
  }
  else {
    if ((hostname=="lxmpd-ui.jinr.ru")||(hostname=="lxmpd-ui"))    // linux farm
      path_to_URQMD_files = "/opt/exp_soft/mpd/urqmd/";
    else {
      if ( (hostname=="mpd")||(hostname=="mpd.jinr.ru")
           ||(hostname=="nc12.jinr.ru")||(hostname=="nc13.jinr.ru")||(hostname=="se63-36.jinr.ru")
	   ||(hostname=="se63-37.jinr.ru")||(hostname=="se63-40.jinr.ru")||(hostname=="se51-99.jinr.ru") )
	path_to_URQMD_files = "/opt/data/";                        // mpd, nc11
      else{
	if (hostname == "seashore")
          path_to_URQMD_files = "/data/mpd/";
	else {
	  if ((hostname=="kanske")||(hostname=="kanske.itep.ru"))     // Moscow
	    path_to_URQMD_files ="/scratch2/kmikhail/data4mpd/UrQMD/1.3/";
	  else 
            path_to_URQMD_files = gSystem->Getenv("HOME") + TString("/");
	}
      }
    }
  }
  return  path_to_URQMD_files;
}

// check whether file exists
bool CheckFileExist(TString fileName){
    gSystem->ExpandPathName(fileName);
    if (gSystem->AccessPathName(fileName.Data()) == true)
    {
        cout<<endl<<"no specified file: "<<fileName<<endl;
        return false;
    }

    return true;
}
