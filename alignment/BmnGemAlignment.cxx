#include "BmnGemAlignment.h"

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
fChainIn(NULL),
fChainOut(NULL),
fTrHits(NULL),
fOnlyMille(onlyMille) {
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
   
    
    // Calculate total number of stations not to be used (they are marked by 1000 in the SetSignalToNoise() method)
    Int_t nStatNotUsed = 0;

    for (Int_t iStat = 0; iStat < fNstat; iStat++) {
        if (Abs((fThresh[iStat] - 1000.)) < 0.1)
            nStatNotUsed++;
        else
            fNumStatUsed.push_back(iStat);
    }

    fStatUsed = fNstat - nStatNotUsed;

    TCanvas* c = new TCanvas("superCave", "superCave", 1200, 800);
    c->Divide(1, 2);
    TH2F* h1 = new TH2F("_hYZ", "_hYZ", 150, 0., 150., 100, -50., 50.);
    TH2F* h2 = new TH2F("_hXZ", "_hXZ", 150, 0., 150., 100, -50., 50.);
    
    if (fOnlyMille == kTRUE)
        return;

    for (Int_t iEv = 0; iEv < fNumEvents; iEv++) {
        fChainIn->GetEntry(iEv);
        if (iEv % 1000 == 0)
            cout << "Event# = " << iEv << endl;
        //
        h1->Reset();
        h2->Reset();

        fGemHits->Delete();
        fGemTracks->Delete();
        fContainer->Delete();

        BmnGemStripStationSet* fDetector = new BmnGemStripStationSet_RunSummer2016();

        for (Int_t iStation = 0; iStation < fDetector->GetNStations(); ++iStation) {
            BmnGemStripStation* station = fDetector->GetGemStation(iStation);
            for (Int_t iMod = 0; iMod < station->GetNModules(); iMod++) {
                BmnGemStripReadoutModule* module = station->GetReadoutModule(iMod);
                module->SetStripSignalThreshold(fThreshold);
            }
        }

        // Loop over digits and put a signal to strips
        for (Int_t iDigit = 0; iDigit < fGemDigits->GetEntriesFast(); iDigit++) {
            BmnGemStripDigit* dig = (BmnGemStripDigit*) fGemDigits->UncheckedAt(iDigit);

            if (dig->GetStripSignal() / dig->GetStripSignalNoise() < fThresh[dig->GetStation()])
                continue;

            BmnGemStripStation* station = fDetector->GetGemStation(dig->GetStation());
            BmnGemStripReadoutModule* module = station->GetReadoutModule(dig->GetModule());

            Int_t layer = dig->GetStripLayer();

            if (layer == 0)
                module->SetValueOfLowerStrip(dig->GetStripNumber(), dig->GetStripSignal());

            else
                module->SetValueOfUpperStrip(dig->GetStripNumber(), dig->GetStripSignal());
        }

        for (Int_t iStation = 0; iStation < fDetector->GetNStations(); ++iStation) {
            BmnGemStripStation* station = fDetector->GetGemStation(iStation);
            station->ProcessPointsInStation();

            for (Int_t iMod = 0; iMod < station->GetNModules(); iMod++) {
                BmnGemStripReadoutModule* module = station->GetReadoutModule(iMod);

                Double_t z = module->GetZPositionRegistered();

                Int_t NIntersectionPointsInModule = module->GetNIntersectionPoints();

                for (Int_t iPoint = 0; iPoint < NIntersectionPointsInModule; ++iPoint) {
                    Double_t x = module->GetIntersectionPointX(iPoint);
                    Double_t y = module->GetIntersectionPointY(iPoint);

//                    if (x < fXhitMin || x > fXhitMax || y < fYhitMin || y > fYhitMax)
//                        continue;

                    Double_t x_err = module->GetIntersectionPointXError(iPoint);
                    Double_t y_err = module->GetIntersectionPointYError(iPoint);
                    Double_t z_err = 0.0;

                    h1->Fill(z, y);
                    h2->Fill(z, x);

                    BmnGemStripHit* hit = new((*fGemHits)[fGemHits->GetEntriesFast()]) BmnGemStripHit(iStation, TVector3(x, y, z), TVector3(x_err, y_err, 0.), iPoint);
                    hit->SetDx(x_err);
                    hit->SetDy(y_err);
                    hit->SetDz(z_err);
                    hit->SetStation(iStation);
                    hit->SetModule(iMod);
                }
            }
        }

        //                h1->SetTitle(Form("Ev# %d", iEv));
        //                h1->SetMarkerStyle(20);
        //                h2->SetTitle(Form("Ev# %d", iEv));
        //                h2->SetMarkerStyle(20);
        //
        //                if (h1->GetEntries() > 4 && h2->GetEntries() > 4) {
        //                    c->cd(1);
        //                    h1->Draw("P");
        //                    c->cd(2);
        //                    h2->Draw("P");
        //                    c->Update();
        //                     gSystem->Sleep(1000 * 3);
        //                    //gSystem->Sleep(1000 * 3600);
        //                    gSystem->ProcessEvents();
        //                }
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
        for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
            BmnGemTrack* track = (BmnGemTrack*) fGemTracks->UncheckedAt(iTrack);
            chi2.push_back(track->GetChi2());
        }
        vector <Double_t>::const_iterator it_min = min_element(chi2.begin(), chi2.end());
        for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
            BmnGemTrack* track = (BmnGemTrack*) fGemTracks->UncheckedAt(iTrack);
            if (Abs(track->GetChi2() - *it_min) < 0.1) {
                BmnAlignmentContainer* cont = new ((*fContainer)[fContainer->GetEntriesFast()]) BmnAlignmentContainer();
                cont->SetEventNum(iEv);
                cont->SetTx(track->GetParamFirst()->GetTx());
                cont->SetTy(track->GetParamFirst()->GetTy());
                cont->SetX0(track->GetParamFirst()->GetX());
                cont->SetY0(track->GetParamFirst()->GetY());
                cont->SetZ0(track->GetParamFirst()->GetZ());
                cont->SetTrackHits(track->GetHits());
                break;
            }
        }
        fRecoTree->Fill();
        delete fDetector;
    }
    fRecoTree->Write();
    fRecoFile->Close();
}

