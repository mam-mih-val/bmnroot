#include "BmnTrackConv.h"

BmnTrackConv::BmnTrackConv(Int_t run_period, Int_t run_number, BmnSetup setup) {
    fSetup = setup;
    fPeriodId = run_period;
    fRunId = run_number;
    fDstTreeName = "BMN_DIGIT";

    fCBMMCEvHeaderName = "MCEventHeader.";
    fCBMEvHeaderName = "EventHeader.";
    fCBMoldBMNEvHeaderName = "EventHeaderBmn";
    fCBMGlobalTracksName = "StsTrack";
    fCBMGlobalTracksCSCName = "StsTrackCSC";
    fCBMHitsName = "StsHit";
    fCBMClustersName = "StsCluster";
    //    fCBMDigisName = "StsDigi";
    fCBMVertexName = "PrimaryVertex.";
    fCBMPointsName = "StsPoint";
    fCBMCSCPointsName = "CSCPoint";
    fCBMBDPointsName = "BdPoint";
    fTof400PointsName = "TOF1Point";

    fBMNMCEvHeaderName = "MCEventHeader.";
    fBMNEvHeaderName = "DstEventHeader.";
    fBMNMCGlobalTracksName = "MCTrack";
    fBMNGlobalTracksName = "BmnGlobalTrack";
    fBMNGemTracksName = "BmnGemTrack";
    fBMNSilTracksName = "BmnSiliconTrack";
    fBMNCscTracksName = "BmnCscTrack";
    fBMNInnerHitsName = "BmnInnerHits";
    fBMNGemHitsName = "BmnGemStripHit";
    fBMNSilHitsName = "BmnSiliconHit";
    fBMNCscHitsName = "BmnCSCHit";
    fBMNVertexName = "BmnVertex";
    fBMNTof400HitsName = "";
    fBMNTof700HitsName = "";
    fBMNDchHitsName = "";
    fBMNMwpcHitsName = "";
    fBMNGemPointsName = "StsPoint";
    fBMNSilPointsName = "SiliconPoint";
    fBMNCSCPointsName = "CSCPoint";
    fBMNBDPointsName = "BdPoint";
    fBMNTof400PointsName = "TOF400Point";

    iEv = -1;

    TString xmlConfFileNameGEM;
    TString xmlConfFileNameSil;
    TString xmlConfFileNameCSC;
    // Select xml configs
    switch (fPeriodId) {
        case 7:
            if (fSetup == kBMNSETUP) {
                xmlConfFileNameGEM = "GemRunSpring2018.xml";
                xmlConfFileNameSil = "SiliconRunSpring2018.xml";
                xmlConfFileNameCSC = "CSCRunSpring2018.xml";
            } else {
                xmlConfFileNameGEM = "GemRunSRCSpring2018.xml";
                xmlConfFileNameSil = "SiliconRunSRCSpring2018.xml";
                xmlConfFileNameCSC = "CSCRunSRCSpring2018.xml";
            }
            break;
        case 6:
            xmlConfFileNameGEM = "GemRunSpring2017.xml";
            xmlConfFileNameSil = "SiliconRunSpring2017.xml";
            break;
        default:
            printf("Error! Unknown config!\n");
            return;
            break;

    }
    // load detector configs from xml
    if (xmlConfFileNameGEM.Length() > 0) {
        xmlConfFileNameGEM = TString(getenv("VMCWORKDIR")) + "/parameters/gem/XMLConfigs/" + xmlConfFileNameGEM;
        printf("xmlConfFileName %s\n", xmlConfFileNameGEM.Data());
        fGemStationSet = new BmnGemStripStationSet(xmlConfFileNameGEM);
    }
    if (xmlConfFileNameSil.Length() > 0) {
        xmlConfFileNameSil = TString(getenv("VMCWORKDIR")) + "/parameters/silicon/XMLConfigs/" + xmlConfFileNameSil;
        printf("xmlConfFileName %s\n", xmlConfFileNameSil.Data());
        fSilStationSet = new BmnSiliconStationSet(xmlConfFileNameSil);
    }
    if (xmlConfFileNameCSC.Length() > 0) {
        xmlConfFileNameCSC = TString(getenv("VMCWORKDIR")) + "/parameters/csc/XMLConfigs/" + xmlConfFileNameCSC;
        printf("xmlConfFileName %s\n", xmlConfFileNameCSC.Data());
        fCscStationSet = new BmnCSCStationSet(xmlConfFileNameCSC);
    }
}

