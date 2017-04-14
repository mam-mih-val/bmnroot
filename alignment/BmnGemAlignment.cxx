// @(#)bmnroot/alignment:$Id$
// Author: Pavel Batyk <pavel.batyuk@jinr.ru> 2016-01-01

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnGemAlignment                                                            //
//                                                                            //
// Alignment of GEM tracking detectors.                                       //
//                                                                            //
// Uses Volker Blobel and Claus Kleinwort Millepede II                        //
//                                                                            //
//                                                                            //
//                                                                            //
//                                                                            //
//                                                                            //
//                                                                            //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include   "BmnGemAlignment.h"

#include   <algorithm>
#include   <climits>
#include   <cstdio>
#include   <iomanip>
#include   <sstream>
#include   <Rtypes.h>

#include   "TRegexp.h"

#include   "FairTrackParam.h"

#include   "BmnGemAlignmentCorrections.h"
#include   "BmnGemTrack.h"
#include   "BmnMath.h"
#include   "BmnMille.h"

using namespace TMath;

Int_t BmnGemAlignment::fCurrentEvent = 0;

BmnGemAlignment::BmnGemAlignment() :
fResultName(""),
fDebugInfo(kFALSE),
fPreSigma(1.),
fName(""),
fAccuracy(1e-3),
fNumOfIterations(50000),
fNGL(0),
nSelectedTracks(0),
fChi2MaxPerNDF(LDBL_MAX),
fMinHitsAccepted(3),
fUseRealHitErrors(kFALSE),
fTxMin(-LDBL_MAX),
fTxMax(LDBL_MAX),
fTyMin(-LDBL_MAX),
fTyMax(LDBL_MAX),
fUseRegularization(kFALSE),
fHugecut(50.),
fEntries(10),
fOutlierdownweighting(0),
fDwfractioncut(0.0),
fFixX(kFALSE),
fFixY(kFALSE),
fFixZ(kFALSE),
fUseTrackWithMinChi2(kFALSE),
fRunType("target")
{
    fChisqcut[0] = 0.0;
    fChisqcut[1] = 0.0;
    // Declare branch names here
    hitsBranch      = "BmnGemStripHit";
    tracksBranch    = "BmnGemTrack";
    alignCorrBranch = "BmnGemAlignmentCorrections";
    fDetector = new BmnGemStripStationSet_RunWinter2016(fGeometry);
}

InitStatus BmnGemAlignment::Init()
{
    cout <<endl<<"BmnGemAlignment::Init()"<< endl;
    FairRootManager* ioman = FairRootManager::Instance();
    fGemHits   = (TClonesArray*)ioman->GetObject(hitsBranch.Data());
    fGemTracks = (TClonesArray*)ioman->GetObject(tracksBranch.Data());
    fAlignCorr = new TClonesArray(alignCorrBranch.Data());
    ioman->Register(alignCorrBranch.Data(), "GEM", fAlignCorr, kTRUE);
    if (fRunType == "")
        Fatal("BmnGemReco::Init()", "Alignment type has not been specified!!!");
    SetRunType(fRunType);
    fChain = ioman->GetInChain();
    fRecoFileName = ioman->GetInFile()->GetName();
    fName = "alignment_"+fAlignmentType;
    fin_txt = fopen(TString(fName+".txt").Data(), "w");
    return kSUCCESS;
}

void BmnGemAlignment::Exec(Option_t* opt)
{
    StartMille();
    if (fChain->GetEntries() == fCurrentEvent) {
        fclose(fin_txt);
        cout << "Num. of tracks to be used: " << nSelectedTracks << endl;
        BinFilePede(); // Prepare bin-file for PEDE
        StartPede(); // Start PEDE
    }
}

