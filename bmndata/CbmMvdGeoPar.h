// -------------------------------------------------------------------------
// -----                     CbmMvdGeoPar header file                  -----
// -----                  Created 07/11/06  by V. Friese               -----
// -------------------------------------------------------------------------


/**  CbmMvdGeoPar.h
 *@author V.Friese <v.friese@gsi.de>
 *
 * Handling of geometry parameters for MVD. Copied from CbmGeoStsPar.h.
 *
 **/


#ifndef CBMMVDGEOPAR_H
#define CBMMVDGEOPAR_H 1


#include "FairParGenericSet.h"



class CbmMvdGeoPar : public FairParGenericSet {

 public:
  
  /** Constructor **/
  CbmMvdGeoPar(const char* name = "CbmMvdGeoPar",
	       const char* title ="MVD Geometry Parameters",
	       const char* context = "Default");


  /** Destructor **/
  virtual ~CbmMvdGeoPar();


  /** Clear parameter set **/
  virtual void clear();


  /** Put parameters **/
  virtual void putParams(FairParamList* parList);


  /** Get parameters **/
  virtual Bool_t getParams(FairParamList* parList);


  /** Accessors to sensitive and passive nodes **/
  TObjArray* GetGeoSensitiveNodes() { return fGeoSensNodes; }
  TObjArray* GetGeoPassiveNodes()   { return fGeoPassNodes; }


 private:

  TObjArray* fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray* fGeoPassNodes; // List of FairGeoNodes for passive volumes

  CbmMvdGeoPar(const CbmMvdGeoPar&);
  CbmMvdGeoPar& operator=(const CbmMvdGeoPar&);

  ClassDef(CbmMvdGeoPar,1);

};


#endif