BmnTrackConv::~BmnTrackConv() {
    if (fGemStationSet)
        delete fGemStationSet;
    if (fSilStationSet)
        delete fSilStationSet;
    if (fCscStationSet)
        delete fCscStationSet;

}

InitStatus BmnTrackConv::Init() {
    if (fVerbose > 0)
        printf("CBM-->BM@N DST converter init\n");
    iEv++;

    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        fprintf(stderr, "FairRootManager is not instantiated!");
        return kFATAL;
    }
    fCBMMCEvHeader = static_cast<FairMCEventHeader*> (ioman->GetObject(fCBMEvHeaderName));
    if (fCBMMCEvHeader) {
        isMCEVE = kTRUE;
        printf("This is MC generated file\n");
    } else {
        fCBMEvHeader = static_cast<FairEventHeader*> (ioman->GetObject(fCBMEvHeaderName));
        if (!fCBMEvHeader) {
            fprintf(stderr, "No valid event header found!");
            return kFATAL;
        }
    }
    if (isMCEVE) {
        fCBMGlobalTracks = static_cast<TClonesArray*> (ioman->GetObject(fBMNMCGlobalTracksName));
        fCBMPoints = static_cast<TClonesArray*> (ioman->GetObject(fCBMPointsName));
        fCBMCSCPoints = static_cast<TClonesArray*> (ioman->GetObject(fCBMCSCPointsName));
        fCBMBDPoints = static_cast<TClonesArray*> (ioman->GetObject(fCBMBDPointsName));
        fCBMTof400Points = static_cast<TClonesArray*> (ioman->GetObject(fTof400PointsName));

        fBMNMCEvHeader = new FairMCEventHeader(); //out
        ioman->Register(fBMNMCEvHeaderName, "", fBMNMCEvHeader, kTRUE); // last arg: save to file
        fBMNGlobalTracks = new TClonesArray(CbmMCTrack::Class());
        ioman->Register(fBMNMCGlobalTracksName, "", fBMNGlobalTracks, kTRUE);
        fBMNGemPoints = new TClonesArray(CbmStsPoint::Class());
        ioman->Register(fBMNGemPointsName, "", fBMNGemPoints, kTRUE);
        fBMNSilPoints = new TClonesArray(BmnSiliconPoint::Class());
        ioman->Register(fBMNSilPointsName, "", fBMNSilPoints, kTRUE);
        fBMNCSCPoints = new TClonesArray(BmnCSCPoint::Class());
        ioman->Register(fBMNCSCPointsName, "", fBMNCSCPoints, kTRUE);
        fBMNBDPoints = new TClonesArray(BmnBdPoint::Class());
        ioman->Register(fBMNBDPointsName, "", fBMNBDPoints, kTRUE);
        fBMNTof400Points = new TClonesArray(BmnTOF1Point::Class());
        ioman->Register(fBMNTof400PointsName, "", fBMNTof400Points, kTRUE);
    } else {
        fCBMVertex = static_cast<CbmVertex*> (ioman->GetObject(fCBMVertexName));
        fCBMoldBMNEvHeader = static_cast<TClonesArray*> (ioman->GetObject(fCBMoldBMNEvHeaderName));
        if (!fCBMoldBMNEvHeader)
            isMCDST = kTRUE;
        printf("This is%s MC DST\n", (isMCDST ? "" : " not"));
        fCBMGlobalTracks = static_cast<TClonesArray*> (ioman->GetObject(fCBMGlobalTracksCSCName));
        if (!fCBMGlobalTracks) {
            printf("%s tracks not found, we will use common branch %s\n", fCBMGlobalTracksCSCName.Data(), fCBMGlobalTracksName.Data());
            fCBMGlobalTracks = static_cast<TClonesArray*> (ioman->GetObject(fCBMGlobalTracksName));
        }
        fCBMHits = static_cast<TClonesArray*> (ioman->GetObject(fCBMHitsName));
        fCBMClusters = static_cast<TClonesArray*> (ioman->GetObject(fCBMClustersName));
        //        fCBMDigis = static_cast<TClonesArray*> (ioman->GetObject(fCBMDigisName));
        //        printf("CBM_digi_ptr = 0x%08X\n", fCBMDigis);

        fBMNEvHeader = new DstEventHeader(); //out
        ioman->Register(fBMNEvHeaderName, "", fBMNEvHeader, kTRUE); // last arg: save to file
        fBMNVertex = new TClonesArray(BmnVertex::Class());
        ioman->Register(fBMNVertexName, "", fBMNVertex, kTRUE);
        fBMNGlobalTracks = new TClonesArray(BmnGlobalTrack::Class());
        ioman->Register(fBMNGlobalTracksName, "", fBMNGlobalTracks, kTRUE);
        fBMNGemTracks = new TClonesArray(BmnTrack::Class());
        ioman->Register(fBMNGemTracksName, "", fBMNGemTracks, kTRUE);
        fBMNSilTracks = new TClonesArray(BmnTrack::Class());
        ioman->Register(fBMNSilTracksName, "", fBMNSilTracks, kTRUE);
        //    fBMNCscTracks = new TClonesArray(BmnTrack::Class());
        //    ioman->Register(fBMNCscTracksName, "", fBMNCscTracks, kTRUE);

        //    fBMNMwpcTracks = new TClonesArray(BmnMwpcTrack::Class());
        //   ioman->Register(fBMNMwpcTracksName, "", fBMNMwpcTracks, kTRUE);

        fBMNInnerHits = new TClonesArray(BmnHit::Class());
        ioman->Register(fBMNInnerHitsName, "", fBMNInnerHits, kTRUE);
        fBMNGemHits = new TClonesArray(BmnGemStripHit::Class());
        ioman->Register(fBMNGemHitsName, "", fBMNGemHits, kTRUE);
        fBMNSilHits = new TClonesArray(BmnSiliconHit::Class());
        ioman->Register(fBMNSilHitsName, "", fBMNSilHits, kTRUE);
        fBMNCscHits = new TClonesArray(BmnCSCHit::Class());
        ioman->Register(fBMNCscHitsName, "", fBMNCscHits, kTRUE);
        //    fBMNTof400Hits = new TClonesArray(BmnTofHit::Class());
        //   ioman->Register(fBMNTof400HitsName, "", fBMNTof400Hits, kTRUE);
        //    fBMNTof700Hits = new TClonesArray(BmnTofHit::Class());
        //   ioman->Register(fBMNTof700HitsName, "", fBMNTof700Hits, kTRUE);
        //    fBMNDchHits = new TClonesArray(BmnHit::Class());
        //   ioman->Register(fBMNDchHitsName, "", fBMNDchHits, kTRUE);
        //    fBMNMwpcSegment = new TClonesArray(BmnMwpcSegment::Class());
        //   ioman->Register(fBMNMwpcHitsName, "", fBMNMwpcSegment, kTRUE);
    }
    return kSUCCESS;
}

