#include "SRCEvent.h"

SRCEvent::SRCEvent(TVector3 buHit, TVector3 bdHit)
{
  addArm(buHit,bdHit);
}

SRCEvent::~SRCEvent()
{
}

void SRCEvent::addArm(TVector3 h1, TVector3 h2)
{
  armList.push_back(SRCArm(h1,h2));
}
