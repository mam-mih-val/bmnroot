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
 ** System id          on bits  0- 3   00000000000000000000000000011111    << 0     5      31
 ** ADCidx             on bits  5- 8   00000000000000000000000111100000    << 5     4      15
 ** ADCch              on bits  9-14   00000000000000000111111000000000    << 9     6      63
 ** Xidx               on bits 15-19   00000000000011111000000000000000    <<15     5      31
 ** Yidx               on bits 20-24   00000001111100000000000000000000    <<20     5      31
 ** CellSize           on bits 25-26   00000110000000000000000000000000    <<25     2      3
 ** Empty              on bits 27-31   11111000000000000000000000000000    <<27     5     2^5-1
 **
 **/

#ifndef BMNSCWALLADDRESS_H
#define BMNSCWALLADDRESS_H 1

#include "BmnDetectorList.h"  // for kSCWALL
//#include <RtypesCore.h>  // for uint32_t, uint32_t
#include <cassert>  // for assert

class BmnScWallAddress {
public:
  /**
   * \brief Return address from system ID, ADCidx, ADCch, XIdx, YIdx, CellSize.
   * \param[in] ADC index.
   * \param[in] ADC channel.
   * \param[in] unique X position number XIdx.
   * \param[in] unique Y position number YIdx.
   * \param[in] cell size CellSize.
   * \return Address from system ID, ADCidx, ADCch, XIdx, YIdx, CellSize.
   **/
  static uint32_t GetAddress(uint32_t ADCidx, uint32_t ADCch, uint32_t XIdx, uint32_t YIdx, uint32_t CellSize)
  {
    assert(!( (uint32_t)kSCWALL > fgkSystemIdLength | ADCidx > fgkADCidxLength || ADCch > fgkADCchLength || XIdx > fgkXIdxLength || YIdx > fgkYIdxLength || CellSize > fgkCellSizeLength ));
    return (  (((uint32_t)kSCWALL) << fgkSystemIdShift) 
            | (ADCidx << fgkADCidxShift)
            | (ADCch << fgkADCchShift)
            | (XIdx << fgkXIdxShift)
            | (YIdx << fgkYIdxShift)
            | (CellSize << fgkCellSizeShift)  );
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
   * \brief Return ADC idx from address.
   * \param[in] address Unique channel address.
   * \return ADC idx from address.
   **/
  static uint32_t GetADCidx(uint32_t address)
  {
    return (address & (fgkADCidxLength << fgkADCidxShift)) >> fgkADCidxShift;
  }

  /**
   * \brief Return ADC channel from address.
   * \param[in] address Unique channel address.
   * \return ADC channel from address.
   **/
  static uint32_t GetADCch(uint32_t address)
  {
    return (address & (fgkADCchLength << fgkADCchShift)) >> fgkADCchShift;
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


private:
  // Length of the index of the corresponding volume
  static const uint32_t fgkSystemIdLength  = 31;    // 2^5 - 1
  static const uint32_t fgkADCidxLength  = 15;      // 2^4 - 1
  static const uint32_t fgkADCchLength  = 63;       // 2^6 - 1
  static const uint32_t fgkXIdxLength = 31;         // 2^5 - 1
  static const uint32_t fgkYIdxLength = 31;         // 2^5 - 1
  static const uint32_t fgkCellSizeLength = 3;      // 2^2 - 1

  // Number of a start bit for each volume
  static const uint32_t fgkSystemIdShift  = 0;
  static const uint32_t fgkADCidxShift = 5;
  static const uint32_t fgkADCchShift = 9;
  static const uint32_t fgkXIdxShift = 15;
  static const uint32_t fgkYIdxShift = 20;
  static const uint32_t fgkCellSizeShift = 25;

};

#endif
