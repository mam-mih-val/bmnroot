#include "BmnLambdaAnalRun7.h"

void BmnLambdaAnalRun7::targetListsByCutsEstablished(set <TString>& commonCuts, set <TString>& uniqueCuts) {

    for (auto it0 = fTargCutsMap.begin(); it0 != fTargCutsMap.end(); it0++) {
        vector <TString> targets;
        targets.push_back(it0->first);

        BmnParticlePairCut* cut0 = it0->second;

        for (auto it1 = fTargCutsMap.begin(); it1 != fTargCutsMap.end(); it1++) {

            if (it0 == it1)
                continue;

            TString tar1 = it1->first;
            BmnParticlePairCut* cut1 = it1->second;

            if (*cut0 == *cut1)
                targets.push_back(tar1);
        }

        if (targets.size() > 1) {
            for (auto it2 : targets)
                commonCuts.insert(it2);
        } else
            uniqueCuts.insert(targets.at(0));
    }
}

void BmnLambdaAnalRun7::doAllTargetsAnal() {
    // Looking for data related to trigg. eff ...
    if (fTrigEffData.size())
        ReadData();

    set <TString> commonCuts;
    set <TString> uniqueCuts;

    targetListsByCutsEstablished(commonCuts, uniqueCuts);

    cout << "Targets with the same cuts ..." << endl;
    for (auto it : commonCuts)
        cout << it << " ";
    cout << endl;

    cout << "Targets with unique cuts ..." << endl;
    for (auto it : uniqueCuts)
        cout << it << " ";
    cout << endl;

    TH1F* hSpectraAll = new TH1F("massSpectrumAll", "massSpectrumAll", 75, xLow, xUp);

    // Getting spectrum for each target ...
    TCanvas* d = new TCanvas("d", "d", 1200, 1200);
    d->Divide(TMath::Sqrt(fTargCutsMap.size()), TMath::Sqrt(fTargCutsMap.size()));

    for (auto it0 = fTargCutsMap.begin(); it0 != fTargCutsMap.end(); it0++) {
        d->cd(1 + distance(fTargCutsMap.begin(), it0));

        fTarget.clear();

        fTarget.push_back(it0->first);

        hSpectrumImproved = new TH1F(Form("massSpectrum_%s", fTarget.at(0).Data()), "massSpectrum", 75, xLow, xUp);
        hSpectrumImproved->SetTitle(Form("Invariant mass: #Lambda^{0} #rightarrow #pi^{-} + p (%s)", fTarget.at(0).Data()));

        vector <TString> fList = createFilelist();
        cout << "nFiles# " << fList.size() << endl;

        // Getting cut values to be used ...
        BmnParticlePairCut* cut = fTargCutsMap.find(fTarget.at(0))->second;

        for (auto it = fList.begin(); it != fList.end(); it++) {
            if (fNFiles && distance(fList.begin(), it) == fNFiles)
                break;

            ReadFile(*it, cut, fTrigEffs);
        }

        hSpectraAll->Add(hSpectrumImproved);
        // hSpectraAll->Sumw2();

        // Fitting obtained spectrum ...
        Double_t mean = -1., sigma = -1.;
        pair <Double_t, Double_t> T = make_pair(-1., -1.);
        pair <Double_t, Double_t> B = make_pair(-1., -1.);

        fitSpectrum(hSpectrumImproved, mean, sigma, T, B);

        d->cd(1 + distance(fTargCutsMap.begin(), it0));
        hSpectrumImproved->Draw("PE1X0");
        DrawH(hSpectrumImproved, mean, sigma, T, B);
    }

    TCanvas* c = new TCanvas("c", "c", 800, 600);
    c->Divide(1, 1);

    // Fitting obtained spectrum ...
    Double_t mean = -1., sigma = -1.;
    pair <Double_t, Double_t> T = make_pair(-1., -1.);
    pair <Double_t, Double_t> B = make_pair(-1., -1.);

    fitSpectrum(hSpectraAll, mean, sigma, T, B);

    gStyle->SetOptStat(0);

    c->cd(1);
    hSpectraAll->Draw("PE1X0");

    TString targs = " ";
    for (auto it : fTargCutsMap)
        targs += it.first + " ";

    hSpectraAll->SetTitle(Form("Invariant mass: #Lambda^{0} #rightarrow #pi^{-} + p (%s)", targs.Data()));
    DrawH(hSpectraAll, mean, sigma, T, B, commonCuts.size());

    c->SaveAs("allTargets.pdf");
    d->SaveAs("allTargets_sepView.pdf");

    delete c;
    delete d;
}

