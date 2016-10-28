#include "BmnGemAlignment.h"
#include "../gem/BmnGemStripConfiguration.h"

BmnGemAlignment::BmnGemAlignment(Char_t* filename, Char_t* outname, Bool_t onlyMille) :
fGemDigits(NULL),
fRecoFileName(NULL),
fRecoTree(NULL),
fGemHits(NULL),
fDebugInfo(kFALSE),
fContainer(NULL),
fNstat(7),
fStatUsed(0),
fMaxNofHits(fNstat),
fGemTracks(NULL),
fSignalToNoise(1.),
fChi2MaxPerNDF(LDBL_MAX),
fThreshold(0.0),
fMinHitsAccepted(3),
fXMin(-LDBL_MAX),
fXMax(LDBL_MAX),
fYMin(-LDBL_MAX),
fYMax(LDBL_MAX),
fTxMin(-LDBL_MAX),
fTxMax(LDBL_MAX),
fTyMin(-LDBL_MAX),
fTyMax(LDBL_MAX),
fXresMax(LDBL_MAX),
fYresMax(LDBL_MAX),
fChainIn(NULL),
fOnlyMille(onlyMille),
fRunType(""),
fSigmaX(1.),
fSigmaY(1.),
fNGL_PER_STAT(0),
fIterationsNum(1),
fTrHits(NULL),
fWriteHitsOnly(kFALSE) {
    fRecoFileName = outname;
    if (fOnlyMille == kFALSE) {
        fDigiFilename = filename;
        fChainIn = new TChain("cbmsim");
        fChainIn->Add(fDigiFilename);

        cout << "#events: " << fChainIn->GetEntries() << endl;
        fNumEvents = fChainIn->GetEntries();
        fChainIn->SetBranchAddress("GEM", &fGemDigits);

        fRecoFile = new TFile(fRecoFileName, "recreate");
        fRecoTree = new TTree("cbmsim", "cbmsim");

        fGemHits = new TClonesArray("BmnGemStripHit");
        fGemTracks = new TClonesArray("BmnGemTrack");
        fContainer = new TClonesArray("BmnAlignmentContainer");

        fRecoTree->Branch("BmnGemStripHit", &fGemHits);
        fRecoTree->Branch("BmnGemTrack", &fGemTracks);
        fRecoTree->Branch("BmnAlignmentContainer", &fContainer);

        fTrHits = new TClonesArray("BmnGemStripHit");
    }
}

