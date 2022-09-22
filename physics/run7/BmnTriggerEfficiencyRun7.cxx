#include "BmnTriggerEfficiencyRun7.h"

void BmnTriggerEfficiencyRun7::triggerEfficiency() {

    if (!fSpectraFile.IsNull()) {

        // Efficiency for trigger that contains either FD @and@ BD conditions (like, BT + BD1 + FD2) ...
        if (fSpectraFile.Contains("FD") && fSpectraFile.Contains("BD")) {
            // Triggers with three or more conditions are not supported now ...
            if (fEffFiles.size() > 2)
                return;

            BmnLambdaAnalRun7* lAnal = new BmnLambdaAnalRun7();

            // Loop over add. files passed ...
            for (auto effFile : fEffFiles)
                lAnal->SetTriggerEffData(effFile.ReplaceAll("eff_", "").ReplaceAll(".root", ""), effFile); // FIXME !!!

            lAnal->ReadData();

            TClonesArray* filledData = lAnal->GetFilledArray();

            TClonesArray* out = new TClonesArray("TriggerEfficiency");

            for (Int_t iData1 = 0; iData1 < filledData->GetEntriesFast(); iData1++) {
                TriggerEfficiency* eff1 = (TriggerEfficiency*) filledData->UncheckedAt(iData1);

                TString trig1 = eff1->trigger();
                Int_t minMul1 = eff1->multilplicity().first;
                Int_t maxMul1 = eff1->multilplicity().second;

                // Starting from iData1 ...
                for (Int_t iData2 = iData1; iData2 < filledData->GetEntriesFast(); iData2++) {
                    TriggerEfficiency* eff2 = (TriggerEfficiency*) filledData->UncheckedAt(iData2);

                    TString trig2 = eff2->trigger();
                    Int_t minMul2 = eff2->multilplicity().first;
                    Int_t maxMul2 = eff2->multilplicity().second;

                    if (minMul1 != minMul2 || maxMul1 != maxMul2 || trig1 == trig2)
                        continue;

                    Double_t eff = eff1->efficiency() * eff2->efficiency();

                    Double_t relErr1 = eff1->efficiencyError() / eff1->efficiency();
                    Double_t relErr2 = eff2->efficiencyError() / eff2->efficiency();

                    Double_t relErr = TMath::Sqrt(relErr1 * relErr1 + relErr2 * relErr2);
                    Double_t absErr = eff * relErr;

                    new ((*out) [out->GetEntriesFast()]) TriggerEfficiency(fTrigger, make_pair(minMul1, maxMul1), make_pair(eff, absErr));
                }
            }

            // Clear multiplicity map from set values before ...
            fMultMap.clear();

            // Setting number of multiplicity bins (possibly, could change before by user)
            nMultBins = out->GetEntriesFast();

            vector <Double_t> triggEffs;
            vector <Double_t> triggEffErrs;

            for (Int_t iEntry = 0; iEntry < out->GetEntriesFast(); iEntry++) {
                TriggerEfficiency* eff = (TriggerEfficiency*) out->UncheckedAt(iEntry);

                fMultMap[iEntry] = make_pair(eff->multilplicity().first, eff->multilplicity().second);
                triggEffs.push_back(eff->efficiency());
                triggEffErrs.push_back(eff->efficiencyError());
            }

            delete lAnal;

            TFile* eff = new TFile(Form("eff_%s.root", fTrigger.Data()), "recreate");
            TH1F* effGr = new TH1F("Eff", Form("Efficiency of %s", fTrigger.Data()), triggEffs.size(), 0., triggEffs.size());

            for (Int_t iMultBin = 0; iMultBin < nMultBins; iMultBin++) {
                effGr->SetBinContent(iMultBin + 1, (triggEffs.at(iMultBin) > 1.) ? 1. : triggEffs.at(iMultBin));
                effGr->SetBinError(iMultBin + 1, triggEffErrs.at(iMultBin));

                Int_t min = fMultMap.find(iMultBin)->second.first;
                Int_t max = fMultMap.find(iMultBin)->second.second;

                effGr->GetXaxis()->SetBinLabel(iMultBin + 1, Form("[%d, %d)", min, max));
            }

            effGr->SetMarkerStyle(20);
            effGr->SetMarkerColor(kSpring - 6);
            effGr->SetLineColor(kSpring - 6);
            effGr->SetLineWidth(1);
            effGr->GetXaxis()->SetTitle("nTracks in V_{p}");
            effGr->GetYaxis()->SetTitle("");
            effGr->GetXaxis()->SetNdivisions(5);
            effGr->GetYaxis()->SetNdivisions(5);
            effGr->GetYaxis()->SetLabelOffset(0.01);
            effGr->GetXaxis()->SetLabelSize(0.06);
            effGr->GetYaxis()->SetLabelSize(0.04);
            effGr->GetXaxis()->SetTitleSize(0.05);
            effGr->GetYaxis()->SetTitleSize(0.05);
            effGr->GetXaxis()->SetTitleOffset(0.85);
            effGr->GetYaxis()->SetTitleOffset(1.5);
            effGr->GetYaxis()->CenterTitle();
            effGr->Draw("PE1X0");

            eff->Write();
            delete eff;

            return;
        }

        // Efficiency for trigger that contains either FD @or@ BD conditions

        TFile* f = new TFile(fSpectraFile.Data());
        if (!f->IsOpen())
            return;

        TString targ = "";
        for (auto target : fTarget)
            targ += target;

        auto it = fMultMap.begin();

        const Int_t nTrigCond = 2;

        map <Int_t, pair < TH1F*, TH1F*>> histoMap; // mult. bin --> pair of (denom., nom.)

        for (Int_t iMultBin = 0; iMultBin < nMultBins; iMultBin++) {
            auto itShifted = next(it, iMultBin);

            vector <TH1F*> vSet;
            for (Int_t iCond = 0; iCond < nTrigCond; iCond++) {
                TString hName = "Target: " + targ +
                        TString::Format(", Trigger: %s (%s)", fTrigger.Data(), (iCond == TriggConditions::severe) ? "nominator" : "denominator") +
                        TString::Format(", minMult (>=) %d, maxMult (<) %d", itShifted->second.first, itShifted->second.second);

                TH1F* h = (TH1F*) f->Get(hName.Data());
                vSet.push_back(h);
            }

            histoMap[iMultBin] = make_pair(vSet.at(0), vSet.at(1)); // denom., nom. (!!!)
        }

        Double_t mean, sigma = -1.;
        pair <Double_t, Double_t> T = make_pair(-1., -1.);
        pair <Double_t, Double_t> B = make_pair(-1., -1.);

        TFile* out = new TFile("outH.root", "recreate");

        vector <TH1F*> noms;
        vector <TH1F*> denoms;

        vector <Double_t> nomsSignal;
        vector <Double_t> denomsSignal;

        vector <Double_t> errsRelTot;

        for (auto histo : histoMap) {
            TH1F* hNom = histo.second.second;
            TH1F* hDenom = histo.second.first;

            TF1* f1 = nullptr;
            Double_t dS = 0., dB = 0., errTotRelNom = 0., errTotRelDenom = 0.;

            // Fitting spectra for certain mult. bin ...
            fitSpectrum(hNom, mean, sigma, T, B);
            nomsSignal.push_back(T.second - B.second);

            delete hNom->GetListOfFunctions()->FindObject("f");
            delete hNom->GetListOfFunctions()->FindObject("f2");
            f1 = (TF1*) hNom->GetListOfFunctions()->FindObject("f1");
            f1->SetLineColor(kRed);
            dS = TMath::Sqrt(T.second - B.second);
            dB = deltaB(f1, fitSpectrum((TH1F*) hNom->Clone()));

            errTotRelNom = TMath::Sqrt((dS / (T.second - B.second)) * (dS / (T.second - B.second))
                    + (dB / B.second) * (dB / B.second));
            hNom->Write();

            //
            fitSpectrum(hDenom, mean, sigma, T, B);
            denomsSignal.push_back(T.second - B.second);

            delete hDenom->GetListOfFunctions()->FindObject("f");
            delete hDenom->GetListOfFunctions()->FindObject("f2");
            f1 = (TF1*) hDenom->GetListOfFunctions()->FindObject("f1");
            f1->SetLineColor(kRed);
            dS = TMath::Sqrt(T.second - B.second);
            dB = deltaB(f1, fitSpectrum((TH1F*) hDenom->Clone()));

            errTotRelDenom = TMath::Sqrt((dS / (T.second - B.second)) * (dS / (T.second - B.second))
                    + (dB / B.second) * (dB / B.second));
            hDenom->Write();

            noms.push_back(hNom);
            denoms.push_back(hDenom);

            errsRelTot.push_back(TMath::Sqrt(errTotRelNom * errTotRelNom + errTotRelDenom * errTotRelDenom));
        }

        TCanvas* c = new TCanvas("c", "c", 1200, 600);
        c->Divide(nMultBins, 2);

        gStyle->SetOptStat(0);
        gStyle->SetTitleFontSize(0.1);
        gStyle->SetTitleY(1.0);

        for (Int_t iMultBin = 0; iMultBin < nMultBins; iMultBin++) {
            TH1F* n = noms.at(iMultBin);
            c->cd(iMultBin + 1);
            n->Draw("PE1X0");
            n->SetTitle("");

            TLatex *t1 = new TLatex();
            t1->SetNDC(); //we want to draw this object in the Normalized system [0,1]
            t1->SetTextSize(.045);
            TString name = n->GetName();
            name.ReplaceAll("Target: PbSnCuAl,", "").ReplaceAll("Trigger: ", "").ReplaceAll("nominator", "nom.").ReplaceAll("minMult", "nVpTr:").ReplaceAll("maxMult", "");
            t1->DrawLatex(.15, .15, name.Data());
            t1->DrawLatex(.1, .85, Form("S (bin cont. in sig. reg. - bckgrnd) = %d", (Int_t) nomsSignal.at(iMultBin)));

            TH1F* d = denoms.at(iMultBin);
            c->cd(iMultBin + 1 + nMultBins);
            d->Draw("PE1X0");
            d->SetTitle("");

            t1 = new TLatex();
            t1->SetNDC(); //we want to draw this object in the Normalized system [0,1]
            t1->SetTextSize(.045);
            name = d->GetName();
            name.ReplaceAll("Target: PbSnCuAl,", "").ReplaceAll("Trigger: ", "").ReplaceAll("denominator", "den.").ReplaceAll("minMult", "nVpTr:").ReplaceAll("maxMult", "");
            t1->DrawLatex(.15, .15, name.Data());
            t1->SetTextSize(.04);
            t1->DrawLatex(.15, .85, Form("S (bin cont. in sig. reg. - bckgrnd) = %d", (Int_t) denomsSignal.at(iMultBin)));
        }

        c->SaveAs(Form("triggEff_%s_multBins.pdf", fTrigger.Data()));

        vector <Double_t> triggEffs;
        for (Int_t iMultBin = 0; iMultBin < nMultBins; iMultBin++)
            triggEffs.push_back(nomsSignal.at(iMultBin) / denomsSignal.at(iMultBin));

        TFile* eff = new TFile(Form("eff_%s.root", fTrigger.Data()), "recreate");
        TH1F* effGr = new TH1F("Eff", Form("Efficiency of %s", fTrigger.Data()), triggEffs.size(), 0., triggEffs.size());

        for (Int_t iMultBin = 0; iMultBin < nMultBins; iMultBin++) {
            effGr->SetBinContent(iMultBin + 1, (triggEffs.at(iMultBin) > 1.) ? 1. : triggEffs.at(iMultBin));
            effGr->SetBinError(iMultBin + 1, triggEffs.at(iMultBin) * errsRelTot.at(iMultBin));

            Int_t min = fMultMap.find(iMultBin)->second.first;
            Int_t max = fMultMap.find(iMultBin)->second.second;

            effGr->GetXaxis()->SetBinLabel(iMultBin + 1, Form("[%d, %d)", min, max));
        }

        effGr->SetMarkerStyle(20);
        effGr->SetMarkerColor(kSpring - 6);
        effGr->SetLineColor(kSpring - 6);
        effGr->SetLineWidth(1);
        effGr->GetXaxis()->SetTitle("nTracks in V_{p}");
        effGr->GetYaxis()->SetTitle("");
        effGr->GetXaxis()->SetNdivisions(5);
        effGr->GetYaxis()->SetNdivisions(5);
        effGr->GetYaxis()->SetLabelOffset(0.01);
        effGr->GetXaxis()->SetLabelSize(0.06);
        effGr->GetYaxis()->SetLabelSize(0.04);
        effGr->GetXaxis()->SetTitleSize(0.05);
        effGr->GetYaxis()->SetTitleSize(0.05);
        effGr->GetXaxis()->SetTitleOffset(0.85);
        effGr->GetYaxis()->SetTitleOffset(1.5);
        effGr->GetYaxis()->CenterTitle();
        effGr->Draw("PE1X0");

        eff->Write();
        delete eff;

        delete f;
        delete out;

        return;
    }

    // Getting geom. cut map (for all targets) ...
    map <TString, BmnParticlePairCut*> tCutMap = GetTargetCutsMap();

    BmnParticlePairCut* cuts = nullptr;

    TString targ = "";
    for (auto target : fTarget) {
        targ += target;
        cuts = tCutMap.find(target)->second;
    }

    // Preparing histos ...
    const Int_t nTrigCond = 2; // Nominator and denominator (always two) ...
    hSpectra = new TH1F**[nTrigCond];
    hMult = new TH1F*[nTrigCond];

    auto it = fMultMap.begin();

    for (Int_t iCond = 0; iCond < nTrigCond; iCond++) {
        hSpectra[iCond] = new TH1F*[nMultBins];

        TString hName = "Target: " + targ +
                TString::Format(", Trigger: %s (%s)", fTrigger.Data(), (iCond == TriggConditions::severe) ? "nominator" : "denominator");

        hMult[iCond] = new TH1F(hName.Data(), hName.Data(), 100, 0., 100.);

        for (Int_t iMultBin = 0; iMultBin < nMultBins; iMultBin++) {
            auto itShifted = next(it, iMultBin);
            hName = "Target: " + targ +
                    TString::Format(", Trigger: %s (%s)", fTrigger.Data(), (iCond == TriggConditions::severe) ? "nominator" : "denominator") +
                    TString::Format(", minMult (>=) %d, maxMult (<) %d", itShifted->second.first, itShifted->second.second);
            hSpectra[iCond][iMultBin] = new TH1F(hName.Data(), hName.Data(), 75, xLow, xUp);
        }
    }

    // Processing dst's ...
    if (cuts) {
        vector <TString> triggsSet;

        // Setting datasets to be processed ...
        if (fTrigger == "BT+FD3" || fTrigger == "BT+FD2") {
            triggsSet.push_back("BT+BD2");
            triggsSet.push_back("BT+BD3");
        } else if (fTrigger == "BT+BD3" || fTrigger == "BT+BD2" || fTrigger == "BT+BD1") {
            triggsSet.push_back("BT+FD2");
            triggsSet.push_back("BT+FD3");
        } else if (fTrigger == "BT+BD1+FD2")
            return;

        else
            Fatal("BmnTriggerEfficiencyRun7::triggerEfficiency", "Trigger condition not supported!!!");

        for (auto dst : createFilelist(triggsSet)) {
            // Getting denom. histos (base) ...
            isAddTriggerCondition = kFALSE;
            ReadFile(dst, cuts);

            // Getting nomin. histos (severe) ...
            isAddTriggerCondition = kTRUE;
            ReadFile(dst, cuts);
        }
    }

    // Writing filled histos to out file ...
    TFile* f = new TFile(Form("out_%s_start%d_finish%d.root", fTrigger.Data(), fStartRun, fFinishRun), "recreate");

    for (Int_t iCond = 0; iCond < nTrigCond; iCond++)
        for (Int_t iMultBin = 0; iMultBin < nMultBins; iMultBin++)
            hSpectra[iCond][iMultBin]->Write();

    delete f;
}