void BmnTrackConv::Exec(Option_t *option) {
    if (isMCEVE)
        ProcessEVE();
    else
        ProcessDST();
}

void BmnTrackConv::ProcessEVE() {
    fBMNGlobalTracks->Delete();
    fBMNGemPoints->Delete();
    fBMNSilPoints->Delete();
    fBMNCSCPoints->Delete();
    fBMNBDPoints->Delete();
    fBMNTof400Points->Delete();
    // copy event header content
    fBMNMCEvHeader->SetEventID(fCBMMCEvHeader->GetEventID());
    fBMNMCEvHeader->SetRunID(fCBMMCEvHeader->GetRunID());
    TVector3 vertex;
    fCBMMCEvHeader->GetVertex(vertex);
    fBMNMCEvHeader->SetVertex(vertex);
    fBMNMCEvHeader->SetTime(fCBMMCEvHeader->GetT());
    fBMNMCEvHeader->SetB(fCBMMCEvHeader->GetEventID());
    fBMNMCEvHeader->SetNPrim(fCBMMCEvHeader->GetNPrim());
    fBMNMCEvHeader->SetRotX(fCBMMCEvHeader->GetRotX());
    fBMNMCEvHeader->SetRotY(fCBMMCEvHeader->GetRotY());
    fBMNMCEvHeader->SetRotZ(fCBMMCEvHeader->GetRotZ());
    // copy tracks&points
//    printf("absorbing %s  --> %s\n", fCBMGlobalTracks->GetClass()->GetName(), fBMNGlobalTracks->GetClass()->GetName());
    fBMNGlobalTracks->AbsorbObjects(fCBMGlobalTracks);
//    printf("absorbing %s  --> %s\n", fCBMCSCPoints->GetClass()->GetName(), fBMNCSCPoints->GetClass()->GetName());
    fBMNCSCPoints->AbsorbObjects(fCBMCSCPoints);
//    printf("absorbing %s  --> %s\n", fCBMBDPoints->GetClass()->GetName(), fBMNBDPoints->GetClass()->GetName());
    fBMNBDPoints->AbsorbObjects(fCBMBDPoints);
//    printf("absorbing %s  --> %s\n", fCBMTof400Points->GetClass()->GetName(), fBMNTof400Points->GetClass()->GetName());
    fBMNTof400Points->AbsorbObjects(fCBMTof400Points);
    // separate array to Gem and Sil
    for (Int_t i = 0; i < fCBMPoints->GetEntriesFast(); i++) {
        CbmStsPoint *pt = static_cast<CbmStsPoint*> (fCBMPoints->UncheckedAt(i));
        Int_t iSt = pt->GetStation();
        if (iSt == -1) { // Silicon (no station number for silicon in the CBM digitizer =( ) 
            Int_t trackID = pt->GetTrackID();
            Int_t detID = pt->GetDetectorID();
            Int_t eventID = pt->GetEventID();
            Double_t tof = pt->GetTime();
            Double_t len = pt->GetLength();
            Double_t eLoss = pt->GetEnergyLoss();
            Int_t isPrimary = 0; // @TODO
            Double_t charge = 0;
            Int_t pdgId = 0;
            Int_t iMod = pt->GetModule();
            TVector3 posIn;
            TVector3 posOut;
            TVector3 momIn;
            TVector3 momOut;
            pt->Position(posIn);
            pt->PositionOut(posOut);
            pt->Momentum(momIn);
            pt->MomentumOut(momOut);
            BmnSiliconPoint * silPt =
                    new((*fBMNSilPoints)[fBMNSilPoints->GetEntriesFast()])
                    BmnSiliconPoint(trackID, detID,
                    posIn, posOut, momIn, momOut,
                    tof, len, eLoss,
                    isPrimary, charge, pdgId
                    );
            silPt->SetStation(iSt);
            silPt->SetModule(iMod);
            silPt->SetEventID(eventID);
        } else { // GEM
            CbmStsPoint * gemPt =
                    new((*fBMNGemPoints)[fBMNGemPoints->GetEntriesFast()])
                    CbmStsPoint(*pt);
        }
    }

}

