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

    if (!(fMcList.size() * fDataList.size()))
        doTargetAnal();
    else
        doTargetAnalPath();
}

void BmnLambdaAnalRun7::doTargetAnalPath() {
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

        hSpectraY[iY] = new TH1F(Form("Y = [%G, %G]", y0, y1), Form("Y = [%G, %G]", y0, y1), 75, xLow, xUp);
    }

    BmnParticlePairCut* cut = fTargCutsMap.find(fTarget.at(0))->second;

    vector <vector < TString >> lists
    {
        fMcList, fDataList
    };

    map <Int_t, vector < TH1F >> pathBinsPt[2]; // path bin [mc or data] --> vector of *hSpectraPt[iBin]
    map <Int_t, vector < TH1F >> pathBinsY[2]; // path bin [mc or data]--> vector of *hSpectraY[iBin]

    // Loop over lists (mc, data)
    for (Int_t iList = 0; iList < lists.size(); iList++) {         

        // Loop over each path bin ...
        for (auto path = fPathBins.begin(); path != fPathBins.end(); path++) {
            // Resetting hSpectraPt and hSpectraY ...
            for (Int_t iPt = 0; iPt < nPtBins; iPt++)
                hSpectraPt[iPt]->Reset();

            for (Int_t iY = 0; iY < nYBins; iY++)
                hSpectraY[iY]->Reset();

            // Loop over each list (dst files)
            for (auto dst = lists.at(iList).begin(); dst != lists.at(iList).end(); dst++) {
                if (fNFiles && distance(lists.at(iList).begin(), dst) == fNFiles)
                    break;

                ReadFile(*dst, cut, fTrigEffs, path->second.first, path->second.second);
            }

            vector <TH1F> tmp;

            for (Int_t iPt = 0; iPt < nPtBins; iPt++)
                tmp.push_back(*hSpectraPt[iPt]);
            pathBinsPt[iList][distance(fPathBins.begin(), path)] = tmp;

            tmp.clear();

            for (Int_t iY = 0; iY < nYBins; iY++)
                tmp.push_back(*hSpectraY[iY]);
            pathBinsY[iList][distance(fPathBins.begin(), path)] = tmp;
        }
    }

    vector <SdeltaS> data;

    TH1F* h = nullptr;
    Double_t mean = -1., sigma = -1.;
    pair <Double_t, Double_t> T = make_pair(-1., -1.);
    pair <Double_t, Double_t> B = make_pair(-1., -1.);

    for (Int_t iList = 0; iList < lists.size(); iList++) {
        for (auto pathBin : pathBinsPt[iList]) {
            Int_t bin = pathBin.first;
            vector <TH1F> vec = pathBin.second;

            for (Int_t iPt = 0; iPt < nPtBins; iPt++) {
                TH1F* hPt = &vec.at(iPt);

                h = (TH1F*) hPt->Clone();

                fitSpectrum(h, mean, sigma, T, B);

                delete hPt->GetListOfFunctions()->FindObject("f");
                delete hPt->GetListOfFunctions()->FindObject("f2");

                TF1* f1 = (TF1*) h->GetListOfFunctions()->FindObject("f1");

                Double_t dS = TMath::Sqrt(T.second - B.second);
                Double_t dB = deltaB(f1, fitSpectrum(hPt));

                TF1* f11 = (TF1*) hPt->GetListOfFunctions()->FindObject("f11");

                Double_t errTotRelNom = TMath::Sqrt((dS / (T.second - B.second)) * (dS / (T.second - B.second))
                        + (dB / B.second) * (dB / B.second));

                SdeltaS ss;
                ss.dSet = (iList) ? "DATA" : "MC";
                ss.pathBin = bin;
                ss.ptBin = iPt;
                ss.s = Int_t(T.second - B.second);
                ss.ds = Int_t(errTotRelNom * (T.second - B.second));

                data.emplace_back(std::move(ss));
            }
        }

        for (auto pathBin : pathBinsY[iList]) {
            Int_t bin = pathBin.first;
            vector <TH1F> vec = pathBin.second;
            for (Int_t iY = 0; iY < nYBins; iY++) {
                TH1F* hY = &vec.at(iY);

                h = (TH1F*) hY->Clone();

                fitSpectrum(h, mean, sigma, T, B);

                delete hY->GetListOfFunctions()->FindObject("f");
                delete hY->GetListOfFunctions()->FindObject("f2");

                TF1* f1 = (TF1*) h->GetListOfFunctions()->FindObject("f1");

                Double_t dS = TMath::Sqrt(T.second - B.second);
                Double_t dB = deltaB(f1, fitSpectrum(hY));

                TF1* f11 = (TF1*) hY->GetListOfFunctions()->FindObject("f11");

                Double_t errTotRelNom = TMath::Sqrt((dS / (T.second - B.second)) * (dS / (T.second - B.second))
                        + (dB / B.second) * (dB / B.second));

                SdeltaS ss;
                ss.dSet = (iList) ? "DATA" : "MC";
                ss.pathBin = bin;
                ss.yBin = iY;
                ss.s = Int_t(T.second - B.second);
                ss.ds = Int_t(errTotRelNom * (T.second - B.second));

                data.emplace_back(std::move(ss));
            }
        }
    }

    // Drawing obtained histos ...
    const Int_t nSets = 2;

    TH1F * pathPt[nSets][fPtBinMap.size()];
    for (Int_t iSet = 0; iSet < nSets; iSet++)
        for (Int_t iPt = 0; iPt < nPtBins; iPt++) {
            Double_t min = fPtBinMap.find(iPt)->second.first;
            Double_t max = fPtBinMap.find(iPt)->second.second;

            Int_t nBins = fPathBins.size();
            Double_t xMin = fPathBins.begin()->second.first;
            Double_t xMax = fPathBins.rbegin()->second.second;

            pathPt[iSet][iPt] = new TH1F(Form("%d, %G < P_{t} < %G [GeV/c]", iSet, min, max), Form("%G < P_{t} < %G [GeV/c]; Path [cm]; N_{#Lambda}", min, max), nBins, xMin, xMax);

            for (auto bin = fPathBins.begin(); bin != fPathBins.end(); bin++) {
                pathPt[iSet][iPt]->GetXaxis()->SetBinLabel(1 + bin->first, Form("(%G - %G)", bin->second.first, bin->second.second));
                pathPt[iSet][iPt]->GetXaxis()->SetLabelSize(0.07);
                pathPt[iSet][iPt]->GetXaxis()->SetTitleSize(0.05);
                pathPt[iSet][iPt]->GetYaxis()->SetLabelSize(0.06);
                pathPt[iSet][iPt]->GetYaxis()->SetTitleSize(0.06);
                pathPt[iSet][iPt]->GetYaxis()->CenterTitle();
            }
        }

    TH1F * pathY[nSets][fPtBinMap.size()];
    for (Int_t iSet = 0; iSet < nSets; iSet++)
        for (Int_t iY = 0; iY < nYBins; iY++) {
            Double_t min = fYBinMap.find(iY)->second.first;
            Double_t max = fYBinMap.find(iY)->second.second;

            Int_t nBins = fPathBins.size();
            Double_t xMin = fPathBins.begin()->second.first;
            Double_t xMax = fPathBins.rbegin()->second.second;

            pathY[iSet][iY] = new TH1F(Form("%d, %G < Y < %G", iSet, min, max), Form("%G < Y < %G; Path [cm]; N_{#Lambda}", min, max), nBins, xMin, xMax);

            for (auto bin = fPathBins.begin(); bin != fPathBins.end(); bin++) {
                pathY[iSet][iY]->GetXaxis()->SetBinLabel(1 + bin->first, Form("(%G - %G)", bin->second.first, bin->second.second));
                pathY[iSet][iY]->GetXaxis()->SetLabelSize(0.07);
                pathY[iSet][iY]->GetXaxis()->SetTitleSize(0.05);
                pathY[iSet][iY]->GetYaxis()->SetLabelSize(0.06);
                pathY[iSet][iY]->GetYaxis()->SetTitleSize(0.06);
                pathY[iSet][iY]->GetYaxis()->CenterTitle();
            }
        }

    // Filling created histograms ...
    for (SdeltaS it : data) {
        TString dSample = it.dSet;

        Int_t pt = it.ptBin;
        Int_t y = it.yBin;
        Int_t path = it.pathBin;

        Int_t s = it.s;
        Int_t ds = it.ds;

        if (pt > -1) {
            pathPt[dSample.Contains("MC") ? 0 : 1][pt]->SetBinContent(path + 1, s);
            pathPt[dSample.Contains("MC") ? 0 : 1][pt]->SetBinError(path + 1, 0);
        }

        if (y > -1) {
            pathY[dSample.Contains("MC") ? 0 : 1][y]->SetBinContent(path + 1, s);
            pathY[dSample.Contains("MC") ? 0 : 1][y]->SetBinError(path + 1, 0);
        }
    }

    gStyle->SetOptStat(0);
    gStyle->SetTitleFontSize(0.1);
    gStyle->SetTitleY(1.0);
    gStyle->SetPaintTextFormat("4.3f");

    Int_t ww = 800;
    Int_t wh = 800;
    Int_t nDivs = (nPtBins > nYBins) ? nPtBins : nYBins;

    TCanvas* c = new TCanvas("c", "c", nDivs * ww, 2 * wh);
    c->Divide(nDivs, 2);

    Int_t iPad = 1;

    for (Int_t iPt = 0; iPt < nPtBins; iPt++) {
        c->cd(iPad)->SetLeftMargin(.16);
        
        // Normalization procedure (for MC?) ...
        TH1F* hMc = pathPt[0][iPt];
        TH1F* hData = pathPt[1][iPt];
        
        // Bin sum over all MC
        Double_t mcSum = 0.;
        for (Int_t iBin = 1; iBin < hMc->GetNbinsX() + 1; iBin++)
            mcSum += hMc->GetBinContent(iBin); 
        
        // Bin sum over all data
        Double_t dataSum = 0.;
        for (Int_t iBin = 1; iBin < hData->GetNbinsX() + 1; iBin++)
            dataSum += hData->GetBinContent(iBin);
        
        for (Int_t iBin = 1; iBin < pathPt[0][iPt]->GetNbinsX() + 1; iBin++)
            pathPt[0][iPt]->SetBinContent(iBin, (dataSum / mcSum) * pathPt[0][iPt]->GetBinContent(iBin)); // MC


        Double_t max0 = pathPt[0][iPt]->GetMaximum();
        Double_t max1 = pathPt[1][iPt]->GetMaximum();

        pathPt[0][iPt]->GetYaxis()->SetRangeUser(0, 1.3 * ((max0 > max1) ? max0 : max1));
        pathPt[1][iPt]->GetYaxis()->SetRangeUser(0, 1.3 * ((max0 > max1) ? max0 : max1));

        pathPt[0][iPt]->Draw("PE1X0");
        pathPt[0][iPt]->SetLineColor(kBlue);
        pathPt[0][iPt]->SetMarkerColor(kBlue);
        pathPt[0][iPt]->SetMarkerSize(5);
        pathPt[0][iPt]->SetMarkerStyle(20);

        pathPt[1][iPt]->Draw("PE1X0same");
        pathPt[1][iPt]->SetLineColor(kRed);
        pathPt[1][iPt]->SetMarkerColor(kRed);
        pathPt[1][iPt]->SetMarkerSize(5);
        pathPt[1][iPt]->SetMarkerStyle(20);

        iPad++;
    }

    iPad = nDivs + 1;

    for (Int_t iY = 0; iY < nYBins; iY++) {
        c->cd(iPad)->SetLeftMargin(.16);
        
        // Normalization procedure (for MC?) ...
        TH1F* hMc = pathY[0][iY];
        TH1F* hData = pathY[1][iY];
        
        // Bin sum over all MC
        Double_t mcSum = 0.;
        for (Int_t iBin = 1; iBin < hMc->GetNbinsX() + 1; iBin++)
            mcSum += hMc->GetBinContent(iBin); 
        
        // Bin sum over all data
        Double_t dataSum = 0.;
        for (Int_t iBin = 1; iBin < hData->GetNbinsX() + 1; iBin++)
            dataSum += hData->GetBinContent(iBin);
        
        for (Int_t iBin = 1; iBin < pathY[0][iY]->GetNbinsX() + 1; iBin++)
            pathY[0][iY]->SetBinContent(iBin, (dataSum / mcSum) * pathY[0][iY]->GetBinContent(iBin)); // MC

        Double_t max0 = pathY[0][iY]->GetMaximum();
        Double_t max1 = pathY[1][iY]->GetMaximum();

        pathY[0][iY]->GetYaxis()->SetRangeUser(0, 1.3 * ((max0 > max1) ? max0 : max1));
        pathY[1][iY]->GetYaxis()->SetRangeUser(0, 1.3 * ((max0 > max1) ? max0 : max1));

        pathY[0][iY]->Draw("PE1X0");
        pathY[0][iY]->SetLineColor(kBlue);
        pathY[0][iY]->SetMarkerColor(kBlue);
        pathY[0][iY]->SetMarkerSize(5);
        pathY[0][iY]->SetMarkerStyle(20);

        pathY[1][iY]->Draw("PE1X0same");
        pathY[1][iY]->SetLineColor(kRed);
        pathY[1][iY]->SetMarkerColor(kRed);
        pathY[1][iY]->SetMarkerSize(5);
        pathY[1][iY]->SetMarkerStyle(20);

        iPad++;
    }

    c->SaveAs("nLambdaPathBins.pdf");
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

        hSpectraY[iY] = new TH1F(Form("Y = [%G, %G]", y0, y1), Form("Y = [%G, %G]", y0, y1), 75, xLow, xUp);
    }

    vector <TString> fList;
    if (isMc)
        fList = fInFiles;
    else
        fList = createFilelist();
    cout << "nFiles# " << fList.size() << endl;

    BmnParticlePairCut* cut = fTargCutsMap.find(fTarget.at(0))->second;

    for (auto it = fList.begin(); it != fList.end(); it++) {
        if (fNFiles && distance(fList.begin(), it) == fNFiles)
            break;

        ReadFile(*it, cut, fTrigEffs);
    }

    gStyle->SetOptStat(0);
    gStyle->SetTitleFontSize(0.1);
    gStyle->SetTitleY(1.0);
    gStyle->SetPaintTextFormat("4.3f");

    Int_t ww = 800;
    Int_t wh = 800;
    Int_t nDivs = (nPtBins > nYBins) ? nPtBins : nYBins;

    TCanvas* c = new TCanvas("c", "c", nDivs * ww, 2 * wh);
    c->Divide(nDivs, 2);

    Int_t iPad = 1;

    TH1F* h = nullptr;
    for (Int_t iPt = 0; iPt < nPtBins; iPt++) {
        c->cd(iPad);
        hSpectraPt[iPt]->Draw("PE1X0");
        hSpectraPt[iPt]->SetMarkerStyle(20);
        hSpectraPt[iPt]->SetMarkerSize(2);
        hSpectraPt[iPt]->SetMarkerColor(kSpring - 6);
        hSpectraPt[iPt]->SetLineColor(kSpring - 6);
        hSpectraPt[iPt]->SetLineWidth(1);

        if (!iPt) {
            hSpectraPt[iPt]->GetXaxis()->SetTitle("M_{(p + #pi^{-})}, GeV/c^{2}");
            hSpectraPt[iPt]->GetXaxis()->SetTitleOffset(-1);
        } else
            hSpectraPt[iPt]->GetXaxis()->SetTitle("");
        hSpectraPt[iPt]->GetYaxis()->SetTitle("");
        hSpectraPt[iPt]->GetXaxis()->SetNdivisions(5);
        hSpectraPt[iPt]->GetYaxis()->SetNdivisions(5);
        hSpectraPt[iPt]->GetYaxis()->SetLabelOffset(0.01);
        hSpectraPt[iPt]->GetXaxis()->SetLabelSize(0.06);
        hSpectraPt[iPt]->GetYaxis()->SetLabelSize(0.06);
        hSpectraPt[iPt]->GetXaxis()->SetTitleSize(0.05);
        hSpectraPt[iPt]->GetYaxis()->SetTitleSize(0.05);
        hSpectraPt[iPt]->GetXaxis()->SetTitleOffset(0.85);
        hSpectraPt[iPt]->GetYaxis()->SetTitleOffset(1.5);
        hSpectraPt[iPt]->GetYaxis()->CenterTitle();

        Double_t mean = -1., sigma = -1.;
        pair <Double_t, Double_t> T = make_pair(-1., -1.);
        pair <Double_t, Double_t> B = make_pair(-1., -1.);

        h = (TH1F*) hSpectraPt[iPt]->Clone();

        fitSpectrum(h, mean, sigma, T, B);

        delete hSpectraPt[iPt]->GetListOfFunctions()->FindObject("f");
        delete hSpectraPt[iPt]->GetListOfFunctions()->FindObject("f2");

        TF1* f1 = (TF1*) h->GetListOfFunctions()->FindObject("f1");
        // f1->SetLineColor(kRed);
        Double_t dS = TMath::Sqrt(T.second - B.second);
        Double_t dB = deltaB(f1, fitSpectrum(hSpectraPt[iPt]));
        TF1* f11 = (TF1*) hSpectraPt[iPt]->GetListOfFunctions()->FindObject("f11");
        f11->SetLineColor(kRed);

        Double_t errTotRelNom = TMath::Sqrt((dS / (T.second - B.second)) * (dS / (T.second - B.second))
                + (dB / B.second) * (dB / B.second));

        TLatex *t1 = new TLatex();
        t1->SetNDC(); //we want to draw this object in the Normalized system [0,1]
        t1->SetTextSize(.05);
        t1->DrawLatex(.25, .85, Form("Numb. of #Lambda^{0}  = (%d #pm %d) ", Int_t(T.second - B.second), Int_t(errTotRelNom * (T.second - B.second))));

        fSdeltaS_PtBinMap[iPt] = make_pair(Int_t(T.second - B.second), Int_t(errTotRelNom * (T.second - B.second)));

        iPad++;
    }

    iPad = nDivs + 1;

    for (Int_t iY = 0; iY < nYBins; iY++) {
        c->cd(iPad);
        hSpectraY[iY]->Draw("PE1X0");
        hSpectraY[iY]->SetMarkerStyle(20);
        hSpectraY[iY]->SetMarkerSize(2);
        hSpectraY[iY]->SetMarkerColor(kSpring - 6);
        hSpectraY[iY]->SetLineColor(kSpring - 6);
        hSpectraY[iY]->SetLineWidth(1);
        hSpectraY[iY]->GetXaxis()->SetTitle("");
        hSpectraY[iY]->GetYaxis()->SetTitle("");
        hSpectraY[iY]->GetXaxis()->SetNdivisions(5);
        hSpectraY[iY]->GetYaxis()->SetNdivisions(5);
        hSpectraY[iY]->GetYaxis()->SetLabelOffset(0.01);
        hSpectraY[iY]->GetXaxis()->SetLabelSize(0.06);
        hSpectraY[iY]->GetYaxis()->SetLabelSize(0.06);
        hSpectraY[iY]->GetXaxis()->SetTitleSize(0.05);
        hSpectraY[iY]->GetYaxis()->SetTitleSize(0.05);
        hSpectraY[iY]->GetXaxis()->SetTitleOffset(0.85);
        hSpectraY[iY]->GetYaxis()->SetTitleOffset(1.5);
        hSpectraY[iY]->GetYaxis()->CenterTitle();

        Double_t mean = -1., sigma = -1.;
        pair <Double_t, Double_t> T = make_pair(-1., -1.);
        pair <Double_t, Double_t> B = make_pair(-1., -1.);

        h = (TH1F*) hSpectraY[iY]->Clone();

        fitSpectrum(h, mean, sigma, T, B);

        delete hSpectraY[iY]->GetListOfFunctions()->FindObject("f");
        delete hSpectraY[iY]->GetListOfFunctions()->FindObject("f2");

        TF1* f1 = (TF1*) h->GetListOfFunctions()->FindObject("f1");
        Double_t dS = TMath::Sqrt(T.second - B.second);
        Double_t dB = deltaB(f1, fitSpectrum(hSpectraY[iY]));
        TF1* f11 = (TF1*) hSpectraY[iY]->GetListOfFunctions()->FindObject("f11");
        f11->SetLineColor(kRed);

        Double_t errTotRelNom = TMath::Sqrt((dS / (T.second - B.second)) * (dS / (T.second - B.second))
                + (dB / B.second) * (dB / B.second));

        TLatex *t1 = new TLatex();
        t1->SetNDC(); //we want to draw this object in the Normalized system [0,1]
        t1->SetTextSize(.05);
        t1->DrawLatex(.25, .85, Form("Numb. of #Lambda^{0}  = (%d #pm %d) ", Int_t(T.second - B.second), Int_t(errTotRelNom * (T.second - B.second))));

        fSdeltaS_YBinMap[iY] = make_pair(Int_t(T.second - B.second), Int_t(errTotRelNom * (T.second - B.second)));

        iPad++;
    }

    TString cName = "";
    if (fTarget.size() < 2)
        cName = fTarget.at(0);
    else
        for (auto tar : fTarget)
            cName += tar;

    TString prefName = (isMc) ? "MC" : "DATA";
    c->SaveAs(Form("%s_PtY_%s.pdf", cName.Data(), prefName.Data()));
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
