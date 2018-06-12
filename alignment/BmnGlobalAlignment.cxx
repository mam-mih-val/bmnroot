// @(#)bmnroot/alignment:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-03-31

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnGlobalAlignment                                                         //
//                                                                            //
// Alignment of tracking detectors.                                           //
//                                                                            //
// Uses Volker Blobel and Claus Kleinwort Millepede II                        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#include <iconv.h>
#include <TGraph.h>
#include <TStyle.h>
#include "BmnGlobalAlignment.h"
#include "CbmVertex.h"
#include "BmnSiliconHit.h"

Int_t BmnGlobalAlignment::fCurrentEvent = 0;
Int_t BmnGlobalAlignment::trackCounter = 0;

void BmnGlobalAlignment::Finish() {
    if (fIsField) {
        delete fKalman;
        delete fMagField;
    }
}

BmnGlobalAlignment::~BmnGlobalAlignment() {
    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
        delete [] fixedGemElements[iStat];
    delete [] fixedGemElements;

    delete [] fDetectorSet;
    delete mwpcGeo;
    delete fDetectorGEM;
    delete fDetectorSI;
    if (Labels) {
        delete [] Labels;
        system("cp millepede.res Millepede.res");
        // Remove useless text files finalizing the code execution
        system("rm millepede.*");
    }
}

BmnGlobalAlignment::BmnGlobalAlignment(TString inFileName) :
fMwpcHits(NULL),
fGemHits(NULL),
fTof1Hits(NULL),
fDchHits(NULL),
fMwpcTracks(NULL),
fGemTracks(NULL),
fDchTracks(NULL),
fGlobalTracks(NULL),
fMwpcAlignCorr(NULL),
fGemAlignCorr(NULL),
fTofAlignCorr(NULL),
fDchAlignCorr(NULL),
fUseTrackWithMinChi2(kFALSE),
fTxMin(-LDBL_MAX),
fTxMax(LDBL_MAX),
fTyMin(-LDBL_MAX),
fTyMax(LDBL_MAX),
fTxLeft(0.),
fTyLeft(0.),
fTxRight(0.),
fTyRight(0.),
fIsExcludedTx(kFALSE),
fIsExcludedTy(kFALSE),
fMinHitsAccepted(3),
fChi2MaxPerNDF(LDBL_MAX),
nSelectedTracks(0),
fUseRealHitErrors(kFALSE),
fNumOfIterations(50000),
fAccuracy(1e-3),
fPreSigma(1.),
fUseRegularization(kFALSE),
fHugecut(50.),
fEntries(10),
fNGL(0),
fNLC(4),
fOutlierdownweighting(0),
fDwfractioncut(0.0),
fDebug(kFALSE),
fIsField(kFALSE),
fMagField(NULL),
fField(NULL),
fKalman(NULL),
Labels(NULL),
fUseVp(kTRUE),
fUseGemConstraints(kTRUE),
fUseSiliconConstraints(kTRUE) {
    fRecoFileName = inFileName;

    fBranchMwpcHits = "BmnMwpcHit";
    fBranchSiHits = "BmnSiliconHit";
    fBranchGemHits = "BmnGemStripHit";
    fBranchTof1Hits = "BmnTof1Hit";
    fBranchDchHits = "BmnDchHit";

    fBranchMwpcTracks = "BmnMwpcTrack";
    fBranchGemTracks = "BmnGemTrack";
    fBranchDchTracks = "BmnDchTrack";
    fBranchGlobalTracks = "BmnGlobalTrack";

    fBranchMwpcAlignCorr = "BmnMwpcAlignCorrections";
    fBranchGemAlignCorr = "BmnGemAlignCorrections";
    fBranchTofAlignCorr = "BmnTofAlignCorrections";
    fBranchDchAlignCorr = "BmnDchAlignCorrections";
    fBranchSiAlignCorr = "BmnSiliconAlignCorrections";

    fBranchGemResiduals = "BmnResiduals";
    fBranchFairEventHeader = "EventHeader.";

    fBranchVertex = "BmnVertex";

    CreateDetectorGeometries();
}

InitStatus BmnGlobalAlignment::Init() {
    cout << " BmnGlobalAlignment::Init() " << endl;
    cout << "Use detectors: MWPC - " << fDetectorSet[1] << " GEM - " <<
            fDetectorSet[0] << " DCH - " << fDetectorSet[2] << " PRIMARY_VERTEX - " << fDetectorSet[3] <<
            " SILICON - " << fDetectorSet[4] << endl;

    TChain* chain = new TChain("cbmsim");
    chain->Add(fRecoFileName.Data());
    FairEventHeader* evHeader = NULL;
    chain->SetBranchAddress(fBranchFairEventHeader.Data(), &evHeader);
    chain->GetEntry(0);
    fRunId = evHeader->GetRunId();
    delete chain;

    Double_t fieldVolt = 0.;
    UniDbRun* runInfo = NULL;
    if (fRunId != 0) {
        runInfo = UniDbRun::GetRun(7, fRunId); // FIXME
        if (!runInfo)
            throw;
        fieldVolt = *runInfo->GetFieldVoltage();
        fIsField = (fieldVolt > 10.) ? kTRUE : kFALSE;
    }

    FairRootManager* ioman = FairRootManager::Instance();

    fMwpcHits = (TClonesArray*) ioman->GetObject(fBranchMwpcHits.Data());
    fMwpcTracks = (TClonesArray*) ioman->GetObject(fBranchMwpcTracks.Data());

    fSiHits = (TClonesArray*) ioman->GetObject(fBranchSiHits.Data());

    fGemHits = (TClonesArray*) ioman->GetObject(fBranchGemHits.Data());
    fGemTracks = (TClonesArray*) ioman->GetObject(fBranchGemTracks.Data());

    fTof1Hits = (TClonesArray*) ioman->GetObject(fBranchTof1Hits.Data());

    fDchHits = (TClonesArray*) ioman->GetObject(fBranchDchHits.Data());
    fDchTracks = (TClonesArray*) ioman->GetObject(fBranchDchTracks.Data());

    fGlobalTracks = (TClonesArray*) ioman->GetObject(fBranchGlobalTracks.Data());

    fFairEventHeader = (FairEventHeader*) ioman->GetObject(fBranchFairEventHeader.Data());

    fGemResiduals = (TClonesArray*) ioman->GetObject(fBranchGemResiduals.Data());

    fMwpcAlignCorr = new TClonesArray(fBranchMwpcAlignCorr.Data());
    fGemAlignCorr = new TClonesArray(fBranchGemAlignCorr.Data());
    fDchAlignCorr = new TClonesArray(fBranchDchAlignCorr.Data());
    fSiAlignCorr = new TClonesArray(fBranchSiAlignCorr.Data());

    ioman->Register(fBranchMwpcAlignCorr.Data(), "MWPC", fMwpcAlignCorr, kTRUE);
    ioman->Register(fBranchGemAlignCorr.Data(), "GEM", fGemAlignCorr, kTRUE);
    ioman->Register(fBranchSiAlignCorr.Data(), "SI", fSiAlignCorr, kTRUE);
    ioman->Register(fBranchDchAlignCorr.Data(), "DCH", fDchAlignCorr, kTRUE);

    fChain = ioman->GetInChain();
    fin_txt = fopen("alignment.txt", "w");

    fCanv = new TCanvas("c1", "c1", 1200, 800);
    fCanv->Divide(2, 1);

    // Read current geometry (RunSpring2017) from database
    Char_t* geoFileName = (Char_t*) "current_geo_file.root";
    Int_t res_code = UniDbRun::ReadGeometryFile(7, fRunId, geoFileName); // FIXME
    if (res_code != 0) {
        cout << "Geometry file can't be read from the database" << endl;
        exit(-1);
    }
    TGeoManager::Import(geoFileName);

    fVertex = (TClonesArray*) ioman->GetObject(fBranchVertex.Data());
    fKalman = new BmnKalmanFilter();
    return kSUCCESS;
}

