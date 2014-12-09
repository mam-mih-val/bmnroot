// -------------------------------------------------------------------------
// -----               CbmTofDetectorId_v07a header file               -----
// -----                 Created 20/11/12  by F. Uhlig                 -----
// -------------------------------------------------------------------------


/** CbmTofDetectorId.h
 ** Defines unique detector identifier for all TOF modules.
 ** This class is the implementation for tof geometry version v07a
 ** and v12a
 ** All classes which uses this scheme should have a data member
 ** of this class 
 ** @author F. Uhlig <f.uhlig@gsi.de>
 **/
 
//                                  3         2         1          shift length
/** Current definition:            10987654321098765432109876543210
 ** System ID (kTOF=6) on bits 0-4 00000000000000000000000000011111        31
 ** Super Module type on bits 5-8  00000000000000000000000111100000 <<5    15
 ** Module NR on bits 9-18         00000000000001111111111000000000 <<9  1023
 ** Gap NR on bits 19-22           00000000011110000000000000000000 <<19   15
 ** Cell NR on bits 23-32          11111111100000000000000000000000 <<23  511
 **/
 

#ifndef CBMTOFDETECTORID_V07A_H
#define CBMTOFDETECTORID_V07A_H 1

#include"CbmTofDetectorId.h"

class CbmTofDetectorId_v07a : public CbmTofDetectorId
{

  public:
  
  /** Constructor **/
  CbmTofDetectorId_v07a();

  /** Destructor **/
  ~CbmTofDetectorId_v07a() {};
  
  /** Get complete system info from detector ID
   ** This will return a pointer to an integer
   ** array of length array_length
   **/
  CbmTofDetectorInfo GetDetectorInfo(const Int_t detectorId);

  /** Get the global sytem ID **/
  Int_t GetSystemId(Int_t detectorId);

  /** Get SMType number from detector ID **/
   Int_t GetSMType(const Int_t detectorId);

  /** Get smodule number from detector ID **/
   Int_t GetSModule(const Int_t detectorId);

  /** Get counter number from detector ID **/
  Int_t GetCounter(const Int_t detectorId);

  /** Get sector number from detector ID **/
  Int_t GetGap(const Int_t detectorId);
  
  /** Get cell number from detector ID **/
  Int_t GetCell(const Int_t detectorId);
  
  /** Get region number from detector ID **/
  Int_t GetRegion(const Int_t detectorId);

  /** Get cell number from detector ID.
   ** This is masking the the gap number
   ** if this is set.
   **/
  Int_t GetCellId(const Int_t detectorId);

  /** Calculate the unique detector ID
   ** This will return a pointer to an integer
   ** array of length array_length
   **/
  Int_t SetDetectorInfo(const CbmTofDetectorInfo detectorInfo);
  
 private:
  
  static const Int_t array_length = 5;
  static const Int_t shiftarray[];
  static const Int_t bitarray[];
  Int_t result_array[array_length];
  Int_t maskarray[array_length];
  Int_t modulemask;
};

#endif
