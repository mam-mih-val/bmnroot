#ifndef __SRCEVENT_H__
#define __SRCEVENT_H__

#include "TVector3.h"


class SRCEvent
{
 public:
  SRCEvent(TVector3 buHit, TVector3 bdHit, TVector3 lgHit, TVector3 ltHit, TVector3 rgHit, TVector3 rtHit);
  ~SRCEvent();
  TVector3 bHits[2];
  TVector3 lHits[2];
  TVector3 rHits[2];
};

#endif