void BmnGlobalAlignment::Exec(Option_t* opt) {
    fFairEventHeader->SetMCEntryNumber(fCurrentEvent);
    fFairEventHeader->SetRunId(fRunId);
    fCurrentEvent++;
    if (fCurrentEvent % 1000 == 0)
        cout << "Event# = " << fCurrentEvent << endl;

    Bool_t isEventRequestedType = kFALSE;
    // To be used for rough alignment (1205 + 1233 in RUN6)
    // 2 tracks simultaneously and Tx1 * Tx2 < 0
    Int_t nGemTracksInEvent = 2;
    if (fGemTracks->GetEntriesFast() == nGemTracksInEvent) {
        Double_t Tx1 = ((BmnGemTrack*) fGemTracks->UncheckedAt(0))->GetParamFirst()->GetTx();
        Double_t Tx2 = ((BmnGemTrack*) fGemTracks->UncheckedAt(1))->GetParamFirst()->GetTx();
        if (Tx1 * Tx2 < 0.)
            isEventRequestedType = kTRUE;
    }

    // Choose a single track with min. chi2-value if necessary (in case of target run)
    // This part of code should be removed a.s.a.p
    Double_t Chi2Min = LDBL_MAX;
    Int_t trID = 0;
    if (fUseTrackWithMinChi2)
        for (Int_t iGlobTrack = 0; iGlobTrack < fGlobalTracks->GetEntriesFast(); iGlobTrack++) {
            BmnGlobalTrack* globTrack = (BmnGlobalTrack*) fGlobalTracks->UncheckedAt(iGlobTrack);
            if (globTrack->GetChi2() < Chi2Min) {
                Chi2Min = globTrack->GetChi2();
                trID = iGlobTrack;
            }
        }

    Bool_t isMerged = (fFairEventHeader->GetRunId() == 0) ? kTRUE : kFALSE;
    for (Int_t iGlobTrack = 0; iGlobTrack < fGlobalTracks->GetEntriesFast(); iGlobTrack++) {
        if (isMerged && !isEventRequestedType)
            break;
        BmnGlobalTrack* globTrack = (BmnGlobalTrack*) fGlobalTracks->UncheckedAt(iGlobTrack);
        if (fUseTrackWithMinChi2 && iGlobTrack != trID)
            continue;
        FairTrackParam* params = globTrack->GetParamFirst();

        Double_t chi2 = globTrack->GetChi2();
        Int_t ndf = globTrack->GetNDF();
        Int_t nHits = globTrack->GetNHits();
        Double_t Tx = params->GetTx();
        Double_t Ty = params->GetTy();

        // Use track constraints if necessary
        if (Tx < fTxMin || Tx > fTxMax || Ty < fTyMin || Ty > fTyMax || nHits < fMinHitsAccepted || chi2 / ndf > fChi2MaxPerNDF)
            continue;

        // Exclude a range from the selected range of track. params (in order not to take into account tracks with almost zero values of track params.)
        if (fIsExcludedTx && Tx > fTxLeft && Tx < fTxRight)
            continue;
        if (fIsExcludedTy && Ty > fTyLeft && Ty < fTyRight)
            continue;

        Int_t idx[] = {globTrack->GetGemTrackIndex(), globTrack->GetMwpcTrackIndex(), globTrack->GetDchTrackIndex(),
            globTrack->GetGemTrackIndex(), globTrack->GetSilHitIndex()};

        Char_t buff[50000] = {""};
        Bool_t zhopa = kFALSE;

        for (Int_t iDet = 0; iDet < nDetectors; iDet++) {
            TString detName = (iDet == 0) ? "GEM" : (iDet == 1) ? "MWPC" : (iDet == 2) ? "DCH" :
                    (iDet == 3) ? "VERTEX" : (iDet == 4) ? "SILICON" : "";
            if (fDetectorSet[iDet])
                if (idx[iDet] != -1) {
                    sprintf(buff, "%s%s\n", buff, detName.Data());
                    if (!fIsField)
                        MilleNoFieldRuns(globTrack, idx[iDet], iDet, buff);
                    else {
                        if (!MilleFieldRuns(idx[iDet], iDet, buff)) {
                            sprintf(buff, "%s", "");
                            break;
                        }
                    }
                } else {
                    zhopa = kTRUE;
                    break;
                } else
                PrintToFullFormat(detName, buff);
        }
        if (!zhopa) {
            fprintf(fin_txt, "%s", buff);
            nSelectedTracks++;
        }
    }

    if (fChain->GetEntries() == fCurrentEvent) {
        fclose(fin_txt);
        MakeBinFile();
        MakeSteerFile();
        cout << "Num. of tracks to be used: " << nSelectedTracks << endl;
        Pede();
    }
}

void BmnGlobalAlignment::PrintToFullFormat(TString detName, Char_t* buff) {
    // 2 means two directions (x, y) with values to be depended on presence of MagField
    // fIsMagField --> (YZ-plane: Y0, Ty (2); XZ-plane: c (1), Tx^{i} i = 0 .. 5 (6). Totally: 2 and 7)
    const Int_t nCases = 2;
    Int_t zeroLoc[nCases] = {(!fIsField) ? fNLC : 2, (!fIsField) ? fNLC : 2};
    // Int_t zeroLoc[nCases] = {(!fIsField) ? fNLC : 6, (!fIsField) ? fNLC : 2};
    Int_t zeroGem = 0;
    Int_t zeroMwpc = 3; // To be fixed !!!
    Int_t zeroDch = 3; // To be fixed !!!
    Int_t zeroVertex = 3; // To be fixed !!!
    Int_t zeroSi = 0;
    Int_t nMeas = 2;

    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
        zeroGem += 3 * fDetectorGEM->GetGemStation(iStat)->GetNModules();

    for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++)
        zeroSi += 3 * fDetectorSI->GetSiliconStation(iStat)->GetNModules();

    Int_t zeroTot[nCases] = {zeroLoc[0] + zeroGem + zeroMwpc + zeroDch + zeroVertex + zeroSi + nMeas, zeroLoc[1] + zeroGem + zeroMwpc + zeroDch + zeroVertex + zeroSi + nMeas};
    TString zeroLine[nCases] = {"", ""};

    for (Int_t iCase = 0; iCase < nCases; iCase++)
        for (Int_t iZero = 0; iZero < zeroTot[iCase]; iZero++)
            zeroLine[iCase] += "0. ";

    sprintf(buff, "%s%s\n", buff, detName.Data());

    if (detName == "SILICON") {
        for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++)
            for (Int_t iMod = 0; iMod < fDetectorSI->GetSiliconStation(iStat)->GetNModules(); iMod++)
                for (Int_t iRow = 0; iRow < nCases; iRow++)
                    sprintf(buff, "%s%d %d %s\n", buff, iStat, iMod, zeroLine[iRow].Data());
    } else
        for (Int_t iRow = 0; iRow < nCases; iRow++)
            sprintf(buff, "%s%s\n", buff, zeroLine[iRow].Data());
}

