/** @file CbmStsElement.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 27.05.2013
 **/


#include "TGeoManager.h"
#include "TGeoPhysicalNode.h"

#include "setup/CbmStsElement.h"
#include "setup/CbmStsSetup.h"


// -----   Default constructor   -------------------------------------------
CbmStsElement::CbmStsElement() : TNamed(),
                                 fAddress(0),
                                 fLevel(kStsNofLevels),
                                 fNode(NULL),
                                 fDaughters()
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsElement::CbmStsElement(const char* name, const char* title,
                             Int_t level,
                             TGeoPhysicalNode* node) :
                             TNamed(name, title),
                             fAddress(0),
                             fLevel(kStsSystem),
                             fNode(node),
                             fDaughters()
{
  SetLevel(level);
}
// -------------------------------------------------------------------------



// -----   Add a daughter element   ----------------------------------------
void CbmStsElement::AddDaughter(CbmStsElement* element) {
  if ( ! element ) return;
  if ( element->GetLevel() != fLevel + 1) {
    LOG(error) << fName << ": Trying to add an element of level "
               << element->GetLevel() << " as daughter of level "
               << fLevel << "! Command will be ignored.";
    return;

  }
  element->fAddress = CbmStsAddress::SetElementId(fAddress,
                                                  element->GetLevel(),
                                                  GetNofDaughters() );
  fDaughters.push_back(element);
}
// -------------------------------------------------------------------------



// -----   Get a daughter element   ----------------------------------------
CbmStsElement* CbmStsElement::GetDaughter(Int_t index) const {
  if ( index < 0 || index >=GetNofDaughters() ) return NULL;
  return fDaughters[index];
}
// -------------------------------------------------------------------------



// -----   Recursively read daughters from geometry   ----------------------
void CbmStsElement::InitDaughters() {


  // --- Catch absence of TGeoManager
  if ( ! gGeoManager ) {
    LOG(error) << fName << ": cannot initialise without TGeoManager!";
    return;
  }

  // --- No daughter elements below sensor level
  if ( fLevel > kStsSensor ) return;

  // --- Catch physical node not being set
  if ( ! fNode ) {
    LOG(error) << fName << ": physical node is not set!";
    return;
  }

  TGeoNode* mNode = fNode->GetNode();        // This node
  TString mPath = fNode->GetName();          // Path to this node
  for (Int_t iNode = 0; iNode < mNode->GetNdaughters(); iNode++) {

    // Check name of daughter node for level name
    TString dName = mNode->GetDaughter(iNode)->GetName();
    if ( dName.Contains(CbmStsSetup::GetLevelName(fLevel + 1),
                        TString::kIgnoreCase ) ) {

      // Create physical node
      const char* name = "";
      const char* title = CbmStsSetup::GetLevelName(fLevel+1);
      TString dPath = mPath + "/" + dName;
      TGeoPhysicalNode* pnNode = new TGeoPhysicalNode(dPath.Data());

      // Create element and add it as daughter
      CbmStsElement* dElement = new CbmStsElement(name, title,
                                                  fLevel + 1, pnNode);
      AddDaughter(dElement);

      // Call method recursively for the daughter elements
      dElement->InitDaughters();

    } // name of daughter node

  } // daughter node loop

}
// -------------------------------------------------------------------------



// -----   Print   ---------------------------------------------------------
void CbmStsElement::Print(Option_t* opt) const {
  LOG(info) << fName << ": " << fTitle << ", address " << fAddress
            << ", index " << GetIndex() << ", level " << fLevel;
}
// -------------------------------------------------------------------------



// -----   Set element level   ---------------------------------------------
void CbmStsElement::SetLevel(Int_t level) {
  switch (level) {
    case kStsSystem:     fLevel = kStsSystem;     break;
    case kStsStation:    fLevel = kStsStation;    break;
    case kStsLadder:     fLevel = kStsLadder;     break;
    case kStsHalfLadder: fLevel = kStsHalfLadder; break;
    case kStsModule:     fLevel = kStsModule;     break;
    case kStsSensor:     fLevel = kStsSensor;     break;
    default: LOG(error) << fName << ": trying to set the level to "
                        << level << " while max. level is "
                        << kStsNofLevels - 1; break;
  }
}
// -------------------------------------------------------------------------


ClassImp(CbmStsElement)