void BmnTrackConv::ProcessDST() {
    fBMNGlobalTracks->Delete();
    fBMNSilTracks->Delete();
    fBMNGemTracks->Delete();
    //    fBMNCscTracks->Delete();
    fBMNInnerHits->Delete();
    fBMNSilHits->Delete();
    fBMNGemHits->Delete();
    fBMNCscHits->Delete();
    fBMNVertex->Delete();

    fMapHit.resize(fCBMHits->GetEntriesFast(), 0);
    // copy event id
    if (isMCDST) {
        fBMNEvHeader->SetEventId(iEv);
        fBMNEvHeader->SetEventTime(fCBMEvHeader->GetEventTime());
        fBMNEvHeader->SetRunId(fCBMEvHeader->GetRunId());
    } else {
        BmnEventHeader * eh = static_cast<BmnEventHeader*> (fCBMoldBMNEvHeader->At(0));
        fBMNEvHeader->SetEventId(eh->GetEventId());
        fBMNEvHeader->SetEventTime(eh->GetEventTime());
        fBMNEvHeader->SetEventTimeTS(eh->GetEventTimeTS());
        fBMNEvHeader->SetRunId(eh->GetRunId());
    }
    // copy vertex
    CbmVertex* vtxCBM = static_cast<CbmVertex*> (fCBMVertex);
    TMatrixFSym cov(3);
    vtxCBM->CovMatrix(cov);
    BmnVertex * vtx = new((*fBMNVertex)[fBMNVertex->GetEntriesFast()])BmnVertex(
            vtxCBM->GetX(),
            vtxCBM->GetY(),
            vtxCBM->GetZ(),
            vtxCBM->GetChi2(),
            vtxCBM->GetNDF(),
            vtxCBM->GetNTracks(),
            cov,
            -1,
            vtxCBM->GetTrackInds());

    // copy hits
    for (Int_t iHit = 0; iHit < fCBMHits->GetEntriesFast(); iHit++) {
        CbmStsHit* cbmHit = static_cast<CbmStsHit*> (fCBMHits->UncheckedAt(iHit));
        Int_t fClusterIndexFront = cbmHit->GetDigi(0);
        Int_t fClusterIndexBack = cbmHit->GetDigi(1);
        CbmStsCluster *clusterF = static_cast<CbmStsCluster*> (fCBMClusters->UncheckedAt(fClusterIndexFront));
        CbmStsCluster *clusterB = static_cast<CbmStsCluster*> (fCBMClusters->UncheckedAt(fClusterIndexBack));
        Int_t iSt = cbmHit->GetStationNr();
        Int_t iMod = cbmHit->GetSectorNr();
        if (iSt > fSilStationSet->GetNStations()) { // behind silicon
            iSt -= fSilStationSet->GetNStations();
            if (iSt > fGemStationSet->GetNStations()) { // behind GEM
                iSt -= fGemStationSet->GetNStations();
                BmnCSCHit * hit =
                        new((*fBMNCscHits)[fBMNCscHits->GetEntriesFast()])
                        BmnCSCHit(0,
                        TVector3(cbmHit->GetX(), cbmHit->GetY(), cbmHit->GetZ()),
                        TVector3(cbmHit->GetDx(), cbmHit->GetDy(), cbmHit->GetDz()), 0);
                fMapHit[iHit] = fBMNCscHits->GetEntriesFast() - 1;
                hit->SetDetId(kGEM);
                hit->SetCovXY(cbmHit->GetCovXY());
                hit->SetTimeStamp(cbmHit->GetTimeStamp());
                hit->SetTimeStampError(cbmHit->GetTimeStampError());
                hit->SetStation(iSt - 1);
                hit->SetModule(CscModCbm2Bmn(iSt, iMod));
                hit->SetStripTotalSignalInLowerLayer(clusterF->GetQtot());
                hit->SetStripTotalSignalInUpperLayer(clusterB->GetQtot());
                hit->SetStripPositionInLowerLayer(clusterF->GetMean());
                hit->SetStripPositionInUpperLayer(clusterB->GetMean());
                hit->SetClusterSizeInLowerLayer(clusterF->GetNDigis());
                hit->SetClusterSizeInUpperLayer(clusterB->GetNDigis());

            } else {
                BmnGemStripHit * hit =
                        new((*fBMNGemHits)[fBMNGemHits->GetEntriesFast()])
                        BmnGemStripHit(0,
                        TVector3(cbmHit->GetX(), cbmHit->GetY(), cbmHit->GetZ()),
                        TVector3(cbmHit->GetDx(), cbmHit->GetDy(), cbmHit->GetDz()), 0);
                fMapHit[iHit] = fBMNGemHits->GetEntriesFast() - 1;
                hit->SetDetId(kGEM);
                hit->SetCovXY(cbmHit->GetCovXY());
                hit->SetTimeStamp(cbmHit->GetTimeStamp());
                hit->SetTimeStampError(cbmHit->GetTimeStampError());
                hit->SetStation(iSt - 1);
                hit->SetModule(GemModCbm2Bmn(iSt, iMod));
                hit->SetStripTotalSignalInLowerLayer(clusterF->GetQtot());
                hit->SetStripTotalSignalInUpperLayer(clusterB->GetQtot());
                hit->SetStripPositionInLowerLayer(clusterF->GetMean());
                hit->SetStripPositionInUpperLayer(clusterB->GetMean());
                hit->SetClusterSizeInLowerLayer(clusterF->GetNDigis());
                hit->SetClusterSizeInUpperLayer(clusterB->GetNDigis());
            }
        } else {
            BmnSiliconHit * hit =
                    new((*fBMNSilHits)[fBMNSilHits->GetEntriesFast()])
                    BmnSiliconHit(0,
                    TVector3(cbmHit->GetX(), cbmHit->GetY(), cbmHit->GetZ()),
                    TVector3(cbmHit->GetDx(), cbmHit->GetDy(), cbmHit->GetDz()), 0);
            fMapHit[iHit] = fBMNSilHits->GetEntriesFast() - 1;
            hit->SetDetId(kSILICON);
            hit->SetCovXY(cbmHit->GetCovXY());
            hit->SetTimeStamp(cbmHit->GetTimeStamp());
            hit->SetTimeStampError(cbmHit->GetTimeStampError());
            hit->SetStation(iSt - 1);
            hit->SetModule(iMod - 1);
            hit->SetStripTotalSignalInLowerLayer(clusterF->GetQtot());
            hit->SetStripTotalSignalInUpperLayer(clusterB->GetQtot());
            hit->SetStripPositionInLowerLayer(clusterF->GetMean());
            hit->SetStripPositionInUpperLayer(clusterB->GetMean());
            hit->SetClusterSizeInLowerLayer(clusterF->GetNDigis());
            hit->SetClusterSizeInUpperLayer(clusterB->GetNDigis());
        }

    }
    // recreate track structure
    for (Int_t iTrack = 0; iTrack < fCBMGlobalTracks->GetEntriesFast(); iTrack++) {
        //                    printf("iTrack %d\n", iTrack);
        CbmStsTrack* cbmTrack = static_cast<CbmStsTrack*> (fCBMGlobalTracks->UncheckedAt(iTrack));
        BmnTrack silTr;
        BmnTrack gemTr;
        BmnTrack cscTr;
        BmnGlobalTrack * gTrack =
                new((*fBMNGlobalTracks)[fBMNGlobalTracks->GetEntriesFast()])BmnGlobalTrack();
        gTrack->SetParamFirst(*(cbmTrack->GetParamFirst()));
        gTrack->SetParamLast(*(cbmTrack->GetParamLast()));
        gTrack->SetPDG(cbmTrack->GetPidHypo());
        gTrack->SetChi2(cbmTrack->GetChi2());
        gTrack->SetNDF(cbmTrack->GetNDF());

        for (Int_t iHit = 0; iHit < cbmTrack->GetNStsHits(); iHit++) {
            Int_t iHitArr = cbmTrack->GetStsHitIndex(iHit);
            CbmStsHit* cbmHit = static_cast<CbmStsHit*> (fCBMHits->UncheckedAt(iHitArr));
            Int_t iSt = cbmHit->GetStationNr();
            Int_t iMod = cbmHit->GetSectorNr();
            if (iSt > fSilStationSet->GetNStations()) { // behind silicon
                iSt -= fSilStationSet->GetNStations();
                if (iSt > fGemStationSet->GetNStations()) { // behind GEM
                    iSt -= fGemStationSet->GetNStations();
                    BmnHit* hit = static_cast<BmnHit*> (fBMNCscHits->UncheckedAt(fMapHit[iHitArr]));
                    gTrack->AddHit(fMapHit[iHitArr], hit);
                    gTrack->SetCscHitIndex(fMapHit[iHitArr]);

                } else {
                    BmnHit* hit = static_cast<BmnHit*> (fBMNGemHits->UncheckedAt(fMapHit[iHitArr]));
                    gemTr.AddHit(fMapHit[iHitArr], hit);
                    gTrack->AddHit(fMapHit[iHitArr], hit);
                }
            } else {
                BmnHit* hit = static_cast<BmnHit*> (fBMNSilHits->UncheckedAt(fMapHit[iHitArr]));
                silTr.AddHit(fMapHit[iHitArr], hit);
                gTrack->AddHit(fMapHit[iHitArr], hit);

            }
        }

        silTr.SortHits();
        gemTr.SortHits();
        cscTr.SortHits();
        if (silTr.GetNHits()) {
            new ((*fBMNSilTracks)[fBMNSilTracks->GetEntriesFast()]) BmnTrack(silTr);
            gTrack->SetSilTrackIndex(fBMNSilTracks->GetEntriesFast() - 1);
        }
        if (gemTr.GetNHits()) {
            new ((*fBMNGemTracks)[fBMNGemTracks->GetEntriesFast()]) BmnTrack(gemTr);
            gTrack->SetGemTrackIndex(fBMNGemTracks->GetEntriesFast() - 1);
        }
        gTrack->SortHits();
        //        if (cscTr.GetNHits()){ // will work someday
        //            new ((*fBMNSilTracks)[fBMNSilTracks->GetEntriesFast()]) BmnTrack(cscTr);
        //            gTrack->SetSilTrackIndex(fBMNSilTracks->GetEntriesFast() - 1);
        //        }

    }
}