FairTrackParam BmnGlobalAlignment::UseKalman(BmnGemTrack* track, Int_t iHit, Int_t sign) {
    const Double_t delta = 1.; // in cm.
    FairTrackParam* zToBePredicted = track->GetParamFirst();
    BmnGemStripHit* hit = (BmnGemStripHit*) fGemHits->UncheckedAt(track->GetHitIndex(iHit));

    fKalman->TGeoTrackPropagate(zToBePredicted, hit->GetZ() + sign * delta, 2212, NULL, NULL, "field");

    FairTrackParam parUpdate = *zToBePredicted;
    Double_t chi = 0.0;
    fKalman->Update(&parUpdate, hit, chi);

    //    return *zToBePredicted;
    return parUpdate;
}

Bool_t BmnGlobalAlignment::MilleFieldRuns(Int_t idx, Int_t iDet, Char_t* buff) {
    Int_t modGemTotal = 0;
    TString mwpcPart = "0.0 0.0 0.0";
    TString dchPart = mwpcPart;

    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
        modGemTotal += fDetectorGEM->GetGemStation(iStat)->GetNModules();

    if (iDet == 0) {
        BmnGemTrack* track = (BmnGemTrack*) fGemTracks->UncheckedAt(idx);
        // cout << "Nhits = " << track->GetNHits() << endl;

        // Fit track in XZ-plane to extract local. params.
        TGraph xzTrackProfile, yzTrackProfile;
        Int_t currPoint = 0;
        for (Int_t iHit = 0; iHit < track->GetNHits(); iHit++) {
            BmnGemStripHit* hit = (BmnGemStripHit*) fGemHits->UncheckedAt(track->GetHitIndex(iHit));
            xzTrackProfile.SetPoint(currPoint, hit->GetZ(), hit->GetX());
            yzTrackProfile.SetPoint(currPoint, hit->GetZ(), hit->GetY());
            currPoint++;
        }
        TFitResultPtr fitRes = xzTrackProfile.Fit("pol2", "SQww");
        TFitResultPtr fitResY = yzTrackProfile.Fit("pol1", "Sww");
        if (Bool_t(fitRes->Status())) // kFALSE returned by Status() means a converged fit
            return kFALSE;

        Double_t fitParams[fitRes->NPar()]; // c, b, a
        for (Int_t iPar = 0; iPar < fitRes->NPar(); iPar++)
            fitParams[iPar] = fitRes->Parameter(iPar);

        // Double_t zPol2Vertex = -fitParams[1] / (2 * fitParams[2]);
        // cout << "zPol2Vertex = " << zPol2Vertex << endl;
        fCanv->cd(1);
        xzTrackProfile.Draw("AP*");
        fCanv->cd(1)->Modified();
        fCanv->cd(2);
        yzTrackProfile.Draw("AP*");
        fCanv->cd(2)->Modified();
        fCanv->Update();
        fCanv->Draw();
        //      delete xzTrackProfile;

        // Calculate derivatives (dx(z) / dTx^{i}) of loc. params
        Double_t locDer[fDetectorGEM->GetNStations()];
        for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
            locDer[iStat] = 0.;

        Bool_t posHitsPredict[track->GetNHits()];
        for (Int_t iHit = 0; iHit < track->GetNHits(); iHit++)
            posHitsPredict[iHit] = kTRUE;

        Double_t Tx[track->GetNHits()]; // Array to store obtained Tx at Z-hit positions

        for (Int_t iHit = 0; iHit < track->GetNHits(); iHit++) {
            BmnGemStripHit* hit = (BmnGemStripHit*) fGemHits->UncheckedAt(track->GetHitIndex(iHit));
            FairTrackParam zLeft = UseKalman(track, iHit, -1);
            FairTrackParam zRight = UseKalman(track, iHit, +1);
            FairTrackParam zCurrentHit = UseKalman(track, iHit, 0);

            Tx[iHit] = zCurrentHit.GetTx();

            // Check quality of obtained prediction
            // cout << iHit << " " << Abs(zCurrentHit.GetX() - hit->GetX()) << endl;
            if (Abs(zCurrentHit.GetX() - hit->GetX()) < 0.5) {
                posHitsPredict[iHit] = kFALSE;
                // break;
            }

            // Make a parabolic fit for x(z) and a linear one for Tx(z) @ zCurrent, zLeft and zRight (3 points)
            // to estimate dx(z) / dTx(z) --> locDers
            const Int_t nPoints = 3;
            FairTrackParam paramsAtDiffZ[nPoints] = {zLeft, zCurrentHit, zRight};

            TGraph xZ, txZ;
            currPoint = 0;
            for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
                xZ.SetPoint(currPoint, paramsAtDiffZ[iPoint].GetZ(), paramsAtDiffZ[iPoint].GetX());
                txZ.SetPoint(currPoint, paramsAtDiffZ[iPoint].GetZ(), paramsAtDiffZ[iPoint].GetTx());
                currPoint++;
            }

            TFitResultPtr fitResX = xZ.Fit("pol2", "SQww");
            TFitResultPtr fitResTx = txZ.Fit("pol1", "SQww");

            if (Bool_t(fitResX->Status()) || Bool_t(fitResTx->Status())) // kFALSE returned by Status() means a converged fit
                return kFALSE;

            Double_t fitParamsPol2[fitResX->NPar()]; // = {fitResX->Parameter(0), fitResX->Parameter(1), fitResX->Parameter(2)}; // c, b, a (y = ax^2 + bx +c)
            Double_t fitParamsPol1[fitResTx->NPar()]; // = {fitResTx->Parameter(0), fitResTx->Parameter(1)}; // b, k (y = kx + b)

            for (Int_t iPar = 0; iPar < fitResX->NPar(); iPar++)
                fitParamsPol2[iPar] = fitResX->Parameter(iPar);

            for (Int_t iPar = 0; iPar < fitResTx->NPar(); iPar++)
                fitParamsPol1[iPar] = fitResTx->Parameter(iPar);

            //            locDer[hit->GetStation()] = (2 * fitParamsPol2[2] * zCurrentHit.GetZ() + fitParamsPol2[1]) / fitParamsPol1[1];
            //           locDer[hit->GetStation()] = (zRight.GetX() - zLeft.GetX()) / (zRight.GetTx() - zLeft.GetTx());
            locDer[hit->GetStation()] = hit->GetZ() + fitParams[1] / (2 * fitParams[2]);
            //           cout << (zRight.GetX() - zLeft.GetX()) / (zRight.GetTx() - zLeft.GetTx()) << " " <<
            //                   ((2 * fitParamsPol2[2] * zCurrentHit.GetZ() + fitParamsPol2[1]) / fitParamsPol1[1]) << endl;
            //          Int_t stat = hit->GetStation();
            //           Double_t z = hit->GetZ();
            // locDer[stat] = z + zPol2Vertex;



            //            locDer[stat] = paramsNew.GetTx() / (2 * fitParams[2]);
            if (std::isnan(locDer[hit->GetStation()])
                    //  || Abs(locDer[hit->GetStation()]) > 500.
                    )
                return kFALSE;
        }

        Bool_t isGoodTrack = kTRUE; // reject track or not
        for (Int_t iHit = 0; iHit < track->GetNHits(); iHit++)
            if (!posHitsPredict[iHit]) {
                isGoodTrack = kFALSE;
                break;
            }

        if (isGoodTrack) {
            for (Int_t iHit = 0; iHit < track->GetNHits(); iHit++) {
                BmnGemStripHit* hit = (BmnGemStripHit*) fGemHits->UncheckedAt(track->GetHitIndex(iHit));
                FairTrackParam zLeft = UseKalman(track, iHit, -1);
                FairTrackParam zRight = UseKalman(track, iHit, +1);
                FairTrackParam zCurrentHit = UseKalman(track, iHit, 0);

                /// Debug info ...
                cout << hit->GetStation() << " zCurr = " << zCurrentHit.GetZ() <<
                        " xCurr = " << zCurrentHit.GetX() <<
                        " TxCurr = " << zCurrentHit.GetTx() <<
                        " zR = " << zRight.GetZ() << " zL = " << zLeft.GetZ() <<
                        " xR = " << zRight.GetX() << " xL = " << zLeft.GetX() << " TxR = " << zRight.GetTx() << " TxL = " << zLeft.GetTx() <<
                        " der. = " << locDer[hit->GetStation()] << " xHit = " << hit->GetX() << endl;
                /// ...
            }
        }

        // Prepare string with the calculated loc. derivatives
        TString locDerNullString = "";
        TString locDerString = "";
        for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++) {
            locDerString += TString::Format("%f ", locDer[iStat]);
            locDerNullString += TString::Format("%f ", 0.);
        }

        Int_t nModulesProcessed = 0;
        for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++) {
            Int_t nMod = fDetectorGEM->GetGemStation(iStat)->GetNModules();
            for (Int_t iMod = 0; iMod < nMod; iMod++) {
                nModulesProcessed++;
                Int_t iHit;
                for (iHit = 0; iHit < track->GetNHits(); iHit++) {
                    BmnGemStripHit* hit = (BmnGemStripHit*) fGemHits->UncheckedAt(track->GetHitIndex(iHit));
                    Short_t stat = hit->GetStation();
                    Int_t mod = hit->GetModule();
                    TString zeroEnd = "", zeroBeg = "";
                    if (stat == iStat && mod == iMod) {
                        Double_t X = hit->GetX();
                        Double_t Y = hit->GetY();
                        Double_t Z = hit->GetZ();
                        Char_t* locDerX = Form("%d %d %G %f ", stat, mod, 1., locDer[iStat]);
                        //Char_t* locDerX = Form("%d %d %s", stat, mod, locDerString.Data());
                        // Char_t* locDerY = Form("%d %d %G %s %G %G ", stat, mod, 0., locDerNullString.Data(), 1., Z);
                        Char_t* locDerY = Form("%d %d %G %f ", stat, mod, 1., Z);

                        // cout << locDerX << endl;
                        // cout << locDerY << endl;
                        Char_t* measX = Form(" %f %f ", X, fUseRealHitErrors ? hit->GetDx() : 1.);
                        Char_t* measY = Form(" %f %f ", Y, fUseRealHitErrors ? hit->GetDy() : 1.);

                        Int_t N_zeros_beg = 3 * (nModulesProcessed - 1);
                        Int_t N_zeros_end = 3 * (modGemTotal - nModulesProcessed);

                        //                        Char_t* globDerX = Form(" 1. 0. %f", 2 * fitParams[2] * Z + fitParams[1]);
                        Char_t* globDerX = Form(" 1. 0. %f", Tx[iHit]);
                        Char_t* globDerY = Form(" 0. 1. %f", track->GetParamFirst()->GetTy());

                        for (Int_t i = 0; i < N_zeros_beg; i++)
                            zeroBeg += "0. ";
                        for (Int_t i = 0; i < N_zeros_end; i++)
                            zeroEnd += "0. ";
                        sprintf(buff, "%s%s%s %s %s%s %s %s\n", buff, locDerX, zeroBeg.Data(), globDerX, zeroEnd.Data(), mwpcPart.Data(), dchPart.Data(), measX);
                        sprintf(buff, "%s%s%s %s %s%s %s %s\n", buff, locDerY, zeroBeg.Data(), globDerY, zeroEnd.Data(), mwpcPart.Data(), dchPart.Data(), measY);

                        break;
                    }
                }
                if (iHit == track->GetNHits()) {
                    TString zeroLine = "";
                    for (Int_t iModTot = 0; iModTot < 3 * modGemTotal; iModTot++)
                        zeroLine += "0. ";

                    zeroLine += mwpcPart + " " + dchPart;

                    for (Int_t iRow = 0; iRow < 2; iRow++) {
                        if (iRow == 0)
                            sprintf(buff, "%s%d %d 0. 0. %s 0. 0.\n", buff, iStat, iMod, zeroLine.Data()); // local = 7 -> zeroLine -> meas -> dmeas
                            // sprintf(buff, "%s%d %d 0. 0. 0. 0. 0. 0. %s 0. 0.\n", buff, iStat, iMod, zeroLine.Data()); // local = 7 -> zeroLine -> meas -> dmeas
                        else
                            sprintf(buff, "%s%d %d 0. 0. %s 0. 0.\n", buff, iStat, iMod, zeroLine.Data()); // local = 2 -> zeroLine -> meas -> dmeas
                    }
                }
            }
        }
    }
    return kTRUE;
}

