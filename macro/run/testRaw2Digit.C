
using namespace std;

void testRaw2Digit(char *fname="bmn_run0166.root") {
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();
    /////////////////////////////////////////////////////////////////////////////////////
    int RUN; sscanf(&fname[strlen(fname) - 8], "%d", &RUN);
    const char *mapping;
    if (RUN < 189) mapping = "DCH_map_Feb20_Feb25.txt"; else mapping = "DCH_map_Mar4.txt";
    BmnDchRaw2Digit DCH(mapping); DCH.print();
    cout << "Process RUN:  " << RUN << endl;
    /////////////////////////////////////////////////////////////////////////////////////
    TFile *_f_in = new TFile(fname, "READ");
    TTree *_t_in = (TTree *) _f_in->Get("BMN_RAW");
    TClonesArray *sync_raw = new TClonesArray("BmnTDCDigit");
    TClonesArray *dch_raw = new TClonesArray("BmnTDCDigit");
    TClonesArray *t0_raw = new TClonesArray("BmnTDCDigit");
    _t_in->SetBranchAddress("bmn_sync", &sync_raw);
    _t_in->SetBranchAddress("bmn_dch", &dch_raw);
    _t_in->SetBranchAddress("bmn_t0", &t0_raw);
    /////////////////////////////////////////////////////////////////////////////////////
    unsigned long EVENT,TIME_SEC,TIME_NS;
    char str[100]; sprintf(str,"bmn_run%04d_digit.root",RUN); 
    TFile *_f_out = new TFile(str, "RECREATE");
    _f_out->SetCompressionLevel(1);
    TTree *_t_out = new TTree("BMN_DIGIT","test_bmn");
    TClonesArray * dch_digit   = new TClonesArray("BmnDchDigit");
    _t_out->Branch("bmn_run",       &RUN,"bmn_run/I");   
    _t_out->Branch("bmn_event",     &EVENT,"bmn_event/I");   
    _t_out->Branch("bmn_time_sec",  &TIME_SEC,"bmn_time_sec/I");   
    _t_out->Branch("bmn_time_ns",   &TIME_NS,"bmn_time_ns/I");   
    _t_out->Branch("bmn_dch_digit",   &dch_digit);   
    /////////////////////////////////////////////////////////////////////////////////////
    for (int ev = 0; ev < _t_in->GetEntries(); ev++) {
        if ((ev % 100) == 0) printf("%d\n", EVENT);
        sync_raw->Clear(); dch_raw->Clear(); t0_raw->Clear();
        dch_digit->Clear();  
        _t_in->GetEntry(ev);

        DCH.fillEvent(dch_raw, sync_raw, t0_raw, dch_digit);
        DCH.getEventInfo(&EVENT,&TIME_SEC,&TIME_NS);

        if (DCH.get_t0() == 0 ||EVENT==0) continue;
        _t_out->Fill();
    }
    /////////////////////////////////////////////////////////////////////////////////////
    _f_in->Close();
    _t_out->Print();
    _t_out->Write(); 
    _f_out->Close();

}
