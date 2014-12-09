/** CbmStsDetectorId.cxx
 **@author V.Friese <v.friese@gsi.de>
 **@since 21.10.2009
 **@version 1.0
 **/


#include "CbmStsDetectorId.h"


/** --- Define here the number of bits for the address fields
 ** --- Do not alter without need: will destroy backwards compability
 ** --- The first 4 bits are reserved for the system ID (kSTS)
 **/
const Int_t CbmStsDetectorId::fgkStatBits =  4;
//AZ const Int_t CbmStsDetectorId::fgkSectBits = 10;
const Int_t CbmStsDetectorId::fgkSectBits =  8;
const Int_t CbmStsDetectorId::fgkSideBits =  1;
//AZ const Int_t CbmStsDetectorId::fgkChanBits = 10;
const Int_t CbmStsDetectorId::fgkChanBits = 12;


/** Bit shift is calculated from above **/
const Int_t CbmStsDetectorId::fgkStatShift =  4;
const Int_t CbmStsDetectorId::fgkSectShift = 
  CbmStsDetectorId::fgkStatShift + CbmStsDetectorId::fgkStatBits;
const Int_t CbmStsDetectorId::fgkSideShift = 
  CbmStsDetectorId::fgkSectShift + CbmStsDetectorId::fgkSectBits;
const Int_t CbmStsDetectorId::fgkChanShift = 
  CbmStsDetectorId::fgkSideShift + CbmStsDetectorId::fgkSideBits;


/** Bit mask is calculated from above **/
const Int_t CbmStsDetectorId::fgkStatMask = (1 << CbmStsDetectorId::fgkStatBits) - 1;
const Int_t CbmStsDetectorId::fgkSectMask = (1 << CbmStsDetectorId::fgkSectBits) - 1;
const Int_t CbmStsDetectorId::fgkSideMask = (1 << CbmStsDetectorId::fgkSideBits) - 1;
const Int_t CbmStsDetectorId::fgkChanMask = (1 << CbmStsDetectorId::fgkChanBits) - 1;



ClassImp(CbmStsDetectorId)
