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
    LOG(fatal) << "fName: no TGeoManager!";
    return kFALSE;
  }

  // --- Get cave (top node)
  LOG(info) << fName << ": " << " Reading geometry from TGeoManager "
            << geo->GetName();
  geo->CdTop();
  TGeoNode* cave = geo->GetCurrentNode();
  LOG(info) << "Top node: " << cave->GetName();

  // --- Get top STS node
  TGeoNode* sts = NULL;
  for (Int_t iNode = 0; iNode < cave->GetNdaughters(); iNode++) {
    TString name = cave->GetDaughter(iNode)->GetName();
     if ( name.Contains("STS", TString::kIgnoreCase) ) {
      sts = cave->GetDaughter(iNode);
      geo->CdDown(iNode);
      LOG(info) << fName << ": found STS node " << sts->GetName();
      break;
    }
  }
  if ( ! sts ) {
    LOG(error) << fName << ": No top STS node found in geometry!";
    return kFALSE;
  }

  // --- Create physical node for sts
  TString path = cave->GetName();
  path = path + "/" + sts->GetName();
  LOG(info) << "Path to STS is " << path;
  fNode = new TGeoPhysicalNode(path);
  LOG(info) << "PN name " << fNode->GetName();

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

