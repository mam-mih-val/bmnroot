#include "SRCEvent.h"

SRCEvent::SRCEvent(TVector3 buHit, TVector3 bdHit, TVector3 lgHit, TVector3 ltHit, TVector3 rgHit, TVector3 rtHit)
{
  bHits[0]=buHit;
  bHits[1]=bdHit;
  lHits[0]=lgHit;
  lHits[1]=ltHit;
  rHits[0]=rgHit;
  rHits[1]=rtHit;
}

SRCEvent::~SRCEvent()
{
}