void BmnGlobalAlignment::MilleNoFieldRuns(BmnGlobalTrack* glTrack, Int_t idx, Int_t iDet, Char_t* buff) {
    Int_t modGemTotal = 0;
    Int_t modSiTotal = 0;

    TString mwpcPart = "0.0 0.0 0.0";
    TString dchPart = "0.0 0.0 0.0";
    TString vertexPart = "0.0 0.0 0.0";

    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
        modGemTotal += fDetectorGEM->GetGemStation(iStat)->GetNModules();

    for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++)
        modSiTotal += fDetectorSI->GetSiliconStation(iStat)->GetNModules();

    if (iDet == 0) {
        BmnGemTrack* track = (BmnGemTrack*) fGemTracks->UncheckedAt(idx);
        Int_t nModulesProcessed = 0;
        for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++) {
            Int_t nMod = fDetectorGEM->GetGemStation(iStat)->GetNModules();
            for (Int_t iMod = 0; iMod < nMod; iMod++) {
                nModulesProcessed++;
                Int_t iHit;
                for (iHit = 0; iHit < track->GetNHits(); iHit++) {
                    BmnGemStripHit* hit = (BmnGemStripHit*) fGemHits->UncheckedAt(track->GetHitIndex(iHit));
                    Short_t stat = hit->GetStation();
                    Int_t mod = hit->GetModule();
                    TString zeroEnd = "", zeroBeg = "";
                    if (stat == iStat && mod == iMod) {
                        Double_t X = hit->GetX();
                        Double_t Y = hit->GetY();
                        Double_t Z = hit->GetZ();
                        Char_t* locDerX = Form("%d %d 1. %f 0. 0. ", stat, mod, Z);
                        Char_t* locDerY = Form("%d %d 0. 0. 1. %f ", stat, mod, Z);

                        Char_t* measX = Form("%f %f ", X, fUseRealHitErrors ? hit->GetDx() : 1.);
                        Char_t* measY = Form("%f %f ", Y, fUseRealHitErrors ? hit->GetDy() : 1.);

                        Int_t N_zeros_beg = 3 * (nModulesProcessed - 1);
                        Int_t N_zeros_end = 3 * (modGemTotal - nModulesProcessed);

                        Char_t* globDerX = Form("1. 0. %f", track->GetParamFirst()->GetTx());
                        Char_t* globDerY = Form("0. 1. %f", track->GetParamFirst()->GetTy());

                        for (Int_t i = 0; i < N_zeros_beg; i++)
                            zeroBeg += "0. ";
                        for (Int_t i = 0; i < N_zeros_end; i++)
                            zeroEnd += "0. ";

                        TString siPart = "";
                        for (Int_t iModTot = 0; iModTot < 3 * modSiTotal; iModTot++)
                            siPart += "0. ";

                        sprintf(buff, "%s%s%s %s %s%s %s %s %s %s\n", buff, locDerX, zeroBeg.Data(), globDerX, zeroEnd.Data(), mwpcPart.Data(), dchPart.Data(), vertexPart.Data(),
                                siPart.Data(), measX);
                        sprintf(buff, "%s%s%s %s %s%s %s %s %s %s\n", buff, locDerY, zeroBeg.Data(), globDerY, zeroEnd.Data(), mwpcPart.Data(), dchPart.Data(), vertexPart.Data(),
                                siPart.Data(), measY);
                        break;
                    }
                }
                if (iHit == track->GetNHits()) {
                    TString zeroLine = "";
                    for (Int_t iModTot = 0; iModTot < 3 * modGemTotal; iModTot++)
                        zeroLine += "0. ";

                    zeroLine += mwpcPart + " " + dchPart + " " + vertexPart + " ";

                    TString siPart = "";
                    for (Int_t iModTot = 0; iModTot < 3 * modSiTotal; iModTot++)
                        siPart += "0. ";

                    zeroLine += siPart;

                    for (Int_t iRow = 0; iRow < 2; iRow++)
                        sprintf(buff, "%s%d %d 0. 0. 0. 0. %s 0. 0.\n", buff, iStat, iMod, zeroLine.Data()); // local = 4 -> zeroLine -> meas -> dmeas
                }
            }
        }
    }

    // MWPC
    if (iDet == 1) {
        BmnMwpcTrack* track = (BmnMwpcTrack*) fMwpcTracks->UncheckedAt(idx);
        // Get center z-coordinate between centers of both MWPC's
        Double_t zMid = ((mwpcGeo->GetChamberCenter(0)).Z() + (mwpcGeo->GetChamberCenter(1)).Z()) / 2.;
        Double_t Tx = track->GetParamFirst()->GetTx();
        Double_t Ty = track->GetParamFirst()->GetTy();

        BmnMwpcHit* hit0 = (BmnMwpcHit*) fMwpcHits->UncheckedAt(track->GetHitIndex(0));
        Double_t X = hit0->GetX() + Tx * zMid;
        Double_t Y = hit0->GetY() + Ty * zMid;
        Double_t Z = zMid;

        Char_t* locDerX = Form("1. %f 0. 0. ", Z);
        Char_t* locDerY = Form("0. 0. 1. %f ", Z);
        Char_t* measX = Form("%f %f ", X, fUseRealHitErrors ? hit0->GetDx() : 1.);
        Char_t* measY = Form("%f %f ", Y, fUseRealHitErrors ? hit0->GetDy() : 1.);

        Char_t* globDerX = Form("1. 0. %f", Tx);
        Char_t* globDerY = Form("0. 1. %f", Ty);

        TString zeroLine = "";
        for (Int_t i = 0; i < 3 * modGemTotal; i++)
            zeroLine += "0. ";

        sprintf(buff, "%s%s%s %s %s %s\n", buff, locDerX, zeroLine.Data(), globDerX, dchPart.Data(), measX);
        sprintf(buff, "%s%s%s %s %s %s\n", buff, locDerY, zeroLine.Data(), globDerY, dchPart.Data(), measY);
    }

    // DCH
    if (iDet == 2) {
        BmnDchTrack* track = (BmnDchTrack*) fDchTracks->UncheckedAt(idx);

        BmnDchHit hit; // A single fiction hit pertaining to a dch matched segment at Zmid = (Zdch1 + Zdch2) / 2
        hit.SetX(track->GetParamFirst()->GetX());
        hit.SetY(track->GetParamFirst()->GetY());
        hit.SetZ(track->GetParamFirst()->GetZ());

        Double_t X = hit.GetX();
        Double_t Y = hit.GetY();
        Double_t Z = hit.GetZ();

        Char_t* locDerX = Form("1. %f 0. 0. ", Z);
        Char_t* locDerY = Form("0. 0. 1. %f ", Z);
        Char_t* measX = Form("%f %f ", X, 1. / Sqrt(12));
        Char_t* measY = Form("%f %f ", Y, 1. / Sqrt(12));
        Char_t* globDerX = Form("1. 0. %f", track->GetParamFirst()->GetTx());
        Char_t* globDerY = Form("0. 1. %f", track->GetParamFirst()->GetTy());

        TString zeroLine = "";
        for (Int_t i = 0; i < 3 * modGemTotal; i++)
            zeroLine += "0. ";

        sprintf(buff, "%s%s%s %s %s %s\n", buff, locDerX, zeroLine.Data(), mwpcPart.Data(), globDerX, measX);
        sprintf(buff, "%s%s%s %s %s %s\n", buff, locDerY, zeroLine.Data(), mwpcPart.Data(), globDerY, measY);
    }

    // PRIMARY_VERTEX_INFO
    if (iDet == 3) {
        CbmVertex* Vp = (CbmVertex*) fVertex->UncheckedAt(0);
        BmnGlobalTrack* track = (BmnGlobalTrack*) fGlobalTracks->UncheckedAt(idx); // get current glob-track to extract Tx and Ty

        Char_t* locDerX = Form("1. %f 0. 0. ", Vp->GetRoughZ());
        Char_t* locDerY = Form("0. 0. 1. %f ", Vp->GetRoughZ());
        Char_t* globDerX = Form("1. 0. %f", track->GetParamFirst()->GetTx());
        Char_t* globDerY = Form("0. 1. %f", track->GetParamFirst()->GetTy());
        Char_t* measX = Form("%f %f ", Vp->GetRoughX(), fUseRealHitErrors ? 1. / Sqrt(12) : 1.);
        Char_t* measY = Form("%f %f ", Vp->GetRoughY(), fUseRealHitErrors ? 1. / Sqrt(12) : 1.);

        TString zeroLine = "";
        for (Int_t i = 0; i < 3 * modGemTotal; i++)
            zeroLine += "0. ";

        sprintf(buff, "%s%s%s %s %s %s %s\n", buff, locDerX, zeroLine.Data(), mwpcPart.Data(), dchPart.Data(), globDerX, measX);
        sprintf(buff, "%s%s%s %s %s %s %s\n", buff, locDerY, zeroLine.Data(), mwpcPart.Data(), dchPart.Data(), globDerY, measY);
    }

    // SILICON
    if (iDet == 4) {
        Int_t nModulesProcessed = 0;
        for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++) {
            for (Int_t iMod = 0; iMod < fDetectorSI->GetSiliconStation(iStat)->GetNModules(); iMod++) {
                nModulesProcessed++;
                BmnSiliconHit* hit = (BmnSiliconHit*) fSiHits->UncheckedAt(idx);
                Int_t stat = hit->GetStation();
                Int_t mod = hit->GetModule();
                TString zeroEnd = "", zeroBeg = "";
                if (stat == iStat && mod == iMod) {
                    Char_t* locDerX = Form("%d %d 1. %f 0. 0. ", hit->GetStation(), hit->GetModule(), hit->GetZ());
                    Char_t* locDerY = Form("%d %d 0. 0. 1. %f ", hit->GetStation(), hit->GetModule(), hit->GetZ());

                    Int_t N_zeros_beg = 3 * (nModulesProcessed - 1);
                    Int_t N_zeros_end = 3 * (modSiTotal - nModulesProcessed);

                    Char_t* globDerX = Form("1. 0. %f", glTrack->GetParamFirst()->GetTx());
                    Char_t* globDerY = Form("0. 1. %f", glTrack->GetParamFirst()->GetTy());

                    for (Int_t i = 0; i < N_zeros_beg; i++)
                        zeroBeg += "0. ";
                    for (Int_t i = 0; i < N_zeros_end; i++)
                        zeroEnd += "0. ";

                    Char_t* measX = Form("%f %f", hit->GetX(), fUseRealHitErrors ? hit->GetDx() : 1.);
                    Char_t* measY = Form("%f %f", hit->GetY(), fUseRealHitErrors ? hit->GetDy() : 1.);

                    TString zeroLineFromGem = "";
                    for (Int_t i = 0; i < 3 * modGemTotal; i++)
                        zeroLineFromGem += "0. ";
                    sprintf(buff, "%s%s%s %s %s %s %s %s %s %s\n", buff, locDerX, zeroLineFromGem.Data(), mwpcPart.Data(), dchPart.Data(), vertexPart.Data(), zeroBeg.Data(),
                            globDerX, zeroEnd.Data(), measX);
                    sprintf(buff, "%s%s%s %s %s %s %s %s %s %s\n", buff, locDerY, zeroLineFromGem.Data(), mwpcPart.Data(), dchPart.Data(), vertexPart.Data(), zeroBeg.Data(),
                            globDerY, zeroEnd.Data(), measY);
                } else {
                    TString zeroLine = "";
                    for (Int_t iModTot = 0; iModTot < 3 * modGemTotal; iModTot++)
                        zeroLine += "0. ";

                    zeroLine += mwpcPart + " " + dchPart + " " + vertexPart + " ";

                    for (Int_t iModTot = 0; iModTot < 3 * modSiTotal; iModTot++)
                        zeroLine += "0. ";

                    for (Int_t iRow = 0; iRow < 2; iRow++)
                        sprintf(buff, "%s%d %d 0. 0. 0. 0. %s 0. 0.\n", buff, iStat, iMod, zeroLine.Data()); // local = 4 -> zeroLine -> meas -> dmeas
                }
            }
        }
    }
}

