/** @file BmnSsdAddress.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @date 07.12.2018
 **/

#ifndef BMNSSDADDRESS_H
#define BMNSSDADDRESS_H 1

#include <cassert>
#include <sstream>
#include "FairLogger.h"


/** Enumerator for the hierarchy levels of the SSD setup **/
enum ESsdElementLevel {
  kSsdSystem,
  kSsdUnit,
  kSsdLadder,
  kSsdHalfLadder,
  kSsdModule,
  kSsdSensor,
  kSsdSide,
  kSsdNofLevels
};



/** Namespace BmnSsdAddress
 ** @brief Functions to encode or decode the address field of SSD data.
 ** @author V.Friese <v.friese@gsi.de>
 **
 ** The current definition (version 1) of the address bit field for the SSD is:
 **
 ** Level           Bits  Max. Elements  Bit Position
 ** System (kSSD)     4         16          0 -  3
 ** Unit              6         64          4 -  9
 ** Ladder            5         32         10 - 14
 ** HalfLadder        1          2         15
 ** Module            5         32         16 - 20
 ** Sensor            4         16         21 - 24
 ** Side              1          2         25
 ** Unused            2                    26 - 27
 ** Version           4         16         28 - 31
 **/
namespace BmnSsdAddress {

  const UInt_t kCurrentVersion = 1;

  // --- These values are not to be changed if backward compatibility
  // --- shall be maintained.
  const Int_t kVersionSize    = 4;   // Bits for version number
  const Int_t kVersionShift   = 28;  // First bit for version number
  const Int_t kVersionMask    = (1 << kVersionSize) -1;


  /** @brief Construct address
   ** @param unit         Unit index
   ** @param ladder       Ladder index in station
   ** @param halfladder   Halfladder index in ladder
   ** @param module       Module index within halfladder
   ** @param sensor       Sensor index within module
   ** @param side         Side (0=front, 1=back) of sensor
   ** @param channel      Channel number
   ** @return Unique element address
   **/
  Int_t GetAddress(UInt_t unit = 0,
                   UInt_t ladder = 0,
                   UInt_t halfladder = 0,
                   UInt_t module = 0,
                   UInt_t sensor = 0,
                   UInt_t side = 0,
                   UInt_t version = kCurrentVersion);


  /** @brief Construct address
   ** @param elementIds   Array of element indices in their mother volumes
   ** @return Unique element address
   **/
  Int_t GetAddress(UInt_t* elementId, UInt_t version);


  /** @brief Construct the address of an element from the address of a
   ** descendant element.
   ** @param address Address of descendant element
   ** @param level   Desired hierarchy level
   ** @return Address of element at desired hierarchy level
   **
   ** This strips of the address information of all hierarchy levels
   ** below the desired one.
   **/
  Int_t GetMotherAddress(Int_t address, Int_t level);


  /** @brief Get the index of an element
   ** @param address Unique element address
   ** @param level Hierarchy level
   ** @return Element index
   **/
  UInt_t GetElementId(Int_t address, Int_t level);


  /** @brief Get system Id (should be kSSD)
   ** @param address Unique element address
   **/
  UInt_t GetSystemId(Int_t address);


  /** @brief Extract version number
   ** @param address Unique element address
   ** @value Version number
   **
   ** The version is encoded in the last 6 bits (58 to 63).
   ** The maximal number of versions is 64.
   **/
  UInt_t GetVersion(Int_t address);


  /** @brief Set the index of an element, leaving the other element levels
   ** untouched
   ** @param address Unique element address
   ** @param level   Hierarchy level
   ** @param newId   New element index
   ** @return New address
   **/
  Int_t SetElementId(Int_t address, Int_t level, UInt_t newId);


  /** @brief String output
   ** @param address Unique element address
   **/
  std::string ToString(Int_t address);

} // Namespace BmnSsdAddress


#endif // BMNSSDADDRESS_H


