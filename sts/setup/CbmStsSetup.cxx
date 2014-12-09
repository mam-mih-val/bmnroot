/** @file CbmStsSetup.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 27.05.2013
 **/


#include "TGeoManager.h"
#include "TGeoPhysicalNode.h"
#include "setup/CbmStsSetup.h"


// -----   Initialisation of static singleton pointer   ----------------------
CbmStsSetup* CbmStsSetup::fgInstance = NULL;
// ---------------------------------------------------------------------------



// -----   Level names   -----------------------------------------------------
const TString CbmStsSetup::fgkLevelName[] = { "sts",
                                              "station",
                                              "ladder",
                                              "halfladder",
                                              "module",
                                              "sensor",
                                              "side",
                                              "channel" };
// ---------------------------------------------------------------------------



// -----   Constructor   -----------------------------------------------------
CbmStsSetup::CbmStsSetup() : CbmStsElement("STS Setup",
                                           "system",
                                           kStsSystem)
{
}
// ---------------------------------------------------------------------------



// -----   Intialisation from TGeoManager   ----------------------------------
Bool_t CbmStsSetup::Init(TGeoManager* geo) {

  // --- Catch non-existence of GeoManager
  if ( ! geo ) {
    LOG(FATAL) << "fName: no TGeoManager!" << FairLogger::endl;
    return kFALSE;
  }

  // --- Get cave (top node)
  LOG(INFO) << fName << ": " << " Reading geometry from TGeoManager "
            << geo->GetName() << FairLogger::endl;
  geo->CdTop();
  TGeoNode* cave = geo->GetCurrentNode();
  LOG(INFO) << "Top node: " << cave->GetName() << FairLogger::endl;

  // --- Get top STS node
  TGeoNode* sts = NULL;
  for (Int_t iNode = 0; iNode < cave->GetNdaughters(); iNode++) {
    TString name = cave->GetDaughter(iNode)->GetName();
     if ( name.Contains("STS", TString::kIgnoreCase) ) {
      sts = cave->GetDaughter(iNode);
      geo->CdDown(iNode);
      LOG(INFO) << fName << ": found STS node " << sts->GetName()
                << FairLogger::endl;
      break;
    }
  }
  if ( ! sts ) {
    LOG(ERROR) << fName << ": No top STS node found in geometry!"
               << FairLogger::endl;
    return kFALSE;
  }

  // --- Create physical node for sts
  TString path = cave->GetName();
  path = path + "/" + sts->GetName();
  LOG(INFO) << "Path to STS is " << path << FairLogger::endl;
  fNode = new TGeoPhysicalNode(path);
  LOG(INFO) << "PN name " << fNode->GetName() << FairLogger::endl;

  // --- Initialise daughter elements
  InitDaughters();


  return kTRUE;
}
// ---------------------------------------------------------------------------




// -----   Instance   --------------------------------------------------------
CbmStsSetup* CbmStsSetup::Instance() {
  if ( ! fgInstance ) fgInstance = new CbmStsSetup();
  return fgInstance;
}
// ---------------------------------------------------------------------------

ClassImp(CbmStsSetup)

