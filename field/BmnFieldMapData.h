// -------------------------------------------------------------------------
// -----                    BmnFieldMapData header file                -----
// -----                V. Friese                 13/02/06             -----
// -------------------------------------------------------------------------


/** BmnFieldMapData.h
 ** @author V.Friese <v.friese@gsi.de>
 ** @since 14.02.2006
 ** @version1.0
 **
 ** This class holds the real data arrays of a magnetic field map,
 ** which are read from / written to file. Nota bene: Field values
 ** are in Tesla, in contrast to BmnFieldMap, whcih holds the
 ** field in kG.
 **/


#ifndef CBMMAGFIELDMAPDATA_H
#define CBMMAGFIELDMAPDATA_H


#include "TNamed.h"


class TArrayF;

class BmnFieldMap;



class BmnFieldMapData : public TNamed {

public:


  /** Default constructor **/
  BmnFieldMapData();


  /** Standard constructor **/
  BmnFieldMapData(const char* name);


  /** Constructor from an existing BmnFieldMap **/
  BmnFieldMapData(const char* name, const BmnFieldMap& map);
  

  /** Destructor **/
  virtual ~BmnFieldMapData();


  /** Accessors to field parameters in local coordinate system **/
  Int_t    GetType()  const { return fType; }
  Double_t GetXmin()  const { return fXmin; } 
  Double_t GetYmin()  const { return fYmin; }
  Double_t GetZmin()  const { return fZmin; }
  Double_t GetXmax()  const { return fXmax; }  
  Double_t GetYmax()  const { return fYmax; }
  Double_t GetZmax()  const { return fZmax; }
  Int_t    GetNx()    const { return fNx; }
  Int_t    GetNy()    const { return fNy; }
  Int_t    GetNz()    const { return fNz; }


  /** Accessors to the field value arrays **/
  TArrayF* GetBx() const { return fBx; }
  TArrayF* GetBy() const { return fBy; }
  TArrayF* GetBz() const { return fBz; }

	
	
 private:

  /** Type of map. 1 = BmnFieldMap, 2 = Sym2, 3 = Sym3 **/
  Int_t fType;

  /** Field limits in local coordinate system **/
  Double_t fXmin, fXmax;
  Double_t fYmin, fYmax;
  Double_t fZmin, fZmax;


  /** Number of grid points  **/
  Int_t fNx, fNy, fNz;


  /** Arrays with the field values in T **/
  TArrayF* fBx;
  TArrayF* fBy;
  TArrayF* fBz;

  BmnFieldMapData(const BmnFieldMapData&) = delete;
  BmnFieldMapData& operator=(const BmnFieldMapData&) = delete;

  ClassDef(BmnFieldMapData,1) 
};


#endif
