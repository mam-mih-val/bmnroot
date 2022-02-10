/** \class BmnFHCalAddress
 ** \brief BmnFHCal interface class to the unique address
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \version 1.0
 **
 ** BmnFHCalAddress is the class for the concrete interfaces to the unique address, 
 ** which is encoded in a 32-bit field (uint32_t), for the FHCal detector elements.
 **
 **                                     3         2         1         0   Shift  Bits  Values
 ** Current definition:                10987654321098765432109876543210
 ** System id          on bits  0- 4   00000000000000000000000000011111    << 0     5      31
 ** ModType            on bits  5- 6   00000000000000000000000001100000    << 5     2      3
 ** ModIdx             on bits  7-12   00000000000000000001111110000000    << 7     6      63
 ** SecIdx             on bits 13-16   00000000000000011110000000000000    <<13     4      15
 ** Xidx               on bits 17-20   00000000000111100000000000000000    <<17     4      15
 ** Yidx               on bits 21-24   00000001111000000000000000000000    <<21     4      15
 ** Zidx               on bits 25-28   00011110000000000000000000000000    <<25     4      15
 ** Empty              on bits 29-31   11100000000000000000000000000000    <<31     3      7
 **
 **/

#ifndef BmnFHCalAddress_H
#define BmnFHCalAddress_H 1

#include "BmnDetectorList.h"  // for kFHCAL
#include <cassert>  // for assert

class BmnFHCalAddress {
public:
  /**
   * \brief Return address from system ID, Module type, Module ID, Section ID, XIdx, YIdx, ZIdx.
   * \param[in] Module type.
   * \param[in] Module id.
   * \param[in] Section id.
   * \param[in] unique X position number XIdx.
   * \param[in] unique Y position number YIdx.
   * \param[in] unique Z position number ZIdx.
   * \return Address from system ID, Module type, Module ID, Section ID, XIdx, YIdx, ZIdx.
   **/
  static uint32_t GetAddress(uint32_t ModuleType, uint32_t ModuleId, uint32_t SectionId, uint32_t XIdx, uint32_t YIdx, uint32_t ZIdx)
  {
    assert((uint32_t)kFHCAL <= fgkSystemIdLength);
    assert(ModuleType <= fgkModTypeLength);
    assert(ModuleId <= fgkModIdxLength);
    assert(SectionId <= fgkSecIdxLength);
    assert(XIdx <= fgkXIdxLength);
    assert(YIdx <= fgkYIdxLength);
    assert(ZIdx <= fgkZIdxLength);
    return (  (((uint32_t)kFHCAL) << fgkSystemIdShift) 
            | (ModuleType << fgkModTypeShift)
            | (ModuleId << fgkModIdxShift)
            | (SectionId << fgkSecIdxShift)
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
   * \brief Return Module id from address.
   * \param[in] address Unique channel address.
   * \return Module id from address.
   **/
  static uint32_t GetModuleType(uint32_t address)
  {
    return (address & (fgkModTypeLength << fgkModTypeShift)) >> fgkModTypeShift;
  }

  /**
   * \brief Return Module id from address.
   * \param[in] address Unique channel address.
   * \return Module id from address.
   **/
  static uint32_t GetModuleId(uint32_t address)
  {
    return (address & (fgkModIdxLength << fgkModIdxShift)) >> fgkModIdxShift;
  }

  /**
   * \brief Return Section id from address.
   * \param[in] address Unique channel address.
   * \return ModSectionule id from address.
   **/
  static uint32_t GetSectionId(uint32_t address)
  {
    return (address & (fgkSecIdxLength << fgkSecIdxShift)) >> fgkSecIdxShift;
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


private:
  // Length of the index of the corresponding volume
  static const uint32_t fgkSystemIdLength  = 31;    // 2^5 - 1
  static const uint32_t fgkModTypeLength  = 3;      // 2^2 - 1
  static const uint32_t fgkModIdxLength = 63;       // 2^6 - 1
  static const uint32_t fgkSecIdxLength = 15;       // 2^4 - 1 
  static const uint32_t fgkXIdxLength = 15;         // 2^4 - 1
  static const uint32_t fgkYIdxLength = 15;         // 2^4 - 1
  static const uint32_t fgkZIdxLength = 15;         // 2^4 - 1

  // Number of a start bit for each volume
  static const uint32_t fgkSystemIdShift  = 0;
  static const uint32_t fgkModTypeShift = 5;
  static const uint32_t fgkModIdxShift = 7;
  static const uint32_t fgkSecIdxShift = 13;
  static const uint32_t fgkXIdxShift = 17;
  static const uint32_t fgkYIdxShift = 21;
  static const uint32_t fgkZIdxShift = 25;

};

#endif
