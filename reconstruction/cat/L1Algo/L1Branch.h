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
 *  L1 branch class
 *
 *====================================================================
 */

#ifndef L1Branch_H
#define L1Branch_H

#include "L1StsHit.h"
#include <vector>

class L1Triplet;


struct L1Branch
{
  L1Branch():Quality(0),Momentum(0),chi2(0),StsHits(){
    StsHits.reserve(10);
  }
    
  unsigned short int Quality;
  float Momentum, chi2;
  std::vector<THitI> StsHits;

  void Set( unsigned char iStation, unsigned char Length, float Chi2, float Qp ){
    unsigned short int ista = 16-iStation;
    float tmp = sqrt(Chi2)/3.5*255;
    if( tmp>255 ) tmp = 255;
    unsigned short int chi_2 = 255 - static_cast<unsigned char>( tmp );
    Quality = (Length<<12) + (ista<<8) + chi_2;
    Momentum = 1.0/fabs(Qp);
//    chi2 = chi_2;
    chi2 = Chi2;
  }

  void SetLength( unsigned char Length ){
    Quality += - (Quality>>12) + (Length<<12);
  }

  static bool compareChi2(const L1Branch &a, const L1Branch &b){
    return (a.Quality > b.Quality );
  }
  static bool comparePChi2(const L1Branch *a, const L1Branch *b){
    return compareChi2(*a,*b);
  }
  static bool compareMomentum(const L1Branch &a, const L1Branch &b){
    return (a.Momentum > b.Momentum );
  }
  static bool comparePMomentum(const L1Branch *a, const L1Branch *b){
    return compareMomentum(*a,*b);
  }

};

#endif
