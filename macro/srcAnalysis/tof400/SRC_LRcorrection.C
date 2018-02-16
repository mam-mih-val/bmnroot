/*
Written by Efrain Segarra
Adapted version of ToF400 LR correction with some MASSIVE fixes
When on real data, need to edit the minimum level based on grass of histogram
 */

void SRC_LRcorrection(TString file = "")
{

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries

    TString inName;
    inName = file;
    cout << "Open file " << inName << endl << endl;

    TFile *FileIn = new TFile(inName.Data(), "READ");
    if (!FileIn)
    {
        cout << "File " << inName.Data() << " is not found" << endl;
        break;
    }

    TString Name1, Name2;
    TH1D ***hDtLR = new TH1D**[10];
    TH1D ***hTimeLR = new TH1D***[10];
    double minimum[10][48] = 0.;
    FileIn->cd("Tof400");
    for (Int_t Plane = 0; Plane < 10; Plane++)
    {
        hDtLR[Plane] = new TH1D*[48];
        hTimeLR[Plane] = new TH1D*[48];
	Name1.Clear();
        Name1 = Form("Plane_%d", Plane);
	FileIn->cd(Form("Tof400/Plane_%d/Detector", Plane));
        for (Int_t Str = 0; Str < 48; Str++)
        {	
            // For each strip, get the output of run_reco_src.C and grab the
	    // output L-R histograms
	    Name2.Clear();
            Name2 = Form("Hist_dtLR_%s_str%d", Name1.Data(), Str);
            hDtLR[Plane][Str] = (TH1D*) gDirectory->Get(Name2.Data());
            Int_t nBins = hDtLR[Plane][Str]->GetNbinsX();
            
	    // Create our own histograms that we will fill based on if there are
	    // enough events above some minimum threshold (which depends on integral events
	    // in the strip
	    Name2 = Form("Hist_dTimeLR_%s_str%d", Name1.Data(), Str);
	    hTimeLR[Plane][Str] = new TH1D(Name2.Data(), Name2.Data(), 1024, -12., 12.);

	    // The minimum is just the total number of entries in the strip divided by 200 (chosen arbitrarily...)
            minimum[Plane][Str] = hDtLR[Plane][Str]->GetEntries() / 200.;	                
	    
	    for (Int_t i = 0; i < nBins; i++)
            {
		if (hDtLR[Plane][Str]->GetBinContent(i) > minimum[Plane][Str]) hTimeLR[Plane][Str]->SetBinContent(i, minimum[Plane][Str]);
                if (hDtLR[Plane][Str]->GetBinContent(i) < minimum[Plane][Str]) hTimeLR[Plane][Str]->SetBinContent(i, 0);
            }
        }
    }

    TFile *fileout = new TFile("LR_CorrectionHistograms.root", "RECREATE");

    ofstream f_call;
    TString NameCallFile = "TOF400_LRCorr_Period_SRC.dat";
    cout << "Write call to " << NameCallFile.Data() << endl;
    f_call.open(NameCallFile.Data());
    f_call << "Plane\tStrip\tEfrain_Shift\tOld_Shift" << endl << "=====================================================" << endl;

    TDirectory * DirPlane[10];
    
    // Now for each strip distribution, we need to get the left edge and the right edge of the hTimeLR in order to make the
    // middle of that width at 0
    TF1 *MyGaus = new TF1("MyGaus", "gaus", -24., 24.);
    for (Int_t Plane = 0; Plane < 10; Plane++)
    {
        Name1.Clear();
        Name1 = Form("Plane%d", Plane);
        DirPlane[Plane] = fileout->mkdir(Name1.Data());

        DirPlane[Plane] -> cd();
	
        for (Int_t Str = 0; Str < 48; Str++)
        {
	    double mean = 0.;
	    Int_t binL = hTimeLR[Plane][Str]->FindFirstBinAbove(0.);
	    Int_t binR = hTimeLR[Plane][Str]->FindLastBinAbove(0.);

	    Double_t tL = hTimeLR[Plane][Str]->GetXaxis()->GetBinCenter( binL );
	    Double_t tR = hTimeLR[Plane][Str]->GetXaxis()->GetBinCenter( binR );
		
	    // CONVENTION that I will SUBTRACT the mean LRCorrection later on. 
	    // So for distribution that is shifted left of 0, I want to subtract a negative number (i.e. shift it right)
	    // and for distribution that is shifted right of 0, i want to subtract a postiive number (i.e. shift it left)
	    if( TMath::Abs(tL) != TMath::Abs(tR) ){
		mean = 0.5*(TMath::Abs(tR) - TMath::Abs(tL));
	    }
	    else if( TMath::Abs(tL) == TMath::Abs(tR) ){
		 mean = 0.;
	    }
	    
            if (Str > 0 && Str < 47)
            {
                f_call << Plane << "\t" << Str << "\t" << mean << "\t"  << hTimeLR[Plane][Str]->GetMean() <<endl;
            } else f_call << Plane << "\t" << Str << "\t" << -11.9 << "\t" << -11.9 << endl;
	    
	    hTimeLR[Plane][Str]->Write();
        }


    }

    f_call.close();

    fileout->cd();
    fileout->Close();
    FileIn->Close();


}

void TestFitGaus()
{
    TH1I *hTest = new TH1I("hTest", "hTest", 400, 0, 10);
    for (Int_t i = 165; i <= 235; i++)
        hTest -> Fill(i * 0.025, 10);
    hTest->Fit("gaus");
    TCanvas *c1 = new TCanvas("c1", "c1", 50, 50, 550, 550);
    hTest->Draw();
}
