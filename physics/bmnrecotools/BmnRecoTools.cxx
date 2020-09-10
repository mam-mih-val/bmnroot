

#include "BmnRecoTools.h"

BmnRecoTools::BmnRecoTools() {
}

BmnRecoTools::~BmnRecoTools() {
}

Bool_t BmnRecoTools::IsReconstructable(
        TClonesArray* mcTracks, TClonesArray* gemPoints, TClonesArray* silPoints, TClonesArray* cscPoints,
        Int_t code, vector<Int_t> &outCodes, Int_t minHits) {

    Int_t decaysWritten = 0;

    for (Int_t iTrack = 0; iTrack < mcTracks->GetEntriesFast(); iTrack++) {
        CbmMCTrack* track = (CbmMCTrack*) mcTracks->UncheckedAt(iTrack);
        if (track->GetMotherId() != -1 || track->GetPdgCode() != code)
            continue;
        //                        printf("found %dth track for %d\n", iTrack, track->GetPdgCode());
        vector<Int_t> outHitsCnt; ///<- found hits for the each offspring
        outHitsCnt.resize(outCodes.size(), 0);
        // find the offsprings & count their hits in the inner tracker
        for (Int_t jTrack = 0; jTrack < mcTracks->GetEntriesFast(); jTrack++) {
            CbmMCTrack* offspringTrack = (CbmMCTrack*) mcTracks->UncheckedAt(jTrack);
            if (offspringTrack->GetMotherId() != iTrack)
                continue;
            auto it = find(outCodes.begin(), outCodes.end(), offspringTrack->GetPdgCode());
            if (it == outCodes.end())
                continue;
            Int_t iOffspring = it - outCodes.begin();
            //                                printf("found %dth offspring for %d\n", iOffspring, offspringTrack->GetPdgCode());

            /** Count GEM points */
            UInt_t stations = 0u;
            for (Int_t iPoint = 0; iPoint < gemPoints->GetEntriesFast(); iPoint++) {
                CbmStsPoint* point = (CbmStsPoint*) gemPoints->UncheckedAt(iPoint);
                if (point->GetTrackID() != jTrack)
                    continue;
                //                                printf("found %dth gem point for %d\n", iPoint, offspringTrack->GetPdgCode());
                UInt_t iStation = point->GetStation();
                if (TESTBIT(stations, iStation)) // primitive test whether the track is curly
                    return kFALSE;
                SETBIT(stations, iStation);
                outHitsCnt[iOffspring]++;
            }
            /** Count Silicon points */
            stations = 0u;
            for (Int_t iPoint = 0; iPoint < silPoints->GetEntriesFast(); iPoint++) {
                BmnSiliconPoint* point = (BmnSiliconPoint*) silPoints->UncheckedAt(iPoint);
                if (point->GetTrackID() != jTrack)
                    continue;
                //                                printf("found %dth sil point for %d\n", iPoint, offspringTrack->GetPdgCode());
                UInt_t iStation = point->GetStation();
                if (TESTBIT(stations, iStation))
                    return kFALSE;
                SETBIT(stations, iStation);
                outHitsCnt[iOffspring]++;
            }
            /** Count CSC points */
            //            for (Int_t iPoint = 0; iPoint < cscPoints->GetEntriesFast(); iPoint++) {
            //                BmnCSCPoint* cscPoint = (BmnCSCPoint*) cscPoints->UncheckedAt(iPoint);
            //                if (cscPoint->GetTrackID() != jTrack)
            //                    continue;
            //                //                                printf("found %dth csc point for %d\n", iPoint, offspringTrack->GetPdgCode());
            //                outHitsCnt[iOffspring]++;
            //            }
        }
        Int_t insuff = -1;
        for (Int_t iOffspring = 0; iOffspring < outCodes.size(); iOffspring++) {
            //                                printf("%d counts %d hits\n", iOffspring, outHitsCnt[iOffspring]);
            if (outHitsCnt[iOffspring] < minHits) {
                insuff = iOffspring;
                break;
            }
        }
        if (insuff > -1)
            continue;
        decaysWritten++;
    }
    //        printf("\n Found %d Good Lambda Decays\n", decaysWritten);
    return (decaysWritten == 1);

}