void BmnTrackConv::Finish() {
}

void BmnTrackConv::FinishEvent() {

}

Int_t BmnTrackConv::GemModCbm2Bmn(Int_t iSt, Int_t iModCbm) {
    Int_t iModBmn = 0;
    switch (fPeriodId) {
        case 7:
            switch (iSt) {
                case 1:
                case 4:
                case 6:
                    iModBmn = (4 - iModCbm) / 2;
                    break;
                case 2:
                case 3:
                case 5:
                    iModBmn = (iModCbm - 1) / 2;
                    break;
                default:
                    break;
            }
            break;
        case 6:
            break;
        default:
            break;
    }
    return iModBmn;
}

Int_t BmnTrackConv::CscModCbm2Bmn(Int_t iSt, Int_t iModCbm) {
    Int_t iModBmn = 0;
    switch (fPeriodId) {
        case 7:
            iModBmn = (3 - iModCbm) / 2;
            break;
        default:
            break;
    }
    return iModBmn;
}

//Int_t BmnTrackConv::CalcClusterSize(CbmStsCluster* cluster, TClonesArray* digiAr){
//for (Int_t iDigi = 0; iDigi < cluster->GetNDigis(); iDigi++) {
//    Int_t digiIndex = cluster->GetDigi(iDigi);
//    CbmStsDigi *digi = static_cast<CbmStsDigi*>(digiAr->UncheckedAt(digiIndex));
//    Int_t chNr = digi->GetChannelNr();
//    //Int_t station = digi->GetStationNr();
//    //Int_t sector = digi->GetSectorNr();
//    //Int_t side = digi->GetSide();
//    //Int_t adc = digi->GetAdc();
//}
//    
//}


ClassImp(BmnTrackConv)
