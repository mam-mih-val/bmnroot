/**
 * \file BmnDetectorSetup.xx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2012-2014
 */
#include "BmnDetectorSetup.h"

#include "TGeoManager.h"
#include "TObjArray.h"
#include "TGeoNode.h"

#include <cassert>
#include <iostream>

using std::cout;

BmnDetectorSetup::BmnDetectorSetup() : fDet() {
}

BmnDetectorSetup::~BmnDetectorSetup() {
}

void BmnDetectorSetup::SetDet(DetectorId detId, Bool_t isDet) {
    fDet[detId] = isDet;
}

Bool_t BmnDetectorSetup::GetDet(DetectorId detId) const {
    assert(fDet.count(detId) != 0);
    return fDet.find(detId)->second;
}

Bool_t BmnDetectorSetup::CheckDetectorPresence(const std::string& name) const {
    assert(gGeoManager != NULL);

    TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
    for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
        TGeoNode* node = (TGeoNode*) nodes->At(iNode);
        if (TString(node->GetName()).Contains(name.c_str())) {
            return kTRUE;
        }
    }
    return kFALSE;
}

void BmnDetectorSetup::DetermineSetup() {
    fDet[kMWPC] = CheckDetectorPresence("MWPC");
    fDet[kGEM] = CheckDetectorPresence("GEM");
    fDet[kDCH] = CheckDetectorPresence("DCH");
    fDet[kTOF1] = CheckDetectorPresence("TOF400");
    fDet[kTOF] = CheckDetectorPresence("tof2");
}

string BmnDetectorSetup::ToString() const {
    string str = "-I- BMN detector setup information: \n";
    str += "  Tracking detectors found in setup: ";
    if (fDet.find(kMWPC)->second) str += "MWPC ";
    if (fDet.find(kGEM)->second) str += "GEM ";
    if (fDet.find(kTOF1)->second) str += "TOF400 ";
    if (fDet.find(kDCH)->second) str += "DCH ";
    if (fDet.find(kTOF)->second) str += "TOF700 ";
    str += "\n";
    return str;
}
