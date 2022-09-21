#include "BmnTrackingQaOffline.h"

BmnTrackingQaOffline::BmnTrackingQaOffline() :
fPeriod(7),
histoMan(nullptr) {
    // Defining trigger map ...
    fTriggerMap["Beam Trigger + BD ( > 0) & Si (> 2)"] = "BT+BD0+FD2";
    fTriggerMap["Beam Trigger + BD (>3) & Si(>3)"] = "BT+BD3+FD3";
    fTriggerMap["Beam Trigger + BD > 1 & Si > 3"] = "BT+BD1+FD3";
    fTriggerMap["Beam Trigger + BD >5"] = "BT+BD5";
    fTriggerMap["Beam Trigger + BD(>1) and Si(>2)"] = "BT+BD1+FD2";
    fTriggerMap["Beam Trigger + BD(>=2) and FD(>=2)"] = "BT+BD1+FD1";
    fTriggerMap["Beam Trigger + BD(>=2) and FD(>=3)"] = "BT+BD1+FD2";
    fTriggerMap["Beam Trigger + BD(>=2) and FD(>=4)"] = "BT+BD1+FD3";
    fTriggerMap["Beam Trigger + BD(>=3)"] = "BT+BD2";
    fTriggerMap["Beam Trigger + BD(>=3) and FD(>=3)"] = "BT+BD2+FD2";
    fTriggerMap["Beam Trigger + BD(>=3) and FD(>=4)"] = "BT+BD2+FD3";
    fTriggerMap["Beam Trigger + BD>3"] = "BT+BD3";
    fTriggerMap["Beam Trigger + Si (> 4)"] = "BT+FD4";
    fTriggerMap["Beam Trigger + Si >3"] = "BT+FD3";
    fTriggerMap["Beam Trigger + Si >= 3"] = "BT+FD2";
    fTriggerMap["Beam Trigger + Veto Off + BD(>=4)"] = "BT+BD3+VetoOff";
    fTriggerMap["Beam Trigger (BC1+BC2+T0+Veto)"] = "BT";
    fTriggerMap["Beam BC1 + BC2"] = "BC1+BC2";
    fTriggerMap["Beam BC1"] = "BC1";
}

