// -------------------------------------------------------------------------
// -----                    BmnFieldMapSym2 header file                -----
// -----       Created 09/06/05  by E. Litvinenko (BmnFieldMapLIT.h)   -----
// -----                Redesign 20/02/06  by V. Friese                -----
// -------------------------------------------------------------------------


/** BmnFieldMapSym2.h
 ** @author E. Litvinenko <litvin@nf.jinr.ru>
 ** @author V.Friese <v.friese@gsi.de>
 ** @since 09.06.2005
 ** @version1.0
 **
 ** Magnetic field map on a 3-D grid with symmetries w.r.t. the x and y
 ** coordinate axes. The map is only stored in the quadrant x>0, y>0.
 ** The symmetries are:
 ** - Bx is antisymmetric in x and y
 ** - By is symmetric in x and y
 ** - Bz is symmetric in x antisymmetric in y
 **
 ** Field values are hold and returned in kG.
 **/


#ifndef CBMMAGFIELDMAPSYM2_H
#define CBMMAGFIELDMAPSYM2_H 1


#include "BmnFieldMap.h"


class BmnFieldPar;


class BmnFieldMapSym2 : public BmnFieldMap {


public:


  /** Default constructor **/
  BmnFieldMapSym2();


  /** Standard constructor
   ** @param name       Name of field map
   ** @param fileType   R = ROOT file, A = ASCII
   **/
  BmnFieldMapSym2(const char* mapName, const char* fileType = "R");


  /** Constructor from BmnFieldPar **/
  BmnFieldMapSym2(BmnFieldPar* fieldPar);


  /** Destructor **/
  virtual ~BmnFieldMapSym2();


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
  Double_t fHemiX, fHemiY;   //!


  ClassDef(BmnFieldMapSym2,1) 

};


#endif