void BmnGemAlignment::PrepareData() {
    if (GetRunType() == "") {
        cout << "Specify a run type" << endl;
        return;
    }
    // Calculate total number of stations not to be used (they are marked by 1000 in the SetSignalToNoise() method)
    Int_t nStatNotUsed = 0;

    for (Int_t iStat = 0; iStat < fNstat; iStat++) {
        if (Abs((fThresh[iStat] - 1000.)) < 0.1)
            nStatNotUsed++;
        else
            fNumStatUsed.push_back(iStat);
    }

    fStatUsed = fNstat - nStatNotUsed;

    if (fOnlyMille == kTRUE)
        return;

    for (Int_t iEv = 0; iEv < fNumEvents; iEv++) {
        fChainIn->GetEntry(iEv);
        if (iEv % 1000 == 0)
            cout << "Event# = " << iEv << endl;

        fGemHits->Delete();
        fGemTracks->Delete();
        fContainer->Delete();

        BmnGemStripStationSet* fDetector = new BmnGemStripStationSet_RunSummer2016(BmnGemStripConfiguration::RunSummer2016_ALIGNMENT);

        for (Int_t iStation = 0; iStation < fDetector->GetNStations(); ++iStation) {
            BmnGemStripStation* station = fDetector->GetGemStation(iStation);
            for (Int_t iMod = 0; iMod < station->GetNModules(); iMod++) {
                BmnGemStripModule* module = station->GetModule(iMod);
                for (Int_t iLayer = 0; iLayer < module->GetNStripLayers(); ++iLayer) {
                    module->GetStripLayer(iLayer).SetClusterFindingThreshold(fThreshold);
                }
            }
        }

        //       Loop over digits and put a signal to strips
        for (Int_t iDigit = 0; iDigit < fGemDigits->GetEntriesFast(); iDigit++) {
            BmnGemStripDigit* dig = (BmnGemStripDigit*) fGemDigits->UncheckedAt(iDigit);

            if (dig->GetStripSignal() / dig->GetStripSignalNoise() < fThresh[dig->GetStation()])
                continue;

            BmnGemStripStation* station = fDetector->GetGemStation(dig->GetStation());
            BmnGemStripModule* module = station->GetModule(dig->GetModule());

            Int_t layer = dig->GetStripLayer();

            module->SetStripSignalInLayer(layer, dig->GetStripNumber(), dig->GetStripSignal());
        }

        // Create hits in modules
        for (Int_t iStation = 0; iStation < fDetector->GetNStations(); ++iStation) {
            BmnGemStripStation* station = fDetector->GetGemStation(iStation);
            station->ProcessPointsInStation();

            for (Int_t iMod = 0; iMod < station->GetNModules(); iMod++) {
                BmnGemStripModule* module = station->GetModule(iMod);

                Double_t z = module->GetZPositionRegistered();

                Int_t NIntersectionPointsInModule = module->GetNIntersectionPoints();

                for (Int_t iPoint = 0; iPoint < NIntersectionPointsInModule; ++iPoint) {
                    Double_t x = module->GetIntersectionPointX(iPoint);
                    Double_t y = module->GetIntersectionPointY(iPoint);

                    Double_t x_err = module->GetIntersectionPointXError(iPoint);
                    Double_t y_err = module->GetIntersectionPointYError(iPoint);
                    Double_t z_err = 0.0;

                    x *= -1.;

                    if (x < fXMin || x > fXMax || y < fYMin || y > fYMax)
                        continue;

                    // x --> -x in order to go to the BM@N reference frame
                    BmnGemStripHit* hit = new((*fGemHits)[fGemHits->GetEntriesFast()]) BmnGemStripHit(iStation, TVector3(x, y, z), TVector3(x_err, y_err, 0.), iPoint);
                    hit->SetDx(x_err);
                    hit->SetDy(y_err);
                    hit->SetDz(z_err);
                    hit->SetStation(iStation);
                    hit->SetModule(iMod);
                    hit->SetIndex(fGemHits->GetEntriesFast() - 1);
                }
            }
        }

        if (fWriteHitsOnly == kFALSE) {
            // Checking for maximal number of hits
            if (fGemHits->GetEntriesFast() == 0 || fGemHits->GetEntriesFast() > fMaxNofHits) {
                delete fDetector;
                continue;
            }

            // Fill hits over stations
            vector <BmnGemStripHit*> stat[fNstat];
            for (Int_t iHit = 0; iHit < fGemHits->GetEntriesFast(); iHit++) {
                BmnGemStripHit* hit = (BmnGemStripHit*) fGemHits->UncheckedAt(iHit);
                stat[hit->GetStation()].push_back(hit);
            }

            // Checking for empty stations
            Int_t emptyStat = 0;
            vector <Int_t> nonEmptyStatNumber;
            for (Int_t iStat = 0; iStat < fNstat; iStat++) {
                if (stat[iStat].size() < 1)
                    emptyStat++;
                else
                    nonEmptyStatNumber.push_back(iStat);
            }

            // Checking for (nStat - 1) empty stations
            if (emptyStat == fNstat - 1) {
                delete fDetector;
                continue;
            }

            // Checking for minimal number of hits per track
            if (nonEmptyStatNumber.size() == 2) {
                delete fDetector;
                continue;
            }

            vector <BmnGemStripHit*> hits;
            goToStations(hits, stat, 0);

            // Searching for one track with min. value of chi2 and putting its params. to align. container
            vector <Double_t> chi2;
            if (fBeamRun)
                for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
                    BmnGemTrack* track = (BmnGemTrack*) fGemTracks->UncheckedAt(iTrack);
                    chi2.push_back(track->GetChi2());
                }
            vector <Double_t>::const_iterator it_min = min_element(chi2.begin(), chi2.end());
            for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
                BmnGemTrack* track = (BmnGemTrack*) fGemTracks->UncheckedAt(iTrack);
                Double_t tx = track->GetParamFirst()->GetTx();
                Double_t ty = track->GetParamFirst()->GetTy();
                Double_t x0 = track->GetParamFirst()->GetX();
                Double_t y0 = track->GetParamFirst()->GetY();
                Double_t z0 = track->GetParamFirst()->GetZ();

                if (fBeamRun) {
                    if (tx < fTxMin || tx > fTxMax || ty < fTyMin || ty > fTyMax || Abs(track->GetChi2() - Float_t(*it_min)) > FLT_EPSILON)
                        continue;
                } else {
                    if (tx < fTxMin || tx > fTxMax || ty < fTyMin || ty > fTyMax)
                        continue;
                }

                // Use tracks without common hits
                Bool_t isUsed = kFALSE;
                for (Int_t iHit = 0; iHit < track->GetNHits(); iHit++) {
                    BmnGemStripHit* hit = (BmnGemStripHit*) fGemHits->UncheckedAt(track->GetHitIndex(iHit));
                    if (hit->IsUsed()) {
                        isUsed = kTRUE;
                        break;
                    }
                    hit->SetUsing(kTRUE);
                }

                if (isUsed)
                    continue;

                BmnAlignmentContainer* cont = new ((*fContainer)[fContainer->GetEntriesFast()]) BmnAlignmentContainer();
                cont->SetEventNum(iEv);
                cont->GetParamFirst()->SetTx(tx);
                cont->GetParamFirst()->SetTy(ty);
                cont->GetParamFirst()->SetX(x0);
                cont->GetParamFirst()->SetY(y0);
                cont->GetParamFirst()->SetZ(z0);
                cont->SetTrackIndex(iTrack);
                cont->SetNHits(track->GetNHits());
                cont->SetChi2(track->GetChi2());
                cont->SetNDF(track->GetNDF());

                if (fDebugInfo) {
                    cout << "Track Info: " << endl;
                    cout << "Event# " << iEv << endl;
                    cout << "Nhits = " << track->GetNHits() << endl;
                    cout << "Chi2 = " << track->GetChi2() << endl;
                    cout << "NDF = " << track->GetNDF() << endl;
                    cout << "Chi2 / NDF = " << track->GetChi2() / track->GetNDF() << endl;
                    cout << "Tx = " << tx << " Ty = " << ty << endl;
                    cout << "X0 = " << x0 << " Y0 = " << y0 << " Z0 = " << z0 << endl;
                    cout << endl;
                }
            }
        }
        fRecoTree->Fill();
        delete fDetector;
    }
    fRecoTree->Write();
    fRecoFile->Close();
}