vector <TString> BmnTriggerEfficiencyRun7::createFilelist(vector <TString> triggsSet) {
    vector <TString> list;

    // Access to trigger information ...
    BmnDataTriggerInfo* trInfo = new BmnDataTriggerInfo();

    for (Int_t iFile = fStartRun; iFile < fFinishRun; iFile++) {
        UniRun* pCurrentRun = UniRun::GetRun(fPeriod, iFile);

        // Check presense of the current run in DB ...
        if (!pCurrentRun)
            continue;

        if (pCurrentRun->GetBeamParticle() != fBeam)
            continue;

        // Trigger check ...
        Bool_t isNeededTrigger = kFALSE;

        for (auto it : triggsSet) {
            if (trInfo->GetTrigger(iFile) != it)
                continue;
            else {
                isNeededTrigger = kTRUE;
                break;
            }
        }

        if (!isNeededTrigger)
            continue;

        TString* targ = pCurrentRun->GetTargetParticle();

        // Skipping runs w/o target ...
        if (!targ)
            continue;

        Bool_t isNeededTarget = kFALSE;

        for (auto it : fTarget) {
            if (it != *targ)
                continue;
            else {
                isNeededTarget = kTRUE;
                break;
            }
        }

        if (!isNeededTarget)
            continue;

        for (TString tmp : fInFiles) {
            if (!tmp.Contains(Form("%d", iFile)))
                continue;

            list.push_back(tmp);
            break;
        }
    }

    delete trInfo;

    return list;
}

