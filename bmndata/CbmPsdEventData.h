/** CbmPsdHit.h
 **@author Alla Maevskaya <alla@inr.ru>
 **@since 23.10.2012
 **@version 1.0
 **
 ** Data class for PSD reconstruction
 ** Energy deposition per event
 **/


#ifndef CBMPSDEventData_H
#define CBMPSDEventData_H 1


#include "CbmDetectorList.h"
#include "TObject.h"

#include <vector>

class CbmPsdEventData : public TObject
{

 public:

  /**   Default constructor   **/
  CbmPsdEventData();
//  CbmPsdEventData(const char *name) ;
  
  CbmPsdEventData(Float_t rp, Float_t b, Float_t edepi, Float_t edepo) ;
  
  
  /**   Destructor   **/
  virtual ~CbmPsdEventData();
  
 
  
  /**   Setters - Getters   **/
  
  Float_t GetRP() const { return fRP; }
  void SetRP(Float_t rp) {fRP = rp;}
  
//  Float_t GetImpact() const { return fB; }
  Float_t GetB() const { return fB; }
  void SetB(Float_t b) {  fB = b; }

  Float_t GetEdepInner() const { return fEdepInner; }
  void SetEdepInner(Float_t edepi) {fEdepInner = edepi;}
  
  Float_t GetEdepOuter() const { return fEdepOuter; }
  void SetEdepOuter(Float_t edepo) {fEdepOuter = edepo;}
  
  Float_t GetEdepTotal() const { return fEdepTotal; }
  void SetEdepTotal(Float_t edept) {fEdepTotal = edept;}
  
  void Print();
  
 private:


  /**   Data members  **/
 
   Float_t fRP;
   Float_t fB;
   Float_t fEdepInner;
   Float_t fEdepOuter;
   Float_t fEdepTotal;
  
  ClassDef(CbmPsdEventData,1);

};


#endif