BmnTrackingQaOffline::BmnTrackingQaOffline(TString setup, Int_t period, TString ext) :
fPath(nullptr),
histoMan(nullptr),
fPeriod(period),
fSetup(setup),
start(-1),
finish(-1),
hDst(nullptr),
fGlobTracks(nullptr),
fTof400Hits(nullptr),
fTof700Hits(nullptr),
fDchHits(nullptr),
fCscHits(nullptr),
fVertices(nullptr),
fKalman(nullptr),
fMagField(nullptr),
fAna(nullptr),
isEventAnal(kFALSE),
isTrackAnal(kFALSE),
isMatchAnal(kFALSE),
isEoS(kFALSE),
fEosHost("root://ncm.jinr.ru/") {
    
    fAna = new FairRunAna();
    Char_t* geoFileName = (Char_t*) "current_geo_file.root";
    Int_t res_code = UniRun::ReadGeometryFile(fPeriod, 4649, geoFileName);
    if (res_code != 0) {
        cout << "Geometry file can't be read from the database" << endl;
        exit(-1);
    }

    TGeoManager::Import(geoFileName);
    
    // Defining start and finish ...
    if (period == 7 && setup.Contains("BM@N")) {
        start = 3589;
        finish = 5186;
    } else if (period == 7 && setup.Contains("SRC")) {
        start = 2041;
        finish = 3589;
    }

    // Defining trigger map ...
    fTriggerMap["Beam Trigger + BD ( > 0) & Si (> 2)"] = "BT+BD0+FD2";
    fTriggerMap["Beam Trigger + BD (>3) & Si(>3)"] = "BT+BD3+FD3";
    fTriggerMap["Beam Trigger + BD > 1 & Si > 3"] = "BT+BD1+FD3";
    fTriggerMap["Beam Trigger + BD >5"] = "BT+BD5";
    fTriggerMap["Beam Trigger + BD(>1) and Si(>2)"] = "BT+BD1+FD2";
    fTriggerMap["Beam Trigger + BD(>=2) and FD(>=2)"] = "BT+BD1+FD1";
    fTriggerMap["Beam Trigger + BD(>=2) and FD(>=3)"] = "BT+BD1+FD2";
    fTriggerMap["Beam Trigger + BD(>=2) and FD(>=4)"] = "BT+BD1+FD3";
    fTriggerMap["Beam Trigger + BD(>=3)"] = "BT+BD2";
    fTriggerMap["Beam Trigger + BD(>=3) and FD(>=3)"] = "BT+BD2+FD2";
    fTriggerMap["Beam Trigger + BD(>=3) and FD(>=4)"] = "BT+BD2+FD3";
    fTriggerMap["Beam Trigger + BD>3"] = "BT+BD3";
    fTriggerMap["Beam Trigger + Si (> 4)"] = "BT+FD4";
    fTriggerMap["Beam Trigger + Si >3"] = "BT+FD3";
    fTriggerMap["Beam Trigger + Si >= 3"] = "BT+FD2";
    fTriggerMap["Beam Trigger + Veto Off + BD(>=4)"] = "BT+BD3+VetoOff";
    fTriggerMap["Beam Trigger (BC1+BC2+T0+Veto)"] = "BT";
    fTriggerMap["Beam BC1 + BC2"] = "BC1+BC2";
    fTriggerMap["Beam BC1"] = "BC1";

    // Calling histo manager ...
    histoMan = new BmnTrackingQaOfflineDraw();
    if (ext.Contains("root"))
        histoMan->usedOutExtension = ext;
}

TString BmnTrackingQaOffline::GetTrigger(Int_t run) {
    TObjArray* runRecord = ElogRecord::GetRecords(fPeriod, run);

    TString trigger = "";

    TIter it(runRecord);
    ElogRecord* curRecord;
    while ((curRecord = (ElogRecord*) it())) {
        if (curRecord->GetTriggerId())
            trigger = ElogTrigger::GetTrigger(*(curRecord->GetTriggerId()))->GetTriggerInfo();
    }

    auto trigIter = fTriggerMap.find(trigger);

    if (trigIter != fTriggerMap.end())
        trigger = trigIter->second;

    return trigger;
}

pair <Int_t, Int_t> BmnTrackingQaOffline::FindBinXY(TString condition) {

    pair <Int_t, Int_t> binXY = make_pair(-1, -1);

    // Simple matching
    if (condition == "tof400")
        binXY = make_pair(1, 1);
    else if (condition == "tof700")
        binXY = make_pair(2, 2);
    else if (condition == "csc")
        binXY = make_pair(3, 3);
    else if (condition == "dch")
        binXY = make_pair(4, 4);

        // Matching OneToOne
    else if (condition == "tof400+tof700")
        binXY = make_pair(1, 2);
    else if (condition == "tof400+csc")
        binXY = make_pair(1, 3);
    else if (condition == "tof400+dch")
        binXY = make_pair(1, 4);

    else if (condition == "tof700+csc")
        binXY = make_pair(2, 3);
    else if (condition == "tof700+dch")
        binXY = make_pair(2, 4);
    else if (condition == "csc+dch")
        binXY = make_pair(3, 4);

        // Matching TwoToOne
    else if (condition == "tof400+tof700+csc")
        binXY = make_pair(1, 1);
    else if (condition == "tof400+tof700+dch")
        binXY = make_pair(1, 2);
    else if (condition == "tof700+csc+dch")
        binXY = make_pair(2, 2);
    else if (condition == "tof400+csc+dch")
        binXY = make_pair(3, 2);

        // Matching ThreeToOne
    else if (condition == "tof400+tof700+csc+dch")
        binXY = make_pair(1, 1);

    return binXY;
}

