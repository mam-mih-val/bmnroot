#include "BmnGemAlignment.h"

Int_t BmnGemAlignment::fCurrentEvent = 0;

BmnGemAlignment::BmnGemAlignment() :
fDebugInfo(kFALSE),
fSigma(1.), fPreSigma(1.), fName(""),
fAccuracy(1e-3), fNumOfIterations(50000), fNGL(0),
fIterationsNum(1), nSelectedTracks(0) {

    // Declare branch names here
    hitsBranch = "BmnGemStripHit";
    tracksBranch = "BmnGemTrack";
    tracksSelectedBranch = "BmnAlignmentContainer";
    alignCorrBranch = "BmnGemAlignmentCorrections";

    fDetector = new BmnGemStripStationSet_RunSummer2016(BmnGemStripConfiguration::RunSummer2016);
}

InitStatus BmnGemAlignment::Init() {

    cout << "\nBmnGemAlignment::Init()\n ";

    FairRootManager* ioman = FairRootManager::Instance();
    fGemHits = (TClonesArray*) ioman->GetObject("BmnGemStripHit");
    fGemTracks = (TClonesArray*) ioman->GetObject("BmnGemTrack");
    fContainer = (TClonesArray*) ioman->GetObject("BmnAlignmentContainer");

    fAlignCorr = new TClonesArray(alignCorrBranch);
    ioman->Register(alignCorrBranch, "GEM", fAlignCorr, kTRUE);

    if (fRunType == "")
        Fatal("BmnGemReco::Init()", "Alignment type has not been specified!!!");

    fChain = ioman->GetInChain();
    fRecoFileName = ioman->GetInFile()->GetName();
    for (Int_t iEvent = 0; iEvent < fChain->GetEntries(); iEvent++) {
        fChain->GetEntry(iEvent);
        nSelectedTracks += fContainer->GetEntriesFast();
    }

    cout << "Number of selected tracks is: " << nSelectedTracks << endl;

    fName = "alignment_" + fAlignmentType;
    fin_txt = fopen(TString(fName + ".txt").Data(), "w");
    fprintf(fin_txt, "%d\n", nSelectedTracks);

    return kSUCCESS;
}

void BmnGemAlignment::Exec(Option_t* opt) {
    StartMille();
    if (fChain->GetEntries() == fCurrentEvent) {
        fclose(fin_txt);
        BinFilePede(); // Prepare bin-file for PEDE
        StartPede(); // Start PEDE
    }
}

void BmnGemAlignment::StartMille() {
    fCurrentEvent++;
    if (fCurrentEvent % 1000 == 0)
        cout << "Event# = " << fCurrentEvent << endl;

    Int_t modTotal = 0;
    for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++)
        modTotal += fDetector->GetGemStation(iStat)->GetNModules();

    for (Int_t iAlign = 0; iAlign < fContainer->GetEntriesFast(); iAlign++) {
        BmnAlignmentContainer* cont = (BmnAlignmentContainer*) fContainer->UncheckedAt(iAlign);
        BmnGemTrack* track = (BmnGemTrack*) fGemTracks->UncheckedAt(cont->GetTrackIndex());

        for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++) {
            Int_t nMod = fDetector->GetGemStation(iStat)->GetNModules();
            for (Int_t iMod = 0; iMod < nMod; iMod++) {
                Int_t iHit;
                for (iHit = 0; iHit < track->GetNHits(); iHit++) {
                    BmnGemStripHit* hit = (BmnGemStripHit*) fGemHits->UncheckedAt(track->GetHitIndex(iHit));
                    Double_t X = hit->GetX();
                    Double_t Y = hit->GetY();
                    Double_t Z = hit->GetZ();
                    Short_t stat = hit->GetStation();
                    Int_t mod = hit->GetModule();

                    TString globDerX = "", globDerY = "", zeroEnd = "", zeroBeg = "";

                    if (stat == iStat && mod == iMod) {
                        Char_t* locDerX = Form("%d %d 1. %f 0. 0. ", stat, mod, Z);
                        Char_t* locDerY = Form("%d %d 0. 0. 1. %f ", stat, mod, Z);

                        Char_t* measX = Form("%f %f ", X, 1. * fSigma);
                        Char_t* measY = Form("%f %f ", Y, 1. * fSigma);

                        Int_t N_zeros_beg = stat + mod;
                        Int_t N_zeros_end = fDetector->GetNStations() - N_zeros_beg;

                        if (fAlignmentType == "xy") {
                            globDerX = "1. 0.";
                            globDerY = "0. 1.";

                            for (Int_t i = 0; i < N_zeros_beg; i++)
                                zeroBeg += "0. 0. ";

                            for (Int_t i = 0; i < N_zeros_end; i++)
                                zeroEnd += "0. 0. ";
                        } else {
                            Double_t Tx = track->GetParamFirst()->GetTx();
                            Double_t Ty = track->GetParamFirst()->GetTy();

                            globDerX = TString(Form("1. 0. %f", Tx));
                            globDerY = TString(Form("0. 1. %f", Ty));

                            for (Int_t i = 0; i < N_zeros_beg; i++)
                                zeroBeg += "0. 0. 0. ";

                            for (Int_t i = 0; i < N_zeros_end; i++)
                                zeroEnd += "0. 0. 0. ";
                        }

                        fprintf(fin_txt, "%s%s %s %s%s\n", locDerX, zeroBeg.Data(), globDerX.Data(), zeroEnd.Data(), measX);
                        fprintf(fin_txt, "%s%s %s %s%s\n", locDerY, zeroBeg.Data(), globDerY.Data(), zeroEnd.Data(), measY);

                        break;
                    }
                }
                if (iHit == track->GetNHits()) {
                    TString zeroLine = "";
                    Int_t nPar = (fAlignmentType == "xy") ? 2 : 3;

                    for (Int_t iModTot = 0; iModTot < nPar * modTotal; iModTot++)
                        zeroLine += "0. ";

                    for (Int_t iRow = 0; iRow < 2; iRow++)
                        fprintf(fin_txt, "%d %d 0. 0. 0. 0. %s 0. 0.\n", iStat, iMod, zeroLine.Data()); // local = 4 -> zeroLine -> meas -> dmeas
                }
            }
        }
    }
}