//static void BmnRecoTools::ShowHitMakerEff(TClonesArray * hits, BmnSiliconStationSet set) {
//    
//    vector<vector<vector<Int_t  > > > histSiliconStrip;
//    for (Int_t iStation = 0; iStation < set->GetNStations(); iStation++) {
//        vector<vector<TH1F*> > rowGEM;
//        BmnSiliconStation* st = set->GetSiliconStation(iStation);
//        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
//            vector<UInt_t> colGEM;
//            BmnSiliconModule *mod = st->GetModule(iModule);
//            colGEM.resize(mod->GetNStripLayers(), 0)
//            rowGEM.push_back(colGEM);
//        }
//        histSiliconStrip.push_back(rowGEM);
//    }
//
//    for (Int_t iHit = 0; iHit < hits->GetEntriesFast(); iHit++) {
//        BmnHit *hit = (BmnHit *) hits->UncheckedAt(iHit);
//        hit->Get
//    }
//}

vector<TString> BmnRecoTools::GetFileVecFromDir(TString dir) {
    vector<TString> vec;
    struct dirent **namelist;
    const regex re(".+.r12");
    Int_t n;
    n = scandir(dir, &namelist, 0, alphasort);
    if (n < 0) {
        perror("scandir");
        return vec;
    } else {
        for (Int_t i = 0; i < n; ++i) {
            TString _curFile = TString(namelist[i]->d_name);
            if (regex_match(namelist[i]->d_name, re)) {
                vec.push_back(dir + _curFile);
                //                printf("dir file %d %s\n", i, _curFile.Data());
            }
            free(namelist[i]);
        }
        free(namelist);
    }
    return vec;
}

void BmnRecoTools::FillSetStsPoints(
        TClonesArray* pts,
        BmnGemStripStationSet* set,
        //        vector<TH2* > &hitVec) {
        vector<vector<vector<TH2* > > > &hitVec) {
    //        vector<vector<vector<UInt_t > > > &hitVec) {
    for (Int_t iHit = 0; iHit < pts->GetEntriesFast(); iHit++) {
        CbmStsPoint *pt = (CbmStsPoint *) pts->UncheckedAt(iHit);
        Int_t iSt = pt->GetStation();
        Int_t iMod = pt->GetModule();
        //        Double_t x = ((FairMCPoint*) pt)->GetX();
        //        Double_t y = ((FairMCPoint*) pt)->GetY();
        BmnGemStripModule* mod = set->GetStation(iSt)->GetModule(iMod);
        Double_t driftCenterShift = 0.0;
        if (mod->GetElectronDriftDirection() == ForwardZAxisEDrift)
            driftCenterShift = 0.15;
        else
            driftCenterShift = 0.75;
        Double_t x = pt->GetX(pt->GetZ() + driftCenterShift);
        Double_t y = pt->GetY(pt->GetZ() + driftCenterShift);
        Int_t iLayerOwned = -1;
        for (Int_t iLay = 0; iLay < mod->GetStripLayers().size(); iLay++) {
            BmnGemStripLayer l = mod->GetStripLayer(iLay);
            if (l.IsPointInsideStripLayer(x * (-1.0), y)) {
                iLayerOwned = iLay;
                break;
            }
        }
        if (iLayerOwned == -1)
            continue;
        Int_t iZone = iLayerOwned / 2;
        //        printf("zax = %08X\n",hitVec[pt->GetStation()][pt->GetModule()][iZone]->GetZaxis());
        Double_t Pz = ((FairMCPoint*) pt)->GetPz();
        if (Pz > 0)
            //            hitVec[pt->GetStation()]->Fill(
            //                ((FairMCPoint*) pt)->GetPx() / Pz,
            //                ((FairMCPoint*) pt)->GetPy() / Pz);
            //        hitVec[pt->GetStation()]->Fill(x, y);
            hitVec[pt->GetStation()][pt->GetModule()][iZone]->Fill(x, y);
        //        hitVec[pt->GetStation()][pt->GetModule()][iZone]++;
    }
}

