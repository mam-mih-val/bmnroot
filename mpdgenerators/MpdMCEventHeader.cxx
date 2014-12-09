/** MpdMCEventHeader.cxx
 *  @author V.Voronyuk, origin: CBMRoot
 ** Data class (level MC) containing information about the input event.
 **/


#include "MpdMCEventHeader.h"
#include "FairRootManager.h"

#include <iostream>
using std::cout;
using std::endl;

// -----   Default constructor   ---------------------------------------
MpdMCEventHeader::MpdMCEventHeader() 
  : FairMCEventHeader(),
    fPhi (0.)
{
}
// -----   Constructor with run identifier   ---------------------------
MpdMCEventHeader::MpdMCEventHeader(UInt_t runId) 
  : FairMCEventHeader(runId),
    fPhi (0.)
{
}
// -----   Standard constructor   --------------------------------------
MpdMCEventHeader::MpdMCEventHeader(UInt_t runId, Int_t iEvent, 
          Double_t x, Double_t y,  Double_t z, Double_t t,
          Double_t b, Double_t phi, Int_t nPrim)
  : FairMCEventHeader(iEvent, x, y, z, t, b, nPrim),
    fPhi(phi)
{
}
// -----   Destructor   ------------------------------------------------
MpdMCEventHeader::~MpdMCEventHeader() { }
// ---------------------------------------------------------------------


// -----   Public method Reset   ---------------------------------------
void MpdMCEventHeader::Reset()
{
  FairMCEventHeader::Reset();
  fPhi = 0.;
}
// ---------------------------------------------------------------------

void MpdMCEventHeader::Register()
{ 
  cout <<"Register MpdMCEventHeader."<<endl; 
  FairRootManager::Instance()->Register("MCEventHeader.", "Event", this , kTRUE);
}
// ---------------------------------------------------------------------
ClassImp(MpdMCEventHeader)