void BmnGemAlignment::StartMille()
{
    Int_t nPar = (fAlignmentType == "xy") ? 2 : 3;
    fCurrentEvent++;
    if (fCurrentEvent % 1000 == 0)
        cout << "Event# = " << fCurrentEvent << endl;
    Int_t modTotal = 0;
    for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++)
        modTotal += fDetector->GetGemStation(iStat)->GetNModules();

    // Choose a single track with min. chi2-value (in case of target run)
    Double_t Chi2Min = LDBL_MAX;
    Int_t trID = 0;
    if (fUseTrackWithMinChi2) {
        for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
            BmnGemTrack* track = (BmnGemTrack*) fGemTracks->UncheckedAt(iTrack);
            if (track->GetChi2() < Chi2Min) {
                Chi2Min = track->GetChi2();
                trID = iTrack;
            }
        }
    }
    for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
        BmnGemTrack* track = (BmnGemTrack*) fGemTracks->UncheckedAt(iTrack);
        if (fUseTrackWithMinChi2)
            if (iTrack != trID)
                continue;
        FairTrackParam* params = track->GetParamFirst();
        Double_t chi2 = track->GetChi2();
        Double_t ndf = track->GetNDF();
        // Use track constraints if necessary
        if (!(
                fTxMin < params->GetTx() && params->GetTx() < fTxMax
                &&
                fTyMin < params->GetTy() && params->GetTy() < fTyMax
                &&
                track->GetNHits() > fMinHitsAccepted
                &&
                chi2 / ndf < fChi2MaxPerNDF))
            continue;
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
                    TString globDerX = "", globDerY = "", zeroEnd = "", zeroBeg = "";
                    if (stat == iStat && mod == iMod) {
                        Double_t X = hit->GetX();
                        Double_t Y = hit->GetY();
                        Double_t Z = hit->GetZ();
                        Char_t* locDerX = Form("%d %d 1. %f 0. 0. ", stat, mod, Z);
                        Char_t* locDerY = Form("%d %d 0. 0. 1. %f ", stat, mod, Z);
                        Char_t* measX = Form("%f %f ", X, fUseRealHitErrors ? hit->GetDx() : 1.);
                        Char_t* measY = Form("%f %f ", Y, fUseRealHitErrors ? hit->GetDy() : 1.);
                        Int_t N_zeros_beg = nPar * (nModulesProcessed - 1);
                        Int_t N_zeros_end = nPar * (modTotal - nModulesProcessed);
                        if (fAlignmentType == "xy") {
                            globDerX = "1. 0.";
                            globDerY = "0. 1.";
                        } else {
                            globDerX = TString(Form("1. 0. %f", track->GetParamFirst()->GetTx()));
                            globDerY = TString(Form("0. 1. %f", track->GetParamFirst()->GetTy()));
                        }
                        for (Int_t i = 0; i < N_zeros_beg; i++)
                            zeroBeg += "0. ";
                        for (Int_t i = 0; i < N_zeros_end; i++)
                            zeroEnd += "0. ";
                        fprintf(fin_txt, "%s%s %s %s%s\n", locDerX, zeroBeg.Data(), globDerX.Data(), zeroEnd.Data(), measX);
                        fprintf(fin_txt, "%s%s %s %s%s\n", locDerY, zeroBeg.Data(), globDerY.Data(), zeroEnd.Data(), measY);
                        break;
                    }
                }
                if (iHit == track->GetNHits()) {
                    TString zeroLine = "";
                    for (Int_t iModTot = 0; iModTot < nPar * modTotal; iModTot++)
                        zeroLine += "0. ";
                    for (Int_t iRow = 0; iRow < 2; iRow++)
                        fprintf(fin_txt, "%d %d 0. 0. 0. 0. %s 0. 0.\n", iStat, iMod, zeroLine.Data()); // local = 4 -> zeroLine -> meas -> dmeas
                }
            }
        }
        nSelectedTracks++;
    }
}

void BmnGemAlignment::BinFilePede()
{
    ifstream fout_txt;
    fout_txt.open(TString(fName + ".txt").Data(), ios::in);
    Int_t NLC = 4;
    Int_t NGL_PER_STAT = (fAlignmentType == "xy") ? 2 : 3;
    // Calculate number of glob. params.
    Int_t nTotMod = 0;
    for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++)
        nTotMod += fDetector->GetGemStation(iStat)->GetNModules();
    fNGL = NGL_PER_STAT * nTotMod;
    const Int_t dim = fNGL;
    Int_t* Labels = new Int_t[dim];
    for (Int_t iEle = 0; iEle < dim; iEle++)
        Labels[iEle] = 1 + iEle;
    Double_t rMeasure, dMeasure;
    Int_t nMod;
    Int_t nGem;
    Double_t DerGl[dim], DerLc[NLC];
    BmnMille* Mille = new BmnMille(TString(fName + ".bin").Data(), kTRUE, kFALSE);
    for (Int_t iTrack = 0; iTrack < nSelectedTracks; iTrack++) {
        for (Int_t iDim = 0; iDim < 2; iDim++)
            for (Int_t iStation = 0; iStation < fDetector->GetNStations(); iStation++) {
                for (Int_t iMod = 0; iMod < fDetector->GetGemStation(iStation)->GetNModules(); iMod++) {
                    fout_txt >> nGem >> nMod;
                    for (Int_t iVar = 0; iVar < NLC; iVar++)
                        fout_txt >> DerLc[iVar];
                    for (Int_t iVar = 0; iVar < dim; iVar++)
                        fout_txt >> DerGl[iVar];
                    fout_txt >> rMeasure >> dMeasure;
                    Mille->mille(NLC, DerLc, dim, DerGl, Labels, rMeasure, dMeasure);
                    if (fDebugInfo)
                        DebugInfo(nGem, nMod, NLC, dim, DerLc, DerGl, rMeasure, dMeasure);
                }
            }
        Mille->end();
        if (fDebugInfo)
            cout << "========================> Another one RECORD = " << iTrack + 1 << " --> " << endl;
    }
    delete Mille;
    delete Labels;
    fout_txt.close();
}