void BmnRecoTools::FillSetStsHits(
        TClonesArray* pts, TClonesArray* hits,
        BmnGemStripStationSet* set,
        //        vector<TH2* > &hitVec,
        //        vector<TH1D* > &hrx,
        //        vector<TH1D* > &hry
        //) {
        vector<vector<vector<TH2* > > > &hitVec) {
    //        vector<vector<vector<UInt_t > > > &hitVec) {

    for (Int_t iPt = 0; iPt < pts->GetEntriesFast(); iPt++) {
        CbmStsPoint *pt = (CbmStsPoint *) pts->UncheckedAt(iPt);
        Int_t iSt = pt->GetStation();
        Int_t iMod = pt->GetModule();
        //        Double_t x = ((FairMCPoint*) pt)->GetX();
        //        Double_t y = ((FairMCPoint*) pt)->GetY();
        Double_t thr = 0.;
        Int_t iLayerOwned = -1;
        BmnGemStripModule* mod = set->GetStation(iSt)->GetModule(iMod);
        Double_t driftCenterShift = 0.0;
        if (mod->GetElectronDriftDirection() == ForwardZAxisEDrift)
            driftCenterShift = 0.15;
        else
            driftCenterShift = 0.75;
        Double_t x = pt->GetX(pt->GetZ() + driftCenterShift);
        Double_t y = pt->GetY(pt->GetZ() + driftCenterShift);
        for (Int_t iLay = 0; iLay < mod->GetStripLayers().size(); iLay++) {
            BmnGemStripLayer l = mod->GetStripLayer(iLay);
            //                printf("%d th layer pitch %f\n", iLay, l.GetPitch());
            if (l.IsPointInsideStripLayer(x * (-1.0), y)) {
                //                layers.push_back(l);
                iLayerOwned = iLay;
                //                printf("%d th layer found\n", iLay);
                if (iLay > 1) // hot zone
                    thr = 0.5;
                else
                    thr = 0.5;
                thr = thr*thr;
                break;
            }
        }
        if (iLayerOwned == -1)
            continue;
        Int_t iZone = iLayerOwned / 2;
        //        printf("Point   %f : %f     thr %f\n", x, y, thr); 
        BmnHit *hitClosest = nullptr;
        Double_t MinDistance = DBL_MAX;
        for (Int_t iHit = 0; iHit < hits->GetEntriesFast(); iHit++) {
            BmnHit *hit = (BmnHit *) hits->UncheckedAt(iHit);
            if (
                    hit->GetFlag() == kFALSE ||
                    hit->GetStation() != iSt ||
                    hit->GetModule() != iMod)
                continue;
            Double_t dist = Sq(hit->GetX() - x) + Sq(hit->GetY() - y);
            //            hrx[pt->GetStation()]->Fill(hit->GetX() - x);
            //            hry[pt->GetStation()]->Fill(hit->GetY() - y);
            //            printf("Hit     %f : %f   dist = %f\n", hit->GetX(), hit->GetY(), dist);
            if (dist < MinDistance) {
                hitClosest = hit;
                MinDistance = dist;
            }
        }
        if (MinDistance < thr) {
            //            printf("Hit     %f : %f   dist = %f   Minimum! \n", hitClosest->GetX(), hitClosest->GetY(), MinDistance);
            Double_t Pz = ((FairMCPoint*) pt)->GetPz();
            if (Pz > 0)
                //                hitVec[pt->GetStation()]->Fill(
                //                    ((FairMCPoint*) pt)->GetPx() / Pz,
                //                    ((FairMCPoint*) pt)->GetPy() / Pz);
                //            hitVec[pt->GetStation()]->Fill(x, y);
                hitVec[pt->GetStation()][pt->GetModule()][iZone]->Fill(x, y);
            //        hitVec[pt->GetStation()][pt->GetModule()][iZone]++;
            hitClosest->SetFlag(kFALSE);
        } else {
            //            printf("\tnot found in st %d mod %d\n", iSt, iMod);
        }
    }
}