const Int_t BmnGlobalAlignment::MakeBinFile() {
    ifstream fout_txt;
    fout_txt.open("alignment.txt", ios::in);

    // Calculate number of glob. params.
    const Int_t ngl_per_subdetector = 3; // x, y and z corrs to each det. subsyst. (GEM, MWPC, DCH, PRIMARY_VERTEX, SILICON at the moment)
    // GEM
    Int_t gem = 0;
    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
        gem += ngl_per_subdetector * fDetectorGEM->GetGemStation(iStat)->GetNModules();

    // MWPC
    Int_t mwpc = ngl_per_subdetector;

    // DCH
    Int_t dch = ngl_per_subdetector;

    // VERTEX
    Int_t vertex = ngl_per_subdetector;

    // SILICON
    Int_t silicon = 0;
    for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++)
        silicon += ngl_per_subdetector * fDetectorSI->GetSiliconStation(iStat)->GetNModules();

    const Int_t dimLabel = gem + mwpc + dch + vertex + silicon;
    Labels = new Int_t[dimLabel];
    for (Int_t iEle = 0; iEle < dimLabel; iEle++)
        Labels[iEle] = 1 + iEle;

    BmnMille* Mille = new BmnMille("alignment.bin", kTRUE, kFALSE);

    TString detName, coordName;
    Int_t stat, mod;
    Double_t DerGl[dimLabel]; //, DerLc[fNLC];
    Double_t rMeasure, dMeasure;

    // Loop over selected tracks
    for (Int_t iTrack = 0; iTrack < nSelectedTracks; iTrack++) {
        // Read GEM info
        fout_txt >> detName;
        if (fDebug)
            cout << detName << endl;

        for (Int_t iStation = 0; iStation < fDetectorGEM->GetNStations(); iStation++) {
            for (Int_t iMod = 0; iMod < fDetectorGEM->GetGemStation(iStation)->GetNModules(); iMod++) {
                for (Int_t iLine = 0; iLine < 2; iLine++) {
                    Int_t nElements = (!fIsField) ? fNLC : (iLine == 0) ? 2 : 2;
                    // Int_t nElements = (!fIsField) ? fNLC : (iLine == 0) ? 6 : 2;
                    Double_t DerLc[nElements];
                    for (Int_t iElement = 0; iElement < nElements; iElement++)
                        DerLc[iElement] = 0.;
                    coordName = (iLine == 0) ? " x = " : " y = ";
                    fout_txt >> stat >> mod;
                    for (Int_t iVar = 0; iVar < nElements; iVar++)
                        fout_txt >> DerLc[iVar];
                    for (Int_t iVar = 0; iVar < dimLabel; iVar++)
                        fout_txt >> DerGl[iVar];
                    fout_txt >> rMeasure >> dMeasure;
                    Mille->mille(nElements, DerLc, dimLabel, DerGl, Labels, rMeasure, dMeasure);
                    if (fDebug)
                        cout << "Stat: " << stat << " Mod: " << mod << coordName << rMeasure << " " << dMeasure << endl;
                }
            }
        }

        // Read MWPC, DCH, VERTEX
        for (Int_t iDet = 0; iDet < 3; iDet++) {
            fout_txt >> detName;
            if (fDebug)
                cout << detName << " " << endl;

            for (Int_t iLine = 0; iLine < 2; iLine++) {
                Int_t nElements = (!fIsField) ? fNLC : (iLine == 0) ? 2 : 2;
                // Int_t nElements = (!fIsField) ? fNLC : (iLine == 0) ? 6 : 2;
                Double_t DerLc[nElements];
                for (Int_t iElement = 0; iElement < nElements; iElement++)
                    DerLc[iElement] = 0.;
                coordName = (iLine == 0) ? " x = " : " y = ";
                for (Int_t iVar = 0; iVar < nElements; iVar++)
                    fout_txt >> DerLc[iVar];
                for (Int_t iVar = 0; iVar < dimLabel; iVar++)
                    fout_txt >> DerGl[iVar];
                fout_txt >> rMeasure >> dMeasure;
                Mille->mille(nElements, DerLc, dimLabel, DerGl, Labels, rMeasure, dMeasure);
                if (fDebug)
                    cout << coordName << rMeasure << " " << dMeasure << endl;
            }
        }

        // Read SILICON
        fout_txt >> detName;
        if (fDebug)
            cout << detName << endl;

        for (Int_t iStation = 0; iStation < fDetectorSI->GetNStations(); iStation++) {
            for (Int_t iMod = 0; iMod < fDetectorSI->GetSiliconStation(iStation)->GetNModules(); iMod++) {
                for (Int_t iLine = 0; iLine < 2; iLine++) {
                    Int_t nElements = (!fIsField) ? fNLC : (iLine == 0) ? 2 : 2;
                    Double_t DerLc[nElements];
                    for (Int_t iElement = 0; iElement < nElements; iElement++)
                        DerLc[iElement] = 0.;
                    coordName = (iLine == 0) ? " x = " : " y = ";
                    fout_txt >> stat >> mod;

                    for (Int_t iVar = 0; iVar < nElements; iVar++)
                        fout_txt >> DerLc[iVar];

                    for (Int_t iVar = 0; iVar < dimLabel; iVar++)
                        fout_txt >> DerGl[iVar];

                    fout_txt >> rMeasure >> dMeasure;
                    Mille->mille(nElements, DerLc, dimLabel, DerGl, Labels, rMeasure, dMeasure);
                    Mille->mille(nElements, DerLc, dimLabel, DerGl, Labels, rMeasure, dMeasure);
                    if (fDebug)
                        cout << "Stat: " << stat << " Mod: " << mod << coordName << rMeasure << " " << dMeasure << endl;
                }
            }
        }
        Mille->end();
    }

    delete Mille;
    fout_txt.close();

    return dimLabel;
}