void BmnGemAlignment::StartPede()
{
    MakeSteerFile(); // Create initial steer file
    fCommandToRunPede = "pede steer.txt";
    system(fCommandToRunPede.Data());
    vector<Double_t> corr; // vector to store obtained corrections
    // Put alignment corrections to outFile
    ifstream resFile("millepede.res", ios::in);
    ReadPedeOutput(resFile, corr);
    resFile.close();
    const Int_t nStat = fDetector->GetNStations();
    Int_t nEntries = 0;
    // ref test:
    TClonesArray& fAlignCorrRef = *fAlignCorr;
    Int_t corrStoreCounter = 0; // ref test

    for (Int_t iStat = 0; iStat < nStat; iStat++) {
        Int_t nModul = fDetector->GetGemStation(iStat)->GetNModules();
        for (Int_t iMod = 0; iMod < nModul; iMod++) {
            Int_t corrCounter = 0;
            Double_t buff[3] = {0., 0., 0.};
            if (fDebugInfo)
                cout <<"nEntries = "<<nEntries<<" fNGL+1 = "<<fNGL+1<< endl;
            for (Int_t iPar=nEntries; iPar<fNGL+1; iPar++) {
                buff[corrCounter] = corr[iPar];
                corrCounter++;
                if (corrCounter == ((fAlignmentType == "xy") ? 2 : 3)) {
                   if (fDebugInfo) {
                        cout <<"fAlignCorr->GetEntriesFast() = "<<fAlignCorr->GetEntriesFast()<< endl;
                        cout <<"corrStoreCounter             = "<<corrStoreCounter            << endl;
                    }

                  //BmnGemAlignmentCorrections* tmp = new((*fAlignCorr)[fAlignCorr->GetEntriesFast()]) BmnGemAlignmentCorrections();
                    // ref test:
                    BmnGemAlignmentCorrections* tmp = new(  fAlignCorrRef[corrStoreCounter])           BmnGemAlignmentCorrections();

                    tmp->SetStation(iStat);
                    tmp->SetModule(iMod);
                    tmp->SetX(buff[0]);
                    tmp->SetY(buff[1]);
                    tmp->SetZ(buff[2]);
                    // ref test:
                    corrStoreCounter++;

                    if (fDebugInfo)
                        cout <<"iStat = "<<iStat<<" iMod = "<<iMod<<" iPar = "<<iPar<<" "<<buff[0]<<" "<<buff[1]<<" "<<buff[2]<< endl;
                }
                if ((iPar + 1) % ((fAlignmentType == "xy") ? 2 : 3) == 0) {
                    nEntries = iPar + 1;
                    break;
                }
            }
        }
    }
    //system(Form("cp millepede.res Millepede_%s_%s.res", fRecoFileName.Data(), fAlignmentType.Data()));

    // insert fAlignmentType before iteration number:
    TRegexp re = "_it[0-9]+";
    if (fResultName.Contains("_it")) {
        TSubString itNr = fResultName(re);
        TString fResultNameNoItNr = fResultName;
        fResultNameNoItNr.ReplaceAll(itNr, "");
        if (fDebugInfo) cout <<"fResultNameNoItNr = "+fResultNameNoItNr<< endl;
        system("cp millepede.res Millepede_"+fResultNameNoItNr+"_"+fAlignmentType+itNr+".res"); }
    else {
        if (fDebugInfo) cout <<"fResultName       = "+fResultName      << endl;
        system("cp millepede.res Millepede_"+fResultName      +"_"+fAlignmentType     +".res");
    }
    // Anatoly.Solomin@jinr.ru 2017-02-15 20:42:13

    system("rm millepede.*");
}

