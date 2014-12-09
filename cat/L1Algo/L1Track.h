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
 *  L1 track class
 *
 *====================================================================
 */

#ifndef L1Track_H
#define L1Track_H

class L1Track
{
  public:
  unsigned char NHits;
  float Momentum;
  double TFirst[6], CFirst[15], TLast[6], CLast[15], chi2;
  int NDF;
};

#endif