void BmnGlobalAlignment::MakeSteerFile() {
    const Int_t dim = MakeBinFile(); // return labels of params.

    FILE* steer = fopen("steer.txt", "w");
    TString alignType = "alignment.bin";
    fprintf(steer, "%s\n", alignType.Data());
    fprintf(steer, "method inversion %d %f\n", fNumOfIterations, fAccuracy);
    if (fUseRegularization)
        fprintf(steer, "regularization 1.0\n");
    fprintf(steer, "hugecut %G\n", fHugecut);
    if (fChisqcut[0] * fChisqcut[1] != 0)
        fprintf(steer, "chisqcut %G %G\n", fChisqcut[0], fChisqcut[1]);
    fprintf(steer, "entries %d\n", fEntries);
    fprintf(steer, "outlierdownweighting %d\n", fOutlierdownweighting);
    fprintf(steer, "dwfractioncut %G\n", fDwfractioncut);
    fprintf(steer, "Parameter\n");

    Int_t parCounterGem = 0;
    Int_t parCounterSi = 0;
    const Int_t nParams = 3;

    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < fDetectorGEM->GetGemStation(iStat)->GetNModules(); iMod++)
            parCounterGem++;

    for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < fDetectorSI->GetSiliconStation(iStat)->GetNModules(); iMod++)
            parCounterSi++;

    Int_t startIdx = 0;
    for (Int_t iDet = 0; iDet < nDetectors; iDet++) {
        if (iDet == 0) {// Process GEMs to mark fixed stations if exist
            for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++) {
                for (Int_t iPar = 0; iPar < fDetectorGEM->GetGemStation(iStat)->GetNModules() * nParams; iPar++)
                    fprintf(steer, "%d %G %G\n", Labels[startIdx + iPar], 0., (fixedGemElements[iStat][iPar / nParams]) ? -1. : fPreSigma);
                startIdx += fDetectorGEM->GetGemStation(iStat)->GetNModules() * nParams;
            }
        } else if (iDet == 4) { // Process SILICON to mark fixed modules if exist
            for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++) {
                for (Int_t iPar = 0; iPar < fDetectorSI->GetSiliconStation(iStat)->GetNModules() * nParams; iPar++) {
                    //                   Double_t scale = ((startIdx + iPar + 1) % 3 == 2) ? 0.01 : 1.;
                    fprintf(steer, "%d %G %G\n", Labels[startIdx + iPar], 0., (fixedSiElements[iStat][iPar / nParams]) ? -1. : (fPreSigma * 1.));
                }
            }
        } else if (iDet == 1 || iDet == 2 || iDet == 3) {// MWPC, DCH, VERTEX
            //  if (iDet == 1) startIdx -= nParams;
            for (Int_t iPar = 0; iPar < nParams; iPar++)
                fprintf(steer, "%d %G %G\n", Labels[startIdx + iPar], 0., (fDetectorSet[iDet]) ? fPreSigma : -1.);
            startIdx += nParams;
        }
    }

    // 1. Calculate center-of-gravity along Z-axis (GEM + SI)
    vector <Double_t> z_GEM;
    vector <Double_t> z_SI;

    vector <Double_t> z_GEM_SI;

    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < fDetectorGEM->GetGemStation(iStat)->GetNModules(); iMod++)
            z_GEM.push_back(fDetectorGEM->GetGemStation(iStat)->GetModule(iMod)->GetZPositionRegistered());

    for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < fDetectorSI->GetSiliconStation(iStat)->GetNModules(); iMod++)
            z_SI.push_back(fDetectorSI->GetSiliconStation(iStat)->GetModule(iMod)->GetZPositionRegistered());

    z_GEM_SI.reserve(z_GEM.size() + z_SI.size());
    z_GEM_SI.insert(z_GEM_SI.end(), z_GEM.begin(), z_GEM.end());
    z_GEM_SI.insert(z_GEM_SI.end(), z_SI.begin(), z_SI.end());

    Double_t zSum = 0.;
    for (Int_t iSize = 0; iSize < z_GEM_SI.size(); iSize++)
        zSum += z_GEM_SI[iSize];

    Double_t zC = zSum / z_GEM_SI.size();

    // 2. Calculate dZ = Zpos - Zc (GEM + SI)
    vector <Double_t> deltaZ;

    for (Int_t iSize = 0; iSize < z_GEM_SI.size(); iSize++) {
        deltaZ.push_back(z_GEM_SI[iSize] - zC);
    }

    // 3. Apply constraints (six in general) to prevent a total shift of the detector (iStep = 0, 1) and define scaling (iStep = 2)
    //          iConstrSet = 0
    // Wi * a_Xi = 0,    (iStep = 0), Wi = delta_Zi
    // Wi * a_Yi = 0,    (iStep = 1), Wi = delta_Zi
    // Wi * a_Zi = 0,    (iStep = 2), Wi = Zi
    //          iConstrSet = 1
    // Wi * a_Xi = 0,    (iStep = 0), Wi = 1.
    // Wi * a_Yi = 0,    (iStep = 1), Wi = 1.
    // Wi * a_Zi = 0,    (iStep = 2), Wi = 1.

    Int_t modCounter = 0;
    for (Int_t iConstrSet = 0; iConstrSet < 2; iConstrSet++)
        for (Int_t iStep = 0; iStep < 3; iStep++) {
            fprintf(steer, "constraint 0.0\n");
            modCounter = 0;
            for (Int_t iPar = 0; iPar < dim; iPar++) {
                if (Labels[iPar] > nParams * parCounterGem && Labels[iPar] < 1 + nParams * parCounterGem + nParams * 3)
                    continue;

                if (Labels[iPar] % 3 == iStep + 1) {
                    fprintf(steer, "%d %G\n", Labels[iPar], (iConstrSet == 0) ? deltaZ[modCounter] : 1.);
                    modCounter++;
                } else if (iStep == 2 && Labels[iPar] % 3 == 0) {
                    fprintf(steer, "%d %G\n", Labels[iPar], (iConstrSet == 0) ? (deltaZ[modCounter] += zC) : 1.);
                    modCounter++;
                }
            }
        }
    fclose(steer);
}

