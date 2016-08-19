#include <float.h>

#include "BmnAlignmentContainer.h"

BmnAlignmentContainer::BmnAlignmentContainer() :
fX0(0.),
fY0(0.),
fZ0(0.),
fTx(0.),
fTy(0.),
fTrackHits(NULL),
fXresMax(LDBL_MAX),
fYresMax(LDBL_MAX) {


}

BmnAlignmentContainer::~BmnAlignmentContainer() {

}
