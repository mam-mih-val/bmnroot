#include "BmnCoordinateDetQa.h"

BmnCoordinateDetQa::BmnCoordinateDetQa(TString det, UInt_t runId) :
fHistoManager(new BmnQaHistoManager()) {
    Bool_t isBMN = kFALSE;
    Bool_t isSRC = kFALSE;

    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");

    if (runId > 3589) // FIXME!
        isBMN = kTRUE;
    else
        isSRC = kTRUE;

    // GEM  
    if (det.Contains("GEM")) {
        TString confGEM = isBMN ? "GemRunSpring2018.xml" : isSRC ? "GemRunSRCSpring2018.xml" : "";
        TString gPathGemConfig = gPathConfig + "/parameters/gem/XMLConfigs/";
        fDetGem = new BmnGemStripStationSet(gPathGemConfig + confGEM);

        DistributionOfFiredStrips <BmnGemStripStationSet> ((void*) fDetGem, "GEM");
    } 
    
    else if (det.Contains("SILICON")) {
        // SILICON
        TString confSIL = isBMN ? "SiliconRunSpring2018.xml" : isSRC ? "SiliconRunSRCSpring2018.xml" : "";
        TString gPathSilConfig = gPathConfig + "/parameters/silicon/XMLConfigs/";
        fDetSilicon = new BmnSiliconStationSet(gPathSilConfig + confSIL);
        
        DistributionOfFiredStrips <BmnSiliconStationSet> ((void*) fDetSilicon, "SILICON");       
    } 
    
    else if (det.Contains("CSC")) {
        // CSC
        TString confCSC = "CSCRunSpring2018.xml";
        TString gPathCscConfig = gPathConfig + "/parameters/csc/XMLConfigs/";
        fDetCsc = new BmnCSCStationSet(gPathCscConfig + confCSC);
        
        DistributionOfFiredStrips <BmnCSCStationSet> ((void*) fDetCsc, "CSC");
    } 
    
    else
        throw;
}
