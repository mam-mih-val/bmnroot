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
#include "BmnGlobalAlignment.h"

Int_t BmnGlobalAlignment::fCurrentEvent = 0;
Int_t BmnGlobalAlignment::trackCounter = 0;

BmnGlobalAlignment::~BmnGlobalAlignment() {
    for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++)
        delete [] fixedGemElements[iStat];
    delete [] fixedGemElements;

    delete [] fDetectorSet;
    delete mwpcGeo;
    delete fDetector;
    delete [] Labels;
    system("cp millepede.res Millepede.res");
    // Remove useless text files finalizing the code execution
    system("rm millepede.*");
}

BmnGlobalAlignment::BmnGlobalAlignment(BmnGemStripConfiguration::GEM_CONFIG config) :
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
fGeometry(config),
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
fDebug(kFALSE) {
    nDetectors = 3;
    fDetectorSet = new TString[nDetectors]();

    // Create GEM detector ------------------------------------------------------
    switch (fGeometry) {
        case BmnGemStripConfiguration::RunWinter2016:
            fDetector = new BmnGemStripStationSet_RunWinter2016(fGeometry);
            cout << "   Current Configuration : RunWinter2016" << "\n";
            break;

        case BmnGemStripConfiguration::RunSpring2017:
            fDetector = new BmnGemStripStationSet_RunSpring2017(fGeometry);
            cout << "   Current Configuration : RunSpring2017" << "\n";
            break;

        default:
            fDetector = NULL;
    }

    fixedGemElements = new Bool_t*[fDetector->GetNStations()];
    for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++) {
        fixedGemElements[iStat] = new Bool_t[fDetector->GetGemStation(iStat)->GetNModules()];
    }

    for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < fDetector->GetGemStation(iStat)->GetNModules(); iMod++)
            fixedGemElements[iStat][iMod] = kFALSE;

    // Initialize MWPC geometry
    mwpcGeo = new BmnMwpcGeometry();

    fBranchMwpcHits = "BmnMwpcHit";
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
}

InitStatus BmnGlobalAlignment::Init() {
    cout << " BmnGlobalAlignment::Init() " << endl;
    Bool_t isUsedMwpc = kTRUE, isUsedGem = kTRUE, isUsedDch = kTRUE;

    if (fDetectorSet[0] == "")
        isUsedGem = kFALSE;

    if (fDetectorSet[1] == "")
        isUsedMwpc = kFALSE;

    if (fDetectorSet[2] == "")
        isUsedDch = kFALSE;

    cout << "Use detectors: MWPC - " << isUsedMwpc << " GEM - " <<
            isUsedGem << " DCH - " << isUsedDch << endl;

    FairRootManager* ioman = FairRootManager::Instance();

    fMwpcHits = (TClonesArray*) ioman->GetObject(fBranchMwpcHits.Data());
    fMwpcTracks = (TClonesArray*) ioman->GetObject(fBranchMwpcTracks.Data());

    fGemHits = (TClonesArray*) ioman->GetObject(fBranchGemHits.Data());
    fGemTracks = (TClonesArray*) ioman->GetObject(fBranchGemTracks.Data());

    fTof1Hits = (TClonesArray*) ioman->GetObject(fBranchTof1Hits.Data());

    fDchHits = (TClonesArray*) ioman->GetObject(fBranchDchHits.Data());
    fDchTracks = (TClonesArray*) ioman->GetObject(fBranchDchTracks.Data());

    fGlobalTracks = (TClonesArray*) ioman->GetObject(fBranchGlobalTracks.Data());

    fMwpcAlignCorr = new TClonesArray(fBranchMwpcAlignCorr.Data());
    fGemAlignCorr = new TClonesArray(fBranchGemAlignCorr.Data());
    fTofAlignCorr = new TClonesArray(fBranchTofAlignCorr.Data());
    fDchAlignCorr = new TClonesArray(fBranchDchAlignCorr.Data());

    ioman->Register(fBranchMwpcAlignCorr.Data(), "MWPC", fMwpcAlignCorr, kTRUE);
    ioman->Register(fBranchGemAlignCorr.Data(), "GEM", fGemAlignCorr, kTRUE);
    // ioman->Register(fBranchTofAlignCorr.Data(), "TOF", fTofAlignCorr, kTRUE);
    ioman->Register(fBranchDchAlignCorr.Data(), "DCH", fDchAlignCorr, kTRUE);

    fChain = ioman->GetInChain();
    fRecoFileName = ioman->GetInFile()->GetName();
    fin_txt = fopen("alignment.txt", "w");
    return kSUCCESS;
}

