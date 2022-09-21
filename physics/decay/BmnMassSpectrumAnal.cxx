#include "BmnMassSpectrumAnal.h"
#include "BmnTwoParticleDecay.h"
#include "efficiency/BmnDataTriggerInfo.h"
#include "run7/BmnTriggerEfficiencyRun7.h"

BmnMassSpectrumAnal::BmnMassSpectrumAnal() :
fPeriod(7), fStartRun(3589), fFinishRun(4710), fBeam("Ar"),
xLow(1.07), xUp(1.22), fNFiles(0), fSignal(0.), fBackground(0.), fSignalBinMin(18), fSignalBinMax(26) {

}

BmnMassSpectrumAnal::BmnMassSpectrumAnal(TString dir) :
hSpectra(nullptr), hSpectraPt(nullptr), hSpectraY(nullptr), hSpectrumImproved(nullptr), xLow(1.07), xUp(1.22), fSignal(0.), fBackground(0.), fSignalBinMin(18), fSignalBinMax(26),
fNFiles(0), nPtBins(0), nYBins(0),
fParticlePairCuts(new TClonesArray("BmnParticlePairCut")), isPtY(kFALSE),
fPeriod(7), fStartRun(3589), fFinishRun(4710), fBeam("Ar") {
    fTarget.resize(0);

    // Composing input file list ...
    TSystemDirectory dirFiles(dir.Data(), dir.Data());
    TList* files = dirFiles.GetListOfFiles();

    TSystemFile *file;
    TIter next((TCollection*) files);

    fInFiles.resize(0);

    while ((file = (TSystemFile*) next())) {

        TString fname = file->GetName();

        if (!file->IsDirectory() && fname.EndsWith(".root"))
            fInFiles.push_back(dir + (TString) file->GetName());
    }
}

Bool_t BmnMassSpectrumAnal::checkFit(BmnParticlePairCut* cut) {
    Double_t B = cut->B();
    Double_t S = cut->S();

    Double_t mass = cut->mass();
    Double_t width = cut->width();

    // Skipping resulted spectra wide and narrow signals ..
    if (width > 0.005 || width < 0.002)
        return kFALSE;

    if (TMath::Abs(mass - 1.1157) > width)
        return kFALSE;

    // Skipping bad spectra with incorrect (not satisfying) values of B and S 
    if (S < fSignal || B < 0. || B > 10000.) // FIXME
        return kFALSE;

    return kTRUE;
}

