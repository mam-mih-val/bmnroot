#include "BmnAlignmentContainer.h"

BmnAlignmentContainer::BmnAlignmentContainer() : BmnGemTrack() {
    fEventNumber = 0;
    fIndex = 0;
    fXresMax = LDBL_MAX;
    fYresMax = LDBL_MAX;
}

BmnAlignmentContainer::~BmnAlignmentContainer() {

}
