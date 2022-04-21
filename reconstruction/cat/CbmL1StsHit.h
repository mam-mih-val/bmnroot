#ifndef _CbmL1StsHit_h_
#define _CbmL1StsHit_h_

#include <vector>
using std::vector;

  // hits with hit-mcpoint match information
class CbmL1StsHit
{
  public:
  CbmL1StsHit():hitId(0),extIndex(0),mcPointIds(){};
  CbmL1StsHit(int hitId_, int extIndex_)
    :hitId(hitId_),extIndex(extIndex_),mcPointIds(){};

  int hitId;              // index of L1StsHit in algo->vStsHits array. Should be equal to index of this in L1->vStsHits
  int extIndex;          // index of hit in the TClonesArray array
  vector<int> mcPointIds; // indices of CbmL1MCPoint in L1->vMCPoints array
};

#endif