void BmnGlobalAlignment::Pede() {
    system("pede steer.txt");
    ifstream resFile("millepede.res", ios::in);
    ReadPedeOutput(resFile);
    resFile.close();
}

void BmnGlobalAlignment::ReadPedeOutput(ifstream& resFile) {
    if (!resFile) {
        cout << "BmnGlobalAlignment::ReadPedeOutput" << " No input file found!!" << endl;
        throw;
    }
    resFile.ignore(numeric_limits<streamsize>::max(), '\n');

    const Int_t nParams = 3;
    Double_t* corrs = new Double_t[nParams];

    // Read GEMs
    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++) {
        for (Int_t iMod = 0; iMod < fDetectorGEM->GetGemStation(iStat)->GetNModules(); iMod++) {
            ExtractCorrValues(resFile, corrs);
            BmnGemAlignCorrections* gemCorrs = new((*fGemAlignCorr)[fGemAlignCorr->GetEntriesFast()]) BmnGemAlignCorrections();
            gemCorrs->SetStation(iStat);
            gemCorrs->SetModule(iMod);
            gemCorrs->SetCorrections(corrs);
        }
    }

    // Read MWPC, DCH and VERTEX
    for (Int_t iDet = 0; iDet < 3; iDet++) {
        ExtractCorrValues(resFile, corrs);
        if (iDet == 0) {
            BmnMwpcAlignCorrections* mwpcCorrs = new((*fMwpcAlignCorr)[fMwpcAlignCorr->GetEntriesFast()]) BmnMwpcAlignCorrections();
            mwpcCorrs->SetCorrections(corrs);
        } else if (iDet == 1) {
            BmnDchAlignCorrections* dchCorrs = new((*fDchAlignCorr)[fDchAlignCorr->GetEntriesFast()]) BmnDchAlignCorrections();
            dchCorrs->SetCorrections(corrs);
        } else {
            // VERTEX??? // FIXME
        }
    }

    // Read SILICON
    for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++) {
        for (Int_t iMod = 0; iMod < fDetectorSI->GetSiliconStation(iStat)->GetNModules(); iMod++) {
            ExtractCorrValues(resFile, corrs);
            BmnSiliconAlignCorrections* siCorrs = new((*fSiAlignCorr)[fSiAlignCorr->GetEntriesFast()]) BmnSiliconAlignCorrections();
            siCorrs->SetStation(iStat);
            siCorrs->SetModule(iMod);
            siCorrs->SetCorrections(corrs);
        }
    }

    delete [] corrs;
}

