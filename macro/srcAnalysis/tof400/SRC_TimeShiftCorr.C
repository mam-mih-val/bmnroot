/*
Written by Efrain Segarra
When on real data, need to edit fitting procedure properly
 */

void SRC_TimeShiftCorr(TString file = "")
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
    TH1D ***hDt = new TH1D**[10];
    TH1D ***hTimeLR = new TH1D***[10];
    TF1  ***fits = new TF1***[10];
    double maximum[10][48] = 0.;
    double gammaTime[10][48] = 0.;
    double expectedTime[10][48];
    FileIn->cd("Tof400");
    for (Int_t Plane = 0; Plane < 10; Plane++)
    {
        hDt[Plane] = new TH1D*[48];
        hTimeLR[Plane] = new TH1D*[48];
	fits[Plane] = new TF1*[48];
	Name1.Clear();
        Name1 = Form("Plane_%d", Plane);
	FileIn->cd(Form("Tof400/Plane_%d/ToF", Plane));
        for (Int_t Str = 0; Str < 48; Str++)
        {	
	    expectedTime[Plane][Str] = 21.782;
            // For each strip, get the output of run_reco_src.C and grab the
	    // output L-R histograms
	    Name2.Clear();
            Name2 = Form("Hist_Dt_%s_str%d", Name1.Data(), Str);
            hDt[Plane][Str] = (TH1D*) gDirectory->Get(Name2.Data());
            Int_t nBins = hDt[Plane][Str]->GetNbinsX();
            
	    maximum[Plane][Str] = hDt[Plane][Str]->GetXaxis()->GetBinCenter(hDt[Plane][Str]->GetMaximumBin() + 5 );
	    double startL = hDt[Plane][Str]->GetXaxis()->GetBinCenter(1);
		
	    fits[Plane][Str] = new TF1("","gaus",startL,maximum[Plane][Str]);
	    hDt[Plane][Str]->Fit(fits[Plane][Str],"QR");
	    
	    double foundTime = fits[Plane][Str]->GetParameter(1) - fits[Plane][Str]->GetParameter(2);
            //cout << foundTime << " " << expectedTime[Plane][Str] << "\n";
	    gammaTime[Plane][Str] = expectedTime[Plane][Str] - foundTime;
	}
    }

    TFile *fileout = new TFile("TimeShiftHistograms.root", "RECREATE");

    ofstream f_call;
    TString NameCallFile = "TOF400_TimeShiftCorr_Period_SRC.dat";
    f_call.open(NameCallFile.Data());
    f_call << "Plane\tStrip\tEfrain_Correction" << endl << "=====================================================" << endl;

    TDirectory * DirPlane[10];
    for (Int_t Plane = 0; Plane < 10; Plane++)
    {
        Name1.Clear();
        Name1 = Form("Plane%d", Plane);
        DirPlane[Plane] = fileout->mkdir(Name1.Data());

        DirPlane[Plane] -> cd();
	
        for (Int_t Str = 0; Str < 48; Str++){
        	if( Str > 0 && Str < 48)
		{
	   	f_call <<  Plane << "\t" << Str << "\t" << gammaTime[Plane][Str] << endl;
	    	hDt[Plane][Str]->Write();
        	}
		else f_call << Plane << "\t" << Str << "\t" << -11.9 << endl;
	}

    }

    f_call.close();

    fileout->cd();
    fileout->Close();
    FileIn->Close();


}

