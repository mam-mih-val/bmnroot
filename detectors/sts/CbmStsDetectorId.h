/** CbmStsDetectorId.h
 **@author V.Friese <v.friese@gsi.de>
 **@since 21.10.2009
 **@version 1.0
 **
 ** Prescription for encoding / decoding the detector ID (integer)
 ** from / to station nr., sector nr., side and channel nr.
 **
 ** Ranges:
 ** - Station number    4 bits     1 - 15
 ** - Sector number     8 bits     1 - 255 //AZ
 ** - Side              1 bit      0 - 1
 ** - Channel number   12 bits     1 - 4095 //AZ
 **
 ** The value 0 is reserved to denote the next higher hierarchy level,
 ** i.e. station 3, sector 0 denotes the entire station 3, not a specific sector.
 **
 ** All data classes using this functionality have to inherit from 
 ** this class.
 **/

#ifndef CBMSTSDETECTORID_H
#define CBMSTSDETECTORID_H 1

#include "BmnDetectorList.h"

class CbmStsDetectorId
{
 public:
  /**    Constructor (empty)   **/
  CbmStsDetectorId() { }


  /**    Destructor (empty)   **/
  virtual ~CbmStsDetectorId() { }


  /**    Create unique detector ID
   **@param  station   Station number (0 - 15)
   **@param  sector    Sector number  (0 - 255)
   **@param  side      0 = front side / 1 = back side
   **@param  channel   channel number (0 - 4095)
   **/
  Int_t DetectorId(Int_t station, Int_t sector, 
		   Int_t side, Int_t channel) const {
    return ( kGEM & 15 )              // System ID on bits 0-3
      | ( ( station & fgkStatMask ) << fgkStatShift )   // Station number
      | ( ( sector  & fgkSectMask ) << fgkSectShift )   // Sector number
      | ( ( side    & fgkSideMask ) << fgkSideShift )   // Side on bit
      | ( ( channel & fgkChanMask ) << fgkChanShift );  // Channel number
  }


  /** Get system identifier from detectorId **/
  Int_t SystemId(Int_t detectorId) const {
    return ( detectorId & 15 );
  }


  /** Get station number from detectorId **/
  Int_t StationNr(Int_t detectorId) const {
    return ( (detectorId >> fgkStatShift) & fgkStatMask );
  }


  /** Get sector number from detectorId **/
  Int_t SectorNr(Int_t detectorId) const {
    return ( (detectorId >> fgkSectShift) & fgkSectMask );
  } 


  /** Get side from detectorId **/
  Int_t Side(Int_t detectorId) const {
    return ( (detectorId >> fgkSideShift) & fgkSideMask );
  }


  /** Get channel number from detectorId **/
  Int_t ChannelNr(Int_t detectorId) const {
    return ( (detectorId >> fgkChanShift) & fgkChanMask );
  }


  /** Number of bits for address fields (static) **/
  static const Int_t fgkStatBits;
  static const Int_t fgkSectBits;
  static const Int_t fgkSideBits;
  static const Int_t fgkChanBits;



 private:

  /** Bit shift for address fields (static) **/
  static const Int_t fgkStatShift;
  static const Int_t fgkSectShift;
  static const Int_t fgkSideShift;
  static const Int_t fgkChanShift;

  /** Bit mask for address fields (static) **/
  static const Int_t fgkStatMask;
  static const Int_t fgkSectMask;
  static const Int_t fgkSideMask;
  static const Int_t fgkChanMask;


  ClassDef(CbmStsDetectorId, 1);

};


#endif