void BmnGemAlignment::StartMille() {
    if (GetRunType() == "") {
        cout << "Specify a run type" << endl;
        return;
    }
    TChain* out = new TChain("cbmsim");
    out->Add(fRecoFileName);
    cout << "#recorded entries = " << out->GetEntries() << endl;

    TClonesArray* hits = NULL;
    TClonesArray* tracks = NULL;
    TClonesArray* align = NULL;

    out->SetBranchAddress("BmnGemStripHit", &hits);
    out->SetBranchAddress("BmnGemTrack", &tracks);
    out->SetBranchAddress("BmnAlignmentContainer", &align);

    Int_t nSelectedTracks = 0;
    for (Int_t iEv = 0; iEv < out->GetEntries(); iEv++) {
        out->GetEntry(iEv);
        if (align->GetEntriesFast() > 0)
            nSelectedTracks += align->GetEntriesFast();
    }

    TString name = "alignment_" + fAlignmentType;
    FILE* fin_txt = fopen(TString(name + ".txt").Data(), "w");
    fprintf(fin_txt, "%d\n", nSelectedTracks);

    for (Int_t iEv = 0; iEv < out->GetEntries(); iEv++) {
        out->GetEntry(iEv);

        for (Int_t iAlign = 0; iAlign < align->GetEntriesFast(); iAlign++) {
            BmnAlignmentContainer* cont = (BmnAlignmentContainer*) align->UncheckedAt(iAlign);
            BmnGemTrack* track = (BmnGemTrack*) tracks->UncheckedAt(cont->GetTrackIndex());

            for (Int_t iStat = 0; iStat < fNstat; iStat++) {
                Int_t iHit = 0;
                for (iHit = 0; iHit < track->GetNHits(); iHit++) {
                    BmnGemStripHit* hit = (BmnGemStripHit*) hits->UncheckedAt(track->GetHitIndex(iHit));
                    Double_t X = hit->GetX();
                    Double_t Y = hit->GetY();
                    Double_t Z = hit->GetZ();
                    Short_t stat = hit->GetStation();

                    if (stat == iStat) {
                        Char_t* locDerX = Form("%d 1. %f 0. 0. ", stat, Z);
                        Char_t* locDerY = Form("%d 0. 0. 1. %f ", stat, Z);
                        TString globDerX = "";
                        TString globDerY = "";
                        TString zeroEnd = "";
                        TString zeroBeg = "";

                        Char_t* measX = Form("%f %f ", X, 1. * fSigmaX);
                        Char_t* measY = Form("%f %f ", Y, 1. * fSigmaY);

                        Int_t N_zeros_beg = stat;
                        Int_t N_zeros_end = (fNstat - 1) - N_zeros_beg;

                        if (fAlignmentType == "xy") {
                            globDerX = "1. 0. ";
                            globDerY = "0. 1. ";

                            for (Int_t i = 0; i < N_zeros_beg; i++)
                                zeroBeg += "0. 0. ";

                            for (Int_t i = 0; i < N_zeros_end; i++)
                                zeroEnd += "0. 0. ";
                        } else if (fAlignmentType == "xyz") {
                            Double_t Tx = track->GetParamFirst()->GetTx();
                            Double_t Ty = track->GetParamFirst()->GetTy();

                            globDerX = TString(Form("1. 0. %f", Tx));
                            globDerY = TString(Form("0. 1. %f", Ty));

                            for (Int_t i = 0; i < N_zeros_beg; i++)
                                zeroBeg += "0. 0. 0. ";

                            for (Int_t i = 0; i < N_zeros_end; i++)
                                zeroEnd += "0. 0. 0. ";
                        } else
                            Fatal("", "");

                        fprintf(fin_txt, "%s%s %s %s%s\n", locDerX, zeroBeg.Data(), globDerX.Data(), zeroEnd.Data(), measX);
                        fprintf(fin_txt, "%s%s %s %s%s\n", locDerY, zeroBeg.Data(), globDerY.Data(), zeroEnd.Data(), measY);
                        break;
                    }
                }
                if (iHit == track->GetNHits())
                    if (fAlignmentType == "xy")
                        for (Int_t iFill = 0; iFill < 2; iFill++)
                            fprintf(fin_txt, "%d 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0.\n", iStat);
                    else if (fAlignmentType == "xyz")
                        for (Int_t iFill = 0; iFill < 2; iFill++)
                            fprintf(fin_txt, "%d 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0.\n", iStat);
                    else
                        Fatal("", "");
            }
        }
    }
    delete out;
    fclose(fin_txt);

    ifstream fout_txt;
    fout_txt.open(TString(name + ".txt").Data(), ios::in);

    BinFilePede(fout_txt, name);
    fout_txt.close();
}