void BmnLambdaAnalRun7::doTargetAnal(TString target) {
    fTarget.clear();
    fTarget.push_back(target);

    doTargetAnal();
}

void BmnLambdaAnalRun7::doTargetAnal(vector <TString> targets) {
    fTarget.clear();

    for (auto tar : targets)
        fTarget.push_back(tar);

    doTargetAnal();
}

void BmnLambdaAnalRun7::doTargetAnal() {
    isPtY = kTRUE;

    // Looking for data related to trigg. eff ...
    if (fTrigEffData.size())
        ReadData();

    hSpectraPt = new TH1F*[nPtBins];
    hSpectraY = new TH1F*[nYBins];

    for (Int_t iPt = 0; iPt < nPtBins; iPt++) {
        Double_t pt0 = fPtBinMap.find(iPt)->second.first;
        Double_t pt1 = fPtBinMap.find(iPt)->second.second;

        hSpectraPt[iPt] = new TH1F(Form("P_{t} = [%G, %G] GeV/c", pt0, pt1),
                Form("P_{t} = [%G, %G] GeV/c", pt0, pt1), 75, xLow, xUp);
    }

    for (Int_t iY = 0; iY < nYBins; iY++) {
        Double_t y0 = fYBinMap.find(iY)->second.first;
        Double_t y1 = fYBinMap.find(iY)->second.second;

        hSpectraY[iY] = new TH1F(Form("Y = [%G, %G]", y0, y1),
                Form("Y = [%G, %G]", y0, y1), 75, xLow, xUp);
    }

    vector <TString> fList = createFilelist();
    cout << "nFiles# " << fList.size() << endl;

    BmnParticlePairCut* cut = fTargCutsMap.find(fTarget.at(0))->second;

    for (auto it = fList.begin(); it != fList.end(); it++) {
        if (fNFiles && distance(fList.begin(), it) == fNFiles)
            break;

        ReadFile(*it, cut, fTrigEffs);
    }

    //    if (nPtBins != nYBins)
    //        return;

    gStyle->SetOptStat(0);
    gStyle->SetTitleFontSize(0.1);
    gStyle->SetTitleY(1.0);
    gStyle->SetPaintTextFormat("4.3f");

    Int_t ww = 1200;
    Int_t wh = (!(nPtBins * nYBins)) ? 1200 : 600;

    TCanvas* c = new TCanvas("c", "c", ww, wh);

    Int_t nDivs = (nPtBins) ? nPtBins : (nYBins) ? nYBins : 0;
    c->Divide(nDivs, (nPtBins * nYBins) ? 2 : 1);

    Int_t iPad = 1;

    for (Int_t iPt = 0; iPt < nPtBins; iPt++) {
        c->cd(iPad);
        hSpectraPt[iPt]->Draw("PE1X0");

        Double_t mean = -1., sigma = -1.;
        pair <Double_t, Double_t> T = make_pair(-1., -1.);
        pair <Double_t, Double_t> B = make_pair(-1., -1.);

        TH1F* h = (TH1F*) hSpectraPt[iPt]->Clone();

        fitSpectrum(h, mean, sigma, T, B);

        delete hSpectraPt[iPt]->GetListOfFunctions()->FindObject("f");
        delete hSpectraPt[iPt]->GetListOfFunctions()->FindObject("f2");

        TF1* f1 = (TF1*) h->GetListOfFunctions()->FindObject("f1");
        // f1->SetLineColor(kRed);
        Double_t dS = TMath::Sqrt(T.second - B.second);
        Double_t dB = deltaB(f1, fitSpectrum(hSpectraPt[iPt]));
        TF1* f11 = (TF1*) hSpectraPt[iPt]->GetListOfFunctions()->FindObject("f11");
        f11->SetLineColor(kRed);

        //cout << dS << " -- nom -- " << dB << endl;
        Double_t errTotRelNom = TMath::Sqrt((dS / (T.second - B.second)) * (dS / (T.second - B.second))
                + (dB / B.second) * (dB / B.second));

        TLatex *t1 = new TLatex();
        t1->SetNDC(); //we want to draw this object in the Normalized system [0,1]
        t1->SetTextSize(.05);
        t1->DrawLatex(.25, .85, Form("Numb. of #Lambda^{0}  = (%d #pm %d) ", Int_t(T.second - B.second), Int_t(errTotRelNom * (T.second - B.second))));
        
        fSdeltaS_PtBinMap[iPt] = make_pair(Int_t(T.second - B.second), Int_t(errTotRelNom * (T.second - B.second)));

        iPad++;
    }

    iPad = nPtBins + 1;

    for (Int_t iY = 0; iY < nYBins; iY++) {
        c->cd(iPad);
        hSpectraY[iY]->Draw("PE1X0");

        Double_t mean = -1., sigma = -1.;
        pair <Double_t, Double_t> T = make_pair(-1., -1.);
        pair <Double_t, Double_t> B = make_pair(-1., -1.);

        TH1F* h = (TH1F*) hSpectraY[iY]->Clone();

        fitSpectrum(h, mean, sigma, T, B);

        delete hSpectraY[iY]->GetListOfFunctions()->FindObject("f");
        delete hSpectraY[iY]->GetListOfFunctions()->FindObject("f2");

        TF1* f1 = (TF1*) h->GetListOfFunctions()->FindObject("f1");
        // f1->SetLineColor(kRed);
        Double_t dS = TMath::Sqrt(T.second - B.second);
        Double_t dB = deltaB(f1, fitSpectrum(hSpectraY[iY]));
        TF1* f11 = (TF1*) hSpectraY[iY]->GetListOfFunctions()->FindObject("f11");
        f11->SetLineColor(kRed);

        //cout << dS << " -- nom -- " << dB << endl;
        Double_t errTotRelNom = TMath::Sqrt((dS / (T.second - B.second)) * (dS / (T.second - B.second))
                + (dB / B.second) * (dB / B.second));

        TLatex *t1 = new TLatex();
        t1->SetNDC(); //we want to draw this object in the Normalized system [0,1]
        t1->SetTextSize(.05);
        t1->DrawLatex(.25, .85, Form("Numb. of #Lambda^{0}  = (%d #pm %d) ", Int_t(T.second - B.second), Int_t(errTotRelNom * (T.second - B.second))));
        
        fSdeltaS_YBinMap[iY] = make_pair(Int_t(T.second - B.second), Int_t(errTotRelNom * (T.second - B.second)));

        iPad++;
    }

    vector <TH1F**> histos;
    if (nPtBins)
        histos.push_back(hSpectraPt);
    if (nYBins)
        histos.push_back(hSpectraY);

    for (Int_t iHisto = 0; iHisto < histos.size(); iHisto++) {
        TH1F** h = histos.at(iHisto);
        for (Int_t iPt = 0; iPt < nPtBins; iPt++) {

            h[iPt]->SetMarkerStyle(20);
            h[iPt]->SetMarkerColor(kSpring - 6);
            h[iPt]->SetLineColor(kSpring - 6);
            h[iPt]->SetLineWidth(1);

            //            hSpectraPtY[iPt]->GetXaxis()->SetTitle("M_{(p + #pi^{-})}, GeV/c^{2}");
            h[iPt]->GetXaxis()->SetTitle("");
            h[iPt]->GetYaxis()->SetTitle("");
            h[iPt]->GetXaxis()->SetNdivisions(5);
            h[iPt]->GetYaxis()->SetNdivisions(5);
            h[iPt]->GetYaxis()->SetLabelOffset(0.01);
            h[iPt]->GetXaxis()->SetLabelSize(0.06);
            h[iPt]->GetYaxis()->SetLabelSize(0.06);
            h[iPt]->GetXaxis()->SetTitleSize(0.05);
            h[iPt]->GetYaxis()->SetTitleSize(0.05);
            h[iPt]->GetXaxis()->SetTitleOffset(0.85);
            h[iPt]->GetYaxis()->SetTitleOffset(1.5);
            h[iPt]->GetYaxis()->CenterTitle();
        }
    }

    TString cName = "";
    if (fTarget.size() < 2)
        cName = fTarget.at(0);
    else
        for (auto tar : fTarget)
            cName += tar;

    
    TString prefName = "";
    
    for (auto pt : fPtBinMap) {
        TString tmp = TString::Format("_minBinPt%G_maxBinPt%G", pt.second.first, pt.second.second); 
        prefName += tmp;
    }
    
     for (auto y : fYBinMap) {
        TString tmp = TString::Format("_minBinY%G_maxBinY%G", y.second.first, y.second.second); 
        prefName += tmp;
    }
    
    cout << prefName << endl;
    
    c->SaveAs(Form("%s_PtY%s.pdf", cName.Data(), prefName.Data()));
}