void BmnGemAlignment::BinFilePede() {
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
    Int_t nTracks, nMod;
    Int_t nGem;
    fout_txt >> nTracks;
    Double_t DerGl[dim], DerLc[NLC];

    BmnMille* Mille = new BmnMille(TString(fName + ".bin").Data(), kTRUE, kFALSE);

    for (Int_t iTrack = 0; iTrack < nTracks; iTrack++) {
        for (Int_t iPlane = 0; iPlane < 2 * fDetector->GetNStations(); iPlane++) {
            fout_txt >> nGem >> nMod;

            for (Int_t iVar = 0; iVar < NLC; iVar++)
                fout_txt >> DerLc[iVar];

            for (Int_t iVar = 0; iVar < dim; iVar++)
                fout_txt >> DerGl[iVar];

            fout_txt >> rMeasure >> dMeasure;
            Mille->mille(NLC, DerLc, dim, DerGl, Labels, rMeasure, fSigma * dMeasure);

            if (fDebugInfo)
                DebugInfo(nGem, nMod, NLC, dim, DerLc, DerGl, rMeasure, fSigma * dMeasure);
        }
        Mille->end();
        if (fDebugInfo)
            cout << "========================> Another one RECORD = " << iTrack + 1 << " --> " << endl;
    }
    delete Mille;
    delete Labels;
    fout_txt.close();
}

void BmnGemAlignment::StartPede() {
    Int_t firstIter = 0;
    MakeSteerFile(firstIter); // Create initial steer file

    TString iterNum = "";
    iterNum += firstIter;

    TString commandToExec = "pede steer_" + iterNum + ".txt";
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
    fSigma = atof(buffer);
    pclose(file);
    BinFilePede(); //An action aimed at normalizing sigma to avoid chi2-warning

    system(TString(fCommandToRunPede + " && rm " + random).Data()); // first necessary Pede execution

    for (Int_t iIter = 1; iIter < fIterationsNum; iIter++) {
        ifstream resFile;
        resFile.open("millepede.res", ios::in);
        ReadPedeOutput(resFile, iIter);

        system(Form("pede steer_%d.txt && rm steer_%d.txt", iIter, iIter));
        resFile.close();
    }

    vector <Double_t> corr;

    // Put align. corrections to outFile
    ifstream resFile("millepede.res", ios::in);
    ReadPedeOutput(resFile, corr);
    resFile.close();

    const Int_t nStat = fDetector->GetNStations();

    Int_t nEntries = 0;
    for (Int_t iStat = 0; iStat < nStat; iStat++) {
        Int_t nModul = fDetector->GetGemStation(iStat)->GetNModules();
        for (Int_t iMod = 0; iMod < nModul; iMod++) {
            Int_t corrCounter = 0;
            Double_t buff[3] = {0., 0., 0.};
            for (Int_t iPar = nEntries; iPar < fNGL + 1; iPar++) {

                //                if (corrCounter == 0)
                //                    buff[0] = corr[iPar];
                //
                //                else if (corrCounter == 1)
                //                    buff[1] = corr[iPar];
                //
                //                else
                //                    buff[2] = corr[iPar];

                buff[corrCounter] = corr[iPar];

                corrCounter++;

                if (corrCounter == ((fAlignmentType == "xy") ? 2 : 3)) {
                    BmnGemAlignmentCorrections* tmp = new ((*fAlignCorr)[fAlignCorr->GetEntriesFast()]) BmnGemAlignmentCorrections();
                    tmp->SetStation(iStat);
                    tmp->SetModule(iMod);
                    tmp->SetX(buff[0]);
                    tmp->SetY(buff[1]);
                    tmp->SetZ(buff[2]);

                    // if (fDebugInfo)
                    cout << buff[0] << " " << buff[1] << " " << buff[2] << endl;
                }

                if ((iPar + 1) % ((fAlignmentType == "xy") ? 2 : 3) == 0) {
                    nEntries = iPar + 1;
                    break;
                }
            }
        }
    }

    system(Form("cp millepede.res Millepede_%s_%s.res", fRecoFileName.Data(), fAlignmentType.Data()));
    system("rm millepede.*");
}