void BmnGlobalAlignment::Exec(Option_t* opt) {
    fCurrentEvent++;
    if (fCurrentEvent % 1000 == 0)
        cout << "Event# = " << fCurrentEvent << endl;

    // Choose a single track with min. chi2-value (in case of target run)
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

    for (Int_t iGlobTrack = 0; iGlobTrack < fGlobalTracks->GetEntriesFast(); iGlobTrack++) {
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

        // GCC-4.4.7, to be fixed
        Int_t idx[3] = {globTrack->GetGemTrackIndex(), globTrack->GetMwpcTrackIndex(), globTrack->GetDchTrackIndex()};
        Char_t buff[5000] = {""};
        Bool_t zhopa = kFALSE;

        for (Int_t iDet = 0; iDet < nDetectors; iDet++) {
            TString detName = (iDet == 0) ? "GEM" : (iDet == 1) ? "MWPC" : (iDet == 2) ? "DCH" : "";
            if (fDetectorSet[iDet] != "")
                if (idx[iDet] != -1) {
                    sprintf(buff, "%s%s\n", buff, detName.Data());
                    Mille(idx[iDet], iDet, buff);
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
    Int_t zeroLoc = fNLC;
    Int_t zeroGem = 0;
    Int_t zeroMwpc = 3; // To be fixed !!!
    Int_t zeroDch = 3; // To be fixed !!!
    Int_t nMeas = 2;

    for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++)
        zeroGem += 3 * fDetector->GetGemStation(iStat)->GetNModules();

    Int_t zeroTot = zeroLoc + zeroGem + zeroMwpc + zeroDch + nMeas;
    TString zeroLine = "";
    for (Int_t iZero = 0; iZero < zeroTot; iZero++)
        zeroLine += "0. ";

    sprintf(buff, "%s%s\n", buff, detName.Data());
    for (Int_t iRow = 0; iRow < 2; iRow++)
        sprintf(buff, "%s%s\n", buff, zeroLine.Data());
}

void BmnGlobalAlignment::Mille(Int_t idx, Int_t iDet, Char_t* buff) {
    Int_t modGemTotal = 0;
    TString mwpcPart = "0.0 0.0 0.0";
    TString dchPart = mwpcPart;

    for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++)
        modGemTotal += fDetector->GetGemStation(iStat)->GetNModules();

    if (iDet == 0) {
        BmnGemTrack* track = (BmnGemTrack*) fGemTracks->UncheckedAt(idx);
        Int_t nModulesProcessed = 0;
        for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++) {
            Int_t nMod = fDetector->GetGemStation(iStat)->GetNModules();
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

                    for (Int_t iRow = 0; iRow < 2; iRow++)
                        sprintf(buff, "%s%d %d 0. 0. 0. 0. %s 0. 0.\n", buff, iStat, iMod, zeroLine.Data()); // local = 4 -> zeroLine -> meas -> dmeas
                }
            }
        }
    }

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
}

