#include "BmnCoordinateDetQa.h"

BmnCoordinateDetQa::BmnCoordinateDetQa(TString det, UInt_t id) :
fHistoManager(new BmnQaHistoManager()),
fSteering(new BmnOfflineQaSteering()),
fDetGem(nullptr),
fDetSilicon(nullptr),
fDetCsc(nullptr) {
    fSteering->SetGeometriesByRunId(id, fDetGem, fDetSilicon, fDetCsc);

    if (det.Contains("GEM")) {
        // GEM 
        DistributionOfFiredStrips <BmnGemStripStationSet> ((void*) fDetGem, "GEM");
        DistributionOfFiredStripsVsSignal <BmnGemStripStationSet> ((void*) fDetGem, "GEM");
    } else if (det.Contains("SILICON")) {
        // SILICON
        DistributionOfFiredStrips <BmnSiliconStationSet> ((void*) fDetSilicon, "SILICON");
    } else if (det.Contains("CSC")) {
        // CSC
        DistributionOfFiredStrips <BmnCSCStationSet> ((void*) fDetCsc, "CSC");
    } else
        throw;
}