void BmnGemAlignment::StartMille() {
    fChainOut = new TChain("cbmsim");
    fChainOut->Add(fRecoFileName);
    cout << "#recorded entries = " << fChainOut->GetEntries() << endl;

    TClonesArray* hits = NULL;
    TClonesArray* tracks = NULL;
    TClonesArray* align = NULL;

    fChainOut->SetBranchAddress("BmnGemStripHit", &hits);
    fChainOut->SetBranchAddress("BmnGemTrack", &tracks);
    fChainOut->SetBranchAddress("BmnAlignmentContainer", &align);

    Int_t nSelectedTracks = 0;
    for (Int_t iEv = 0; iEv < fChainOut->GetEntries(); iEv++) {
        fChainOut->GetEntry(iEv);
        if (align->GetEntriesFast() > 0)
            nSelectedTracks++;
    }

    TString name = "alignment";
    FILE* fin_txt = fopen(TString(name + ".txt").Data(), "w");
    fprintf(fin_txt, "%d\n", nSelectedTracks);

    for (Int_t iEv = 0; iEv < fChainOut->GetEntries(); iEv++) {
        fChainOut->GetEntry(iEv);

        for (Int_t iAlign = 0; iAlign < align->GetEntriesFast(); iAlign++) {
            BmnAlignmentContainer* cont = (BmnAlignmentContainer*) align->UncheckedAt(iAlign);
            TClonesArray* trHits = cont->GetTrackHits();

            for (Int_t iStat = 0; iStat < fNstat; iStat++) {
                Int_t iHit = 0;
                for (iHit = 0; iHit < trHits->GetEntriesFast(); iHit++) {
                    BmnGemStripHit* hit = (BmnGemStripHit*) trHits->UncheckedAt(iHit);
                    Double_t Z = hit->GetZ();
                    Short_t stat = hit->GetStation();

                    if (stat == iStat) {
                        if (fAlignmentType == "xy") {
                            Char_t* locDerX = Form("%d 1. %f 0. 0. ", stat, Z);
                            Char_t* locDerY = Form("%d 0. 0. 1. %f ", stat, Z);

                            Char_t* globDerX = Form("1. 0. ");
                            Char_t* globDerY = Form("0. 1. ");

                            Char_t* measX = Form("%f %f ", hit->GetX(), hit->GetDx());
                            Char_t* measY = Form("%f %f ", hit->GetY(), hit->GetDy());

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
                    }
                }
                if (iHit == trHits->GetEntriesFast())
                    for (Int_t iFill = 0; iFill < 2; iFill++)
                        fprintf(fin_txt, "%d 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0.\n", iStat);
            }
        }
    }
    fclose(fin_txt);

    ifstream fout_txt;
    fout_txt.open(TString(name + ".txt").Data(), ios::in);
    Int_t nTracks, nGem, NLC, NGL;

    if (fAlignmentType == "xy")
        AlignmentdXdY(fout_txt, nTracks, nGem, NLC, NGL, name);

    fout_txt.close();

    TString commandToExec = "pede " + TString(GetSteerFileName());
    fCommandToRunPede = commandToExec;
}

void BmnGemAlignment::AlignmentdXdY(ifstream& fout_txt, Int_t nTracks, Int_t nGem, Int_t NLC, Int_t NGL, TString name) {
    NLC = 4;
    NGL = 2 * fStatUsed;

    const Int_t dim = NGL;
    Int_t* Labels = new Int_t[dim];
    for (Int_t iEle = 0; iEle < dim; iEle++)
        Labels[iEle] = 1 + iEle;

    for (Int_t iEle = 0; iEle < dim; iEle++)
        cout << Labels[iEle] << endl;

    Double_t rMeasure, dMeasure;
    fout_txt >> nTracks;
    Double_t DerGl[NGL], DerLc[NLC];

    //    BmnMille* Mille = new BmnMille(TString(name + ".bin").Data(), kFALSE, kTRUE);
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
            }
            else
                fout_txt.ignore(numeric_limits<streamsize>::max(), '\n');

            // Mille->mille(NLC, DerLc, NGL, DerGl, Labels, rMeasure, dMeasure);
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
        Double_t Tx = par->GetTx();
        Double_t Ty = par->GetTy();
        Double_t X0 = par->GetX();
        Double_t Y0 = par->GetY();
        
        if (Tx > fTxMin && Tx < fTxMax && Ty > fTyMin && Ty < fTyMax && X0 > fXMin && X0 < fXMax && Y0 > fYMin && Y0 < fYMax) {
            BmnGemTrack* newTrack = new ((*fGemTracks)[fGemTracks->GetEntriesFast()]) BmnGemTrack();

            for (Int_t iHit = 0; iHit < hits.size(); iHit++) {
                BmnGemStripHit* trHit = ((BmnGemStripHit*) hits.at(iHit));
                newTrack->AddHit(trHit);
            }

            newTrack->SetParamFirst(*par);
            newTrack->SetChi2(chi2);
            newTrack->SetNDF(hits.size());
        }
        delete par;
        delete track;
    }
}

BmnGemAlignment::~BmnGemAlignment() {
    delete fContainer;
    delete fTrHits;
    delete fChainIn;
    delete fChainOut;
    delete fGemDigits;
    delete fGemTracks;
    delete fGemHits;
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

ClassImp(BmnGemAlignment)