const Int_t BmnGlobalAlignment::MakeBinFile() {
    ifstream fout_txt;
    fout_txt.open("alignment.txt", ios::in);

    // Calculate number of glob. params.
    const Int_t ngl_per_subdetector = 3; // x, y and z corrs to each det. subsyst. (GEM, MWPC, DCH at the moment)
    // GEM
    Int_t gem = 0;
    for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++)
        gem += ngl_per_subdetector * fDetector->GetGemStation(iStat)->GetNModules();

    // MWPC
    Int_t mwpc = ngl_per_subdetector;

    // DCH
    Int_t dch = ngl_per_subdetector;

    const Int_t dimLabel = gem + mwpc + dch;
    Labels = new Int_t[dimLabel];
    for (Int_t iEle = 0; iEle < dimLabel; iEle++)
        Labels[iEle] = 1 + iEle;

    BmnMille* Mille = new BmnMille("alignment.bin", kTRUE, kFALSE);

    TString detName, coordName;
    Int_t stat, mod;
    Double_t DerGl[dimLabel], DerLc[fNLC];
    Double_t rMeasure, dMeasure;

    // Loop over selected tracks
    // fDebug = false;
    for (Int_t iTrack = 0; iTrack < nSelectedTracks; iTrack++) {
        // Read GEM info
        fout_txt >> detName;
        if (fDebug)
            cout << detName << endl;

        for (Int_t iStation = 0; iStation < fDetector->GetNStations(); iStation++) {
            for (Int_t iMod = 0; iMod < fDetector->GetGemStation(iStation)->GetNModules(); iMod++) {
                for (Int_t iLine = 0; iLine < 2; iLine++) {
                    coordName = (iLine == 0) ? " x = " : " y = ";
                    fout_txt >> stat >> mod;
                    for (Int_t iVar = 0; iVar < fNLC; iVar++)
                        fout_txt >> DerLc[iVar];
                    for (Int_t iVar = 0; iVar < dimLabel; iVar++)
                        fout_txt >> DerGl[iVar];
                    fout_txt >> rMeasure >> dMeasure;
                    Mille->mille(fNLC, DerLc, dimLabel, DerGl, Labels, rMeasure, dMeasure);
                    if (fDebug)
                        cout << "Stat: " << stat << " Mod: " << mod << coordName << rMeasure << " " << dMeasure << endl;
                }
            }
        }

        // Read MWPC and DCH info
        for (Int_t iDet = 0; iDet < 2; iDet++) {
            fout_txt >> detName;
            if (fDebug)
                cout << detName << endl;

            for (Int_t iLine = 0; iLine < 2; iLine++) {
                coordName = (iLine == 0) ? " x = " : " y = ";
                for (Int_t iVar = 0; iVar < fNLC; iVar++)
                    fout_txt >> DerLc[iVar];
                for (Int_t iVar = 0; iVar < dimLabel; iVar++)
                    fout_txt >> DerGl[iVar];
                fout_txt >> rMeasure >> dMeasure;
                Mille->mille(fNLC, DerLc, dimLabel, DerGl, Labels, rMeasure, dMeasure);
                if (fDebug)
                    cout << coordName << rMeasure << " " << dMeasure << endl;
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

    // Add new det. idx. if necessary
    const Int_t nDets = 3;
    const Int_t nBounds = 2;

    // Reserved labels for det. subsystems: GEM, MWPC, DCH (0, 1, 2)
    Int_t** idxBound = new Int_t*[nDets];
    for (Int_t iDet = 0; iDet < nDets; iDet++)
        idxBound[iDet] = new Int_t[nBounds];

    Int_t parCounter = 0;
    const Int_t nParams = 3;
    for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < fDetector->GetGemStation(iStat)->GetNModules(); iMod++)
            parCounter++;

    idxBound[0][0] = Labels[0];
    idxBound[0][1] = nParams * parCounter;
    idxBound[1][0] = idxBound[0][1] + 1;
    idxBound[1][1] = idxBound[1][0] + (nParams - 1);
    idxBound[2][0] = idxBound[1][1] + 1;
    idxBound[2][1] = idxBound[2][0] + (nParams - 1);

    Int_t startIdx = 0;
    for (Int_t iDet = 0; iDet < nDets; iDet++) {
        if (iDet == 0) // Process GEMs to mark fixed stations if exist
            for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++) {
                for (Int_t iPar = 0; iPar < fDetector->GetGemStation(iStat)->GetNModules() * nParams; iPar++)
                    fprintf(steer, "%d %G %G\n", Labels[startIdx + iPar], 0., (fixedGemElements[iStat][iPar / nParams]) ? -1. : fPreSigma);
                startIdx += fDetector->GetGemStation(iStat)->GetNModules() * nParams;
            } else if (iDet == 1 || iDet == 2) // MWPC and DCH
            for (Int_t iPar = startIdx + (iDet - 1) * nParams; iPar < startIdx + iDet * nParams; iPar++)
                fprintf(steer, "%d %G %G\n", Labels[iPar], 0., (fDetectorSet[iDet] != "") ? fPreSigma : -1.);
    }

    // GEMs
    // 1. Calculate center-of-gravity along Z-axis
    Double_t zSum = 0.;
    Int_t modCounter = 0;
    for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < fDetector->GetGemStation(iStat)->GetNModules(); iMod++) {
            zSum += fDetector->GetGemStation(iStat)->GetModule(iMod)->GetZStartModulePosition();
            modCounter++;
        }
    Double_t zC = zSum / modCounter;

    // 2. Calculate dZ = Zpos - Zc for each GEM plane
    Double_t deltaZ[modCounter];
    for (Int_t iMod = 0; iMod < modCounter; iMod++)
        deltaZ[iMod] = 0.;

    modCounter = 0;
    for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < fDetector->GetGemStation(iStat)->GetNModules(); iMod++) {
            deltaZ[modCounter] = fDetector->GetGemStation(iStat)->GetModule(iMod)->GetZStartModulePosition() - zC;
            modCounter++;
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

    for (Int_t iConstrSet = 0; iConstrSet < 2; iConstrSet++)
        for (Int_t iStep = 0; iStep < 3; iStep++) {
            fprintf(steer, "constraint 0.0\n");
            modCounter = 0;
            for (Int_t iPar = 0; iPar < dim; iPar++) {
                // Remember: this condition is used for GEMs only
                if (Labels[iPar] > idxBound[0][1])
                    break;

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
    if (!resFile)
        Fatal("BmnGlobalAlignment::ReadPedeOutput", "No input file found!!");
    resFile.ignore(numeric_limits<streamsize>::max(), '\n');

    const Int_t nParams = 3;
    Double_t* corrs = new Double_t[nParams];

    // Read GEMs
    for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++) {
        for (Int_t iMod = 0; iMod < fDetector->GetGemStation(iStat)->GetNModules(); iMod++) {
            ExtractCorrValues(resFile, corrs);
            BmnGemAlignCorrections* gemCorrs = new((*fGemAlignCorr)[fGemAlignCorr->GetEntriesFast()]) BmnGemAlignCorrections();
            gemCorrs->SetStation(iStat);
            gemCorrs->SetModule(iMod);
            gemCorrs->SetCorrections(corrs);
        }
    }

    // Read MWPC and DCH
    for (Int_t iDet = 0; iDet < 2; iDet++) {
        ExtractCorrValues(resFile, corrs);
        if (iDet == 0) {
            BmnMwpcAlignCorrections* mwpcCorrs = new((*fMwpcAlignCorr)[fMwpcAlignCorr->GetEntriesFast()]) BmnMwpcAlignCorrections();
            mwpcCorrs->SetCorrections(corrs);
        } else {
            BmnDchAlignCorrections* dchCorrs = new((*fDchAlignCorr)[fDchAlignCorr->GetEntriesFast()]) BmnDchAlignCorrections();
            dchCorrs->SetCorrections(corrs);
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
