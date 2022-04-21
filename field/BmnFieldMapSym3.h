// -------------------------------------------------------------------------
// -----                    BmnFieldMapSym3 header file                -----
// -----          Created 12/01/04  by M. Al/Turany (BmnField.h)       -----
// -----                Redesign 20/02/06  by V. Friese                -----
// -------------------------------------------------------------------------

/** BmnFieldMapSym3.h
 ** @author M.Al/Turany <m.al-turany@gsi.de>
 ** @author V.Friese <v.friese@gsi.de>
 ** @since 12.01.2004
 ** @version1.0
 **
 ** Magnetic field map on a 3-D grid with symmetries w.r.t. the three
 ** coordinate axes. The map is only stored in the octant x>0, y>0, z>0.
 ** The symmetries are:
 ** - Bx is antisymmetric in x and symmetric in y and z
 ** - By is symmetric in x, y and z
 ** - Bz is antisymmetric in x and z and symmetric in y
 **
 ** Field values are hold and returned in kG.
 **/

#ifndef BMNMAGFIELDMAPSYM3_H
#define BMNMAGFIELDMAPSYM3_H 1

#include "BmnFieldMap.h"
#include "BmnFieldPar.h"


class BmnFieldMapSym3 : public BmnFieldMap
{
  public:
    /** Default constructor **/
    BmnFieldMapSym3();

    /** Standard constructor
     ** @param name       Name of field map
     ** @param fileType   R = ROOT file, A = ASCII
     **/
    BmnFieldMapSym3(const char* mapName);

    /** Constructor from BmnFieldPar  **/
    BmnFieldMapSym3(BmnFieldPar* fieldPar);

    /** Destructor **/
    virtual ~BmnFieldMapSym3();

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

    void FillParContainer();

  protected:
    /** Hemispheres of a point (for temporary use)  **/
    Double_t fHemiX, fHemiY, fHemiZ;   //!

  ClassDef(BmnFieldMapSym3,1) 
};

#endif