void BmnGemAlignment::ReadPedeOutput(ifstream& resFile, Int_t iter) {
    if (!resFile)
        Fatal("BmnGemReco::ReadPedeOutput", "No input file found!!");

    // Open new steer file with obtained params.
    FILE* fin = fopen(Form("steer_%d.txt", iter), "w");
    TString alignType = "alignment_" + fAlignmentType + ".bin";
    fprintf(fin, "%s\n", alignType.Data());
    fprintf(fin, "method inversion %d %f\n", fNumOfIterations, fAccuracy);
    fprintf(fin, "regularization 1.0\n");
    fprintf(fin, "Parameter\n");

    TString buff1 = "", buff2 = "", buff3 = "", buff4 = "", buff5 = "";

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
        } else if (size == 68) {
            ss >> buff1 >> buff2 >> buff3 >> buff4 >> buff5;
            fprintf(fin, "%d %f %f\n", buff1.Atoi(), buff2.Atof(), buff3.Atof());
        } else
            cout << "Unsupported format given!";
    }
    fclose(fin);
}

void BmnGemAlignment::ReadPedeOutput(ifstream& resFile, vector <Double_t>& corr) {
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
            cout << "Unsupported format given!";

        corr.push_back(buff2.Atof());
    }
}

void BmnGemAlignment::MakeSteerFile(Int_t iter) {
    FILE* steer = fopen(Form("steer_%d.txt", iter), "w");
    TString alignType = "alignment_" + fAlignmentType + ".bin";
    fprintf(steer, "%s\n", alignType.Data());
    fprintf(steer, "method inversion %d %f\n", fNumOfIterations, fAccuracy);
    fprintf(steer, "regularization 1.0\n");
    fprintf(steer, "Parameter\n");

    Int_t nEntries = 0;
    for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++) {
        for (Int_t iMod = 0; iMod < fDetector->GetGemStation(iStat)->GetNModules(); iMod++) {
            for (Int_t iPar = nEntries; iPar < fNGL + 1; iPar++) {
                fprintf(steer, "%d %f ", iPar + 1, 0.);

                for (Int_t iSize = 0; iSize < fFixedStats.size(); iSize++) {
                    if (find(fFixedStats.begin(), fFixedStats.end(), iStat) != fFixedStats.end())
                        fprintf(steer, "%f\n", -1.);
                    else
                        fprintf(steer, "%f\n", fPreSigma);
                    break;
                }

                if ((iPar + 1) % ((fAlignmentType == "xy") ? 2 : 3) == 0) {
                    nEntries = iPar + 1;
                    break;
                }
            }
        }
    }

    fclose(steer);
}

void BmnGemAlignment::DebugInfo(Int_t nGem, Int_t nMod, Int_t NLC, Int_t NGL, Double_t* DerLc, Double_t* DerGl, Double_t rMeasure, Double_t dMeasure) {
    cout << "nGEM " << nGem << " nMod " << nMod << " nDerLc[ ] = ";
    for (Int_t icVar = 0; icVar < NLC; icVar++) cout << DerLc[icVar] << " ";
    cout << endl;
    cout << "-" << nGem << " nDerGl[ ] = ";
    for (Int_t icVar = 0; icVar < NGL; icVar++) cout << DerGl[icVar] << " ";
    cout << endl;
    cout << "-" << nGem << " rMeasure  = " << rMeasure << " dMeasure = " << dMeasure << endl;
}

ClassImp(BmnGemAlignment)
