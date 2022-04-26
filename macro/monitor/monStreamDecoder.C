R__LOAD_LIBRARY(libFairMQ)
void monStreamDecoder(TString addr = "bmn-daq.he.jinr.ru:32999",
        Int_t periodID = 7,
        BmnSetup bmnSetup = kBMNSETUP) {
    
    BmnOnlineDecoder *deco = new BmnOnlineDecoder();
    deco->SetPeriodID(periodID);
    deco->SetBmnSetup(bmnSetup);
    deco->SetDaqAddress(addr);
    std::map<DetectorId, bool> setup; // flags to determine BM@N setup
    setup.insert(std::make_pair(kBC,        1)); // TRIGGERS
    setup.insert(std::make_pair(kMWPC,      1)); // MWPC
    setup.insert(std::make_pair(kSILICON,   1)); // SILICON
    setup.insert(std::make_pair(kGEM,       1)); // GEM
    setup.insert(std::make_pair(kTOF1,      1)); // TOF-400
    setup.insert(std::make_pair(kTOF,       1)); // TOF-700
    setup.insert(std::make_pair(kDCH,       1)); // DCH
    setup.insert(std::make_pair(kZDC,       0)); // ZDC
    setup.insert(std::make_pair(kECAL,      0)); // ECAL
    setup.insert(std::make_pair(kLAND,      1)); // LAND
    setup.insert(std::make_pair(kTOFCAL,    1)); // LAND
    setup.insert(std::make_pair(kCSC,       1)); // CSC
    setup.insert(std::make_pair(kSCWALL,    1)); // SCWALL
    setup.insert(std::make_pair(kFHCAL,     1)); // FHCAL
    setup.insert(std::make_pair(kHODO,      1)); // HODO
    deco->SetDetectorSetup(setup);
    deco->ProcessStream();
    delete deco;
}

