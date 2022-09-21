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
#include <TRandom2.h>
#include "BmnGlobalAlignment.h"
#include "CbmVertex.h"
#include "BmnSiliconHit.h"

Int_t BmnGlobalAlignment::fCurrentEvent = 0;

void BmnGlobalAlignment::Finish() {

    }

BmnGlobalAlignment::~BmnGlobalAlignment() {
    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
        delete [] fixedGemElements[iStat];
    delete [] fixedGemElements;

    delete [] fDetectorSet;
    delete fDetectorGEM;
    delete fDetectorSI;
    if (Labels) {
        delete [] Labels;
        system("cp millepede.res Millepede.res");
        // Remove useless text files finalizing the code execution
        system("rm millepede.*");
    }
}

BmnGlobalAlignment::BmnGlobalAlignment(Int_t nEvents, TString inFileName, Int_t period, Int_t run, TString misAlignFile, Bool_t doTest) :
nDetectors(2),
fGemHits(nullptr),
fSilTracks(nullptr),
fGemTracks(nullptr),
fGlobalTracks(nullptr),
fGemAlignCorr(nullptr),
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
fUseRealHitErrors(kFALSE),
fNumOfIterations(500000),
fAccuracy(1e-3),
fPreSigma(1.),
fUseRegularization(kFALSE),
fHugecut(50.),
fEntries(10),
fNGL(0),
fNLC(4),
fOutlierdownweighting(0),
fDwfractioncut(0.0),
fDebug(true),
fIsField(kFALSE),
Labels(nullptr),
fUseConstraints(kTRUE),
fRunPeriod(period),
fRunId(run),
fIsDoTest(doTest),
fMisAlignFile(misAlignFile),
fBmnGemMisalign(nullptr) {
    fNEvents = nEvents;
    fNTracks = 0;
    fRecoFileName = inFileName;

    fBranchSiHits = "BmnSiliconHit";
    fBranchGemHits = "BmnGemStripHit";

    fBranchGemTracks = "BmnGemTrack";
    fBranchSilTracks = "BmnSiliconTrack";
    fBranchGlobalTracks = "BmnGlobalTrack";

    fBranchGemAlignCorr = "BmnGemAlignCorrections";
    fBranchSiAlignCorr = "BmnSiliconAlignCorrections";

    fBranchGemResiduals = "BmnResiduals";
    fBranchFairEventHeader = "EventHeader.";

    CreateDetectorGeometries();
    FillMaps();
}

InitStatus BmnGlobalAlignment::Init() {
    cout << " BmnGlobalAlignment::Init() " << endl;
    cout << "Use detectors: GEM - " << fDetectorSet[0] << " SILICON - " << fDetectorSet[1] << endl;

    TChain* chain = new TChain("bmndata");
    chain->Add(fRecoFileName.Data());
    FairEventHeader* evHeader = NULL;
    chain->SetBranchAddress(fBranchFairEventHeader.Data(), &evHeader);
    chain->GetEntry(0);
    delete chain;

    Double_t fieldVolt = 0.;
    UniRun* runInfo = NULL;
    if (fRunId != 0) {
        runInfo = UniRun::GetRun(fRunPeriod, fRunId);
        if (!runInfo)
            throw;
        fieldVolt = *runInfo->GetFieldVoltage();
        fIsField = (fieldVolt > 10.) ? kTRUE : kFALSE;
    }

    FairRootManager* ioman = FairRootManager::Instance();

    fSiHits = (TClonesArray*) ioman->GetObject(fBranchSiHits.Data());
    fGemHits = (TClonesArray*) ioman->GetObject(fBranchGemHits.Data());

    fSilTracks = (TClonesArray*) ioman->GetObject(fBranchSilTracks.Data());
    fGemTracks = (TClonesArray*) ioman->GetObject(fBranchGemTracks.Data());
    fGlobalTracks = (TClonesArray*) ioman->GetObject(fBranchGlobalTracks.Data());

    fFairEventHeader = (FairEventHeader*) ioman->GetObject(fBranchFairEventHeader.Data());

    fGemAlignCorr = new TClonesArray(fBranchGemAlignCorr.Data());
    fSiAlignCorr = new TClonesArray(fBranchSiAlignCorr.Data());

    ioman->Register(fBranchGemAlignCorr.Data(), "GEM", fGemAlignCorr, kTRUE);
    ioman->Register(fBranchSiAlignCorr.Data(), "SI", fSiAlignCorr, kTRUE);

    fChain = ioman->GetInChain();

    Char_t* geoFileName = (Char_t*) "current_geo_file.root";
    Int_t res_code = UniRun::ReadGeometryFile(fRunPeriod, fRunId, geoFileName);
    if (res_code != 0) {
        cout << "Geometry file can't be read from the database" << endl;
        exit(-1);
    }
    TGeoManager::Import(geoFileName);

    if (fIsDoTest) {
        TChain* ch = new TChain("bmndata");
        ch->Add(fMisAlignFile.Data());
        ch->SetBranchAddress(fBranchGemAlignCorr.Data(), &fBmnGemMisalign);
        ch->GetEntry(0);
        delete ch;
    }
    return kSUCCESS;
}

