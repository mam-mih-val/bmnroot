
/** @file BmnSsdElement.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 27.05.2013
 **/

#include "BmnSsdElement.h"

#include <cassert>
#include "TGeoManager.h"
#include "BmnSsdModule.h"
#include "BmnSsdSetup.h"
#include "BmnSsdStation.h"

#include <iomanip>

using std::left;
using std::right;
using std::setw;
using std::pair;
using std::multimap;

// -----   Default constructor   -------------------------------------------
BmnSsdElement::BmnSsdElement() : TNamed(),
                                 fAddress(0),
                                 fLevel(kSsdNofLevels),
                                 fNode(NULL),
                                 fDaughters(),
                                 fMother(NULL)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
BmnSsdElement::BmnSsdElement(Int_t address, Int_t level,
                             TGeoPhysicalNode* node,
                             BmnSsdElement* mother) :
    TNamed(),
    fAddress(address),
    fLevel(kSsdSystem),
    fNode(node),
    fDaughters(),
    fMother(mother)
{
  SetLevel(level);
  SetName(ConstructName(address, fLevel).Data());
}
// -------------------------------------------------------------------------



// ----- Construct the name of an element   --------------------------------
void BmnSsdElement::ConstructName() {

	// Set the name for the SSD system
	if ( GetLevel() == kSsdSystem ) {
		SetName("SSD");
		return;
	}

	// Special case half-ladder ("U"p or "D"own)
	if ( GetLevel() == kSsdHalfLadder ) {
		TString label;
		switch ( BmnSsdAddress::GetElementId(fAddress, kSsdHalfLadder) ) {
			case 0: label = "U"; break;
			case 1: label = "D"; break;
			default: break;
		}
		SetName( fMother->GetName() + label );
		return;
	}

	// For other levels: Expand the name of the mother
	TString label;
	switch ( GetLevel() ) {
		case kSsdUnit:   label = "_U"; break;
		case kSsdLadder: label = "_L"; break;
		case kSsdModule: label = "_M"; break;
		case kSsdSensor: label = "_S"; break;
		default: break;
	}
	label += Form("%02i",
			          BmnSsdAddress::GetElementId(fAddress, GetLevel()) + 1 );
	SetName( fMother->GetName() + label );

}
// -------------------------------------------------------------------------



// -----   Construct name from address   -----------------------------------
TString BmnSsdElement::ConstructName(Int_t address,
                                     ESsdElementLevel level) {

  TString result = "SSD";
  if ( level >= kSsdUnit ) {
    Int_t unit = BmnSsdAddress::GetElementId(address, kSsdUnit);
    result += Form("_U%02i", unit + 1);
    if ( level >= kSsdLadder ) {
      Int_t ladder = BmnSsdAddress::GetElementId(address, kSsdLadder);
      result += Form("_L%02i", ladder + 1);
      if ( level >= kSsdHalfLadder ) {
        Int_t hladder = BmnSsdAddress::GetElementId(address, kSsdHalfLadder);
        result += (hladder == 0 ? "U" : "D");
        if ( level >= kSsdModule ) {
          Int_t module = BmnSsdAddress::GetElementId(address, kSsdModule);
          result += Form("_M%02i", module + 1);
          if ( level >= kSsdSensor ) {
            Int_t sensor = BmnSsdAddress::GetElementId(address, kSsdSensor);
            result += Form("_S%02i", sensor + 1);
          }  //? sensor
        }  //? module
      }  //? halfladder
    }  //? ladder
  }  //? unit

  return result;
}
// -------------------------------------------------------------------------



// -----   Get a daughter element   ----------------------------------------
BmnSsdElement* BmnSsdElement::GetDaughter(Int_t index) const {
  if ( index < 0 || index >=GetNofDaughters() ) return NULL;
  return fDaughters[index];
}
// -------------------------------------------------------------------------



// -----   Get number of elements at lower hierarchy levels   --------------
Int_t BmnSsdElement::GetNofElements(Int_t level) const {

	Int_t nElements = 0;
	if ( level <= fLevel ) nElements = 0;
	else if ( level == fLevel + 1) nElements = GetNofDaughters();
	else
		for (Int_t iDaughter = 0; iDaughter < GetNofDaughters(); iDaughter++)
			nElements += GetDaughter(iDaughter)->GetNofElements(level);

	return nElements;
}
// -------------------------------------------------------------------------



// -----   Recursively read daughters from geometry   ----------------------
void BmnSsdElement::InitDaughters() {

  // --- Catch absence of TGeoManager
  assert( gGeoManager );

  // --- No daughter elements below sensor level
  if ( fLevel > kSsdSensor ) return;

  // --- Catch physical node not being set
  if ( ! fNode ) {
    LOG(error) << fName << ": physical node is not set!";
    return;
  }

  TGeoNode* mNode = fNode->GetNode();   // This node
  TString   mPath = fNode->GetName();   // Full path to this node

  for (Int_t iNode = 0; iNode < mNode->GetNdaughters(); iNode++) {

    // Check name of daughter node for level name
    TString dName = mNode->GetDaughter(iNode)->GetName();
    if ( dName.Contains(BmnSsdSetup::Instance()->GetLevelName(fLevel + 1),
                        TString::kIgnoreCase ) ) {

      // Create physical node
      TString dPath = mPath + "/" + dName;
      TGeoPhysicalNode* pNode = new TGeoPhysicalNode(dPath.Data());

      // Create element and add it as daughter
      UInt_t address = BmnSsdAddress::SetElementId(fAddress,
                                                   fLevel + 1,
                                                   GetNofDaughters());
      BmnSsdElement* dElement = NULL;
      switch ( fLevel) {
      	case kSsdHalfLadder:
      		dElement = new BmnSsdModule(address, pNode, this); break;
     	default:
      		dElement = new BmnSsdElement(address, fLevel+1, pNode, this); break;
      }
      fDaughters.push_back(dElement);

      // Call init method recursively for the daughter elements
      dElement->InitDaughters();

    } // name of daughter node

  } // daughter node loop

}
// -------------------------------------------------------------------------



// -----   Print   ---------------------------------------------------------
void BmnSsdElement::Print(Option_t* opt) const {
  LOG(info) << setw(10) << right << fAddress << "  "
		    << setw(12) << left << fName
		    << "  type " << setw(22) << fTitle << "  path "
		    << fNode->GetName() << "  " << fNode->GetTitle();
  if ( opt[0] == 'R' ) {
	  for (Int_t iDaughter = 0; iDaughter < GetNofDaughters(); iDaughter++)
		  GetDaughter(iDaughter)->Print("R");
  }
}
// -------------------------------------------------------------------------



// -----   Set element level   ---------------------------------------------
void BmnSsdElement::SetLevel(Int_t level) {
  switch (level) {
    case kSsdSystem:     fLevel = kSsdSystem;     break;
    case kSsdUnit   :    fLevel = kSsdUnit;       break;
    case kSsdLadder:     fLevel = kSsdLadder;     break;
    case kSsdHalfLadder: fLevel = kSsdHalfLadder; break;
    case kSsdModule:     fLevel = kSsdModule;     break;
    case kSsdSensor:     fLevel = kSsdSensor;     break;
    default: LOG(fatal) << fName << ": Illegal element level "
        << level; break;
  }
}
// -------------------------------------------------------------------------


ClassImp(BmnSsdElement)

