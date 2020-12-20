#include "BmnTrackConv.h"

BmnTrackConv::BmnTrackConv(Int_t run_period, Int_t run_number, BmnSetup setup) {
    fSetup = setup;
    fPeriodId = run_period;
    fRunId = run_number;
    fDstTreeName = "BMN_DIGIT";
    fPVertexName = "BmnVertex";

    fCBMEvHeaderName = "EventHeader.";
    fCBMoldBMNEvHeaderName = "EventHeaderBmn";
    fCBMGlobalTracksName = "StsTrack";
    fCBMHitsName = "StsHit";
    fCBMClustersName = "StsCluster";
    fCBMDigisName = "StsDigi";

    fBMNEvHeaderName = "DstEventHeader.";
    fBMNGlobalTracksName = "BmnGlobalTrack";
    fBMNGemTracksName = "BmnGemTrack";
    fBMNSilTracksName = "BmnSiliconTrack";
    fBMNCscTracksName = "BmnCscTrack";
    fBMNInnerHitsName = "BmnInnerHits";
    fBMNGemHitsName = "BmnGemStripHit";
    fBMNSilHitsName = "BmnSiliconHit";
    fBMNCscHitsName = "BmnCSCHit";
    fBMNTof400HitsName = "";
    fBMNTof700HitsName = "";
    fBMNDchHitsName = "";
    fBMNMwpcHitsName = "";

    fCBMEvHeader = nullptr;
    fBMNEvHeader = nullptr;
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

/**
 * 
 * fdigiB
 * fdigiF  cluster indices
 * @return 
 */
InitStatus BmnTrackConv::Init() {
    if (fVerbose > 0)
        printf("Primary Vertex Extractor init\n");

    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) Fatal("Init", "FairRootManager is not instantiated");
    fCBMEvHeader = static_cast<FairEventHeader*> (ioman->GetObject(fCBMEvHeaderName));
    fCBMoldBMNEvHeader = static_cast<TClonesArray*> (ioman->GetObject(fCBMoldBMNEvHeaderName));
    fCBMGlobalTracks = static_cast<TClonesArray*> (ioman->GetObject(fCBMGlobalTracksName));
    fCBMHits = static_cast<TClonesArray*> (ioman->GetObject(fCBMHitsName));
    fCBMClusters = static_cast<TClonesArray*> (ioman->GetObject(fCBMClustersName));
    fCBMDigis = static_cast<TClonesArray*> (ioman->GetObject(fCBMDigisName));

    fBMNEvHeader = new DstEventHeader(); //out
    ioman->Register(fBMNEvHeaderName, "", fBMNEvHeader, kTRUE); // last arg: save to file
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

    return kSUCCESS;
}

