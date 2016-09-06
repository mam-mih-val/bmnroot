#include "BmnGemAlignment.h"

BmnGemAlignment::BmnGemAlignment(Char_t* filename, Char_t* outname, Bool_t onlyMille, Bool_t preAlign) :
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
fChi2Max(LDBL_MAX),
fThreshold(0.0),
fMinHitsAccepted(1),
fMaxHitsAccepted(fNstat),
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
fNumLabels(0),
fOnlyMille(onlyMille),
fRunType(""),
fSigmaX(1.),
fSigmaY(1.),
fTrHits(NULL),
fPreAlignXY(preAlign) {
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

        fCorrX = new Double_t[fNstat];
        fCorrY = new Double_t[fNstat];

        if (preAlign == kFALSE) {
            for (Int_t iStat = 0; iStat < fNstat; iStat++) {
                fCorrX[iStat] = 0.;
                fCorrY[iStat] = 0.;
            }
        }
    }
}

void BmnGemAlignment::DoPreAlignmentXY() {
    ifstream file;
    TString dir = getenv("VMCWORKDIR");
    file.open(TString(dir + "/input/prealignmentXY.txt").Data(), ios::in);

    Int_t stat = 0;
    Double_t meanX(0.), meanY(0.), sigmaX(0.), sigmaY(0.);

    for (Int_t iStat = 0; iStat < fNstat; iStat++) {
        file >> stat >> meanX >> sigmaX >> meanY >> sigmaY;
        fCorrX[iStat] = meanX;
        fCorrY[iStat] = meanY;
    }
    file.close();
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

    if (fPreAlignXY)
        DoPreAlignmentXY();

    for (Int_t iEv = 0; iEv < fNumEvents; iEv++) {
        fChainIn->GetEntry(iEv);
        if (iEv % 1000 == 0)
            cout << "Event# = " << iEv << endl;

        fGemHits->Delete();
        fGemTracks->Delete();
        fContainer->Delete();

        BmnGemStripStationSet* fDetector = new BmnGemStripStationSet_RunSummer2016();

        for (Int_t iStation = 0; iStation < fDetector->GetNStations(); ++iStation) {
            BmnGemStripStation* station = fDetector->GetGemStation(iStation);
            for (Int_t iMod = 0; iMod < station->GetNModules(); iMod++) {
                BmnGemStripModule* module = station->GetModule(iMod);
                for(Int_t iLayer = 0; iLayer < module->GetNStripLayers(); ++iLayer) {
                    module->GetStripLayer(iLayer).SetClusterFindingThreshold(fThreshold);
                }
            }
        }

        // Loop over digits and put a signal to strips
        for (Int_t iDigit = 0; iDigit < fGemDigits->GetEntriesFast(); iDigit++) {
            BmnGemStripDigit* dig = (BmnGemStripDigit*) fGemDigits->UncheckedAt(iDigit);

            if (dig->GetStripSignal() / dig->GetStripSignalNoise() < fThresh[dig->GetStation()])
                continue;

            BmnGemStripStation* station = fDetector->GetGemStation(dig->GetStation());
            BmnGemStripModule* module = station->GetModule(dig->GetModule());

            Int_t layer = dig->GetStripLayer();

            module->SetStripSignalInLayer(layer, dig->GetStripNumber(), dig->GetStripSignal());
        }

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

                    Double_t xCorr = -x - fCorrX[iStation];
                    Double_t yCorr = y - fCorrY[iStation];

                    if (xCorr < fXMin || xCorr > fXMax || yCorr < fYMin || yCorr > fYMax)
                        continue;

                    // x --> -x in order to go to the BM@N reference frame
                    BmnGemStripHit* hit = new((*fGemHits)[fGemHits->GetEntriesFast()]) BmnGemStripHit(iStation, TVector3(xCorr, yCorr, z), TVector3(x_err, y_err, 0.), iPoint);
                    hit->SetDx(x_err);
                    hit->SetDy(y_err);
                    hit->SetDz(z_err);
                    hit->SetStation(iStation);
                    hit->SetModule(iMod);
                    hit->SetIndex(fGemHits->GetEntriesFast() - 1);
                }
            }
        }

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
        if (nonEmptyStatNumber.size() <= fMinHitsAccepted || nonEmptyStatNumber.size() >= fMaxHitsAccepted) {
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

            if (fBeamRun)
                if (tx < fTxMin || tx > fTxMax || ty < fTyMin || ty > fTyMax ||
//                        x0 < fXMin || x0 > fXMax || y0 < fYMin || y0 > fYMax ||
                        Abs(track->GetChi2() - Float_t(*it_min)) > FLT_EPSILON)
                    continue;

            // Checking residuals...
            Double_t xResMax = -1.;
            Double_t yResMax = -1.;

            for (Int_t iHit = 0; iHit < track->GetNHits(); iHit++) {
                BmnGemStripHit* hit = (BmnGemStripHit*) fGemHits->UncheckedAt(track->GetHitIndex(iHit));
                Double_t x = hit->GetX();
                Double_t y = hit->GetY();
                Double_t z = hit->GetZ();

                Double_t xRes = Abs(x - (tx * (z - z0) + x0));
                Double_t yRes = Abs(y - (ty * (z - z0) + y0));

                if (xRes > xResMax)
                    xResMax = xRes;

                if (yRes > yResMax)
                    yResMax = yRes;
            }

            Bool_t resFlag = (xResMax > fXresMax || yResMax > fYresMax) ? kTRUE : kFALSE;
            if (resFlag) {
                if (fBeamRun)
                    break;
                else
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
            cont->SetXresMax(xResMax);
            cont->SetYresMax(yResMax);
            cont->GetParamFirst()->SetTx(tx);
            cont->GetParamFirst()->SetTy(ty);
            cont->GetParamFirst()->SetX(x0);
            cont->GetParamFirst()->SetY(y0);
            cont->GetParamFirst()->SetZ(z0);
            cont->SetTrackIndex(iTrack);
            cont->SetNDF(track->GetNHits());
            cont->SetChi2(track->GetChi2());

            if (fDebugInfo) {
                cout << "Track Info: " << endl;
                cout << "Event# " << iEv << endl;
                cout << "Nhits = " << track->GetNHits() << endl;
                cout << "Chi2 = " << track->GetChi2() << endl;
                cout << "Tx = " << tx << " Ty = " << ty << endl;
                cout << "X0 = " << x0 << " Y0 = " << y0 << " Z0 = " << z0 << endl;
                cout << "xResMax = " << xResMax << " yResMax = " << yResMax << endl;
                cout << endl;
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

    TString name = "alignment";
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
                    Double_t Z = hit->GetZ();
                    Short_t stat = hit->GetStation();

                    if (stat == iStat) {
                        if (fAlignmentType == "xy") {
                            Char_t* locDerX = Form("%d 1. %f 0. 0. ", stat, Z);
                            Char_t* locDerY = Form("%d 0. 0. 1. %f ", stat, Z);

                            Char_t* globDerX = Form("1. 0. ");
                            Char_t* globDerY = Form("0. 1. ");

                            Char_t* measX = Form("%f %f ", hit->GetX(), 1. * fSigmaX);
                            Char_t* measY = Form("%f %f ", hit->GetY(), 1. * fSigmaY);

                            Int_t N_zeros_beg = stat;
                            Int_t N_zeros_end = (fNstat - 1) - N_zeros_beg;

                            TString zeroEnd = "";
                            TString zeroBeg = "";
                            for (Int_t i = 0; i < N_zeros_beg; i++)
                                zeroBeg += "0. 0. ";

                            for (Int_t i = 0; i < N_zeros_end; i++)
                                zeroEnd += "0. 0. ";

                            fprintf(fin_txt, "%s%s %s %s%s\n", locDerX, zeroBeg.Data(), globDerX, zeroEnd.Data(), measX);
                            fprintf(fin_txt, "%s%s %s %s%s\n", locDerY, zeroBeg.Data(), globDerY, zeroEnd.Data(), measY);
                            break;
                        }
                        if (fAlignmentType == "xyz") {



                        }
                    }
                }
                if (iHit == track->GetNHits())
                    for (Int_t iFill = 0; iFill < 2; iFill++)
                        fprintf(fin_txt, "%d 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0.\n", iStat);
            }
        }
    }
    delete out;
    fclose(fin_txt);

    ifstream fout_txt;
    fout_txt.open(TString(name + ".txt").Data(), ios::in);
    Int_t nTracks, nGem, NLC, NGL;

    if (fAlignmentType == "xy")
        AlignmentdXdY(fout_txt, nTracks, nGem, NLC, NGL, name);

    fout_txt.close();
}