void BmnGlobalAlignment::ExtractCorrValues(ifstream& resFile, Double_t* corrs) {
    const Int_t nParams = 3;
    TString parValue = "", dummy = "";
    string line;

    for (Int_t iCorr = 0; iCorr < nParams; iCorr++)
        corrs[iCorr] = 0.;
    for (Int_t iLine = 0; iLine < nParams; iLine++) {
        getline(resFile, line);
        stringstream ss(line);
        Int_t size = ss.str().length();
        // 40 and 68 symbols are fixed in the Pede-output by a given format
        if (size == 40)
            ss >> dummy >> parValue >> dummy;
        else if (size == 68)
            ss >> dummy >> parValue >> dummy >> dummy >> dummy;
        else
            cout << "Unsupported format given!" << endl;

        Int_t idx = (iLine % nParams == 0) ? 0 : (iLine % nParams == 1) ? 1 : 2;
        corrs[idx] = -parValue.Atof();
    }
}

void BmnGlobalAlignment::CreateDetectorGeometries() {
    nDetectors = 5; // GEM + MWPC + DCH + PRIMARY_VERTEX_INFO + SILICON
    fDetectorSet = new Bool_t[nDetectors]();

    /// MWPC
    mwpcGeo = new BmnMwpcGeometry();

    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString confSi = "SiliconRunSpring2017.xml"; // FIXME for RUN7 (should be got from the BM@N UniDb)
    TString confGem = "GemRunSpring2018.xml"; // FIXME

    /// SI
    TString gPathSiliconConfig = gPathConfig + "/silicon/XMLConfigs/";
    fDetectorSI = new BmnSiliconStationSet(gPathSiliconConfig + confSi);

    // Define fixed elements of SI-detector...
    fixedSiElements = new Bool_t*[fDetectorSI->GetNStations()];
    for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++) {
        fixedSiElements[iStat] = new Bool_t[fDetectorSI->GetSiliconStation(iStat)->GetNModules()];
    }

    for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < fDetectorSI->GetSiliconStation(iStat)->GetNModules(); iMod++)
            fixedSiElements[iStat][iMod] = kFALSE;

    /// GEM
    TString gPathGemConfig = gPathConfig + "/gem/XMLConfigs/";
    fDetectorGEM = new BmnGemStripStationSet(gPathGemConfig + confGem);

    // Define fixed elements of GEM-detector...
    fixedGemElements = new Bool_t*[fDetectorGEM->GetNStations()];
    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++) {
        fixedGemElements[iStat] = new Bool_t[fDetectorGEM->GetGemStation(iStat)->GetNModules()];
    }

    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < fDetectorGEM->GetGemStation(iStat)->GetNModules(); iMod++)
            fixedGemElements[iStat][iMod] = kFALSE;
}

