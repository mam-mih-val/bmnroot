#include "BmnCoordinateDetQa.h"

BmnCoordinateDetQa::BmnCoordinateDetQa(TString det, UInt_t id) :
fHistoManager(new BmnQaHistoManager()),
fSteering(new BmnOfflineQaSteering()),
fDetGem(nullptr),
fDetSilicon(nullptr),
fDetCsc(nullptr) {
    fSteering->SetGeometriesByRunId(id, fDetGem, fDetSilicon, fDetCsc);
    
    pair <Int_t, TString> periodSetup = fSteering->GetRunAndSetupByRunId(id);
    TString prefix = TString::Format("RUN%d_SETUP_%s_", periodSetup.first, periodSetup.second.Data());

    if (det.Contains("GEM") && fDetGem) {
        // GEM 
        DistributionOfFiredStrips <BmnGemStripStationSet> ((void*) fDetGem, prefix + det);
        DistributionOfFiredStripsVsSignal <BmnGemStripStationSet> ((void*) fDetGem, prefix + det);
    } else if (det.Contains("SILICON") && fDetSilicon) {
        // SILICON
        DistributionOfFiredStrips <BmnSiliconStationSet> ((void*) fDetSilicon, prefix + det);
    } else if (det.Contains("CSC") && fDetCsc) {
        // CSC
        DistributionOfFiredStrips <BmnCSCStationSet> ((void*) fDetCsc, prefix + det);
    }
}