void BmnTriggerEfficiencyRun7::ReadFile(TString f, BmnParticlePairCut* cut0) {
    TChain* out = new TChain("bmndata");
    out->Add(f.Data());

    if (!out->GetFile()) {
        delete out;
        return;
    }

    TClonesArray* particlePair = nullptr;
    out->SetBranchAddress("ParticlePair", &particlePair);
    if (!particlePair)
        return;

    DstEventHeaderExtended* eHeaderExt = nullptr;
    out->SetBranchAddress("DstEventHeaderExtended.", &eHeaderExt);
    if (!eHeaderExt)
        return;

    for (Int_t iEv = 0; iEv < out->GetEntries(); iEv++) {
        out->GetEntry(iEv);

        Int_t nVpTracks = eHeaderExt->nTracks();
        Double_t z = eHeaderExt->Vp().Z();

        // -6. < VpZ < +6. [cm]        
        const Double_t zCor = 6.;

        if (TMath::Abs(z) > zCor || nVpTracks < 2)
            continue;

        // Getting trigger conditions and track multiplicity ...
        if ((fTrigger.Contains("BT+FD3") || fTrigger.Contains("BT+FD2")) && isAddTriggerCondition) {
            // Nominator ...
            nFdCounts = (fTrigger.Contains("BT+FD3")) ? 3 : 2;
            if (eHeaderExt->fd() <= nFdCounts)
                continue;
        } else if ((fTrigger.Contains("BT+BD3") || fTrigger.Contains("BT+BD2") || fTrigger.Contains("BT+BD1")) && isAddTriggerCondition) {
            // Nominator ...
            nBdCounts = (fTrigger.Contains("BT+BD3")) ? 3 : (fTrigger.Contains("BT+BD2")) ? 2 : 1;
            if (eHeaderExt->bd() <= nBdCounts)
                continue;
        }

        for (Int_t iPair = 0; iPair < particlePair->GetEntriesFast(); iPair++) {
            BmnParticlePair* pair = (BmnParticlePair*) particlePair->UncheckedAt(iPair);

            // V0 ...
            Double_t v0z = pair->GetV0Z();

            // Skipping so far reconstructed V0's ...
            if (v0z < 0. || v0z > 50.)
                continue;

            // Geometry topology ...
            Double_t DCA12 = pair->GetDCA12();
            Double_t DCA1 = pair->GetDCA1();
            Double_t DCA2 = pair->GetDCA2();
            Double_t DCA0 = pair->GetDCA0();

            Double_t PATH = pair->GetPath();

            // Track length (GEM part of glob. track) ...               
            Int_t protonNGemHits = pair->GetNHitsPart1("GEM");
            Int_t pionNGemHits = pair->GetNHitsPart2("GEM");

            BmnParticlePairCut* cut = cut0;

            Double_t dca0 = cut->dca0();
            Double_t dca1 = cut->dca1();
            Double_t dca2 = cut->dca2();
            Double_t dca12 = cut->dca12();

            Double_t path = cut->path();

            Int_t nPos = cut->nHitsGemPos();
            Int_t nNeg = cut->nHitsGemNeg();

            if (DCA0 > dca0 || DCA12 > dca12) // <=
                continue;

            if (DCA1 < dca1 || DCA2 < dca2 || PATH < path) // >= 
                continue;

            if (protonNGemHits <= nPos || pionNGemHits <= nNeg) // >
                continue;

            if (FinMultBin(nVpTracks) != -1)
                hSpectra[isAddTriggerCondition ? (TriggConditions::severe) : (TriggConditions::base)][FinMultBin(nVpTracks)]->Fill(pair->GetInvMass());
        }
    }

    cout << "File " << f << " processed# " << endl;
    delete out;
}