void BmnGlobalAlignment::Exec(Option_t* opt) {
    fFairEventHeader->SetMCEntryNumber(fCurrentEvent);
    fFairEventHeader->SetRunId(fRunId);
    fCurrentEvent++;
    if (fCurrentEvent % 1000 == 0)
        cout << "Event# = " << fCurrentEvent << endl;

    if (!fGlobalTracks)
        return;

    for (Int_t iGlobTrack = 0; iGlobTrack < fGlobalTracks->GetEntriesFast(); iGlobTrack++) {
        BmnGlobalTrack* globTrack = (BmnGlobalTrack*) fGlobalTracks->UncheckedAt(iGlobTrack);
        if (fDetectorSet[1] && globTrack->GetSilTrackIndex() == -1)
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

        Int_t* idx = new Int_t[nDetectors];
        idx[0] = globTrack->GetGemTrackIndex();
        idx[1] = globTrack->GetSilTrackIndex();

        vector <BmnMilleContainer*> GEM;
        vector <BmnMilleContainer*> SILICON;

        for (Int_t iDet = 0; iDet < nDetectors; iDet++) {
            if (!fDetectorSet[iDet] || idx[iDet] == -1 || fIsField) //{
                // delete [] idx;
                continue;
            //}
            MilleNoFieldRuns(globTrack, idx[iDet], iDet, GEM, SILICON);
        }

        fCONTAINER[fNTracks] = pair <vector <BmnMilleContainer*>, vector < BmnMilleContainer*>> (SILICON, GEM);
        fNTracks++;
        delete [] idx;
    }

    if (fNEvents == fCurrentEvent) {
        MakeBinFile();
        MakeSteerFile();
        Pede();
    }
}

BmnMilleContainer* BmnGlobalAlignment::FillMilleContainer(BmnGlobalTrack* glTrack, BmnHit* hit) {
    BmnMilleContainer* mille = new BmnMilleContainer();

    mille->SetStation(hit->GetStation());
    mille->SetModule(hit->GetModule());

    vector <Double_t> locDerX;
    vector <Double_t> locDerY;
    locDerX.push_back(1.);
    locDerX.push_back(hit->GetZ());
    locDerX.push_back(0.);
    locDerX.push_back(0.);

    locDerY.push_back(0.);
    locDerY.push_back(0.);
    locDerY.push_back(1.);
    locDerY.push_back(hit->GetZ());

    vector <Double_t> globDerX;
    vector <Double_t> globDerY;
    globDerX.push_back(1.);
    globDerX.push_back(0.);
    globDerX.push_back(glTrack->GetParamFirst()->GetTx());

    globDerY.push_back(0.);
    globDerY.push_back(1.);
    globDerY.push_back(glTrack->GetParamFirst()->GetTy());

    mille->SetLocDers(locDerX, locDerY);
    mille->SetGlobDers(globDerX, globDerY);

    mille->SetMeasures(hit->GetX(), hit->GetY());
    mille->SetDMeasures(fUseRealHitErrors ? 2. * hit->GetDx() : 1., fUseRealHitErrors ? hit->GetDy() : 1.);

    return mille;
}

void BmnGlobalAlignment::MilleNoFieldRuns(BmnGlobalTrack* glTrack, Int_t idx, Int_t iDet, vector <BmnMilleContainer*>& GEM, vector <BmnMilleContainer*>& SILICON) {
    // GEM
    if (iDet == 0) {
        BmnTrack* track = (BmnTrack*) fGemTracks->UncheckedAt(idx);
        for (Int_t iHit = 0; iHit < track->GetNHits(); iHit++) {
            BmnGemStripHit* hit = (BmnGemStripHit*) fGemHits->UncheckedAt(track->GetHitIndex(iHit));
            GEM.push_back(FillMilleContainer(glTrack, hit));
        }
    }

    // SILICON
    if (iDet == 1) {
        BmnTrack* track = (BmnTrack*) fSilTracks->UncheckedAt(idx);
        for (Int_t iHit = 0; iHit < track->GetNHits(); iHit++) {
            BmnSiliconHit* hit = (BmnSiliconHit*) fSiHits->UncheckedAt(track->GetHitIndex(iHit));
            SILICON.push_back(FillMilleContainer(glTrack, hit));
        }
    }
}

