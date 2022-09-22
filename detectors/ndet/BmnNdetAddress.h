/** \class BmnNdetAddress
 ** \brief BmnNdet interface class to the unique address
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \version 1.0
 **
 ** BmnNdetAddress is the class for the concrete interfaces to the unique address, 
 ** which is encoded in a 32-bit field (uint32_t), for the Ndet detector elements.
 **
 **                                     3         2         1         0   Shift  Bits  Values
 ** Current definition:                10987654321098765432109876543210
 ** System id          on bits  0- 4   00000000000000000000000000011111    << 0     5      31
 ** CellId             on bits  5-12   00000000000000000001111111100000    << 5     8      255
 ** LayerId            on bits 13-17   00000000000000111110000000000000    <<13     5      31
 ** Xidx               on bits 18-21   00000000001111000000000000000000    <<18     4      15
 ** Yidx               on bits 22-25   00000011110000000000000000000000    <<22     4      15
 ** Zidx               on bits 26-30   01111100000000000000000000000000    <<26     5      31
 ** 
 **/

#ifndef BmnNdetAddress_H
#define BmnNdetAddress_H 1

#include "BmnDetectorList.h"  // for kNDET
#include <cassert>  // for assert

class BmnNdetAddress {
public:
  /**
   * \brief Return address from system ID, CellId, LayerId, XIdx, YIdx, ZIdx.
   * \param[in] CellId cell id.
   * \param[in] LayerId layer id.
   * \param[in] unique X position number XIdx.
   * \param[in] unique Y position number YIdx.
   * \param[in] unique Z position number ZIdx.
   * \return Address from system ID, CellId, LayerId, XIdx, YIdx, ZIdx.
   **/
  static uint32_t GetAddress(uint32_t CellId, uint32_t LayerId, uint32_t XIdx, uint32_t YIdx, uint32_t ZIdx)
  {
    assert((uint32_t)kNDET <= fgkSystemIdLength);
    assert(CellId <= fgkCellIdLength);
    assert(LayerId <= fgkLayerIdLength);
    assert(XIdx <= fgkXIdxLength);
    assert(YIdx <= fgkYIdxLength);
    assert(ZIdx <= fgkZIdxLength);
    return (  (((uint32_t)kNDET) << fgkSystemIdShift) 
            | (CellId << fgkCellIdShift)
            | (LayerId << fgkLayerIdShift)
            | (XIdx << fgkXIdxShift)
            | (YIdx << fgkYIdxShift)
            | (ZIdx << fgkZIdxShift) );
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
   * \brief Return Layer id from address.
   * \param[in] address Unique channel address.
   * \return Layer id from address.
   **/
  static uint32_t GetLayerId(uint32_t address)
  {
    return (address & (fgkLayerIdLength << fgkLayerIdShift)) >> fgkLayerIdShift;
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
   * \brief Return Z position ID from address.
   * \param[in] address Unique channel address.
   * \return Z position ID from address.
   **/
  static uint32_t GetZIdx(uint32_t address)
  {
    return (address & (fgkZIdxLength << fgkZIdxShift)) >> fgkZIdxShift;
  }

  /**
   * \brief Return valueable address part from address.
   * \param[in] address Unique channel address.
   * \return Valueable address part address.
   **/
  static uint32_t GetFlatIndex(uint32_t address)
  {
    uint32_t mask = (fgkCellIdLength << fgkCellIdShift) | (fgkLayerIdLength << fgkLayerIdShift);
    return ((address & mask) >> fgkCellIdShift);
  }

  /**
   * \brief Return max valueable address part from address.
   * \param[in] address Unique channel address.
   * \return Max Valueable address part address.
   **/
  static uint32_t GetMaxFlatIndex()
  {
    uint32_t mask = (fgkCellIdLength << fgkCellIdShift) | (fgkLayerIdLength << fgkLayerIdShift);
    return (mask >> fgkCellIdShift);
  }
  
private:
  // Length of the index of the corresponding volume
  static const uint32_t fgkSystemIdLength  = 31;    // 2^5 - 1
  static const uint32_t fgkCellIdLength  = 255;     // 2^8 - 1
  static const uint32_t fgkLayerIdLength = 31;      // 2^5 - 1
  static const uint32_t fgkXIdxLength = 15;         // 2^4 - 1
  static const uint32_t fgkYIdxLength = 15;         // 2^4 - 1
  static const uint32_t fgkZIdxLength = 31;         // 2^5 - 1

  // Number of a start bit for each volume
  static const uint32_t fgkSystemIdShift  = 0;
  static const uint32_t fgkCellIdShift = 5;
  static const uint32_t fgkLayerIdShift = 13;
  static const uint32_t fgkXIdxShift = 18;
  static const uint32_t fgkYIdxShift = 22;
  static const uint32_t fgkZIdxShift = 26;

};

#endif
