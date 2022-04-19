/** CbmStsDigiLight.h
 **@author V.Friese <v.friese@gsi.de>
 **@since 28.08.2006
 **@version 2.0
 **
 ** Data class for digital STS information
 ** Data level: RAW
 **
 ** Without inheritance from FairTimeStamp and FairMultiLinkedData
 **
 ** The information is encoded into 8 bytes (1 Long64_t).
 ** Address:                       25 bits
 **   - Station Nr.        4 bits
 **   - Sector Nr.        10 bits
 **   - Side (front/back)  1 bit
 **   - Channel Nr.       10 bits
 ** Charge (ADC):                  12 bits
 ** Time (ns):                     14 bits
 **
 ** The time is relative to the event time. 
 **/

#ifndef CBMSTSDIGILIGHT_H
#define CBMSTSDIGILIGHT_H 1

#include "CbmDigi.h"
#include "CbmStsDetectorId.h"

#include <vector>

class CbmStsDigiLight : public CbmDigi, public CbmStsDetectorId
{
 public:

  /**   Default constructor   **/
  CbmStsDigiLight();


  /**  Constructor from station number, sector number, 
   **  front/back side and channel number
   **@param station  station number (1 -   15)
   **@param sector   sector number  (1 - 1023)
   **@param side     0=front side; 1=back side
   **@param channel  channel number (1 -  1023)
   **@param adc      Charge ADC channel (0 - 4095)
   **@param time     Time within event [ns]
   **/
//   CbmStsDigi(std::vector<Int_t> index, Int_t station, Int_t sector, Int_t side, 
// 	     Int_t channel, Int_t adc, Int_t time);
  CbmStsDigiLight(Int_t station, Int_t sector, Int_t side, 
	     Int_t channel, Int_t adc, Int_t time);


  /**   Destructor   **/
  virtual ~CbmStsDigiLight();


  /**   Add charge   **/
  void AddAdc(Int_t adc);
//   void AddIndex(int index, int adc){
// 			AddLink(FairLink(kStsPoint, index, adc));
// 		}
//   void AddIndex(std::vector<Int_t> index){
// 			SetLinks(FairMultiLinkedData(kStsPoint, index));
// 		}

  /**   System ID    **/
  Int_t GetSystemId() const { return kGEM; }


  /**   Station number   **/
  Int_t GetStationNr() const { return StationNr(GetAddress()); }


  /**   Sector number   **/
  Int_t GetSectorNr() const { return SectorNr(GetAddress()); }


  /**   Side ( 0 = front, 1 = back )   **/
  Int_t GetSide() const { return Side(GetAddress()); }


  /**   Channel number   **/
  Int_t GetChannelNr() const { return ChannelNr(GetAddress()); }


  /**   Charge   **/
  Int_t GetAdc() const { return ( (fData >> fgkCharShift) & fgkCharMask ); }


  /**   Time   **/
  Double_t GetTime() const { return Double_t( (fData >> fgkTimeShift) & fgkTimeMask ); }


  /**   Unique detector ID   **/
  Int_t GetAddress() const {
    return ( kGEM | ( (fData & fgkAddrMask) << 4) );
  }


 private:

  /**   Data members  **/
  Long64_t fData;


  /**   Modifiers   **/
  void SetAdc(Int_t charge);


  /**   Static members for bit operations   **/
  static const Int_t fgkAddrBits;
  static const Int_t fgkCharBits;
  static const Int_t fgkTimeBits;
  static const Int_t fgkCharShift;
  static const Int_t fgkTimeShift;
  static const Long64_t fgkAddrMask;
  static const Long64_t fgkCharMask;
  static const Long64_t fgkTimeMask;


  ClassDef(CbmStsDigiLight,1);

};


#endif