void BmnGemAlignment::ReadPedeOutput(ifstream& resFile, vector<Double_t>& corr)
{
    if (!resFile)
        Fatal("BmnGemReco::ReadPedeOutput", "No input file found!!");
    TString buff1 = "", buff2 = "", buff3 = "", buff4 = "", buff5 = "";
    string line;
    // Go to the second string of existing millepede.res
    resFile.ignore(numeric_limits<streamsize>::max(), '\n');
    while (getline(resFile, line)) {
        stringstream ss(line);
        Int_t size = ss.str().length();
        // 40 and 68 symbols are fixed in the Pede-output by a given format
        if (size == 40)
            ss >> buff1 >> buff2 >> buff3;
        else if (size == 68)
            ss >> buff1 >> buff2 >> buff3 >> buff4 >> buff5;
        else
            cout << "Unsupported format given!" << endl;
        corr.push_back(buff2.Atof());
    }
}

void BmnGemAlignment::MakeSteerFile()
{
    FILE* steer = fopen("steer.txt", "w");
    TString alignType = "alignment_" + fAlignmentType + ".bin";
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
    Int_t nEntries = 0;
    for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++) {
        for (Int_t iMod = 0; iMod < fDetector->GetGemStation(iStat)->GetNModules(); iMod++) {
            for (Int_t iPar = nEntries; iPar < fNGL + 1; iPar++) {
                fprintf(steer, "%d %G ", iPar + 1, 0.);
                for (Int_t iSize = 0; iSize < fFixedStats.size(); iSize++) {
                    if (find(fFixedStats.begin(), fFixedStats.end(), iStat) != fFixedStats.end())
                        fprintf(steer, "%G\n", -1.);
                    else
                        fprintf(steer, "%G\n", fPreSigma);
                    break;
                }
                if ((iPar + 1) % ((fAlignmentType == "xy") ? 2 : 3) == 0) {
                    nEntries = iPar + 1;
                    break;
                }
            }
        }
    }

    // Calculate Zc, stat = 0 is not taken into account.
    Double_t zSum = 0.;
    Int_t modCounter = 0;
    for (Int_t iStat = 1; iStat < fDetector->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < fDetector->GetGemStation(iStat)->GetNModules(); iMod++) {
            zSum += fDetector->GetGemStation(iStat)->GetModule(iMod)->GetZStartModulePosition();
            modCounter++;
        }

    Double_t zC = zSum / modCounter;
    Double_t* deltaZ = new Double_t[modCounter];
    modCounter = 0;
    for (Int_t iStat = 1; iStat < fDetector->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < fDetector->GetGemStation(iStat)->GetNModules(); iMod++) {
            deltaZ[modCounter] = fDetector->GetGemStation(iStat)->GetModule(iMod)->GetZStartModulePosition() - zC;
            modCounter++;
        }

    // w_i(delta_Z_i) * aX_i = 0, w_i(delta_Z_i) * aY_i = 0
    for (Int_t iVar = 0; iVar < 3; iVar++) {
        modCounter = 0;
        fprintf(steer, "constraint 0.0\n");
        for (Int_t iPar = 4; iPar < fNGL + 1; iPar++) // Since we reject stat = 0
            if (iPar % ((fAlignmentType == "xy") ? 2 : 3) == iVar) {
                Float_t buff = (iVar == 0) ? zC : 0.;
                fprintf(steer, "%d %G\n", iPar, deltaZ[modCounter] + buff);
                modCounter++;
            }
    }

    delete [] deltaZ;

    // 1. * aX_i = 0, 1. * aY_i = 0,  1. * aZ_i = 0
    for (Int_t iDim = 0; iDim < 3; iDim++) {
        Int_t remain = (iDim == 0 && fFixX) ? 1 : (iDim == 1 && fFixY) ? 0 : (iDim == 2 && fFixZ) ? 2 : (-1);
        if (remain < 0 || (remain == 2 && fAlignmentType == "xy"))
            continue;
        fprintf(steer, "constraint 0.0\n");
        for (Int_t iPar = 1; iPar < fNGL + 1; iPar++)
            if (iPar % ((fAlignmentType == "xy") ? 2 : 3) == remain)
                fprintf(steer, "%d %G\n", iPar, 1.);
    }
    fclose(steer);
}

void BmnGemAlignment::DebugInfo(Int_t nGem, Int_t nMod, Int_t NLC, Int_t NGL, Double_t* DerLc, Double_t* DerGl, Double_t rMeasure, Double_t dMeasure)
{
    cout << "nGEM " << nGem << " nMod " << nMod << " nDerLc[ ] = ";
    for (Int_t icVar = 0; icVar < NLC; icVar++) cout << DerLc[icVar] << " ";
    cout << endl;
    cout << "-" << nGem << " nDerGl[ ] = ";
    for (Int_t icVar = 0; icVar < NGL; icVar++) cout << DerGl[icVar] << " ";
    cout << endl;
    cout << "-" << nGem << " rMeasure  = " << rMeasure << " dMeasure = " << dMeasure << endl;
}

ClassImp(BmnGemAlignment)
