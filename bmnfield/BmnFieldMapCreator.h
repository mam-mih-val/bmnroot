// -------------------------------------------------------------------------
// -----                   BmnFieldMapCreator header file              -----
// -----                Created 15/01/08       by V. Friese            -----
// -------------------------------------------------------------------------



/** BmnFieldMapCreator.h
 **@author V.Friese <v.friese@gsi.de>
 **@since 15.01.2008
 **@version1.0
 **
 ** Auxiliary class to create a field map out of other magnetic fields
 ** of type BmnField.
 **/


#ifndef CBMFIELDMAPCREATOR_H
#define CBMFIELDMAPCREATOR_H 1

#include "TList.h"

class TArrayF;
class FairField;



class BmnFieldMapCreator 
{


 public:

  /** Default constructor **/
  BmnFieldMapCreator();


  /** Standard constructor 
   **@param mapName  Name of field map. Defines also the output file name.**/
  BmnFieldMapCreator(const char* mapName);


  /** Destructor **/
  virtual ~BmnFieldMapCreator();


  /** Set parameters 
   **@param nx,ny,nz   Number of grid cells in x,y,z
   **@param xmin,xmax  Extension in x coordinate
   **@param ymin,ymax  Extension in y coordinate
   **@param zmin,zmax  Extension in z coordinate
   **/
  void SetGridParameters(Int_t nx, Double_t xmin, Double_t xmax,
			 Int_t ny, Double_t ymin, Double_t ymax,
			 Int_t nz, Double_t zmin, Double_t zmax);


  /** Add an input field **/
  void AddField(FairField* field) { fFieldList.Add(field); }


  /** Create the field map 
   ** By default, the output file name is defined by the map name plus
   ** the extension .root. Other file names can be choosen.
   **@param fileName   Name of output file
   **@value  Success of map creation
   **/
  Bool_t CreateMap(const char* fileName = "");


  /** Accessors **/
  TString  GetMapName() const { return fMapName; }
  Int_t    GetNx()      const { return fNx; }
  Int_t    GetNy()      const { return fNy; }
  Int_t    GetNz()      const { return fNz; }
  Double_t GetXmin()    const { return fXmin; }
  Double_t GetXmax()    const { return fXmax; }
  Double_t GetYmin()    const { return fYmin; }
  Double_t GetYmax()    const { return fYmax; }
  Double_t GetZmin()    const { return fZmin; }
  Double_t GetZmax()    const { return fZmax; }
  TArrayF* GetBx()      const { return fBx; }
  TArrayF* GetBy()      const { return fBy; }
  TArrayF* GetBz()      const { return fBz; }


 private:

  TString  fMapName;        // Name of field map to be created
  Int_t    fNx, fNy, fNz;   // Number of grid points in each dimension
  Double_t fXmin, fXmax;    // Field limits in x
  Double_t fYmin, fYmax;    // Field limits in y
  Double_t fZmin, fZmax;    // Field limits in z
  TArrayF* fBx;             // Array of Bx values;
  TArrayF* fBy;             // Array of By values;
  TArrayF* fBz;             // Array of Bz values;
  TList    fFieldList;      // List of input fields;
  Bool_t   fInit;           // Flag whether map is initialised

  BmnFieldMapCreator(const BmnFieldMapCreator&);
  BmnFieldMapCreator& operator=(const BmnFieldMapCreator&);

  ClassDef(BmnFieldMapCreator,1);

};

#endif