TCutG* BmnRecoTools::Ellipse2CutG(TString name, Double_t x, Double_t y, Double_t rx, Double_t ry, Double_t theta) {
    if (ry < 0.0)
        ry = rx;
    const Int_t nIters = 20;
    TCutG *cutg = new TCutG(name, nIters);
    Double_t ct = TMath::Cos(theta);
    Double_t st = TMath::Sin(theta);
    Double_t dphi = 2 * M_PI / nIters;
    Double_t phi = 0.0;
    for (Int_t i = 0; i < nIters - 1; i++) {
        phi += dphi;
        Double_t projX = rx * TMath::Cos(phi);
        Double_t projY = ry * TMath::Sin(phi);
        cutg->SetPoint(i,
                x + projX * ct - projY*st,
                y + projX * st + projY * ct);
    }
    Double_t firstX = 0.0;
    Double_t firstY = 0.0;
    cutg->GetPoint(0, firstX, firstY);
    cutg->SetPoint(nIters - 1, firstX, firstY); // why??
    return cutg;
}

TF1* BmnRecoTools::GetSignalDistribution(TTree *tree, TClonesArray *arMC,
        TTree *treeDST, TClonesArray *gemHits, TClonesArray *gemTracks, TClonesArray *tracks,
        Double_t lowThr, Int_t ClusterSizeThr, Int_t nBins) {
    Double_t minVal = DBL_MAX;
    Double_t maxVal = DBL_MIN;
    if (treeDST && gemHits/* && gemTracks && tracks*/) {
        Double_t val = 0.0;
        if (gemHits->GetClass() == BmnGemStripHit::Class()) {
            for (Long64_t i = 0; i < tree->GetEntries(); i++) {
                treeDST->GetEntry(i);
                for (Int_t iHit = 0; iHit < gemHits->GetEntriesFast(); iHit++) {
                    BmnGemStripHit *hit = (BmnGemStripHit *) gemHits->UncheckedAt(iHit);
                    val = hit->GetStripTotalSignalInLowerLayer();
                    if (val <= lowThr)
                        continue;
                    if (hit->GetClusterSizeInLowerLayer() <= ClusterSizeThr)
                        continue;
                    if (maxVal < val)
                        maxVal = val;
                    if (minVal > val)
                        minVal = val;
                }
            }
        }
        if (gemHits->GetClass() == BmnSiliconHit::Class()) {
            for (Long64_t i = 0; i < tree->GetEntries(); i++) {
                treeDST->GetEntry(i);
                for (Int_t iHit = 0; iHit < gemHits->GetEntriesFast(); iHit++) {
                    BmnSiliconHit *hit = (BmnSiliconHit *) gemHits->UncheckedAt(iHit);
                    val = hit->GetStripTotalSignalInLowerLayer();
                    if (val <= lowThr)
                        continue;
                    if (hit->GetClusterSizeInLowerLayer() <= ClusterSizeThr)
                        continue;
                    if (maxVal < val)
                        maxVal = val;
                    if (minVal > val)
                        minVal = val;
                }
            }

        }
        if (gemHits->GetClass() == BmnCSCHit::Class()) {
            for (Long64_t i = 0; i < tree->GetEntries(); i++) {
                treeDST->GetEntry(i);
                for (Int_t iHit = 0; iHit < gemHits->GetEntriesFast(); iHit++) {
                    BmnCSCHit *hit = (BmnCSCHit *) gemHits->UncheckedAt(iHit);
                    val = hit->GetStripTotalSignalInLowerLayer();
                    if (val <= lowThr)
                        continue;
                    if (hit->GetClusterSizeInLowerLayer() <= ClusterSizeThr)
                        continue;
                    if (maxVal < val)
                        maxVal = val;
                    if (minVal > val)
                        minVal = val;
                }
            }
        }
    } else
        for (Long64_t i = 0; i < tree->GetEntries(); i++) {
            tree->GetEntry(i);
            for (Int_t iDig = 0; iDig < arMC->GetEntriesFast(); iDig++) {
                BmnStripDigit* dig = (BmnStripDigit*) arMC->At(iDig);
                if (dig->GetStripSignal() <= lowThr)
                    continue;
                Double_t val = dig->GetStripSignal();
                if (maxVal < val)
                    maxVal = val;
                if (minVal > val)
                    minVal = val;
            }
        }
    //    minVal = log(minVal);
    //    maxVal = log(maxVal);
    printf("Min  = %f max = %f\n", minVal, maxVal);
    TString name = Form("hSig_%s_%d", arMC->GetName(), rand());
    TH1F *hSig = new TH1F(name, name, nBins, minVal, maxVal);

    if (treeDST && gemHits/* && gemTracks && tracks*/) {
        /** digs from tracks*/
        if (gemHits->GetClass() == BmnGemStripHit::Class()) {
            for (Long64_t i = 0; i < tree->GetEntries(); i++) {
                treeDST->GetEntry(i);
                for (Int_t iHit = 0; iHit < gemHits->GetEntriesFast(); iHit++) {
                    BmnGemStripHit *hit = (BmnGemStripHit *) gemHits->UncheckedAt(iHit);
                    //                for (Int_t iTrack = 0; iTrack < tracks->GetEntriesFast(); iTrack++) {
                    //                    BmnGlobalTrack* track = (BmnGlobalTrack*) tracks->UncheckedAt(iTrack);
                    //                    if (track->GetGemTrackIndex() != -1) {
                    //                        //                for (Int_t iTrack = 0; iTrack < gemTrack->GetEntriesFast(); iTrack++) {
                    //                        BmnTrack * gemTr = (BmnTrack*) gemTracks->UncheckedAt(track->GetGemTrackIndex());
                    //                        for (Int_t iHit = 0; iHit < gemTr->GetNHits(); iHit++) {
                    //                            BmnGemStripHit *hit = (BmnGemStripHit *) gemHits->UncheckedAt(gemTr->GetHitIndex(iHit));
                    if (hit->GetClusterSizeInLowerLayer() <= ClusterSizeThr)
                        continue;
                    Double_t val = hit->GetStripTotalSignalInLowerLayer();
                    if (val <= lowThr)
                        continue;
                    hSig->Fill(val);
                    //                            BmnMatch match = hit->GetDigitNumberMatch();
                    //                            const vector<BmnLink> links = match.GetLinks();
                    //                            for (const BmnLink& link : links) {
                    //                                if (link.GetIndex() > (arMC->GetEntriesFast() - 1)) {
                    //                                    printf("Warning in event %lld  hit %d! Hits Link index out of range! index = %d  entries %d\n",
                    //                                            i, iHit, link.GetIndex(), arMC->GetEntriesFast());
                    //                                    continue;
                    //                                }
                    //                                BmnStripDigit * dig = (BmnStripDigit*) arMC->At(link.GetIndex());
                    //                                if (dig->GetStripSignal() <= lowThr)
                    //                                    continue;
                    //                                if (strcmp(dig->GetName(), ""))
                    //                                    continue;
                    //                                dig->SetName("used");
                    //                                Double_t val = dig->GetStripSignal();
                    //                                hSig->Fill(val);
                    //                            }
                    //                        }
                    //                        //                }
                    //                    }
                }
            }
        }
        if (gemHits->GetClass() == BmnSiliconHit::Class()) {
            for (Long64_t i = 0; i < tree->GetEntries(); i++) {
                treeDST->GetEntry(i);
                for (Int_t iHit = 0; iHit < gemHits->GetEntriesFast(); iHit++) {
                    BmnSiliconHit *hit = (BmnSiliconHit *) gemHits->UncheckedAt(iHit);
                    if (hit->GetClusterSizeInLowerLayer() <= ClusterSizeThr)
                        continue;
                    Double_t val = hit->GetStripTotalSignalInLowerLayer();
                    if (val <= lowThr)
                        continue;
                    hSig->Fill(val);
                }
            }
        }
        if (gemHits->GetClass() == BmnCSCHit::Class()) {
            for (Long64_t i = 0; i < tree->GetEntries(); i++) {
                treeDST->GetEntry(i);
                for (Int_t iHit = 0; iHit < gemHits->GetEntriesFast(); iHit++) {
                    BmnCSCHit *hit = (BmnCSCHit *) gemHits->UncheckedAt(iHit);
                    if (hit->GetClusterSizeInLowerLayer() <= ClusterSizeThr)
                        continue;
                    Double_t val = hit->GetStripTotalSignalInLowerLayer();
                    if (val <= lowThr)
                        continue;
                    hSig->Fill(val);
                }
            }
        }
    } else // all digs
        for (Long64_t i = 0; i < tree->GetEntries(); i++) {
            tree->GetEntry(i);
            for (Int_t iDig = 0; iDig < arMC->GetEntriesFast(); iDig++) {
                BmnStripDigit* dig = (BmnStripDigit*) arMC->At(iDig);
                Double_t val = dig->GetStripSignal();
                if (val <= lowThr)
                    continue;
                //            val = log(val);
                hSig->Fill(val);
            }
        }
    name = Form("hIntSig_%s_%d", arMC->GetName(), rand());
    TH1F hIntSig = TH1F(name, Form("Signal_%s", arMC->GetName()), nBins, minVal - 0.5 * (maxVal - minVal) / (Double_t) nBins, maxVal + 0.5 * (maxVal - minVal) / (Double_t) nBins);
    Double_t bc = 0;
    for (Int_t i = 0; i < nBins; i++) {
        bc += hSig->GetBinContent(i);
        hIntSig.SetBinContent(i, bc);
        //        printf("i = %d  center = %f  content = %f\n", i, hIntSig.GetBinCenter(i), bc);
    }
//    name = "can";
//    gStyle->SetOptStat(0);
//    TCanvas* can = new TCanvas(name, name, 1600, 900 * 2);
//    can->Divide(1, 2);
//    TVirtualPad * padSig = can->cd(1);
//    padSig->SetLogy();
//    padSig->SetLogx();
//    hSig->Scale();
//    hSig->Draw();
//
//    TVirtualPad * padInt = can->cd(2);
//    padInt->SetLogy();
//    padInt->SetLogx();
    if (bc == 0)
        return nullptr;
    hIntSig.Scale(1 / bc);
    hIntSig.SetLineColor(kRed);
//    hIntSig.Draw();

    TF1 *sig = new TF1(Form("SignalInt_%s", arMC->GetName()),
            [hIntSig, bc, minVal, maxVal, nBins](Double_t *x, Double_t * p) {
                //            Double_t val = (bc - x[0])/bc;//log(x[0]);
                if (x[0] >= maxVal) return 1.0;
                Int_t iBin = hIntSig.GetXaxis()->FindBin(x[0]); //(Int_t) floor(nBins * (x[0] - minVal) / (maxVal - minVal));
                Int_t jBin = iBin + ((x[0] > hIntSig.GetBinCenter(iBin)) ? 1 : -1); // next bin along direction from center to x
                Double_t v = hIntSig.GetBinContent(iBin) +
                        (hIntSig.GetBinContent(jBin) - hIntSig.GetBinContent(iBin))*
                        2 * abs(x[0] - hIntSig.GetBinCenter(iBin)) / (hIntSig.GetBinWidth(iBin) + hIntSig.GetBinWidth(jBin));
                //                printf("x = %f bincenter = %f  iBin = %d  jBin  %d  f(ibin) = %f  f(x) = %f\n",
                //                        x[0], hIntSig.GetBinCenter(iBin), iBin, jBin, hIntSig.GetBinContent(iBin), v);
                return v;
            }, minVal, maxVal, 0); //(bc - maxVal) / bc, (bc - minVal) / bc, 0);
    sig->SetNpx(nBins);
    sig->SetLineColor(kBlue);
    //    hIntSig.Fit(sig, "R");
    //    sig->Draw("same");
//    can->Print(Form("sigs_%s_%d.pdf", arMC->GetName(), tree->GetNbranches()));
    delete hSig;
//    delete can;
    return sig;
}

TF1* BmnRecoTools::GetRescaleFunc(TString name, TF1 *mc, TF1 *ex) {
    TF1 *funcRescale = new TF1(name,
            [mc, ex](Double_t *x, Double_t * p) {
                Double_t r = mc->Eval(x[0]);
                Double_t f = ex->GetX(r); //, ex->GetXmin(), ex->GetXmax(), 1.E-15, 100, kTRUE);
                //                printf("x = %f  mc = %f  y = %f\n", x[0], r, f);
                return f;
            }, mc->GetXmin(), mc->GetXmax(), 0);
    return funcRescale;
}

//ClassImp(BmnRecoTools)
