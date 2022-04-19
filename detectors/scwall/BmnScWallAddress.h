/** \class BmnScWallAddress
 ** \brief BmnScWall interface class to the unique address
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \version 1.0
 **
 ** BmnScWallAddress is the class for the concrete interfaces to the unique address, 
 ** which is encoded in a 32-bit field (uint32_t), for the ScWall detector elements.
 **
 **                                     3         2         1         0   Shift  Bits  Values
 ** Current definition:                10987654321098765432109876543210
 ** System id          on bits  0- 4   00000000000000000000000000011111    << 0     5      31
 ** Cell id            on bits  5-14   00000000000000000111111111100000    << 5    10    1023
 ** Xidx               on bits 15-19   00000000000011111000000000000000    <<15     5      31
 ** Yidx               on bits 20-24   00000001111100000000000000000000    <<20     5      31
 ** CellSize           on bits 25-26   00000110000000000000000000000000    <<25     2      3
 ** Zone idx           on bits 27-31   11111000000000000000000000000000    <<27     5      31
 **
 **/

#ifndef BMNSCWALLADDRESS_H
#define BMNSCWALLADDRESS_H 1

#include "BmnDetectorList.h"  // for kSCWALL
#include <cassert>  // for assert

class BmnScWallAddress {
public:
  /**
   * \brief Return address from system ID, Cell ID, XIdx, YIdx, CellSize, ZoneIdx.
   * \param[in] Cell id.
   * \param[in] unique X position number XIdx.
   * \param[in] unique Y position number YIdx.
   * \param[in] cell size CellSize.
   * \param[in] zone index Zone.
   * \return Address from system ID, Cell ID, XIdx, YIdx, CellSize, Zone.
   **/
  static uint32_t GetAddress(uint32_t CellId, uint32_t XIdx, uint32_t YIdx, uint32_t CellSize, uint32_t Zone)
  {
    assert((uint32_t)kSCWALL <= fgkSystemIdLength);
    assert(CellId <= fgkCellIdLength);
    assert(XIdx <= fgkXIdxLength);
    assert(YIdx <= fgkYIdxLength);
    assert(CellSize <= fgkCellSizeLength);
    assert(Zone <= fgkZoneLength);
    return (  (((uint32_t)kSCWALL) << fgkSystemIdShift) 
            | (CellId << fgkCellIdShift)
            | (XIdx << fgkXIdxShift)
            | (YIdx << fgkYIdxShift)
            | (CellSize << fgkCellSizeShift)
            | (Zone << fgkZoneShift)  );
  }

  /**
   * \brief Return System identifier from address.
   * \param[in] address Unique channel address.
   * \return System identifier from address.
   **/
  static uint32_t GetSystemId(uint32_t address)
  {
    return (address & (fgkSystemIdLength << fgkSystemIdShift)) >> fgkSystemIdShift;
  }

  /**
   * \brief Return Cell id from address.
   * \param[in] address Unique channel address.
   * \return Cell id from address.
   **/
  static uint32_t GetCellId(uint32_t address)
  {
    return (address & (fgkCellIdLength << fgkCellIdShift)) >> fgkCellIdShift;
  }

  /**
   * \brief Return X position ID from address.
   * \param[in] address Unique channel address.
   * \return X position ID from address.
   **/
  static uint32_t GetXIdx(uint32_t address)
  {
    return (address & (fgkXIdxLength << fgkXIdxShift)) >> fgkXIdxShift;
  }

  /**
   * \brief Return Y position ID from address.
   * \param[in] address Unique channel address.
   * \return Y position ID from address.
   **/
  static uint32_t GetYIdx(uint32_t address)
  {
    return (address & (fgkYIdxLength << fgkYIdxShift)) >> fgkYIdxShift;
  }

  /**
   * \brief Return Cell size from address.
   * \param[in] address Unique channel address.
   * \return Cell size from address.
   **/
  static uint32_t GetCellSize(uint32_t address)
  {
    return (address & (fgkCellSizeLength << fgkCellSizeShift)) >> fgkCellSizeShift;
  }

  /**
   * \brief Return Zone idx from address.
   * \param[in] address Unique channel address.
   * \return Zone index from address.
   **/
  static uint32_t GetZoneIdx(uint32_t address)
  {
    return (address & (fgkZoneLength << fgkZoneShift)) >> fgkZoneShift;
  }

private:
  // Length of the index of the corresponding volume
  static const uint32_t fgkSystemIdLength  = 31;    // 2^5 - 1
  static const uint32_t fgkCellIdLength  = 1023;    // 2^10- 1
  static const uint32_t fgkXIdxLength = 31;         // 2^5 - 1
  static const uint32_t fgkYIdxLength = 31;         // 2^5 - 1
  static const uint32_t fgkCellSizeLength = 3;      // 2^2 - 1
  static const uint32_t fgkZoneLength = 31;         // 2^5 - 1

  // Number of a start bit for each volume
  static const uint32_t fgkSystemIdShift  = 0;
  static const uint32_t fgkCellIdShift = 5;
  static const uint32_t fgkXIdxShift = 15;
  static const uint32_t fgkYIdxShift = 20;
  static const uint32_t fgkCellSizeShift = 25;
  static const uint32_t fgkZoneShift = 27;

};

#endif
