/** @file CbmStsAddress.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 17.05.2013
 **/

#ifndef CBMSTSADDRESS_H
#define CBMSTSADDRESS_H 1

#include <iomanip>
#include "Rtypes.h"
#include "FairLogger.h"
#include "CbmAddress.h"


/** Enumerator for STS element levels
 ** If this is changed, the initialisation of the static members
 ** of CbmStsAddress must be changed accordingly!
 **/
enum StsElementLevel {
  kStsSystem,     //!< System = STS
  kStsStation,    //!< Station
  kStsLadder,     //!< Ladder
  kStsHalfLadder, //!< Halfladder
  kStsModule,     //!< Module
  kStsSensor,     //!< Sensor
  kStsSide,       //!< Side
  kStsChannel,    //!< Channel
  kStsNofLevels   //!< Number of STS levels
};



/** @class CbmStsAddress
 ** @brief Interface class to unique address for the STS
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 1.0
 **
 ** The CbmStsAddress interprets and modifies the unique address
 ** for the STS by the proper bit operation on the address bit field.
 **
 ** The current definition of the bit field for the STS is:
 **
 **                                               3         2         1
 ** Level           Bits  Max. Elements  Shift   10987654321098765432109876543210 \n
 ** System (kSTS)     4         16          0    0000000000000000000000000000xxxx \n
 ** Station           4         16          4    000000000000000000000000xxxx0000 \n
 ** Ladder            4         16          8    00000000000000000000xxxx00000000 \n
 ** HalfLadder        1          2         12    0000000000000000000x000000000000 \n
 ** Module            3          8         13    0000000000000000xxx0000000000000 \n
 ** Sensor            2          4         16    00000000000000xx0000000000000000 \n
 ** Side              1          2         18    0000000000000x000000000000000000 \n
 ** Channel          13       8192         19    xxxxxxxxxxxxx0000000000000000000 \n
 **/
class CbmStsAddress : public CbmAddress
{

  public:

    /** Construct address
     ** @param station      Station index
     ** @param ladder       Ladder index in station
     ** @param halfladder   Halfladder index in ladder
     ** @param module       Module index within halfladder
     ** @param sensor       Sensor index within module
     ** @param side         Side (0=front, 1=back) of sensor
     ** @param channel      Channel number
     ** @return Unique element address
     **/
    static UInt_t GetAddress(Int_t station = 0,
                             Int_t ladder = 0,
                             Int_t halfladder = 0,
                             Int_t module = 0,
                             Int_t sensor = 0,
                             Int_t side = 0,
                             Int_t channel = 0);


    static UInt_t GetAddress(Int_t* elementIds);


    /** Get the number of hierarchy levels
     ** For use in macros which do not include this header file.
     ** @return       Number of hierarchy levels
     **/
    static Int_t GetNofLevels() { return kStsNofLevels; }


    /** Get the number of bits for a given hierarchy level
     ** @param level  Requested element level
     ** @return       Number of bits in address field
     **/
    static Int_t GetNofBits(Int_t level) {
      if ( level < 0 || level >= kStsNofLevels ) return 0;
      return fgkBits[level];
    }


    /** Get the index of an element
     ** @param address Unique element address
     ** @param level Hierarchy level
     ** @return Element index
     **/
    static Int_t GetElementId(UInt_t address, Int_t level) {
      if ( level < 0 || level >= kStsNofLevels ) return -1;
      return ( address & ( fgkMask[level] << fgkShift[level] ) )
               >> fgkShift[level];
    }


    /** Print information on the bit field **/
    static void Print();


    /** Set the index of an element
     ** leaving the other element levels untouched
     ** @param address Unique element address
     ** @param level   Hierarchy level
     ** @param newId   New element index
     ** @return New address
     **/
     static UInt_t SetElementId(UInt_t address, Int_t level, Int_t newId) {
       if ( level < 0 || level >= kStsNofLevels ) return address;
       if ( newId >= ( 1 << fgkBits[level]) ) {
         LOG(error) << "Id " << newId << " for STS level " << level
                    << " exceeds maximum (" << (1 << fgkBits[level]) - 1
                    << ")";
         return 0;
       }
       return ( address & (~ (fgkMask[level] << fgkShift[level]) ) )
              | ( newId << fgkShift[level]);
     }


  private:

    /** Number of bits for the different levels **/
    static const Int_t fgkBits[kStsNofLevels];

    /** Bit shifts for the different levels **/
    static const Int_t fgkShift[kStsNofLevels];

    /** Bit masks for the different levels **/
    static const Int_t fgkMask[kStsNofLevels];


    ClassDef(CbmStsAddress,1);
};


#endif /* CBMSTSADDRESS_H  */