void BmnTrackConv::Exec(Option_t *option) {
    fBMNGlobalTracks->Delete();
    fBMNSilTracks->Delete();
    fBMNGemTracks->Delete();
    //    fBMNCscTracks->Delete();
    fBMNInnerHits->Delete();
    fBMNSilHits->Delete();
    fBMNGemHits->Delete();
    fBMNCscHits->Delete();

    fMapHit.resize(fCBMHits->GetEntriesFast(), 0);
    // copy event id
    BmnEventHeader * eh = static_cast<BmnEventHeader*> (fCBMoldBMNEvHeader->At(0));
    fBMNEvHeader->SetEventId(eh->GetEventId());
//                    printf("iev %d nhits %d\n", eh->GetEventId(), fMapHit.size());
    // copy hits
    for (Int_t iHit = 0; iHit < fCBMHits->GetEntriesFast(); iHit++) {
        CbmStsHit* cbmHit = static_cast<CbmStsHit*> (fCBMHits->UncheckedAt(iHit));
        Int_t fClusterIndexFront = cbmHit->GetDigi(0);
        Int_t fClusterIndexBack = cbmHit->GetDigi(1);
        CbmStsCluster *clusterF = static_cast<CbmStsCluster*> (fCBMClusters->UncheckedAt(fClusterIndexFront));
        CbmStsCluster *clusterB = static_cast<CbmStsCluster*> (fCBMClusters->UncheckedAt(fClusterIndexBack));
        Int_t iSt = cbmHit->GetStationNr();
        Int_t iMod = cbmHit->GetSectorNr();
//            if ((iSt > 9) || (iSt == 0))
//                printf("\t\tist %d imod %d\n", iSt, iMod);
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
            //            cbmHit->GetRefIndex();
            //            cbmHit->GetSignalDiv();
            //            Int_t fClusterIndexFront = cbmHit->GetDigi(0);
            //            Int_t fClusterIndexBack = cbmHit->GetDigi(1);
            //            CbmStsCluster *clusterF = static_cast<CbmStsCluster*> (fCBMClusters->UncheckedAt(fClusterIndexFront));
            //            CbmStsCluster *clusterB = static_cast<CbmStsCluster*> (fCBMClusters->UncheckedAt(fClusterIndexBack));

            //            CbmStsDigi* cbmDigi = static_cast<CbmStsDigi*>(fCBMHits->UncheckedAt(clusterF->(iDig)));
            Int_t iSt = cbmHit->GetStationNr();
            Int_t iMod = cbmHit->GetSectorNr();
//            if ((iSt > 9) || (iSt == 0))
//                printf("\t\tist %d imod %d\n", iSt, iMod);
            if (iSt > fSilStationSet->GetNStations()) { // behind silicon
                iSt -= fSilStationSet->GetNStations();
                if (iSt > fGemStationSet->GetNStations()) { // behind GEM
                    iSt -= fGemStationSet->GetNStations();
                    //                    BmnCSCHit * hit =
                    //                            new((*fBMNCscHits)[fBMNCscHits->GetEntriesFast()])
                    //                            BmnCSCHit(0,
                    //                            TVector3(cbmHit->GetX(), cbmHit->GetY(), cbmHit->GetZ()),
                    //                            TVector3(cbmHit->GetDx(), cbmHit->GetDy(), cbmHit->GetDz()), 0);
                    //                    hit->SetDetId(kGEM);
                    //                    hit->SetCovXY(cbmHit->GetCovXY());
                    //                    hit->SetTimeStamp(cbmHit->GetTimeStamp());
                    //                    hit->SetTimeStampError(cbmHit->GetTimeStampError());
                    //                    hit->SetStation(iSt - 1);
                    //                    hit->SetModule(CscModCbm2Bmn(iSt, iMod));
                    //                    hit->SetStripTotalSignalInLowerLayer(clusterF->GetQtot());
                    //                    hit->SetStripTotalSignalInUpperLayer(clusterB->GetQtot());
                    //                    hit->SetStripPositionInLowerLayer(clusterF->GetMean());
                    //                    hit->SetStripPositionInUpperLayer(clusterB->GetMean());
                    //                    hit->SetClusterSizeInLowerLayer(clusterF->GetNDigis());
                    //                    hit->SetClusterSizeInUpperLayer(clusterB->GetNDigis());
                    //                    //                    cscTr.AddHit(fBMNCscHits->GetEntriesFast() - 1, hit);
                    BmnHit* hit = static_cast<BmnHit*> (fBMNCscHits->UncheckedAt(fMapHit[iHitArr]));
                    gTrack->AddHit(fMapHit[iHitArr], hit);
                    gTrack->SetCscHitIndex(fMapHit[iHitArr]);

                } else {
                    //                    BmnGemStripHit * hit =
                    //                            new((*fBMNGemHits)[fBMNGemHits->GetEntriesFast()])
                    //                            BmnGemStripHit(0,
                    //                            TVector3(cbmHit->GetX(), cbmHit->GetY(), cbmHit->GetZ()),
                    //                            TVector3(cbmHit->GetDx(), cbmHit->GetDy(), cbmHit->GetDz()), 0);
                    //                    hit->SetDetId(kGEM);
                    //                    hit->SetCovXY(cbmHit->GetCovXY());
                    //                    hit->SetTimeStamp(cbmHit->GetTimeStamp());
                    //                    hit->SetTimeStampError(cbmHit->GetTimeStampError());
                    //                    hit->SetStation(iSt - 1);
                    //                    hit->SetModule(GemModCbm2Bmn(iSt, iMod));
                    //                    hit->SetStripTotalSignalInLowerLayer(clusterF->GetQtot());
                    //                    hit->SetStripTotalSignalInUpperLayer(clusterB->GetQtot());
                    //                    hit->SetStripPositionInLowerLayer(clusterF->GetMean());
                    //                    hit->SetStripPositionInUpperLayer(clusterB->GetMean());
                    //                    hit->SetClusterSizeInLowerLayer(clusterF->GetNDigis());
                    //                    hit->SetClusterSizeInUpperLayer(clusterB->GetNDigis());

                    BmnHit* hit = static_cast<BmnHit*> (fBMNGemHits->UncheckedAt(fMapHit[iHitArr]));
                    gemTr.AddHit(fMapHit[iHitArr], hit);
                    gTrack->AddHit(fMapHit[iHitArr], hit);
                }
            } else {
                //                BmnSiliconHit * hit =
                //                        new((*fBMNSilHits)[fBMNSilHits->GetEntriesFast()])
                //                        BmnSiliconHit(0,
                //                        TVector3(cbmHit->GetX(), cbmHit->GetY(), cbmHit->GetZ()),
                //                        TVector3(cbmHit->GetDx(), cbmHit->GetDy(), cbmHit->GetDz()), 0);
                //                hit->SetDetId(kSILICON);
                //                hit->SetCovXY(cbmHit->GetCovXY());
                //                hit->SetTimeStamp(cbmHit->GetTimeStamp());
                //                hit->SetTimeStampError(cbmHit->GetTimeStampError());
                //                hit->SetStation(iSt - 1);
                //                hit->SetModule(iMod - 1);
                //                hit->SetStripTotalSignalInLowerLayer(clusterF->GetQtot());
                //                hit->SetStripTotalSignalInUpperLayer(clusterB->GetQtot());
                //                hit->SetStripPositionInLowerLayer(clusterF->GetMean());
                //                hit->SetStripPositionInUpperLayer(clusterB->GetMean());
                //                hit->SetClusterSizeInLowerLayer(clusterF->GetNDigis());
                //                hit->SetClusterSizeInUpperLayer(clusterB->GetNDigis());

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
                    iModBmn = (iModCbm < 2) ? 1 : 0;
                    break;
                case 2:
                case 3:
                case 5:
                    iModBmn = (iModCbm > 2) ? 1 : 0;
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
