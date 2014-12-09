#ifndef L1Triplet_H
#define L1Triplet_H

class L1Triplet
{

 private:

  THitI w0; // left hit (16b)  index in vStsHits array
  THitI w1; // middle hit(16b)
  THitI w2; // right hit(16b)
  unsigned char      b0; // chi2(5b) + level(3b)
  unsigned char      bl; // level
  unsigned char      b1; // qp (8b)
  unsigned char      st; // staL (4b), staM-1-staL (2b), staR-2-staL (2b)
  float chi2;
  
 public:

  unsigned char Cqp;

  std::vector<unsigned int> neighbours;

 L1Triplet(): w0(0), w1(0), w2(0), b0(0), bl(0), b1(0), st(0), chi2(0.),
    Cqp(0), neighbours() {
    neighbours.clear();
  };
  
  void Set( unsigned int iHitL, unsigned int iHitM, unsigned int iHitR,
            unsigned int iStaL, unsigned int iStaM, unsigned int iStaR,
	    unsigned char Level, unsigned char  Qp, 
	    float Chi2  
	    ){ 
    chi2 = Chi2;
    
    w0 = iHitL;
    w1 = iHitM;
    w2 = iHitR;

    Chi2 = sqrt(fabs(Chi2))*31./3.5 ; // 3.5 == 31
    if( Chi2>31 || !finite(Chi2) ) Chi2 = 31;
    b0 = ( (static_cast<unsigned char>( Chi2 ))<<3 ) + (Level%8);
    b1 = Qp;
    bl = Level;

    st = (iStaL<<4) + ((iStaM-iStaL-1)<<2) + (iStaR-iStaL-2);
  }

  THitI GetLHit() const { 
    return w0; 
  }
  THitI GetMHit() const { 
    return w1; 
  }
  THitI GetRHit() const { 
    return w2; 
  }

  // unsigned int GetFirstNeighbour() const { 
  //   return w1; 
  // }
  // unsigned int GetNNeighbours() const { 
  //   return w2; 
  // }
  
  unsigned char GetLevel() const {
//    return b0%8;
      return bl;
  }

  unsigned char GetQp() const {
    return b1;
  }

  float GetChi2() const {
    // float x = (b0>>3)*3.5/31.;
    // return x*x;
    return chi2;
  }

  float GetQpOrig( float MaxInvMom ){
    return(GetQp()*(MaxInvMom*2)/255. - MaxInvMom);
  }

  int GetLSta() const { 
    return st>>4; 
  }
  
  int GetMSta() const { 
    return ((st%16)>>2) + GetLSta() + 1; 
  }
  
  int GetRSta() const {
    return (st%4) + GetLSta() + 2; 
  }

};

#endif
