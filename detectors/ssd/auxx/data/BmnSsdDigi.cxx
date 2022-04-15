/** BmnSsdDigi.cxx
 ** @author V.Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @since 07.12.2018
 ** @version 4.0
 **/

#include "BmnSsdDigi.h"
#include <sstream>
#include "BmnSsdAddress.h"

using std::string;
using std::stringstream;

// --- String output
string BmnSsdDigi::ToString() const {
   stringstream ss;
   ss << "SsdDigi: address " << fAddress << " | channel " << fChannel
      << " | charge " << fCharge << " | time " << fTime;
   return ss.str();
}

ClassImp(BmnSsdDigi)
