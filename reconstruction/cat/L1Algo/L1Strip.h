#ifndef _L1Strip_h_
#define _L1Strip_h_

#include "CbmL1Def.h"

struct L1Strip{
  //L1Strip():f(0),n(0){}
L1Strip():f(0),df(0.0),n(0){} //AZ
  //L1Strip(fscal _f, unsigned short int _n = 0):f(_f),n(_n){}
L1Strip(fscal _f, fscal _df = 0.0, unsigned short int _n = 0):f(_f),df(_df),n(_n){}

  operator fscal() { return f;}
  fscal errf() { return df; } //AZ
//   operator fvec() { return fscal(f);}

// private:
  fscal f;
  fscal df; //AZ
  unsigned short int n; // number of event
};

typedef unsigned /*short*/ int TStripI; // strip index type

#endif
