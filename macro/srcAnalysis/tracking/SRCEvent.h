#ifndef __SRCEVENT_H__
#define __SRCEVENT_H__

#include "TVector3.h"
#include <vector>

class SRCArm
{
 public:
  SRCArm(TVector3 h1, TVector3 h2) { hits[0]=h1; hits[1]=h2; }
  TVector3 hits[2];
};

class SRCEvent
{
 public:
  SRCEvent(TVector3 buHit, TVector3 bdHit);
  ~SRCEvent();

  std::vector<SRCArm> armList;
  void addArm(TVector3 h1, TVector3 h2);
};

#endif
