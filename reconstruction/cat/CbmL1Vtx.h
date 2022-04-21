/*
 *====================================================================
 *
 *  CBM Level 1 Reconstruction 
 *  
 *  Authors: I.Kisel,  S.Gorbunov
 *
 *  e-mail : ikisel@kip.uni-heidelberg.de 
 *
 *====================================================================
 *
 *  L1 vertex class
 *
 *====================================================================
 */

#ifndef CbmL1Vtx_H
#define CbmL1Vtx_H

#include "CbmKFVertexInterface.h"

struct CbmL1Vtx: public CbmKFVertexInterface {
  CbmL1Vtx():
    MC_mass(0), MC_q(0), MC_p(0), MC_x(0), MC_y(0), MC_z(0), MC_px(0), MC_py(0), MC_pz(0),
    MC_ID(0), MC_pdg(0),
    x(0), y(0), z(0), chi2(0),
    NDF(0),
    mass(0), mass_err(0)
  {};
  
  double MC_mass, MC_q, MC_p, MC_x, MC_y, MC_z, MC_px, MC_py, MC_pz;
  int MC_ID, MC_pdg;

  double x, y, z, C[6], chi2;
  int NDF;

  double mass, mass_err;

  Double_t &GetRefX(){ return x; } 
  Double_t &GetRefY(){ return y; } 
  Double_t &GetRefZ(){ return z; } 
  Double_t *GetCovMatrix(){ return C; }   
  Double_t &GetRefChi2()  { return chi2; }   
  Int_t    &GetRefNDF()   { return NDF; }      
  Double_t &GetRefMass()  { return mass; }    
  Double_t &GetRefMassError() { return mass_err; }  
};

#endif
