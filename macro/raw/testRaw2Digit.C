using namespace std;

void testRaw2Digit(char *fname="bmn_run0648.root") {
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();
    /////////////////////////////////////////////////////////////////////////////////////
    int RUN; sscanf(&fname[strlen(fname) - 8], "%d", &RUN);
    const char *mapping;
    if (RUN < 189) mapping = "DCH_map_Feb20_Feb25.txt";
    else if (RUN >= 189 && RUN < 470) mapping = "DCH_map_Mar4.txt";
     else if (RUN >= 470 && RUN < 689) mapping = "DCH_map_Mar11.txt";
    else  mapping = "DCH_map_Run4.txt";
    BmnDchRaw2Digit  DCH(mapping); DCH.print();

    //   BmnZDCRaw2Digit  ZDC("ZDC_map_Feb20.txt"); ZDC.print();
   BmnZDCRaw2Digit  ZDC("ZDC_map_Mar08.txt"); ZDC.print();

    if(RUN < 470) mapping="TOF700_map_Feb20_Mar11.txt";
    else mapping="TOF700_map_Mar11.txt";
    BmnTof2Raw2Digit TOF2(mapping); TOF2.print();

    if(RUN < 470) mapping="TOF400_map_Mar4.txt";
    else mapping="TOF400_map_Mar11.txt";
    BmnTof1Raw2Digit TOF1(mapping); TOF1.print();

    cout << "Process RUN:  " << RUN << endl;


    /////////////////////////////////////////////////////////////////////////////////////
    TFile *_f_in = new TFile(fname, "READ");
    TTree *_t_in = (TTree *) _f_in->Get("BMN_RAW");
    TClonesArray *t0_raw   = new TClonesArray("BmnTDCDigit");
    TClonesArray *sync_raw = new TClonesArray("BmnSyncDigit");
    TClonesArray *dch_raw  = new TClonesArray("BmnTDCDigit");
    TClonesArray *tof1_raw = new TClonesArray("BmnTDCDigit");
    TClonesArray *tof2_raw = new TClonesArray("BmnTDCDigit");
    TClonesArray *zdc_raw  = new TClonesArray("BmnADCDigit");
    TClonesArray *ecal_raw = new TClonesArray("BmnADCDigit");
    _t_in->SetBranchAddress("bmn_t0",    &t0_raw);
    _t_in->SetBranchAddress("bmn_sync",  &sync_raw);
    _t_in->SetBranchAddress("bmn_dch",   &dch_raw);
    _t_in->SetBranchAddress("bmn_tof400",&tof1_raw);
    _t_in->SetBranchAddress("bmn_tof700",&tof2_raw);
    _t_in->SetBranchAddress("bmn_zdc",   &zdc_raw);
    _t_in->SetBranchAddress("bmn_ecal",  &ecal_raw);
    /////////////////////////////////////////////////////////////////////////////////////
    unsigned long EVENT,TIME_SEC,TIME_NS;
    float T0;
    char str[100]; sprintf(str,"bmn_run%04d_digit.root",RUN); 
    TFile *_f_out = new TFile(str, "RECREATE");
    _f_out->SetCompressionLevel(1);
    TTree *_t_out = new TTree("BMN_DIGIT","test_bmn");
    TClonesArray * dch_digit   = new TClonesArray("BmnDchDigit");
    TClonesArray * zdc_digit   = new TClonesArray("BmnZDCDigit");
    TClonesArray * tof2_digit  = new TClonesArray("BmnTof2Digit");
    TClonesArray * tof1_digit  = new TClonesArray("BmnTof1Digit");
    _t_out->Branch("bmn_run",         &RUN,     "bmn_run/I");   
    _t_out->Branch("bmn_event",       &EVENT,   "bmn_event/I");   
    _t_out->Branch("bmn_time_sec",    &TIME_SEC,"bmn_time_sec/I");   
    _t_out->Branch("bmn_time_ns",     &TIME_NS, "bmn_time_ns/I");   
    _t_out->Branch("bmn_t0",          &T0,      "bmn_t0/F");   
    _t_out->Branch("bmn_dch_digit",   &dch_digit);   
    _t_out->Branch("bmn_zdc_digit",   &zdc_digit);   
    _t_out->Branch("bmn_tof2_digit",  &tof2_digit);   
    _t_out->Branch("bmn_tof1_digit",  &tof1_digit);   
    /////////////////////////////////////////////////////////////////////////////////////

    for (int ev = 0; ev < _t_in->GetEntries(); ev++) {
        if ((ev % 100) == 0) printf("%d\n", EVENT);
        sync_raw->Clear(); t0_raw->Clear(); dch_raw->Clear();
        tof1_raw->Clear(); tof2_raw->Clear(); zdc_raw->Clear(); ecal_raw->Clear();

        dch_digit->Clear();
        zdc_digit->Clear();  
        tof2_digit->Clear();  
        tof1_digit->Clear(); 
 
        _t_in->GetEntry(ev);

        DCH.fillEvent(dch_raw, sync_raw, t0_raw, dch_digit);
        DCH.getEventInfo(&EVENT,&TIME_SEC,&TIME_NS);
        TOF2.fillEvent(tof2_raw, sync_raw, t0_raw, tof2_digit);
        TOF1.fillEvent(tof1_raw, sync_raw, t0_raw, tof1_digit);
        ZDC.fillEvent(zdc_raw, zdc_digit);
        T0=DCH.get_t0();
        if (DCH.get_t0() == 0 || EVENT==0) continue;
        _t_out->Fill();
    }
    /////////////////////////////////////////////////////////////////////////////////////
   
    _f_in->Close();
    _t_out->Print();
    _t_out->Write(); 
    _f_out->Close();
}