void BmnGemAlignment::AlignmentdXdYdZ(ifstream& fout_txt, Int_t nTracks, Int_t nGem, Int_t NLC, Int_t NGL, TString name) {




}

void BmnGemAlignment::AlignmentdXdY(ifstream& fout_txt, Int_t nTracks, Int_t nGem, Int_t NLC, Int_t NGL, TString name) {
    NLC = 4;
    NGL = 2 * fStatUsed;

    fNumLabels = NGL;
    const Int_t dim = NGL;
    Int_t* Labels = new Int_t[dim];
    for (Int_t iEle = 0; iEle < dim; iEle++)
        Labels[iEle] = 1 + iEle;

    Double_t rMeasure, dMeasure;
    fout_txt >> nTracks;
    Double_t DerGl[NGL], DerLc[NLC];

    BmnMille* Mille = new BmnMille(TString(name + ".bin").Data(), kTRUE, kFALSE);

    for (Int_t iTrack = 0; iTrack < nTracks; iTrack++) {
        Int_t cntr = 0;
        for (Int_t iPlane = 0; iPlane < 2 * fNstat; iPlane++) {
            fout_txt >> nGem;

            if (find(fNumStatUsed.begin(), fNumStatUsed.end(), nGem) != fNumStatUsed.end()) {
                Double_t sum = 0.;

                for (Int_t iVar = 0; iVar < NLC; iVar++) {
                    fout_txt >> DerLc[iVar];
                    sum += DerLc[iVar];
                }

                Double_t tmp;
                // Go to end of the string
                for (Int_t iPos = 0; iPos < 2 * fNstat; iPos++)
                    fout_txt >> tmp;

                if (sum < 1e-1) {
                    for (Int_t iVar = 0; iVar < 2 * fStatUsed; iVar++)
                        DerGl[iVar] = 0.0;
                } else {
                    for (Int_t iVar = 0; iVar < 2 * fStatUsed; iVar++) {
                        if (iVar == cntr)
                            DerGl[iVar] = 1.0;
                        else
                            DerGl[iVar] = 0.;
                    }
                }
                fout_txt >> rMeasure >> dMeasure;
                Mille->mille(NLC, DerLc, NGL, DerGl, Labels, rMeasure, dMeasure);
                cntr++;

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
    TVector3 vertex, direction;
    Double_t chi2 = LineFit3D(hits, vertex, direction);
    if (chi2 > 0 && chi2 < fChi2Max) {
        BmnGemTrack* track = new BmnGemTrack();
        FairTrackParam* par = new FairTrackParam();
        CreateTrack(direction, vertex, *track, *par, chi2, hits.size());
        BmnGemTrack* newTrack = new ((*fGemTracks)[fGemTracks->GetEntriesFast()]) BmnGemTrack();
        for (Int_t iHit = 0; iHit < hits.size(); iHit++) {
            BmnGemStripHit* trHit = ((BmnGemStripHit*) hits.at(iHit));
            newTrack->AddHit(trHit->GetIndex(), trHit);
        }
        newTrack->SortHits();
        newTrack->SetParamFirst(*par);
        newTrack->SetChi2(chi2);
        newTrack->SetNDF(hits.size());
        delete par;
        delete track;
    }
}

BmnGemAlignment::~BmnGemAlignment() {
    delete fContainer;
    delete fTrHits;
    delete fChainIn;
    delete fGemDigits;
    delete fGemTracks;
    delete fGemHits;

    if (fOnlyMille == kFALSE) {
        delete fCorrX;
        delete fCorrY;
    }
}

Double_t BmnGemAlignment::LineFit3D(vector<BmnGemStripHit*> hits, TVector3& vertex, TVector3& direction) {
    const Int_t nHits = hits.size();

    Double_t Xi = 0.0, Yi = 0.0, Zi = 0.0; // coordinates of current track point
    Double_t Ci;
    Double_t SumXW = 0.0, SumYW = 0.0;
    Double_t SumXWC = 0.0, SumYWC = 0.0;
    Double_t SumW = 0.0;
    Double_t SumC = 0.0, SumC2 = 0.0;
    Double_t Wi = 1.0 / nHits; // weight
    Double_t ZV = ((BmnGemStripHit*) hits.at(0))->GetZ(); //Z-coordinate of vertex
    Double_t ZN = ((BmnGemStripHit*) hits.at(nHits - 1))->GetZ();
    Double_t Az = (ZN - ZV);
    if (Az == 0.0) {
        cout << "Az = 0.0" << endl;
        vertex = TVector3(0.0, 0.0, 0.0);
        direction = TVector3(0.0, 0.0, 0.0);
        return 1000.0;
    }
    for (Int_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = (BmnGemStripHit*) hits.at(i);
        if (hit == NULL) continue;
        Xi = hit->GetX();
        Yi = hit->GetY();
        Zi = hit->GetZ();
        Ci = (Zi - ZV) / Az;
        SumXW += (Xi * Wi);
        SumYW += (Yi * Wi);
        SumXWC += (Xi * Wi * Ci);
        SumYWC += (Yi * Wi * Ci);
        SumC += (Ci * Wi);
        SumC2 += (Ci * Ci * Wi);
        SumW += Wi;
    }
    Double_t koef = (SumC2 * SumW - SumC * SumC);
    if (koef == 0.0) {
        cout << "coef = 0.0" << endl;
        vertex = TVector3(0.0, 0.0, 0.0);
        direction = TVector3(0.0, 0.0, 0.0);
        return 1000.0;
    }
    Double_t XV = (SumXW * SumC2 - SumC * SumXWC) / koef;
    Double_t YV = (SumYW * SumC2 - SumC * SumYWC) / koef;
    Double_t Ax = (SumXWC * SumW - SumC * SumXW) / koef;
    Double_t Ay = (SumYWC * SumW - SumC * SumYW) / koef;

    vertex = TVector3(XV, YV, ZV);
    direction = TVector3(Ax, Ay, Az);

    //Chi2 calculation
    Double_t sumChi2xy = 0.0;
    Double_t sumChi2xz = 0.0;
    Double_t sumChi2yz = 0.0;
    for (Int_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = (BmnGemStripHit*) hits.at(i);
        if (hit == NULL) continue;
        Xi = hit->GetX();
        Yi = hit->GetY();
        Zi = hit->GetZ();

        Double_t Y_Xi = Ay / Ax * (Xi - XV) + YV;
        Double_t X_Zi = Ax / Az * (Zi - ZV) + XV;
        Double_t Y_Zi = Ay / Az * (Zi - ZV) + YV;
        sumChi2xy += ((Y_Xi - Yi) * (Y_Xi - Yi) / Yi / Yi);
        sumChi2xz += ((X_Zi - Xi) * (X_Zi - Xi) / Xi / Xi);
        sumChi2yz += ((Y_Zi - Yi) * (Y_Zi - Yi) / Yi / Yi);
    }
    sumChi2xy /= (nHits - 1);
    sumChi2xz /= (nHits - 1);
    sumChi2yz /= (nHits - 1);

    Double_t chi2 = Max(sumChi2xy, Max(sumChi2xz, sumChi2yz));
    return chi2;
}

void BmnGemAlignment::CreateTrack(TVector3 dir, TVector3 vert, BmnGemTrack& track, FairTrackParam& par, Double_t chi2, Int_t nHits) {
    par.SetPosition(vert);
    par.SetTx(dir.X() / dir.Z());
    par.SetTy(dir.Y() / dir.Z());
    par.SetQp(dir.Mag());
    track.SetParamFirst(par);
    track.SetChi2(chi2);
    track.SetNDF(nHits);
}

Bool_t BmnGemAlignment::isOneTrack(TClonesArray* hits) {
    Bool_t oneTrack = kTRUE;
    for (Int_t iHit = 1; iHit < hits->GetEntriesFast(); iHit++) {
        BmnGemStripHit* hit = (BmnGemStripHit*) hits->UncheckedAt(iHit);
        Short_t stat = hit->GetStation();
        if (iHit != stat) {
            oneTrack = kFALSE;
            break;
        }
    }
    return oneTrack;
}

void BmnGemAlignment::StartPede() {
    TCanvas* c = new TCanvas("alignParams", "alignParams", 1000, 1000);
    vector <TString> steerFileNames = GetSteerFileNames();
    const Int_t dim = steerFileNames.size();
    c->Divide(2, dim);
    TGraphErrors * outGraphX[dim];
    TGraphErrors * outGraphY[dim];
    TString tmp = fRecoFileName;

    for (Int_t iSize = 0; iSize < dim; iSize++) {
        outGraphX[iSize] = new TGraphErrors();
        outGraphY[iSize] = new TGraphErrors();
        TString commandToExec = "pede " + steerFileNames.at(iSize);
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

        StartMille();
        system(TString(fCommandToRunPede + " && rm " + random).Data());

        ifstream resFile;
        resFile.open("millepede.res", ios::in);
        if (!resFile)
            return;

        resFile.ignore(numeric_limits<streamsize>::max(), '\n');

        TString buff1 = "";
        TString buff2 = "";
        TString buff3 = "";
        TString buff4 = "";
        TString buff5 = "";

        string line;
        Int_t pointX = 0;
        Int_t pointY = 0;
        while (getline(resFile, line)) {
            stringstream ss(line);
            Int_t size = ss.str().length();
            // 40 and 68 symbols are fixed in the Pede-output
            if (size == 40) {
                ss >> buff1 >> buff2 >> buff3;
                if (buff1.Atoi() % 2 == 0) {
                    outGraphY[iSize]->SetPoint(pointY, buff1.Atoi(), buff2.Atof());
                    outGraphY[iSize]->SetPointError(pointY, 0., 0.);
                    pointY++;
                } else {
                    outGraphX[iSize]->SetPoint(pointX, buff1.Atoi(), buff2.Atof());
                    outGraphX[iSize]->SetPointError(pointX, 0., 0.);
                    pointX++;
                }
            } else if (size == 68) {
                ss >> buff1 >> buff2 >> buff3 >> buff4 >> buff5;
                if (buff1.Atoi() % 2 == 0) {
                    outGraphY[iSize]->SetPoint(pointY, buff1.Atoi(), buff2.Atof());
                    outGraphY[iSize]->SetPointError(pointY, 0., buff5.Atof());
                    pointY++;
                } else {
                    outGraphX[iSize]->SetPoint(pointX, buff1.Atoi(), buff2.Atof());
                    outGraphX[iSize]->SetPointError(pointX, 0., buff5.Atof());
                    pointX++;
                }
            } else
                cout << "Unsupported format observed!";
        }

        c->cd(2 * iSize + 1)->SetGrid();
        GraphDrawAttibuteSetter(outGraphX[iSize], steerFileNames[iSize]);

        c->cd(2 * iSize + 2)->SetGrid();
        GraphDrawAttibuteSetter(outGraphY[iSize], steerFileNames[iSize]);

        system(Form("cp millepede.res Millepede_%s_%s.res", tmp.Data(), TString(steerFileNames.at(iSize)).Data()));
        system("rm millepede.*");
        resFile.close();
    }
    c->SaveAs(Form("alignParams_%s.png", tmp.Data()));
    delete c;
}

void BmnGemAlignment::GraphDrawAttibuteSetter(TGraphErrors* gr, TString steerFileName) {
    gr->Draw("AP");
    gr->SetMarkerStyle(22);
    gr->SetMarkerSize(1.5);
    gr->GetXaxis()->SetTitle("Param. number");
    gr->GetXaxis()->SetTitleOffset(-0.35);
    gr->GetXaxis()->SetLabelSize(0.09);
    gr->GetXaxis()->SetTitleSize(0.09);
    gr->GetXaxis()->CenterTitle();
    gr->GetYaxis()->SetTitle("Param. value, cm");
    gr->GetYaxis()->SetTitleOffset(-0.3);
    gr->GetYaxis()->CenterTitle();
    gr->GetYaxis()->SetTitleSize(0.09);
    gr->GetYaxis()->SetLabelSize(0.09);
    gr->SetTitle(Form("%s-type of alignment (%s)", GetAlignmentDim().Data(), steerFileName.Data()));
}


ClassImp(BmnGemAlignment)