void BmnGlobalAlignment::_Mille(Double_t* DerLc, Double_t* DerGl, BmnMille* Mille, Int_t iTrack) {
    fITERATOR = next(fCONTAINER.begin(), iTrack);

    TString* dets = new TString[nDetectors];
    dets[0] = "GEM";
    dets[1] = "SILICON";

    for (Int_t iDet = 0; iDet < nDetectors; iDet++) {
        vector <BmnMilleContainer*> cont;
        if (dets[iDet].Contains("GEM"))
            cont = fITERATOR->second.second;
        else if (dets[iDet].Contains("SILICON"))
            cont = fITERATOR->second.first;
        else {
            cout << "Wrong input data! Exiting ..." << endl;
            throw;
        }

        const Int_t nLays = 2;
        TString lays[nLays] = {"X", "Y"};

        for (Int_t iEle = 0; iEle < cont.size(); iEle++) {
            BmnMilleContainer* _cont = cont[iEle];
            for (Int_t iLoc = 0; iLoc < fNLC; iLoc++)
                DerLc[iLoc] = 0.;
            for (Int_t iGlob = 0; iGlob < fNGL; iGlob++)
                DerGl[iGlob] = 0.;

            for (Int_t iLay = 0; iLay < nLays; iLay++) {
                vector <Double_t> locDers = _cont->GetLocDers(lays[iLay]);
                DerLc[0] = locDers[0];
                DerLc[1] = locDers[1];
                DerLc[2] = locDers[2];
                DerLc[3] = locDers[3];

                Int_t idx = dets[iDet].Contains("GEM") ? GemStatModLabel(_cont->GetStation(), _cont->GetModule()) :
                        SiliconStatModLabel(_cont->GetStation(), _cont->GetModule());
                Int_t idxUp = idx - 1; // Upper label value
                Int_t idxMed = idxUp - 1;
                Int_t idxLow = idxMed - 1;

                vector <Double_t> globDers = _cont->GetGlobDers(lays[iLay]);
                DerGl[idxLow] = globDers[0];
                DerGl[idxMed] = globDers[1];
                DerGl[idxUp] = globDers[2];

                Double_t meas = lays[iLay].Contains("X") ? _cont->GetMeasures().X() : _cont->GetMeasures().Y();
                Double_t dmeas = lays[iLay].Contains("X") ? _cont->GetDMeasures().X() : _cont->GetDMeasures().Y();
                Mille->mille(fNLC, DerLc, fNGL, DerGl, Labels, meas, dmeas);

                if (fDebug) {
                    cout << dets[iDet] << endl;
                    cout << lays[iLay] << endl;
                    cout << _cont->GetStation() << " " << _cont->GetModule() << endl;
                    cout << "Loc. ders : ";
                    for (Int_t iLoc = 0; iLoc < fNLC; iLoc++)
                        cout << DerLc[iLoc] << " ";
                    cout << endl;
                    cout << "Glob. ders : ";
                    for (Int_t iGlob = 0; iGlob < fNGL; iGlob++)
                        cout << DerGl[iGlob] << " ";
                    cout << endl;
                    cout << "Meas = " << meas << " dMeas = " << dmeas << endl;
                }
            }
        }
    }
    delete [] dets;
}

const Int_t BmnGlobalAlignment::MakeBinFile() {
    const Int_t ngl_per_subdetector = 3; // x, y and z corrs to each det. subsyst. (GEM, SILICON at the moment)

    // GEM
    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
        fNGL += ngl_per_subdetector * fDetectorGEM->GetGemStation(iStat)->GetNModules();

    // SILICON
    for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++)
        fNGL += ngl_per_subdetector * fDetectorSI->GetSiliconStation(iStat)->GetNModules();

    // Array with labels
    Labels = new Int_t[fNGL];
    for (Int_t iEle = 0; iEle < fNGL; iEle++)
        Labels[iEle] = 1 + iEle;

    // Read collected tracks
    // Arrays with loc. and glob. derivatives
    Int_t nTracks = fCONTAINER.size();
    Double_t DerLc[fNLC];
    Double_t DerGl[fNGL];

    BmnMille* Mille = new BmnMille("alignment.bin", kTRUE, kFALSE);
    for (Int_t iTrack = 0; iTrack < nTracks; iTrack++) {
        if (fDebug)
            cout << "Track# " << iTrack << endl;
        _Mille(DerLc, DerGl, Mille, iTrack);
        Mille->end();
        if (iTrack % 1000 == 0 && !fDebug)
            cout << "Track processed# " << iTrack << endl;
    }

    delete Mille;
    return 0;
}

