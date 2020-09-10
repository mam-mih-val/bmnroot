/** @file BmnDigi.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @date 12.12.2018
 **/

#include "BmnDigi.h"

#include <sstream>
#include "BmnMatch.h"

#include "FairMultiLinkedData.h"

#include <memory>
#include <utility> // std::forward


using std::string;
using std::stringstream;



// -----   Default constructor   -------------------------------------------
BmnDigi::BmnDigi()
 : TObject(),
   fMatch(NULL)
{
}
// -------------------------------------------------------------------------



// -----   Copy constructor (deep copy)   ----------------------------------
BmnDigi::BmnDigi(const BmnDigi& rhs)
  : TObject(rhs),
    fMatch(NULL)
{
  if ( rhs.fMatch ) {
		fMatch = new BmnMatch();
		fMatch->AddLinks( *(rhs.fMatch) );
	}
}
// -------------------------------------------------------------------------



// -----   Move constructor  -----------------------------------------------
BmnDigi::BmnDigi(BmnDigi&& other)
  : TObject(std::forward<BmnDigi>(other)), // should be std::forward
    fMatch(nullptr)
{
	fMatch = other.fMatch;
	other.fMatch = nullptr;
}
// -------------------------------------------------------------------------



// -----   Assignment operator (deep copy)  --------------------------------
BmnDigi& BmnDigi::operator=(const BmnDigi& rhs)
{
  if (this != &rhs) {
    TObject::operator=(rhs);
    if ( rhs.fMatch ) {
  		fMatch = new BmnMatch();
  		fMatch->AddLinks( *(rhs.fMatch) );
    }
    else fMatch = NULL;
  }
  return *this;
}
// -------------------------------------------------------------------------



// -----   Assignment operator (deep copy)  --------------------------------
BmnDigi& BmnDigi::operator=(BmnDigi&& other)
{
  if (this != &other) {
	TObject::operator=(std::forward<BmnDigi>(other));
	// Free the existing resource
	delete fMatch;
	fMatch = other.fMatch;
	other.fMatch = nullptr;
  }
  return *this;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
BmnDigi::~BmnDigi() {
  if ( fMatch) delete fMatch;
}
// -------------------------------------------------------------------------



// -----   Info to string   ------------------------------------------------
string BmnDigi::ToString() const {
  stringstream ss;
  ss << "Digi: address " << GetAddress() << " | time " << GetTime();
  return ss.str();
}
// -------------------------------------------------------------------------


// The following functions are only implemented for the unit tests.
// They can only be called from a derived class via BmnDigi::GetAddress()
Int_t BmnDigi::GetAddress() const
{
  return -111;
}

Int_t BmnDigi::GetSystemId() const
{
  return -111;
}

Double_t BmnDigi::GetTime() const
{
  return -111.;
}

ClassImp(BmnDigi)
