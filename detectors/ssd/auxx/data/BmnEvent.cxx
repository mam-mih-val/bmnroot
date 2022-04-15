/** BmnEvent.cxx
 ** @author V.Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @since 13.12.2018
 ** @version 1.0
 **/

#include "BmnEvent.h"

#include <iostream>
#include <sstream>
#include "FairLogger.h"



// -----   Add data to event   ---------------------------------------------
void BmnEvent::AddData(DataType type, UInt_t index) {

	fIndexMap[type].push_back(index);
	fNofData++;

}
// -------------------------------------------------------------------------



// -----   Get a data index   ----------------------------------------------
UInt_t BmnEvent::GetIndex(DataType type, UInt_t iData) {

	if ( fIndexMap.find(type) == fIndexMap.end() ) return -1;
	if ( fIndexMap[type].size() <= iData ) return -2;
	return fIndexMap.at(type)[iData];

}
// -------------------------------------------------------------------------



// -----   Get number of data of a type in this event   --------------------
Int_t BmnEvent::GetNofData(DataType type) const {

	if ( fIndexMap.find(type) == fIndexMap.end() ) return -1;
	else return fIndexMap.at(type).size();

}
// -------------------------------------------------------------------------



// -----   Set the vertex parameters   -------------------------------------
void BmnEvent::SetVertex(Double_t x, Double_t y, Double_t z, Double_t chi2,
	 Int_t ndf, Int_t nTracks, const TMatrixFSym& covMat) {
	fVertex.SetVertex(x, y, z, chi2, ndf, nTracks, covMat);
}
// -------------------------------------------------------------------------



// -----   String output   -------------------------------------------------
std::string BmnEvent::ToString() const
{
   std::stringstream ss;
   ss << "Event " << fNumber << " at t = " << fTimeStart
  		<< " ns. Registered data types: " << fIndexMap.size()
  		<< ", data objects: " << fNofData << "\n";
   for (auto it = fIndexMap.begin(); it != fIndexMap.end(); it++) {
  	 ss << "          -- Data type " << it->first
  			<< ", number of data " << it->second.size() << "\n";
   }
  return ss.str();
}
// -------------------------------------------------------------------------



ClassImp(BmnEvent)