void BmnTrackingQaOffline::DoAnalisys(Bool_t anal1, Bool_t anal2, Bool_t anal3) {
    if (fPath.Contains("eos"))
        isEoS = kTRUE;

    isEventAnal = anal1;
    isTrackAnal = anal2;
    isMatchAnal = anal3;

    histoMan->fBeams = fBeams;
    histoMan->fTargets = fTargets;
    histoMan->fTriggers = fTriggers;

    histoMan->PrepareHistos();

    TH1F***** h = histoMan->GetH1();
    TH2F* h2 = histoMan->GetH2();
    TH2F* h3 = histoMan->GetH3();
    TH2F***** h4 = histoMan->GetH4();
    TH1F***** h5 = histoMan->GetH5();

    Long_t nTotalGlobTrack[fBeams.size()][fTargets.size()][fTriggers.size()]; // Total number of glob. tracks for all files to be processed
    for (Int_t iBeam = 0; iBeam < fBeams.size(); iBeam++)
        for (Int_t iTarget = 0; iTarget < fTargets.size(); iTarget++)
            for (Int_t iTrigger = 0; iTrigger < fTriggers.size(); iTrigger++)
                nTotalGlobTrack[iBeam][iTarget][iTrigger] = 0;

    if (!anal1 && !anal2 && !anal3)
        return;

    for (Int_t iDst = start; iDst < finish; iDst++) {
        UniRun* pCurrentRun = UniRun::GetRun(fPeriod, iDst);

        // Check presense of the current run in DB ...
        if (!pCurrentRun)
            continue;

        TString* targ = pCurrentRun->GetTargetParticle();

        // Skipping runs w/o target ...
        if (!targ)
            continue;

        pair <Bool_t, Int_t> isBeamInAnal = make_pair(kFALSE, -1);
        pair <Bool_t, Int_t> isTargetInAnal = make_pair(kFALSE, -1);
        pair <Bool_t, Int_t> isTriggerInAnal = make_pair(kFALSE, -1);

        TString target = *targ;
        TString beam = pCurrentRun->GetBeamParticle();
        TString trigger = GetTrigger(iDst);

        // Check beam, target and trigger ...

        for (Int_t iEle = 0; iEle < fBeams.size(); iEle++) {
            if (fBeams[iEle] == beam) {
                isBeamInAnal.first = kTRUE;
                isBeamInAnal.second = iEle;
                break;
            }
        }

        for (Int_t iEle = 0; iEle < fTargets.size(); iEle++) {
            if (fTargets[iEle] == target) {
                isTargetInAnal.first = kTRUE;
                isTargetInAnal.second = iEle;
                break;
            }
        }

        for (Int_t iEle = 0; iEle < fTriggers.size(); iEle++) {
            if (fTriggers[iEle] == trigger) {
                isTriggerInAnal.first = kTRUE;
                isTriggerInAnal.second = iEle;
                break;
            }
        }

        if (!isBeamInAnal.first || !isTargetInAnal.first || !isTriggerInAnal.first)
            continue;

        // Trying to open dst file ...
        TChain* dst = new TChain("bmndata");

        // Trying to get dst file from EoS ...
        if (isEoS) {
            TString path = fPath + "/" + TString::Format("bmndst_%d.root", iDst);
            TString command = "xrdcp -DIRedirCntTimeout 30 " + fEosHost + path + " .";

            gSystem->Exec(command.Data());
            TString dstFile = TString::Format("bmndst_%d.root", iDst);
            dst->Add(dstFile.Data());
        } else {
            TString path = fPath + "/" + TString::Format("bmndst_%d.root", iDst);
            dst->Add(path.Data());
        }

        if (!dst->GetFile()) {
            delete dst;
            continue;
        }
        
        if (isMatchAnal) {
            // Getting mag. field ...
            fMagField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.root");
            fMagField->SetScale(*pCurrentRun->GetFieldVoltage() / 55.87);
            fAna->SetField(fMagField);
            fMagField->Init();
            fIsField = kTRUE; // FIXME
            
            fKalman = new BmnKalmanFilter();
        }        

        if (isEventAnal)
            h2->Fill(fTriggers[isTriggerInAnal.second], fTargets[isTargetInAnal.second], dst->GetEntries() / 1e6);

        dst->SetBranchAddress("DstEventHeader.", &hDst);
        dst->SetBranchAddress("BmnGlobalTrack", &fGlobTracks);
        dst->SetBranchAddress("BmnVertex", &fVertices);
        dst->SetBranchAddress("BmnTof400Hit", &fTof400Hits);
        dst->SetBranchAddress("BmnTof700Hit", &fTof700Hits);
        dst->SetBranchAddress("BmnDchHit", &fDchHits);
        dst->SetBranchAddress("BmnCSCHit", &fCscHits);

        // Loop over events ...
        UInt_t nEventsWithMoreThanOneTrack = 0;

        if (isEventAnal) {
            for (Int_t iEvent = 0; iEvent < dst->GetEntries(); iEvent++) {

                dst->GetEntry(iEvent);

                if (iEvent % 10000 == 0)
                    cout << "Processing file# " << iDst << " Event# " << iEvent << endl;

                Int_t totMult = fGlobTracks->GetEntriesFast();
                if (totMult > 1)
                    nEventsWithMoreThanOneTrack++;
            }

            h3->Fill(fTriggers[isTriggerInAnal.second], fTargets[isTargetInAnal.second], nEventsWithMoreThanOneTrack / 1e6);
        }

        if (isTrackAnal)
            for (Int_t iEvent = 0; iEvent < dst->GetEntries(); iEvent++) {

                dst->GetEntry(iEvent);

                if (iEvent % 10000 == 0)
                    cout << "Processing file# " << iDst << " Event# " << iEvent << endl;

                CbmVertex* vertex = (CbmVertex*) fVertices->UncheckedAt(0);
                h[isBeamInAnal.second][isTargetInAnal.second][isTriggerInAnal.second][0]->Fill(vertex->GetZ());

                Int_t totMult = fGlobTracks->GetEntriesFast();
                h[isBeamInAnal.second][isTargetInAnal.second][isTriggerInAnal.second][1]->Fill(totMult);

                if (totMult > 1)
                    nEventsWithMoreThanOneTrack++;

                Int_t nPos = 0;
                Int_t nNeg = 0;

                // Loop over glob. tracks ...
                for (Int_t iTrack = 0; iTrack < fGlobTracks->GetEntriesFast(); iTrack++) {
                    BmnGlobalTrack* track = (BmnGlobalTrack*) fGlobTracks->UncheckedAt(iTrack);

                    if (track->GetParamLast()->GetQp() > 0.)
                        nPos++;
                    else
                        nNeg++;

                    h[isBeamInAnal.second][isTargetInAnal.second][isTriggerInAnal.second][4]->Fill(track->GetP());
                }

                if (nPos != 0)
                    h[isBeamInAnal.second][isTargetInAnal.second][isTriggerInAnal.second][2]->Fill(nPos);

                if (nNeg != 0)
                    h[isBeamInAnal.second][isTargetInAnal.second][isTriggerInAnal.second][3]->Fill(nNeg);
            }

        if (isMatchAnal) {
            // Matching ...
            TH2F* H0 = h4[isBeamInAnal.second][isTargetInAnal.second][isTriggerInAnal.second][0];
            TH2F* H1 = h4[isBeamInAnal.second][isTargetInAnal.second][isTriggerInAnal.second][1];
            TH2F* H2 = h4[isBeamInAnal.second][isTargetInAnal.second][isTriggerInAnal.second][2];

            // Beta ...
            TH2F* beta400 = h4[isBeamInAnal.second][isTargetInAnal.second][isTriggerInAnal.second][3];
            TH2F* beta700 = h4[isBeamInAnal.second][isTargetInAnal.second][isTriggerInAnal.second][4];
            
            // Residuals ...
            TH1F** residuals = h5[isBeamInAnal.second][isTargetInAnal.second][isTriggerInAnal.second];

            for (Int_t iEvent = 0; iEvent < dst->GetEntries(); iEvent++) {

                dst->GetEntry(iEvent);

                if (iEvent % 10000 == 0)
                    cout << "Processing file# " << iDst << " Event# " << iEvent << endl;

                // Loop over glob. tracks ...
                nTotalGlobTrack[isBeamInAnal.second][isTargetInAnal.second][isTriggerInAnal.second] += fGlobTracks->GetEntriesFast(); // Increasing num. of glob. tracks by each file event to be processed
                for (Int_t iTrack = 0; iTrack < fGlobTracks->GetEntriesFast(); iTrack++) {
                    BmnGlobalTrack* track = (BmnGlobalTrack*) fGlobTracks->UncheckedAt(iTrack);

                    Double_t b400 = track->GetBeta(1);
                    Double_t b700 = track->GetBeta(2);

                    if (b400 < 1.5)
                        beta400->Fill(TMath::Abs(track->GetP()), b400);
                    if (b700 < 1.5)
                        beta700->Fill(TMath::Abs(track->GetP()), b700);

                    Int_t mTof1 = track->GetTof1HitIndex();
                    Int_t mTof2 = track->GetTof2HitIndex();

                    Int_t mCsc = track->GetCscHitIndex(0);

                    Int_t mDch1 = track->GetDch1TrackIndex();
                    Int_t mDch2 = track->GetDch2TrackIndex();
                    Int_t mDch = track->GetDchTrackIndex();

                    BmnHit* tof400Hit = nullptr;
                    BmnHit* tof700Hit = nullptr;
                    BmnHit* dchHit = nullptr;
                    BmnHit* cscHit = nullptr;

                    // Simple matching ...
                    if (mTof1 != -1) {
                        tof400Hit = (BmnHit*) fTof400Hits->UncheckedAt(mTof1);
                        Double_t alreadyFilled = H0->GetBinContent(FindBinXY("tof400").first, FindBinXY("tof400").second);
                        H0->SetBinContent(FindBinXY("tof400").first, FindBinXY("tof400").second, alreadyFilled + 1.);
                    }

                    if (mTof2 != -1) {
                        tof700Hit = (BmnHit*) fTof700Hits->UncheckedAt(mTof2);
                        Double_t alreadyFilled = H0->GetBinContent(FindBinXY("tof700").first, FindBinXY("tof700").second);
                        H0->SetBinContent(FindBinXY("tof700").first, FindBinXY("tof700").second, alreadyFilled + 1.);
                    }

                    if (mCsc != -1) {
                        cscHit = (BmnHit*) fCscHits->UncheckedAt(mCsc);
                        Double_t alreadyFilled = H0->GetBinContent(FindBinXY("csc").first, FindBinXY("csc").second);
                        H0->SetBinContent(FindBinXY("csc").first, FindBinXY("csc").second, alreadyFilled + 1.);
                    }

                    if (mDch1 != -1 || mDch2 != -1 || mDch != -1) {
                        dchHit = (BmnHit*) fDchHits->UncheckedAt(mDch);
                        Double_t alreadyFilled = H0->GetBinContent(FindBinXY("dch").first, FindBinXY("dch").second);
                        H0->SetBinContent(FindBinXY("dch").first, FindBinXY("dch").second, alreadyFilled + 1.);
                    }

                    // OneTwoOne matching ...
                    if (mTof1 != -1 && mTof2 != -1) {
                        Double_t alreadyFilled = H0->GetBinContent(FindBinXY("tof400+tof700").first, FindBinXY("tof400+tof700").second);
                        H0->SetBinContent(FindBinXY("tof400+tof700").first, FindBinXY("tof400+tof700").second, alreadyFilled + 1.);
                    }

                    if (mTof1 != -1 && mCsc != -1) {
                        Double_t alreadyFilled = H0->GetBinContent(FindBinXY("tof400+csc").first, FindBinXY("tof400+csc").second);
                        H0->SetBinContent(FindBinXY("tof400+csc").first, FindBinXY("tof400+csc").second, alreadyFilled + 1.);
                    }

                    if (mTof1 != -1 && (mDch != -1 || mDch1 != -1 || mDch2 != -1)) {
                        Double_t alreadyFilled = H0->GetBinContent(FindBinXY("tof400+dch").first, FindBinXY("tof400+dch").second);
                        H0->SetBinContent(FindBinXY("tof400+dch").first, FindBinXY("tof400+dch").second, alreadyFilled + 1.);
                    }

                    if (mTof2 != -1 && mCsc != -1) {
                        Double_t alreadyFilled = H0->GetBinContent(FindBinXY("tof700+csc").first, FindBinXY("tof700+csc").second);
                        H0->SetBinContent(FindBinXY("tof700+csc").first, FindBinXY("tof700+csc").second, alreadyFilled + 1.);
                    }

                    if (mTof2 != -1 && (mDch != -1 || mDch1 != -1 || mDch2 != -1)) {
                        Double_t alreadyFilled = H0->GetBinContent(FindBinXY("tof700+dch").first, FindBinXY("tof700+dch").second);
                        H0->SetBinContent(FindBinXY("tof700+dch").first, FindBinXY("tof700+dch").second, alreadyFilled + 1.);
                    }

                    if (mCsc != -1 && (mDch != -1 || mDch1 != -1 || mDch2 != -1)) {
                        Double_t alreadyFilled = H0->GetBinContent(FindBinXY("csc+dch").first, FindBinXY("csc+dch").second);
                        H0->SetBinContent(FindBinXY("csc+dch").first, FindBinXY("csc+dch").second, alreadyFilled + 1.);
                    }

                    // TwoToOne matching ...
                    if (mTof1 != -1 && mTof2 != -1 && mCsc != -1) {
                        Double_t alreadyFilled = H1->GetBinContent(FindBinXY("tof400+tof700+csc").first, FindBinXY("tof400+tof700+csc").second);
                        H1->SetBinContent(FindBinXY("tof400+tof700+csc").first, FindBinXY("tof400+tof700+csc").second, alreadyFilled + 1.);
                    }

                    if (mTof1 != -1 && mTof2 != -1 && (mDch != -1 || mDch1 != -1 || mDch2 != -1)) {
                        Double_t alreadyFilled = H1->GetBinContent(FindBinXY("tof400+tof700+dch").first, FindBinXY("tof400+tof700+dch").second);
                        H1->SetBinContent(FindBinXY("tof400+tof700+dch").first, FindBinXY("tof400+tof700+dch").second, alreadyFilled + 1.);
                    }

                    if (mTof2 != -1 && mCsc != -1 && (mDch != -1 || mDch1 != -1 || mDch2 != -1)) {
                        Double_t alreadyFilled = H1->GetBinContent(FindBinXY("tof700+csc+dch").first, FindBinXY("tof700+csc+dch").second);
                        H1->SetBinContent(FindBinXY("tof700+csc+dch").first, FindBinXY("tof700+csc+dch").second, alreadyFilled + 1.);
                    }

                    if (mTof1 != -1 && mCsc != -1 && (mDch != -1 || mDch1 != -1 || mDch2 != -1)) {
                        Double_t alreadyFilled = H1->GetBinContent(FindBinXY("tof400+csc+dch").first, FindBinXY("tof400+csc+dch").second);
                        H1->SetBinContent(FindBinXY("tof400+csc+dch").first, FindBinXY("tof400+csc+dch").second, alreadyFilled + 1.);
                    }

                    // Matching ThreeToOne
                    if (mTof1 != -1 && mTof2 != -1 && mCsc != -1 && (mDch != -1 || mDch1 != -1 || mDch2 != -1)) {
                        Double_t alreadyFilled = H2->GetBinContent(FindBinXY("tof400+tof700+csc+dch").first, FindBinXY("tof400+tof700+csc+dch").second);
                        H2->SetBinContent(FindBinXY("tof400+tof700+csc+dch").first, FindBinXY("tof400+tof700+csc+dch").second, alreadyFilled + 1.);
                    }

                    // Pushing vector with det. hits ...
                    vector <BmnHit*> outerTrackerHits{tof400Hit, tof700Hit, cscHit, dchHit};
                    map <Int_t, pair <Double_t, Double_t>> detIdxResXY = GetResiduals(track, outerTrackerHits);
                    
                    
                    for (auto detector : detIdxResXY) {                       
                        Int_t idx = detector.first;
                        pair <Double_t, Double_t> resid = detector.second;
                        
                        residuals[2 * idx]->Fill(resid.first);
                        residuals[2 * idx + 1]->Fill(resid.second);                       
                    }
                }
            }
        }

        delete dst;

        if (isEoS)
            gSystem->Exec(Form("rm bmndst_%d.root", iDst));
        
        if (isMatchAnal) {
            delete fKalman;
            delete fMagField;
        }
    }

    // Normalizing in the end ...
    for (Int_t iBeam = 0; iBeam < fBeams.size(); iBeam++)
        for (Int_t iTarget = 0; iTarget < fTargets.size(); iTarget++)
            for (Int_t iTrigger = 0; iTrigger < fTriggers.size(); iTrigger++) {

                if (nTotalGlobTrack[iBeam][iTarget][iTrigger] == 0)
                    continue;

                TH2F* H0 = h4[iBeam][iTarget][iTrigger][0];
                TH2F* H1 = h4[iBeam][iTarget][iTrigger][1];
                TH2F* H2 = h4[iBeam][iTarget][iTrigger][2];

                vector <TH2F*> H{H0, H1, H2};

                for (TH2F* histo : H) {
                    for (Int_t iBinX = 1; iBinX < histo->GetNbinsX() + 1; iBinX++)
                        for (Int_t iBinY = 1; iBinY < histo->GetNbinsY() + 1; iBinY++) {
                            Double_t contentBefore = histo->GetBinContent(iBinX, iBinY);

                            Double_t contentAfter = contentBefore / nTotalGlobTrack[iBeam][iTarget][iTrigger];
                            histo->SetBinContent(iBinX, iBinY, 100. * contentAfter);
                        }
                }
            }
}

map <Int_t, pair <Double_t, Double_t>> BmnTrackingQaOffline::GetResiduals(BmnGlobalTrack* track, vector <BmnHit*> hits) {
    // hits --> tof400, tof700, csc, dch

    map <Int_t, pair <Double_t, Double_t>> detResidXY;
    pair <Double_t, Double_t> residXY = make_pair(-1000., -1000.);

    for (Int_t iDet = 0; iDet < hits.size(); iDet++)
        detResidXY[iDet] = residXY;

    Double_t Qp = track->GetParamLast()->GetQp();
    FairTrackParam par(*(track->GetParamFirst()));

    // Loop over detectors ...
    for (Int_t iDet = 0; iDet < hits.size(); iDet++) {
        if (!hits[iDet])
            continue;

        BmnHit* hit = hits[iDet];

        if (fKalman->TGeoTrackPropagate(&par, hit->GetZ(), (Qp > 0) ? 2212 : -211, nullptr, nullptr, fIsField) == kBMNERROR)
            continue;

        Double_t xRes = par.GetX() - hit->GetX();
        Double_t yRes = par.GetY() - hit->GetY();

        residXY = make_pair(xRes, yRes);

        detResidXY[iDet] = residXY;
    }

    return detResidXY;
}