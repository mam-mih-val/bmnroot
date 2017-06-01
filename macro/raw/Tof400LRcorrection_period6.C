/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
const Int_t minimum[10] = {70, 100, 100, 12, 26, 130, 180, 200, 180, 35};

void Tof400LRcorrection_period6(TString file = "")
{

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries

    TH1I *hMean = new TH1I("hMean", "hMean", 2048, -12, 12);
    TH1I *hdMean = new TH1I("hdMean", "hdMean", 2048, -6., 6.);
    TH2I *hdMean_vs_Strip = new TH2I("hdMean_vs_Strip", "hdMean_vs_Strip", 460, 0, 460, 400, -2., 2.);

    TString inName;
    inName = file;
    cout << "Open file " << inName << endl << endl;

    TFile *FileIn = new TFile(inName.Data(), "RAED");
    if (!FileIn)
    {
        cout << "File " << inName.Data() << " is not found" << endl;
        break;
    }

    TString Name1, Name2;
    TH1I ***hDtLR = new TH1I**[10];
    TH1I ***hDifDtLR = new TH1I**[10];

    FileIn->cd("ToF");
    for (Int_t Plane = 0; Plane < 10; Plane++)
    {
        hDtLR[Plane] = new TH1I*[48];
        hDifDtLR[Plane] = new TH1I*[48];
        Name1.Clear();
        Name1 = Form("Plane%d", Plane);
        FileIn->cd(Form("ToF/Plane%d/Detector", Plane));
        gDirectory->pwd();
        //gDirectory->ls();
        for (Int_t Str = 0; Str < 48; Str++)
        {
            Name2.Clear();
            Name2 = Form("Hist_dtLR_%s_str%d", Name1.Data(), Str);
            //hDtLR[Plane][Str] = new TH1I(Name2.Data(), Name2.Data(), 1024, 0, 48);
            //hDtLR[Plane][Str] = (TH1I*) FileIn->GetObjectChecked(Name2.Data(), "TH1I");
            hDtLR[Plane][Str] = (TH1I*) gDirectory->Get(Name2.Data());
            Int_t nBins = hDtLR[Plane][Str]->GetNbinsX();

            Name2 = Form("Hist_DiffdtLR_%s_str%d", Name1.Data(), Str);
            hDifDtLR[Plane][Str] = new TH1I(Name2.Data(), Name2.Data(), 1024, -12., 12.);

            for (Int_t i = 2; i < nBins; i++)
            {
                if (hDtLR[Plane][Str]->GetBinContent(i) > minimum[Plane]) hDtLR[Plane][Str]->SetBinContent(i, minimum[Plane]);
                if (hDtLR[Plane][Str]->GetBinContent(i) < minimum[Plane]) hDtLR[Plane][Str]->SetBinContent(i, 0);
                hDifDtLR[Plane][Str]->SetBinContent(i, hDtLR[Plane][Str]->GetBinContent(i) - hDtLR[Plane][Str]->GetBinContent(i - 1));
            }
        }
    }

    TString outName = file;
    Int_t Point = outName.First('.');
    outName.Replace(Point, 15, "_LRcorr.root");
    TFile *fileout = new TFile(outName.Data(), "RECREATE");

    ofstream f_call;
    TString NameCallFile = "Tof400LRcorr.dat";
    //TString NameCallFile = file;
    //Point = NameCallFile.First('.');
    //NameCallFile.Replace(Point, 15, "_LRcorr.dat");
    cout << "Write call to " << NameCallFile.Data() << endl;
    f_call.open(NameCallFile.Data());
    f_call << "Plane\tStrip\tMeanFit\tMean" << endl << "==========================" << endl;

    TDirectory * DirPlane[10];
    TDirectory * Dir2Plane[10];
    TF1 *MyGaus = new TF1("MyGaus", "gaus", -24., 24.);
    for (Int_t Plane = 0; Plane < 10; Plane++)
    {
        Name1.Clear();
        Name1 = Form("Plane%d", Plane);
        DirPlane[Plane] = fileout->mkdir(Name1.Data());

        Dir2Plane[Plane] = DirPlane[Plane]->mkdir("Diff");
        Dir2Plane[Plane]->cd();
        for (Int_t Str = 0; Str < 48; Str++)
            hDifDtLR[Plane][Str]->Write();

        DirPlane[Plane] -> cd();
        for (Int_t Str = 0; Str < 48; Str++)
        {
            Double_t left = hDtLR[Plane][Str]->GetMaximumBin() * hDtLR[Plane][Str]->GetBinWidth(1) + hDtLR[Plane][Str]->GetXaxis()->GetBinLowEdge(1);
            MyGaus->SetParameters(minimum[Plane] * 1.2, left + 0.9, 5.0);
            MyGaus->SetParLimits(2, 0.1, 7);
            MyGaus->SetParLimits(1, left + 0.5, left + 1.4);
            if (Str > 0 && Str < 47)
            {
                hDtLR[Plane][Str]->Fit("MyGaus", "QB", "");
                f_call << Plane << "\t" << Str << "\t" << MyGaus->GetParameter(1) << "\t" << hDtLR[Plane][Str]->GetMean() << endl;
                hMean -> Fill(MyGaus->GetParameter(1));
                hdMean->Fill(MyGaus->GetParameter(1) - hDtLR[Plane][Str]->GetMean());
                hdMean_vs_Strip->Fill(Plane * 48 + Str, MyGaus->GetParameter(1) - hDtLR[Plane][Str]->GetMean());
            } else f_call << Plane << "\t" << Str << "\t" << 0 << "\t" << left << endl;
            hDtLR[Plane][Str]->Write();
            //cout << " GetMaximumBin " << left << endl;
            //getchar();
        }


    }

    f_call.close();

    fileout->cd();
    hMean->Write();
    hdMean->Write();
    hdMean_vs_Strip->Write();
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
