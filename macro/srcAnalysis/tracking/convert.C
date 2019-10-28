void convert()
{
  // Load the BMNRoot libraries
  gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
  bmnloadlibs(); // load BmnRoot libraries

  // Set up input file
  TFile *infile = new TFile("~/george_convert/Dubna_4p5GeVpercperu_600k_LAND.root");
  TTree *intree = (TTree*)infile->Get("T");
  double hx1_tof400, hy1_tof400, hz1_tof400, hx2_tof400, hy2_tof400, hz2_tof400,
    hx1_GEM, hy1_GEM, hz1_GEM, hx2_GEM, hy2_GEM, hz2_GEM,
    hx1_MWPC, hy1_MWPC, hz1_MWPC, hx2_MWPC, hy2_MWPC, hz2_MWPC,
    time_P1, time_P2;
  intree->SetBranchAddress("hx1_tof400",&hx1_tof400);
  intree->SetBranchAddress("hy1_tof400",&hy1_tof400);
  intree->SetBranchAddress("hz1_tof400",&hz1_tof400);
  intree->SetBranchAddress("hx2_tof400",&hx2_tof400);
  intree->SetBranchAddress("hy2_tof400",&hy2_tof400);
  intree->SetBranchAddress("hz2_tof400",&hz2_tof400);
  intree->SetBranchAddress("hx1_GEM",&hx1_GEM);
  intree->SetBranchAddress("hy1_GEM",&hy1_GEM);
  intree->SetBranchAddress("hz1_GEM",&hz1_GEM);
  intree->SetBranchAddress("hx2_GEM",&hx2_GEM);
  intree->SetBranchAddress("hy2_GEM",&hy2_GEM);
  intree->SetBranchAddress("hz2_GEM",&hz2_GEM);
  intree->SetBranchAddress("hx1_MWPC",&hx1_MWPC);
  intree->SetBranchAddress("hy1_MWPC",&hy1_MWPC);
  intree->SetBranchAddress("hz1_MWPC",&hz1_MWPC);
  intree->SetBranchAddress("hx2_MWPC",&hx2_MWPC);
  intree->SetBranchAddress("hy2_MWPC",&hy2_MWPC);
  intree->SetBranchAddress("hz2_MWPC",&hz2_MWPC);
  intree->SetBranchAddress("time_P1",&time_P1);
  intree->SetBranchAddress("time_P2",&time_P2);
  const int nEvents = intree->GetEntries();

  // Set up the output file
  TFile *outfile = new TFile("out.root","RECREATE");
  TTree *outtree = new TTree("cbmsim","CBMSIM tree");
  TClonesArray *tofArray = new TClonesArray("BmnTofHit");
  TClonesArray *gemArray = new TClonesArray("BmnGemStripHit");
  TClonesArray *mwpcArray = new TClonesArray("BmnMwpcHit");
  outtree->Branch("BmnTof400Hit",&tofArray);
  outtree->Branch("BmnGemStripHit",&gemArray);
  outtree->Branch("BmnMwpcHit",&mwpcArray);
  TClonesArray &tRef=*tofArray;
  TClonesArray &gRef=*gemArray;
  TClonesArray &mRef=*mwpcArray;
  
  // Loop over events
  for (int i=0 ; i<nEvents ; i++)
    {
      if (i%10000==0)
      //if (i%1==0)
	cerr << "Working on event " << i << "\n";

      // Load the input event
      intree->GetEvent(i);

      // Clear the last output event
      tofArray->Clear();
      gemArray->Clear();
      mwpcArray->Clear();

      TVector3 tl(100.*hx1_tof400,100.*hy1_tof400,100.*hz1_tof400);
      TVector3 tr(100.*hx2_tof400,100.*hy2_tof400,100.*hz2_tof400);
      TVector3 gl(100.*hx1_GEM,100.*hy1_GEM,100.*hz1_GEM);
      TVector3 gr(100.*hx2_GEM,100.*hy2_GEM,100.*hz2_GEM);
      TVector3 m1(100.*hx1_MWPC,100.*hy1_MWPC,100.*hz1_MWPC);
      TVector3 m2(100.*hx2_MWPC,100.*hy2_MWPC,100.*hz2_MWPC);

     
      // Create instances of the output classes

      BmnTofHit * lTofHit = new (tRef[0]) BmnTofHit(0,tl,TVector3(0.,0.,0.),0);
      BmnTofHit * rTofHit = new (tRef[1]) BmnTofHit(1,tr,TVector3(0.,0.,0.),1);

      BmnGemStripHit * lGemHit = new(gRef[0]) BmnGemStripHit(0,gl,TVector3(0.,0.,0.),0);
      BmnGemStripHit * rGemHit = new(gRef[1]) BmnGemStripHit(1,gr,TVector3(0.,0.,0.),1);

      BmnMwpcHit * lMwpcHit = new(mRef[0]) BmnMwpcHit(0,m1,TVector3(0.,0.,0.),0);
      BmnMwpcHit * rMwpcHit = new(mRef[1]) BmnMwpcHit(1,m2,TVector3(0.,0.,0.),1);

      // Write to the tree
      outtree->Fill();
    }

  infile->Close();
  outtree->Write();
  outfile->Close();

  cout << "All done.\n";
}
