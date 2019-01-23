/** @file BmnSsdSignal.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @date 12.12.2018
 **/

#include "BmnSsdSignal.h"


// -----   Default constructor   -------------------------------------------
BmnSsdSignal::BmnSsdSignal(Double_t time, Double_t charge,
				                   Int_t index, Int_t entry, Int_t file) :
				                   TObject(),
				                   fTime(time),
				                   fMatch()  {
	fMatch.AddLink(charge, index, entry, file);
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
BmnSsdSignal::~BmnSsdSignal() { }
// -------------------------------------------------------------------------


ClassImp(BmnSsdSignal)
