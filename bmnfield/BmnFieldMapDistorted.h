// -------------------------------------------------------------------------
// -----               BmnFieldMapSym3Distorted header              -----
// -----                  Created 18/01/08  by E.Litvinenko            -----
// -------------------------------------------------------------------------


#ifndef CBMMAGFIELDMAPDISTORTED_H
#define CBMMAGFIELDMAPDISTORTED_H 1


#include "BmnFieldMap.h"

class BmnFieldPar;

class TFormula;

class BmnFieldMapDistorted : public BmnFieldMap {


public:


  /** Default constructor **/
  BmnFieldMapDistorted();

  /** Standard constructor  (with FieldMap Parent Field )
   ** @param mapName                    Name of distorted field map
   ** @param pfDistortionFilename       root file with distortion information
   ** @param parentName                 Name of parent field map
   ** @param fileTypeParent             R = ROOT file, A = ASCII for parent field map
   ** @param pfTypeOfParent             3 = parent is BmnFieldMapSym3, 2 = BmnFieldMapSym2, 1 = BmnFieldMap
   **/
  BmnFieldMapDistorted(const char* mapName, const char* pfDistortionFilename, const char* parentName, 
		       const char* fileTypeParent= "R", Int_t pfTypeOfParent=3);

  /** Constructor (with Constant Parent Field )
   ** @param xMin..zMax                 the rectangular region boundaries (X,Y,Z)
   ** @param bX, By, Bz                 field component values inside the region
   ** @param mapName                    Name of distorted field map
   ** @param pfDistortionFilename       root file with distortion information
   ** @param parentName                 Name of parent constant field
   **/
  BmnFieldMapDistorted (Double_t xMin, Double_t xMax,Double_t yMin, Double_t yMax, 
			Double_t zMin, Double_t zMax, Double_t bX, Double_t bY, Double_t bZ,
		        const char* mapName, const char* pfDistortionFilename, const char* parentName="");

  /** Constructor from BmnFieldPar  **/
  BmnFieldMapDistorted(BmnFieldPar* fieldPar);


  /** Destructor **/
  virtual ~BmnFieldMapDistorted();


  /** Initialisation (read map from mapfile and distortion information from distortion file) **/
  virtual void Init();

  void ReadDistortionInformation(const char *filename=0) ;
  void WriteDistortionInformation(const char *filename=0) ;

  /** Get the field components at a certain point 
   ** @param x,y,z     Point coordinates (global) [cm]
   ** @value Bx,By,Bz  Field components [kG]
   **/
  virtual Double_t GetBx(Double_t x, Double_t y, Double_t z);
  virtual Double_t GetBy(Double_t x, Double_t y, Double_t z);
  virtual Double_t GetBz(Double_t x, Double_t y, Double_t z);

  //  virtual Double_t GetDistorted(Double_t b,Double_t *coords, TFormula *formula);

  virtual void SetType(Int_t pfType=0) { fType = pfType; }

  virtual const char* GetDistortionFilename() {return fDistortionFilename.Data();}
  virtual Bool_t SetDistortionFilename(const char *filename) {fDistortionFilename=filename; return (fDistortionFilename.Length()!=0);}
 
  virtual TFormula* GetDistortionFormula(const char *component_option="y", const char *action_option="m");  // component_opt:"x","y","z";action_opt:"m","a" 
  virtual Bool_t SetDistortionFormula (TFormula* parDistortionFormula, const char *component_option="y", const char *action_option="m" ); 
  virtual Bool_t SetDistortionFormula (const char* parDistortionFormulaText, const char *component_option="y", const char *action_option="m" ); 


  /** Fill main part of distorted field map from parent BmnField  **/
  virtual void SetFromParent(FairField* field);

  /** Get parent Field   **/
  virtual FairField* GetParent() {return fParentField;}

  /** Screen output **/
  virtual void Print();

  /** Set the position of the field centre of the distorted and the parent field**/
  virtual void SetPosition(Double_t x, Double_t y, Double_t z);

  /** Set a global field scaling factor of the distorted and the parent field**/
  virtual void SetScale(Double_t factor);
 
  /** Plot distorted and parend field (By component) **/
  void PlotBy(Int_t n=250, Double_t zmin=-50, Double_t zmax=450);
	
 protected:

  /** Parent field (can be field map or constant field)  **/
  FairField *fParentField;   //!

  /** Field type of parent field.  **/
  Int_t fTypeOfParent;

  /** Filename for field map distortion information **/
      TString  fDistortionFilename;

  /** Formulas for field map distortion **/
      TFormula* fBxDistortionFormulaMult, *fBxDistortionFormulaAdd;     //! getter/setter options: ("x","m"), ("x","a")
      TFormula* fByDistortionFormulaMult, *fByDistortionFormulaAdd;     //! getter/setter options: ("y","m"), ("y","a")
      TFormula* fBzDistortionFormulaMult, *fBzDistortionFormulaAdd;     //! getter/setter options: ("z","m"), ("z","a")

 private:
  BmnFieldMapDistorted(const BmnFieldMapDistorted&);
  BmnFieldMapDistorted& operator=(const BmnFieldMapDistorted&);

  ClassDef(BmnFieldMapDistorted,1) 

};


#endif