void BmnLambdaAnalRun7::DrawH(TH1F* h, Double_t mean, Double_t sigma, pair <Double_t, Double_t> T, pair <Double_t, Double_t> B, Int_t si) {
    h->SetMarkerStyle(20);
    h->SetMarkerColor(kSpring - 6);
    h->SetLineColor(kSpring - 6);
    h->SetLineWidth(1);

    h->GetXaxis()->SetTitle("M_{(p + #pi^{-})}, GeV/c^{2}");
    h->GetYaxis()->SetTitle("");
    h->GetXaxis()->SetNdivisions(5);
    h->GetYaxis()->SetNdivisions(5);
    h->GetYaxis()->SetLabelOffset(0.01);
    h->GetXaxis()->SetLabelSize(0.04);
    h->GetYaxis()->SetLabelSize(0.04);
    h->GetXaxis()->SetTitleSize(0.05);
    h->GetYaxis()->SetTitleSize(0.05);
    h->GetXaxis()->SetTitleOffset(0.85);
    h->GetYaxis()->SetTitleOffset(1.5);
    h->GetYaxis()->CenterTitle();

    TLegend *legend = new TLegend(0.3, 0.15, 0.41, 0.35);
    legend->SetTextFont(72);
    legend->SetTextSize(0.04);

    legend->AddEntry("", Form("Mass = %.4f", mean), 0);
    legend->AddEntry("", Form("Sigma = %.4f", sigma), 0);

    legend->AddEntry("", Form("Numb. of #Lambda^{0} = %d, (%d)", Int_t(T.first - B.first), Int_t(T.second - B.second)), 0);
    legend->AddEntry("", Form("S / B = %4.3f, (%4.3f)", (T.first - B.first) / B.first, (T.second - B.second) / B.second), 0);

    legend->SetTextSize(0.05);
    legend->SetLineColor(0);
    legend->Draw();

    if (si) {
        TLatex *t1 = new TLatex();
        t1->SetNDC(); //we want to draw this object in the Normalized system [0,1]
        t1->SetTextSize(.05);

        BmnParticlePairCut* cut = fTargCutsMap.find(fTarget.at(0))->second;

        t1->DrawLatex(.5, .65, Form("DCA0 = %G cm", cut->dca0()));
        t1->DrawLatex(.5, .7, Form("DCA1 = %G cm", cut->dca1()));
        t1->DrawLatex(.5, .75, Form("DCA2 = %G cm", cut->dca2()));
        t1->DrawLatex(.5, .8, Form("DCA12 = %G cm", cut->dca12()));
        t1->DrawLatex(.5, .85, Form("PATH = %G cm", cut->path()));
    }
}

