// -------------------------------------------------------------------------
// -----                    BmnFieldMapSym1 header file                -----
// -----                 Created 22/09/08  by E.Litvinenko             -----
// -------------------------------------------------------------------------


/** BmnFieldMapSym1.h
 ** @author E. Litvinenko <litvin@nf.jinr.ru>
 ** @since 22.09.2008
 ** @version1.0
 **
 ** Magnetic field map on a 3-D grid with symmetries w.r.t. the x and y
 ** coordinate axes. The map is stored only in half of space x>0.
 ** The symmetries are:
 ** - Bx is antisymmetric in x 
 ** - By is symmetric in x 
 ** - Bz is symmetric in x
 **
 ** Field values are hold and returned in kG.
 **/


#ifndef CBMMAGFIELDMAPSYM1_H
#define CBMMAGFIELDMAPSYM1_H 1


#include "BmnFieldMap.h"


class BmnFieldPar;


class BmnFieldMapSym1 : public BmnFieldMap {


public:


  /** Default constructor **/
  BmnFieldMapSym1();


  /** Standard constructor
   ** @param name       Name of field map
   ** @param fileType   R = ROOT file, A = ASCII
   **/
  BmnFieldMapSym1(const char* mapName, const char* fileType = "R");


  /** Constructor from BmnFieldPar **/
  BmnFieldMapSym1(BmnFieldPar* fieldPar);


  /** Destructor **/
  virtual ~BmnFieldMapSym1();


  /** Get the field components at a certain point 
   ** @param x,y,z     Point coordinates (global) [cm]
   ** @value Bx,By,Bz  Field components [kG]
   **/
  virtual Double_t GetBx(Double_t x, Double_t y, Double_t z);
  virtual Double_t GetBy(Double_t x, Double_t y, Double_t z);
  virtual Double_t GetBz(Double_t x, Double_t y, Double_t z);


  /** Determine whether a point is inside the field map
   ** @param x,y,z              Point coordinates (global) [cm]
   ** @param ix,iy,iz (return)  Grid cell
   ** @param dx,dy,dz (return)  Distance from grid point [cm] if inside
   ** @value kTRUE if inside map, else kFALSE
   **/
  virtual Bool_t IsInside(Double_t x, Double_t y, Double_t z,
			  Int_t& ix, Int_t& iy, Int_t& iz,
			  Double_t& dx, Double_t& dy, Double_t& dz);


	
 protected:

  // Hemispheres of a point (for temporary use)
  Double_t fHemiX;   //!


  ClassDef(BmnFieldMapSym1,1) 

};


#endif
