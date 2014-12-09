/** CbmPsdHit.h
 **@author Alla Maevskaya <alla@inr.ru>
 **@since 23.10.2012
 **@version 1.0
 **
 ** Data class for PSD reconstruction
 ** Energy deposition per module
 **/


#ifndef CBMPSDHIT_H
#define CBMPSDHIT_H 1


#include "CbmDetectorList.h"
#include "TObject.h"

#include <vector>

class CbmPsdHit : public TObject
{

 public:

  /**   Default constructor   **/
  CbmPsdHit() ;
  
  CbmPsdHit(Int_t module, Float_t edep) ;
  
  
  /**   Destructor   **/
  virtual ~CbmPsdHit();
  
 
  
  /**   Setters - Getters   **/
  
  Float_t GetEdep() const { return fEdep; }
  void SetEdep(Float_t edep) {fEdep=edep;}
  
  Int_t GetModuleID() const { return fModuleID; }
  void SetModuleID(Int_t mod) {  fModuleID = mod; }
  
   void Print();
  
 private:


  /**   Data members  **/
 
  Int_t fModuleID;
  Float_t fEdep;

  
  ClassDef(CbmPsdHit,1);

};


#endif
