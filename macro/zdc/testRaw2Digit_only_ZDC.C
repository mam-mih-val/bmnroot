
using namespace std;

void testRaw2Digit_only_ZDC(char *fname="bmn_run0527.root") {
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();
    /////////////////////////////////////////////////////////////////////////////////////
    int RUN; sscanf(&fname[strlen(fname) - 8], "%d", &RUN);
    const char *mapping;

   BmnZDCRaw2Digit  ZDC("ZDC_map_Mar08.txt"); ZDC.print();


    cout << "Process RUN:  " << RUN << endl;


    /////////////////////////////////////////////////////////////////////////////////////
    TFile *_f_in = new TFile(fname, "READ");
    TTree *_t_in = (TTree *) _f_in->Get("BMN_RAW");
    TClonesArray *t0_raw   = new TClonesArray("BmnTDCDigit");
    TClonesArray *sync_raw = new TClonesArray("BmnSyncDigit");
    TClonesArray *zdc_raw  = new TClonesArray("BmnADCDigit");
    _t_in->SetBranchAddress("bmn_t0",    &t0_raw);
    _t_in->SetBranchAddress("bmn_sync",  &sync_raw);
    _t_in->SetBranchAddress("bmn_zdc",   &zdc_raw);
    /////////////////////////////////////////////////////////////////////////////////////
    unsigned long EVENT,TIME_SEC,TIME_NS;
    float T0;
    char str[100]; sprintf(str,"bmn_run%04d_digit_only_zdc.root",RUN); 
    TFile *_f_out = new TFile(str, "RECREATE");
    _f_out->SetCompressionLevel(1);
    TTree *_t_out = new TTree("BMN_DIGIT","test_bmn");
    TClonesArray * zdc_digit   = new TClonesArray("BmnZDCDigit");
    _t_out->Branch("bmn_run",         &RUN,     "bmn_run/I");   
    _t_out->Branch("bmn_event",       &EVENT,   "bmn_event/I");   
    _t_out->Branch("bmn_time_sec",    &TIME_SEC,"bmn_time_sec/I");   
    _t_out->Branch("bmn_time_ns",     &TIME_NS, "bmn_time_ns/I");   
    _t_out->Branch("bmn_t0",          &T0,      "bmn_t0/F");   
    _t_out->Branch("bmn_zdc_digit",   &zdc_digit);   
    /////////////////////////////////////////////////////////////////////////////////////

    for (int ev = 0; ev < _t_in->GetEntries(); ev++) {
        if ((ev % 100) == 0) printf("%d\n", EVENT);
        sync_raw->Clear(); t0_raw->Clear(); zdc_raw->Clear(); 

         zdc_digit->Clear();  
 
        _t_in->GetEntry(ev);

        ZDC.fillEvent(zdc_raw, zdc_digit);
        _t_out->Fill();
    }
    /////////////////////////////////////////////////////////////////////////////////////
   
    _f_in->Close();
    _t_out->Print();
    _t_out->Write(); 
    _f_out->Close();
}