vector <TString> BmnMassSpectrumAnal::createFilelist() {
    vector <TString> list;

    for (Int_t iFile = fStartRun; iFile < fFinishRun; iFile++) {
        UniRun* pCurrentRun = UniRun::GetRun(fPeriod, iFile);

        // Check presense of the current run in DB ...
        if (!pCurrentRun)
            continue;

        if (pCurrentRun->GetBeamParticle() != fBeam)
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
    return list;
}

void BmnMassSpectrumAnal::DoOptimization() {
    vector <TString> selectedFiles = createFilelist();
    cout << "nFiles# " << selectedFiles.size() << endl;

    // Preparing spectrum histograms ...
    hSpectra = new TH1F*[fParticlePairCuts->GetEntriesFast()];
    for (Int_t iCut = 0; iCut < fParticlePairCuts->GetEntriesFast(); iCut++) {

        BmnParticlePairCut* cut = (BmnParticlePairCut*) fParticlePairCuts->UncheckedAt(iCut);
        Double_t dca0 = cut->dca0();
        Double_t dca1 = cut->dca1();
        Double_t dca2 = cut->dca2();
        Double_t dca12 = cut->dca12();

        Double_t path = cut->path();

        Int_t nPos = cut->nHitsGemPos();
        Int_t nNeg = cut->nHitsGemNeg();

        TString hName = TString::Format("dca0 %G dca1 %G dca2 %G dca12 %G path %G nPos %d nNeg %d", dca0, dca1, dca2, dca12, path, nPos, nNeg);
        hSpectra[iCut] = new TH1F(hName.Data(), "massSpectrum", 75, xLow, xUp);
    }

    // Processing files ...
    for (auto it = selectedFiles.begin(); it != selectedFiles.end(); it++) {
        if (fNFiles && distance(selectedFiles.begin(), it) == fNFiles)
            break;

        ReadFile(*it);
    }

    // Setting cuts and B, T, mean (mass), sigma (width) ... 
    for (Int_t iCut = 0; iCut < fParticlePairCuts->GetEntriesFast(); iCut++) {

        Double_t mean = -1., sigma = -1.;
        pair <Double_t, Double_t> T = make_pair(-1., -1.);
        pair <Double_t, Double_t> B = make_pair(-1., -1.);

        if (hSpectra[iCut]->GetEntries() < 250.) // FIXME!!!
            continue;

        if (iCut % 1000 == 0)
            cout << "Fitting cut# " << iCut << endl;

        fitSpectrum(hSpectra[iCut], mean, sigma, T, B);

        BmnParticlePairCut* cut = (BmnParticlePairCut*) fParticlePairCuts->UncheckedAt(iCut);
        cut->SetBackground(B.first);
        cut->SetSignalAndBackground(T.first);

        cut->SetMass(mean);
        cut->SetWidth(sigma);
    }

    const Int_t nCuts = 1; // FIXME
    TFile* fOut = new TFile(Form("h%s.root", fTarget.at(0).Data()), "recreate");

    map <Double_t, Int_t> Smap; // S --> cut
    for (Int_t iCut = 0; iCut < fParticlePairCuts->GetEntriesFast(); iCut++) {
        BmnParticlePairCut* cut = (BmnParticlePairCut*) fParticlePairCuts->UncheckedAt(iCut);

        if (!checkFit(cut))
            continue;

        Double_t S = cut->S();

        Smap[S] = iCut;
    }

    // Getting highest signal values ...

    vector <Int_t> cutsToBeImproved;

    for (auto it = Smap.rbegin(); it != Smap.rend(); it++) {

        if (distance(Smap.rbegin(), it) == nCuts)
            break;

        BmnParticlePairCut* cut = (BmnParticlePairCut*) fParticlePairCuts->UncheckedAt(it->second);

        Double_t B = cut->B();
        Double_t S = cut->S();

        cutsToBeImproved.push_back(it->second);

        hSpectra[it->second]->SetTitle(Form("S = %G, B = %G, S/B = %G", S, B, S / B));
        hSpectra[it->second]->Write();
    }

    // Doing optimization over each cut consequently ...
    for (auto it : cutsToBeImproved) {
        BmnParticlePairCut* cut = (BmnParticlePairCut*) fParticlePairCuts->UncheckedAt(it);

        // Improving dca12 ...
        Double_t dca12Imp = ImproveCutValue(selectedFiles, cut, "dca12");
        cout << "Improved dca12 = " << dca12Imp << endl;
        cut->SetDca12(dca12Imp);

        // Improving dca0 ...
        Double_t dca0Imp = ImproveCutValue(selectedFiles, cut, "dca0");
        cout << "Improved dca0 = " << dca0Imp << endl;
        cut->SetDca0(dca0Imp);

        // Improving dca1 ...
        Double_t dca1Imp = ImproveCutValue(selectedFiles, cut, "dca1");
        cout << "Improved dca1 = " << dca1Imp << endl;
        cut->SetDca1(dca1Imp);

        // Improving dca2 ...
        Double_t dca2Imp = ImproveCutValue(selectedFiles, cut, "dca2");
        cout << "Improved dca2 = " << dca2Imp << endl;
        cut->SetDca2(dca2Imp);

        // Improving path ...
        Double_t path = ImproveCutValue(selectedFiles, cut, "path");
        cout << "Improved path = " << path << endl;
        cut->SetPath(path);
    }

    delete fOut;
}

Double_t BmnMassSpectrumAnal::ImproveCutValue(vector <TString> selectedFiles, BmnParticlePairCut* cut, TString cutName) {
    Double_t step = 0.;
    Int_t nSteps = 0;

    if (cutName.Contains("dca")) {
        step = .1;
        nSteps = 5;
    } else if (cutName.Contains("path")) {
        step = 2.;
        nSteps = 5;
    }

    TClonesArray* cutsImproved = new TClonesArray("BmnParticlePairCut");
    vector <TH1F*> spectraImproved;

    // Adding source cut to be used for adjustment ...
    new ((*cutsImproved) [cutsImproved->GetEntriesFast()]) BmnParticlePairCut(*cut);

    vector <Int_t> signs;

    if (cutName.Contains("path"))
        signs.push_back(+1);
    else {
        signs.push_back(+1);
        signs.push_back(-1);
    }

    for (auto sign : signs)
        for (Int_t iStep = 1; iStep < nSteps + 1; iStep++) {

            // Creating new cut ... 
            BmnParticlePairCut* cutImp = new BmnParticlePairCut(*cut);

            if (cutName == "dca0")
                cutImp->SetDca0(cutImp->dca0() + sign * iStep * step);
            else if (cutName == "dca1")
                cutImp->SetDca1(cutImp->dca1() + sign * iStep * step);
            else if (cutName == "dca2")
                cutImp->SetDca2(cutImp->dca2() + sign * iStep * step);
            else if (cutName == "dca12")
                cutImp->SetDca12(cutImp->dca12() + sign * iStep * step);
            else if (cutName == "path")
                cutImp->SetPath(cutImp->path() + sign * iStep * step);
            else
                Fatal("BmnMassSpectrumAnal::ImproveCutValue", "No correct cut name given!");

            cutImp->SetOrigin("improved");

            // Creating new spectrum ...
            Double_t dca0 = cutImp->dca0();
            Double_t dca1 = cutImp->dca1();
            Double_t dca2 = cutImp->dca2();
            Double_t dca12 = cutImp->dca12();
            Double_t path = cutImp->path();

            Int_t nPos = cutImp->nHitsGemPos();
            Int_t nNeg = cutImp->nHitsGemNeg();

            TString hName = TString::Format("dca0 %G dca1 %G dca2 %G dca12 %G path %G nPos %d nNeg %d", dca0, dca1, dca2, dca12, path, nPos, nNeg);

            hSpectrumImproved = new TH1F(hName.Data(), "massSpectrum", 75, xLow, xUp);

            for (auto it1 = selectedFiles.begin(); it1 != selectedFiles.end(); it1++) {
                if (fNFiles && distance(selectedFiles.begin(), it1) == fNFiles)
                    break;

                ReadFile(*it1, cutImp);
            }

            // Fitting obtained spectrum ...
            Double_t mean = -1., sigma = -1.;
            pair <Double_t, Double_t> T = make_pair(-1., -1.);
            pair <Double_t, Double_t> B = make_pair(-1., -1.);

            fitSpectrum(hSpectrumImproved, mean, sigma, T, B);

            cutImp->SetBackground(B.first);
            cutImp->SetSignalAndBackground(T.first);
            cutImp->SetMass(mean);
            cutImp->SetWidth(sigma);

            if (!checkFit(cutImp))
                continue;

            hSpectrumImproved->SetTitle(Form("S = %G, B = %G, S/B = %G", T.first - B.first, B.first, (T.first - B.first) / B.first));
            hSpectrumImproved->SetName((TString) hSpectrumImproved->GetName() + TString::Format(", improved"));

            // Adding possibly adjusted cut and spectrum ...
            new ((*cutsImproved) [cutsImproved->GetEntriesFast()]) BmnParticlePairCut(*cutImp);

            spectraImproved.push_back((TH1F*) hSpectrumImproved->Clone());

            delete hSpectrumImproved;
        }

    // No additional info added, thus skipping ...
    if (cutsImproved->GetEntriesFast() == 1) {
        if (cutName == "dca0")
            return cut->dca0();
        else if (cutName == "dca1")
            return cut->dca1();
        else if (cutName == "dca2")
            return cut->dca2();
        else if (cutName == "dca12")
            return cut->dca12();
        else if (cutName == "path")
            return cut->path();

        return 0.;
    }

    BmnParticlePairCut source;
    vector <BmnParticlePairCut> improved;

    for (Int_t iEntry = 0; iEntry < cutsImproved->GetEntriesFast(); iEntry++) {
        BmnParticlePairCut* cut0 = (BmnParticlePairCut*) cutsImproved->UncheckedAt(iEntry);

        if (cut0->origin().Contains("improved"))
            improved.push_back(*cut0);
        else
            source = *cut0;
    }

    delete cutsImproved;

    Double_t S0 = source.S();
    Double_t B0 = source.B();

    // Removing cuts increasing B or decreasing S significantly ...
    for (auto it1 = improved.begin(); it1 != improved.end(); it1++) {

        Double_t S1 = (*it1).S();
        Double_t B1 = (*it1).B();

        Double_t dS_S0 = (S1 - S0) / S0;
        Double_t dB_B0 = (B1 - B0) / B0;

        if (dB_B0 > 0. || dB_B0 > dS_S0)
            improved.erase(it1--);
    }

    // If no one improved cut survived ...
    if (!improved.size()) {
        if (cutName == "dca0")
            return cut->dca0();
        else if (cutName == "dca1")
            return cut->dca1();
        else if (cutName == "dca2")
            return cut->dca2();
        else if (cutName == "dca12")
            return cut->dca12();
        else if (cutName == "path")
            return cut->path();

        return 0.;
    }

    // Getting improved cut that reduces signal as much as possible ...
    map <Double_t, Double_t> signalCut;
    for (BmnParticlePairCut it1 : improved) {
        Double_t cVal = -1.;

        if (cutName == "dca0")
            cVal = it1.dca0();
        else if (cutName == "dca1")
            cVal = it1.dca1();
        else if (cutName == "dca2")
            cVal = it1.dca2();
        else if (cutName == "dca12")
            cVal = it1.dca12();
        else if (cutName == "path")
            cVal = it1.path();

        signalCut[it1.S()] = cVal;
    }

    auto it1 = signalCut.rbegin();

    TString hNamePart = TString::Format("%s %G", cutName.Data(), it1->second);

    for (TH1F* h : spectraImproved) {
        if (!((TString) h->GetName()).Contains(hNamePart.Data()))
            continue;

        cout << h->GetName() << endl;
        gFile->cd();
        h->Write();

        break;
    }

    return it1->second;
}

void BmnMassSpectrumAnal::ReadFile(TString f, BmnParticlePairCut* cut0, TClonesArray* triggEffInfo, Double_t pathMin, Double_t pathMax) {
    TChain* out = new TChain("bmndata");
    out->Add(f.Data());

    if (!out->GetFile()) {
        delete out;
        return;
    }

    DstEventHeaderExtended* header = nullptr;
    TClonesArray* particlePair = nullptr;

    out->SetBranchAddress("ParticlePair", &particlePair);
    out->SetBranchAddress("DstEventHeaderExtended.", &header);

    if (!particlePair || !header)
        return;

    // Getting trigger info and its efficiency for certain multiplicity bin ...
    BmnDataTriggerInfo* info = new BmnDataTriggerInfo();
    out->GetEntry();
    TString trigger = info->GetTrigger(header->GetRunId());

    delete info;

    //for (Int_t iEv = 0; iEv < 10000; iEv++) {
    for (Int_t iEv = 0; iEv < out->GetEntries(); iEv++) {
        out->GetEntry(iEv);

        Int_t nVpTracks = header->nTracks();
        Double_t Z = header->Vp().Z();

        if (nVpTracks < 2 || TMath::Abs(Z) > 6.)
            continue;

        Double_t epsilon = 1.;

        if (triggEffInfo) {
            for (Int_t iEntry = 0; iEntry < triggEffInfo->GetEntriesFast(); iEntry++) {
                TriggerEfficiency* eff = (TriggerEfficiency*) triggEffInfo->UncheckedAt(iEntry);

                if (trigger != eff->trigger())
                    continue;

                Int_t minMult = eff->multilplicity().first;
                Int_t maxMult = eff->multilplicity().second;

                if (nVpTracks >= minMult && nVpTracks < maxMult) {
                    epsilon = eff->efficiency();

                    break;
                }
            }
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

            if (!cut0)
                for (Int_t iCut = 0; iCut < fParticlePairCuts->GetEntriesFast(); iCut++) {
                    cut = (BmnParticlePairCut*) fParticlePairCuts->UncheckedAt(iCut);
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

                    hSpectra[iCut]->Fill(pair->GetInvMass());
                } else {
                Double_t dca0 = cut->dca0();
                Double_t dca1 = cut->dca1();
                Double_t dca2 = cut->dca2();
                Double_t dca12 = cut->dca12();

                Double_t path = cut->path();

                Int_t nPos = cut->nHitsGemPos();
                Int_t nNeg = cut->nHitsGemNeg();

                if (DCA0 > dca0 || DCA12 > dca12) // <=
                    continue;

                if (DCA1 < dca1 || DCA2 < dca2) // >= 
                    continue;

                if (!fPathBins.size()) {
                    if (PATH < path) // >=
                        continue;
                } else {
                    if (PATH < pathMin || PATH > pathMax)
                        continue;
                }

                if (protonNGemHits <= nPos || pionNGemHits <= nNeg) // >
                    continue;

                if (hSpectrumImproved)
                    hSpectrumImproved->Fill(pair->GetInvMass(), 1. / epsilon);

                if (isPtY) {
                    // Kinematic topology ...
                    Double_t protonMom = pair->GetMomPart1();
                    Double_t pionMom = pair->GetMomPart2();

                    Double_t protonTx = pair->GetTxPart1();
                    Double_t pionTx = pair->GetTxPart2();

                    Double_t protonTy = pair->GetTyPart1();
                    Double_t pionTy = pair->GetTyPart2();

                    vector <Double_t> kinVec1{protonMom, protonTx, protonTy};
                    if (!isVectorOk(kinVec1))
                        continue;

                    vector <Double_t> kinVec2{pionMom, pionTx, pionTy};
                    if (!isVectorOk(kinVec2))
                        continue;

                    Double_t y = -1., pt = -1.;
                    GetPtY(kinVec1, kinVec2, pt, y);

                    Int_t binPt = FinBin(fPtBinMap, pt);
                    if (binPt != -1)
                        hSpectraPt[binPt]->Fill(pair->GetInvMass(), 1. / epsilon);

                    Int_t binY = FinBin(fYBinMap, y);
                    if (binY != -1)
                        hSpectraY[binY]->Fill(pair->GetInvMass(), 1. / epsilon);

                    // cout << epsilon << endl;
                }

            }
        }
    }

    cout << "File " << f << " processed# " << endl;
    delete out;
}

TFitResultPtr BmnMassSpectrumAnal::fitSpectrum(TH1F* h) {
    BmnMassSpectrumAnal mA;

    vector <Double_t> binErrorsAll;
    vector <Double_t> binErrorsSignal;

    // Saving bin errors for all bins ...
    for (Int_t iBin = 1; iBin < h->GetNbinsX() + 1; iBin++)
        binErrorsAll.push_back(h->GetBinError(iBin));

    // Saving bin errors for signal region ...
    for (Int_t iBin = fSignalBinMin; iBin < fSignalBinMax + 1; iBin++)
        binErrorsSignal.push_back(h->GetBinError(iBin));

    // Setting zero errors for signal region ...
    for (Int_t iBin = fSignalBinMin; iBin < fSignalBinMax + 1; iBin++)
        h->SetBinError(iBin, 0.);

    Double_t xmin = mA.xLow + 0.01;
    Double_t xmax = mA.xUp;
    Double_t n = 6;

    TF1* fitBackground = new TF1("f11", background, xmin, xmax, n + 1);
    fitBackground->SetLineColor(kBlue);
    fitBackground->SetLineStyle(kDashed);
    TFitResultPtr backRes = h->Fit(fitBackground, "SQR+", "", xmin, xmax);

    // Restoring all bin errors ...
    for (Int_t iBin = 1; iBin < h->GetNbinsX() + 1; iBin++)
        h->SetBinError(iBin, binErrorsAll[iBin]);

    if (!backRes.Get())
        return nullptr;
    else
        return backRes;
}

void BmnMassSpectrumAnal::fitSpectrum(TH1F* h, Double_t& mean, Double_t& sigma, pair <Double_t, Double_t>& T, pair <Double_t, Double_t>& B) {
    BmnMassSpectrumAnal mA;

    vector <Double_t> binErrorsAll;
    vector <Double_t> binErrorsSignal;

    // Saving bin errors for all bins ...
    for (Int_t iBin = 1; iBin < h->GetNbinsX() + 1; iBin++)
        binErrorsAll.push_back(h->GetBinError(iBin));

    // Saving bin errors for signal region ...
    for (Int_t iBin = fSignalBinMin; iBin < fSignalBinMax + 1; iBin++)
        binErrorsSignal.push_back(h->GetBinError(iBin));

    // Setting zero errors for signal region ...
    for (Int_t iBin = fSignalBinMin; iBin < fSignalBinMax + 1; iBin++)
        h->SetBinError(iBin, 0.);

    Double_t xmin = mA.xLow + 0.01;
    Double_t xmax = mA.xUp;
    Double_t n = 6;

    TF1* fitBackground = new TF1("f1", background, xmin, xmax, n + 1);
    fitBackground->SetLineColor(kBlue);
    fitBackground->SetLineStyle(kDashed);
    TFitResultPtr backRes = h->Fit(fitBackground, "SQR+", "", xmin, xmax);

    if (!backRes.Get())
        return;

    // Setting zero error for all range ...
    for (Int_t iBin = 1; iBin < h->GetNbinsX() + 1; iBin++)
        h->SetBinError(iBin, 0.);

    // Restoring errors for signal range ...
    for (Int_t iBin = fSignalBinMin; iBin < fSignalBinMax + 1; iBin++)
        h->SetBinError(iBin, binErrorsSignal[iBin - fSignalBinMin]);

    TF1* fitSignal = new TF1("f2", signal, xmin, xmax, 3);

    fitSignal->SetParameter(0, 2000.);
    fitSignal->SetParameter(1, 1.1157); // FIXME!!!
    fitSignal->SetParameter(2, 0.002);
    fitSignal->SetNpx(500);

    fitSignal->SetLineColor(kRed);
    TFitResultPtr sigRes = h->Fit(fitSignal, "SQR0+", "", h->GetBinCenter(fSignalBinMin), h->GetBinCenter(fSignalBinMax));

    if (!sigRes.Get())
        return;

    // Restoring all bin errors ...
    for (Int_t iBin = 1; iBin < h->GetNbinsX() + 1; iBin++)
        h->SetBinError(iBin, binErrorsAll[iBin]);

    TF1* fSpectrum = new TF1("f", fitFunction, xmin, xmax, 10);
    fSpectrum->SetLineColor(kMagenta);
    fSpectrum->SetNpx(500);
    fSpectrum->SetParameter(0, backRes->Parameter(0));
    fSpectrum->SetParameter(1, backRes->Parameter(1));
    fSpectrum->SetParameter(2, backRes->Parameter(2));
    fSpectrum->SetParameter(3, backRes->Parameter(3));
    fSpectrum->SetParameter(4, backRes->Parameter(4));
    fSpectrum->SetParameter(5, backRes->Parameter(5));
    fSpectrum->SetParameter(6, backRes->Parameter(6));
    fSpectrum->SetParameter(7, sigRes->Parameter(0));
    fSpectrum->SetParameter(8, sigRes->Parameter(1));
    fSpectrum->SetParameter(9, sigRes->Parameter(2));

    TFitResultPtr res = h->Fit(fSpectrum, "SQR+", "", xmin, xmax);

    if (!res.Get())
        return;

    mean = res->Parameter(8);
    sigma = TMath::Abs(res->Parameter(9));

    // Calculating T and B by fit ...   
    T.first = fSpectrum->Integral(mean - 3 * sigma, mean + 3 * sigma) / h->GetBinWidth(1);
    B.first = fitBackground->Integral(mean - 3 * sigma, mean + 3 * sigma) / h->GetBinWidth(1);

    // Calculating T and B by histogram ... 
    B.second = 0.;
    T.second = 0.;

    for (Int_t iBin = fSignalBinMin; iBin < fSignalBinMax + 1; iBin++) {
        B.second += fitBackground->Eval(h->GetBinCenter(iBin));
        T.second += h->GetBinContent(iBin);
    }
}

Double_t BmnMassSpectrumAnal::background(Double_t *xx, Double_t *p) {
    BmnMassSpectrumAnal mA;
    // mA.SetSpectrumRange(.452, .602);
    Double_t fA = mA.xLow;
    Double_t fB = mA.xUp;

    vector <Double_t> fT;
    fT.resize(6); // polynomial

    Double_t x = (2.0 * xx[0] - fA - fB) / (fB - fA);
    int order = fT.size();
    if (order == 1)
        return p[0];
    if (order == 2)
        return p[0] + x * p[1];
    // build the polynomials
    fT[0] = 1;
    fT[1] = x;
    for (Int_t i = 1; i < order; ++i) {
        fT[i + 1] = 2 * x * fT[i] - fT[i - 1];
    }
    Double_t sum = p[0] * fT[0];
    for (int i = 1; i <= order; ++i) {
        sum += p[i] * fT[i];
    }
    return sum;
}

// Signal function ...

Double_t BmnMassSpectrumAnal::signal(Double_t *xx, Double_t *p) {
    Double_t arg = (xx[0] - p[1]) / p[2];
    Double_t fitval = p[0] * TMath::Exp(-0.5 * arg * arg);
    return fitval;
}

// Sum of background and peak function

Double_t BmnMassSpectrumAnal::fitFunction(Double_t *x, Double_t *par) {
    return background(x, &par[0]) + signal(x, &par[7]);
}

void BmnMassSpectrumAnal::GetPtY(vector <Double_t> vec1, vector <Double_t> vec2, Double_t& pt, Double_t& Y) {
    // vec --> (p, Tx, Ty)

    Double_t p1 = vec1.at(0);
    Double_t Tx1 = vec1.at(1);
    Double_t Ty1 = vec1.at(2);

    Double_t p2 = vec2.at(0);
    Double_t Tx2 = vec2.at(1);
    Double_t Ty2 = vec2.at(2);

    Double_t px1 = Tx1 * (p1 / TMath::Sqrt(1 + Tx1 * Tx1 + Ty1 * Ty1));
    Double_t px2 = Tx2 * (p2 / TMath::Sqrt(1 + Tx2 * Tx2 + Ty2 * Ty2));

    Double_t py1 = Ty1 * (p1 / TMath::Sqrt(1 + Tx1 * Tx1 + Ty1 * Ty1));
    Double_t py2 = Ty2 * (p2 / TMath::Sqrt(1 + Tx2 * Tx2 + Ty2 * Ty2));

    Double_t pz1 = p1 / TMath::Sqrt(1 + Tx1 * Tx1 + Ty1 * Ty1);
    Double_t pz2 = p2 / TMath::Sqrt(1 + Tx2 * Tx2 + Ty2 * Ty2);

    Double_t px = px1 + px2;
    Double_t py = py1 + py2;
    Double_t pz = pz1 + pz2;

    Double_t p = TMath::Sqrt(px * px + py * py + pz * pz);

    const Double_t m = 1.1157;
    Double_t E = TMath::Sqrt(p * p + m * m);

    pt = TMath::Sqrt(px * px + py * py);

    Y = .5 * TMath::Log((E + pz) / (E - pz));
}