void BmnGemAlignment::BinFilePede(ifstream& fout_txt, TString name) {
    Int_t NLC = 4;
    fNGL_PER_STAT = ((fAlignmentType == "xy") ? 2 : (fAlignmentType == "xyz") ? 3 : 0);
    Int_t NGL = fNGL_PER_STAT * fStatUsed;

    const Int_t dim = NGL;
    Int_t* Labels = new Int_t[dim];
    for (Int_t iEle = 0; iEle < dim; iEle++)
        Labels[iEle] = 1 + iEle;

    Double_t rMeasure, dMeasure;
    Int_t nTracks;
    Int_t nGem;
    fout_txt >> nTracks;
    Double_t DerGl[NGL], DerLc[NLC];

    BmnMille* Mille = new BmnMille(TString(name + ".bin").Data(), kTRUE, kFALSE);

    for (Int_t iTrack = 0; iTrack < nTracks; iTrack++) {
        for (Int_t iPlane = 0; iPlane < 2 * fNstat; iPlane++) {
            fout_txt >> nGem;

            if (find(fNumStatUsed.begin(), fNumStatUsed.end(), nGem) != fNumStatUsed.end()) {

                for (Int_t iVar = 0; iVar < NLC; iVar++)
                    fout_txt >> DerLc[iVar];

                for (Int_t iVar = 0; iVar < NGL; iVar++)
                    fout_txt >> DerGl[iVar];

                fout_txt >> rMeasure >> dMeasure;
                Mille->mille(NLC, DerLc, NGL, DerGl, Labels, rMeasure, dMeasure);

                if (fDebugInfo)
                    DebugInfo(nGem, NLC, NGL, DerLc, DerGl, rMeasure, dMeasure);
            } else
                fout_txt.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        Mille->end();
        if (fDebugInfo)
            cout << "========================> Another one RECORD = " << iTrack + 1 << " --> " << endl;
    }
    delete Mille;
    delete Labels;
}

void BmnGemAlignment::DebugInfo(Int_t nGem, Int_t NLC, Int_t NGL, Double_t* DerLc, Double_t* DerGl, Double_t rMeasure, Double_t dMeasure) {
    cout << "-" << nGem << " n DerLc[ ] = ";
    for (Int_t icVar = 0; icVar < NLC; icVar++) cout << DerLc[icVar] << " ";
    cout << endl;
    cout << "-" << nGem << " n DerGl[ ] = ";
    for (Int_t icVar = 0; icVar < NGL; icVar++) cout << DerGl[icVar] << " ";
    cout << endl;
    cout << "-" << nGem << " rMeasure  = " << rMeasure << " dMeasure = " << dMeasure << endl;
}

void BmnGemAlignment::goToStations(vector<BmnGemStripHit*>& hits, vector<BmnGemStripHit*> *hitsOnStation, Int_t stat) {
    Int_t nextStat = stat + 1;

    if (hitsOnStation[stat].size() < 1) {
        if (stat == fNstat - 1) {
            DeriveFoundTrackParams(hits);
            return;
        }
        goToStations(hits, hitsOnStation, nextStat);
    } else {
        for (Int_t iSize = 0; iSize < hitsOnStation[stat].size(); iSize++) {
            BmnGemStripHit* hit = (BmnGemStripHit*) hitsOnStation[stat].at(iSize);
            hits.push_back(hit);

            if (nextStat == fNstat) {
                DeriveFoundTrackParams(hits);
                hits.pop_back();
                continue;
            }
            goToStations(hits, hitsOnStation, nextStat);
            hits.pop_back();
        }
    }
    nextStat--;
}

void BmnGemAlignment::DeriveFoundTrackParams(vector<BmnGemStripHit*> hits) {
    BmnGemTrack* track = new BmnGemTrack();
    FairTrackParam* par = new FairTrackParam();

    for (Int_t iHit = 0; iHit < hits.size(); iHit++) {
        BmnGemStripHit* trHit = ((BmnGemStripHit*) hits.at(iHit));
        track->AddHit(trHit->GetIndex(), trHit);
    }
    track->SortHits();
    TVector3 zxParams = LineFit(track, fGemHits, "ZX"); // Tx, X0
    TVector3 zyParams = LineFit(track, fGemHits, "ZY"); // Ty, Y0

    if (track->GetNHits() >= fMinHitsAccepted && zxParams.Z() / (track->GetNHits() - 2) < fChi2MaxPerNDF && zyParams.Z() / (track->GetNHits() - 2) < fChi2MaxPerNDF) {
        BmnGemTrack* newTrack = new ((*fGemTracks)[fGemTracks->GetEntriesFast()]) BmnGemTrack(*track);
        par->SetX(zxParams.Y());
        par->SetY(zyParams.Y());
        par->SetZ(0.0);
        par->SetTx(zxParams.X());
        par->SetTy(zyParams.X());
        newTrack->SetNDF(newTrack->GetNHits());
        newTrack->SetChi2(Max(zyParams.Z(), zxParams.Z()));
        newTrack->SetNDF(newTrack->GetNHits() - 2); // Since we estimate two params. from linear fit
        newTrack->SetParamFirst(*par);
    }
    delete par;
    delete track;
}

BmnGemAlignment::~BmnGemAlignment() {
    delete fContainer;
    delete fTrHits;
    delete fChainIn;
    delete fGemDigits;
    delete fGemTracks;
    delete fGemHits;
}

void BmnGemAlignment::StartPede() {
    TCanvas* c = new TCanvas("alignParams", "alignParams", 1500, 800);
    TString steerFileName = GetSteerFileNames();

    c->Divide(fNGL_PER_STAT, 1);
    TGraphErrors* outGraphX = new TGraphErrors();
    TGraphErrors* outGraphY = new TGraphErrors();
    TGraphErrors* outGraphZ = new TGraphErrors();

    TString tmp = fRecoFileName;

    TString commandToExec = "pede " + steerFileName;
    fCommandToRunPede = commandToExec;

    TString random = "";
    gRandom->SetSeed(0);
    random += (Int_t) (gRandom->Rndm(0) * 1000);
    system(TString(fCommandToRunPede + " >> " + random).Data());

    FILE* file = popen(TString("cat " + random + " | grep -e 'by factor' | awk '{print $9}'").Data(), "r");
    if (!file)
        return;

    Char_t buffer[100];
    fgets(buffer, sizeof (buffer), file);
    fSigmaX = atof(buffer);
    fSigmaY = fSigmaX;
    pclose(file);

    StartMille(); // start Mille with normalized sigma to avoid chi2 warning
    system(TString(fCommandToRunPede + " && rm " + random).Data()); // first necessary Pede execution

    for (Int_t iIter = 1; iIter < fIterationsNum; iIter++) {
        ifstream resFile;
        resFile.open("millepede.res", ios::in);
        ReadPedeOutput(resFile, iIter);

        // StartMille();
        system(Form("pede steer_%d.txt && rm steer_%d.txt", iIter, iIter));
        resFile.close();
    }

    ifstream resFile;
    resFile.open("millepede.res", ios::in);
    if (!resFile)
        return;

    // Go to the second string
    resFile.ignore(numeric_limits<streamsize>::max(), '\n');

    TString buff1 = "";
    TString buff2 = "";
    TString buff3 = "";
    TString buff4 = "";
    TString buff5 = "";

    string line;
    Int_t pointX = 0, pointY = 0, pointZ = 0;

    while (getline(resFile, line)) {
        stringstream ss(line);
        Int_t size = ss.str().length();
        // 40 and 68 symbols are fixed in the Pede-output by a given format 
        if (size == 40) {
            ss >> buff1 >> buff2 >> buff3;
            if (buff1.Atoi() % fNGL_PER_STAT == 0) {
                if (fAlignmentType == "xy") {
                    outGraphY->SetPoint(pointY, buff1.Atoi(), 10. * buff2.Atof());
                    outGraphY->SetPointError(pointY++, 0., 0.);
                } else if (fAlignmentType == "xyz") {
                    outGraphZ->SetPoint(pointZ, buff1.Atoi(), 10. * buff2.Atof());
                    outGraphZ->SetPointError(pointZ++, 0., 0.);
                }
            } else if (buff1.Atoi() % fNGL_PER_STAT == 1) {
                outGraphX->SetPoint(pointX, buff1.Atoi(), 10. * buff2.Atof());
                outGraphX->SetPointError(pointX++, 0., 0.);
            } else if (buff1.Atoi() % fNGL_PER_STAT == 2) {
                outGraphY->SetPoint(pointY, buff1.Atoi(), 10. * buff2.Atof());
                outGraphY->SetPointError(pointY++, 0., 0.);
            } else
                Fatal("BmnGemAlignment::StartPede()", "BmnGemAlignment::StartPede()");
        } else if (size == 68) {
            ss >> buff1 >> buff2 >> buff3 >> buff4 >> buff5;
            if (buff1.Atoi() % fNGL_PER_STAT == 0) {
                if (fAlignmentType == "xy") {
                    outGraphY->SetPoint(pointY, buff1.Atoi(), 10. * buff2.Atof());
                    outGraphY->SetPointError(pointY++, 0., 10. * buff5.Atof());
                } else if (fAlignmentType == "xyz") {
                    outGraphZ->SetPoint(pointZ, buff1.Atoi(), 10. * buff2.Atof());
                    outGraphZ->SetPointError(pointZ++, 0., 10. * buff5.Atof());
                }
            } else if (buff1.Atoi() % fNGL_PER_STAT == 1) {
                outGraphX->SetPoint(pointX, buff1.Atoi(), 10. * buff2.Atof());
                outGraphX->SetPointError(pointX++, 0., 10. * buff5.Atof());
            } else if (buff1.Atoi() % fNGL_PER_STAT == 2) {
                outGraphY->SetPoint(pointY, buff1.Atoi(), 10. * buff2.Atof());
                outGraphY->SetPointError(pointY++, 0., 10. * buff5.Atof());
            }
        } else
            cout << "Unsupported format given!";
    }

    c->cd(1)->SetGrid();
    GraphDrawAttibuteSetter(outGraphX, steerFileName);

    c->cd(2)->SetGrid();
    GraphDrawAttibuteSetter(outGraphY, steerFileName);

    if (fAlignmentType == "xyz") {
        c->cd(3)->SetGrid();
        GraphDrawAttibuteSetter(outGraphZ, steerFileName);
    }

    system(Form("cp millepede.res Millepede_%s_%s.res", tmp.Data(), steerFileName.Data()));
    system("rm millepede.*");
    resFile.close();

    c->SaveAs(Form("alignParams_%s_%s.png", tmp.Data(), steerFileName.Data()));
    delete c;
    delete outGraphX;
    delete outGraphY;
    delete outGraphZ;
}

void BmnGemAlignment::ReadPedeOutput(ifstream& resFile, Int_t iter) {
    if (!resFile)
        Fatal("BmnGemAlignment::ReadPedeOutput", "No input file found!!");

    // Open new steer file with obtained params.
    FILE* fin = fopen(Form("steer_%d.txt", iter), "w");
    TString alignType = ((fAlignmentType == "xy") ? "alignment_xy.bin" : (fAlignmentType == "xyz") ? "alignment_xyz.bin" : "");
    TString methodIter = ((fAlignmentType == "xy") ? "method inversion 500 1E-4" : (fAlignmentType == "xyz") ? "method inversion 50000 1E-4" : "");
    fprintf(fin, "%s\n", alignType.Data());
    fprintf(fin, "%s\n", methodIter.Data());
    fprintf(fin, "Parameter\n");

    TString buff1 = "";
    TString buff2 = "";
    TString buff3 = "";
    TString buff4 = "";
    TString buff5 = "";

    string line;
    
    // Go to the second string of existing millepede.res 
    resFile.ignore(numeric_limits<streamsize>::max(), '\n');

    while (getline(resFile, line)) {
        stringstream ss(line);
        Int_t size = ss.str().length();
        // 40 and 68 symbols are fixed in the Pede-output by a given format 
        if (size == 40) {
            ss >> buff1 >> buff2 >> buff3;
            fprintf(fin, "%d %f %f\n", buff1.Atoi(), buff2.Atof(), buff3.Atof());
        }

        else if (size == 68) {
            ss >> buff1 >> buff2 >> buff3 >> buff4 >> buff5;
            fprintf(fin, "%d %f %f\n", buff1.Atoi(), buff2.Atof(), buff3.Atof());
        }

        else
            cout << "Unsupported format given!";
    }
    fclose(fin);
}

void BmnGemAlignment::GraphDrawAttibuteSetter(TGraphErrors* gr, TString steerFileName) {
    gr->Draw("AP");
    gr->SetMarkerStyle(22);
    gr->SetMarkerSize(1.5);
    gr->GetXaxis()->SetTitle("Param. number");
    gr->GetXaxis()->SetTitleOffset(-0.35);
    gr->GetXaxis()->SetLabelSize(0.06);
    gr->GetXaxis()->SetTitleSize(0.06);
    gr->GetXaxis()->CenterTitle();
    gr->GetYaxis()->SetTitle("Param. value, mm");
    gr->GetYaxis()->SetTitleOffset(-0.3);
    gr->GetYaxis()->CenterTitle();
    gr->GetYaxis()->SetTitleSize(0.06);
    gr->GetYaxis()->SetLabelSize(0.06);
    gr->SetTitle(Form("%s-type of alignment (%s)", GetAlignmentDim().Data(), steerFileName.Data()));
}

ClassImp(BmnGemAlignment)