void BmnGlobalAlignment::MakeSteerFile() {
    //    Double_t shiftX = 0., shiftY = 0., shiftZ = 0.;
    //    Double_t*** misAlign = new Double_t**[fDetectorGEM->GetNStations()];
    //    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++) {
    //        misAlign[iStat] = new Double_t*[fDetectorGEM->GetGemStation(iStat)->GetNModules()];
    //        for (Int_t iMod = 0; iMod < fDetectorGEM->GetGemStation(iStat)->GetNModules(); iMod++) {
    //            misAlign[iStat][iMod] = new Double_t[3];
    //            for (Int_t iPar = 0; iPar < 3; iPar++)
    //                misAlign[iStat][iMod][iPar] = 0.;
    //        }
    //    }

    //    if (fIsDoTest && fBmnGemMisalign) {
    //        for (Int_t iCorr = 0; iCorr < fBmnGemMisalign->GetEntriesFast(); iCorr++) {
    //            BmnGemAlignCorrections* align = (BmnGemAlignCorrections*) fBmnGemMisalign->UncheckedAt(iCorr);
    //            Int_t iStat = align->GetStation();
    //            Int_t iMod = align->GetModule();
    //            TVector3 corrsXYZ = align->GetCorrections();
    //            misAlign[iStat][iMod][0] = corrsXYZ.X();
    //            shiftX += misAlign[iStat][iMod][0];
    //            misAlign[iStat][iMod][1] = corrsXYZ.Y();
    //            shiftY += misAlign[iStat][iMod][1];
    //            misAlign[iStat][iMod][2] = corrsXYZ.Z();
    //            shiftZ += misAlign[iStat][iMod][2];
    //        }
    //    }

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
                for (Int_t iPar = 0; iPar < fDetectorGEM->GetGemStation(iStat)->GetNModules() * nParams; iPar++) {
                    if (!fDetectorSet[0])
                        fprintf(steer, "%d %G %G\n", Labels[startIdx + iPar], 0., -1.);
                    else
                        fprintf(steer, "%d %G %G\n", Labels[startIdx + iPar], 0., (fixedGemElements[iStat][iPar / nParams]) ? -1. : fPreSigma);
                }
                startIdx += fDetectorGEM->GetGemStation(iStat)->GetNModules() * nParams;
            }
        } else if (iDet == 1) { // Process SILICON to mark fixed modules if exist
            for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++) {
                for (Int_t iPar = 0; iPar < fDetectorSI->GetSiliconStation(iStat)->GetNModules() * nParams; iPar++) {
                    if (!fDetectorSet[1])
                        fprintf(steer, "%d %G %G\n", Labels[startIdx + iPar], 0., -1.);
                    else
                        fprintf(steer, "%d %G %G\n", Labels[startIdx + iPar], 0., (fixedSiElements[iStat][iPar / nParams]) ? -1. : (fIsDoTest) ? -1. : fPreSigma);
                }
                startIdx += fDetectorSI->GetSiliconStation(iStat)->GetNModules() * nParams;
            }
        }
    }

    if (!fUseConstraints) {
        fclose(steer);
        return;
    }

    // Calculate center-of-gravity along Z-axis (GEM and SI)
    map <pair <Int_t, Int_t>, Double_t> z_GEM;
    map <pair <Int_t, Int_t>, Double_t> z_SI;

    if (fDetectorSet[0]) {
        for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
            for (Int_t iMod = 0; iMod < fDetectorGEM->GetGemStation(iStat)->GetNModules(); iMod++)
                if (!fixedGemElements[iStat][iMod])
                    z_GEM[pair <Int_t, Int_t> (iStat, iMod)] = fDetectorGEM->GetGemStation(iStat)->GetModule(iMod)->GetZPositionRegistered();
            }

    if (fDetectorSet[1]) {
        for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++)
            for (Int_t iMod = 0; iMod < fDetectorSI->GetSiliconStation(iStat)->GetNModules(); iMod++)
                if (!fixedSiElements[iStat][iMod])
                    z_SI[pair <Int_t, Int_t> (iStat, iMod)] = fDetectorSI->GetSiliconStation(iStat)->GetModule(iMod)->GetZPositionRegistered();
    }

    Double_t zSum = 0.;
    for (auto it : z_GEM)
        zSum += it.second;

    for (auto it : z_SI)
        zSum += it.second;

    Double_t zC = zSum / (z_GEM.size() + z_SI.size());
    cout << zC << " " << z_GEM.size() + z_SI.size() << endl;

    // Calculate dZ = Zpos - Zc (GEM and SI)
    map <pair <Int_t, Int_t>, Double_t> dZ_GEM;
    map <pair <Int_t, Int_t>, Double_t> dZ_SI;

    for (auto it : z_GEM)
        dZ_GEM[it.first] = it.second - zC;

    for (auto it : z_SI)
        dZ_SI[it.first] = it.second - zC;

    map <Int_t, Double_t> deltas; // Label --> Correct dZ

    for (auto it : dZ_GEM) {
        Int_t stat = it.first.first;
        Int_t mod = it.first.second;
        Int_t labZ = GemStatModLabel(stat, mod);
        Int_t labY = labZ - 1;
        Int_t labX = labY - 1;
        deltas[labX] = it.second;
        deltas[labY] = it.second;
        deltas[labZ] = it.second + zC;
    }

    for (auto it : dZ_SI) {
        Int_t stat = it.first.first;
        Int_t mod = it.first.second;
        Int_t labZ = SiliconStatModLabel(stat, mod);
        Int_t labY = labZ - 1;
        Int_t labX = labY - 1;
        deltas[labX] = it.second;
        deltas[labY] = it.second;
        deltas[labZ] = it.second + zC;
    }

    if (!fDetectorSet[0] && !fDetectorSet[1])
        return;

    // Apply constraints ...
    // Wi * a_Xi = 0,    (iStep = 0), Wi = 1.
    // Wi * a_Yi = 0,    (iStep = 1), Wi = 1.
    // Wi * a_Zi = 0,    (iStep = 2), Wi = 1.
    for (Int_t iRemain = 0; iRemain < nParams; iRemain++) {
        fprintf(steer, "constraint 0.0\n");
        if (fDetectorSet[0])
            for (Int_t iPar = 0; iPar < parCounterGem * nParams; iPar++) {
                Int_t stat = GetGemStatMod(Labels[iPar])[0];
                Int_t mod = GetGemStatMod(Labels[iPar])[1];
                if (Labels[iPar] % nParams == iRemain && !fixedGemElements[stat][mod])
                    fprintf(steer, "%d %G\n", Labels[iPar], 1.);
            }

        if (fDetectorSet[1])
            for (Int_t iPar = parCounterGem * nParams; iPar < fNGL; iPar++) {
                Int_t stat = GetSiliconStatMod(Labels[iPar])[0];
                Int_t mod = GetSiliconStatMod(Labels[iPar])[1];
                if (Labels[iPar] % nParams == iRemain && !fixedSiElements[stat][mod])
                    fprintf(steer, "%d %G\n", Labels[iPar], 1.);
            }
    }

    // Wi * a_Xi = 0,    (iStep = 0), Wi = delta_Zi
    // Wi * a_Yi = 0,    (iStep = 1), Wi = delta_Zi
    // Wi * a_Zi = 0,    (iStep = 2), Wi = Zi
    for (Int_t iRemain = 0; iRemain < nParams; iRemain++) {
        fprintf(steer, "constraint 0.0\n");
        for (auto it : deltas) {
            if (it.first <= parCounterGem * nParams) {
                Int_t stat = GetGemStatMod(it.first)[0];
                Int_t mod = GetGemStatMod(it.first)[1];
                if (it.first % nParams == iRemain && !fixedGemElements[stat][mod])
                    fprintf(steer, "%d %G\n", it.first, it.second);
            } else {
                Int_t stat = GetSiliconStatMod(it.first)[0];
                Int_t mod = GetSiliconStatMod(it.first)[1];
                if (it.first % nParams == iRemain && !fixedSiElements[stat][mod])
                    fprintf(steer, "%d %G\n", it.first, it.second);
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
    fDetectorSet = new Bool_t[nDetectors]();

    // Choose appropriate configuration (BM@N or SRC)
    Bool_t isSRC = (fRunId < 3589) ? kTRUE : kFALSE; // FIXME
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString confSi = "SiliconRun" + TString(isSRC ? "SRC" : "") + "Spring2018.xml";
    TString confGem = "GemRun" + TString(isSRC ? "SRC" : "") + "Spring2018.xml";

    /// SI
    TString gPathSiliconConfig = gPathConfig + "/parameters/silicon/XMLConfigs/";
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
    TString gPathGemConfig = gPathConfig + "/parameters/gem/XMLConfigs/";
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

