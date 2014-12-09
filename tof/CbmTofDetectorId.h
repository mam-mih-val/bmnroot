// -------------------------------------------------------------------------
// -----                   CbmTofDetectorId header file                -----
// -----                 Created 29/05/09  by F. Uhlig                 -----
// -------------------------------------------------------------------------


/** CbmTofDetectorId.h
 ** Abstract base class for the different implementations to calculate
 ** the information about TOF modules from a unique detetcor ID
 ** @author F. Uhlig <f.uhlig@gsi.de>
 **/
 
#ifndef CBMTOFDETECTORID_H
#define CBMTOFDETECTORID_H 1

#include"Rtypes.h"

class CbmTofDetectorInfo
{
 
 public:

 CbmTofDetectorInfo() : fDetectorSystem(0), fSMtype(0), fSModule(0), 
    fCounter(0), fGap(0), fCell(0) {};
  
 CbmTofDetectorInfo(Int_t detsystem, Int_t smtype, Int_t smodule, 
		    Int_t counter, Int_t gap, Int_t cell)
   : fDetectorSystem(detsystem), fSMtype(smtype), fSModule(smodule),
    fCounter(counter), fGap(gap), fCell(cell) {};
  
  Int_t fDetectorSystem;
  Int_t fSMtype;
  Int_t fSModule;
  Int_t fCounter;
  Int_t fGap;
  Int_t fCell;
};


class CbmTofDetectorId
{

  public:
    
  /** Constructor **/
  CbmTofDetectorId() {;}
  
  
  /** Destructor **/
  virtual ~CbmTofDetectorId()  {;}
  
  /** Get System identifier from detector ID **/
  //  Int_t GetSystemId(const Int_t detectorId);

  /** Get complete system info from detector ID
   ** This will return a pointer to an integer
   ** array of length array_length
   **/
  virtual CbmTofDetectorInfo GetDetectorInfo(const Int_t detectorId) = 0;

  /** Get the global sytem ID **/
  virtual Int_t GetSystemId(Int_t detectorId) = 0;

  /** Get SMType from detector ID **/
  virtual Int_t GetSMType(const Int_t detectorId) = 0;

  /** Get SModule number from detector ID **/
  virtual Int_t GetSModule(const Int_t detectorId) = 0;

  /** Get counter number from detector ID **/
  virtual Int_t GetCounter(const Int_t detectorId) = 0;

  /** Get gap number from detector ID **/
  virtual Int_t GetGap(const Int_t detectorId) = 0;

  /** Get cell number from detector ID **/
  virtual Int_t GetCell(const Int_t detectorId) = 0;

  /** Get region number from detector ID **/
  virtual Int_t GetRegion(const Int_t detectorId) = 0;

  /** Get cell number from detector ID **/
  virtual Int_t GetCellId(const Int_t detectorId) = 0;

  /** Calculate the unique detector ID
   ** This will return a pointer to an integer
   ** array of length array_length
   **/
  virtual Int_t SetDetectorInfo(const CbmTofDetectorInfo detectorInfo) = 0;

  //  char* itoa(int value, char* result, int base);

 private:
 
};

#endif
