#include "BmnGemAlignment.h"

Int_t BmnGemAlignment::fCurrentEvent = 0;

BmnGemAlignment::BmnGemAlignment() :
fNstat(7), fStatUsed(0), fDebugInfo(kFALSE),
fSigma(1.), fPreSigma(1.), fName(""),
fAccuracy(1e-3), fNumOfIterations(50000), fNGL_PER_STAT(0),
fIterationsNum(1), nSelectedTracks(0) {

    // Declare branch names here 
    hitsBranch = "BmnGemStripHit";
    tracksBranch = "BmnGemTrack";
    tracksSelectedBranch = "BmnAlignmentContainer";
}

InitStatus BmnGemAlignment::Init() {

    cout << "\nBmnGemAlignment::Init()\n ";

    FairRootManager* ioman = FairRootManager::Instance();
    fGemHits = (TClonesArray*) ioman->GetObject("BmnGemStripHit");
    fGemTracks = (TClonesArray*) ioman->GetObject("BmnGemTrack");
    fContainer = (TClonesArray*) ioman->GetObject("BmnAlignmentContainer");

    if (fRunType == "")
        Fatal("BmnGemReco::Init()", "Alignment type has not been specified!!!");

    TChain* chain = ioman->GetInChain();
    fRecoFileName = ioman->GetInFile()->GetName();
    for (Int_t iEvent = 0; iEvent < chain->GetEntries(); iEvent++) {
        chain->GetEntry(iEvent);
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
}

void BmnGemAlignment::Finish() {
    fclose(fin_txt);

    BinFilePede(); // Prepare bin-file for PEDE  
    StartPede(); // Start PEDE 
}

void BmnGemAlignment::StartMille() {
    fCurrentEvent++;
    if (fCurrentEvent % 1000 == 0)
        cout << "Event# = " << fCurrentEvent << endl;

    for (Int_t iAlign = 0; iAlign < fContainer->GetEntriesFast(); iAlign++) {
        BmnAlignmentContainer* cont = (BmnAlignmentContainer*) fContainer->UncheckedAt(iAlign);
        BmnGemTrack* track = (BmnGemTrack*) fGemTracks->UncheckedAt(cont->GetTrackIndex());

        for (Int_t iStat = 0; iStat < fNstat; iStat++) {
            Int_t iHit = 0;
            for (iHit = 0; iHit < track->GetNHits(); iHit++) {
                BmnGemStripHit* hit = (BmnGemStripHit*) fGemHits->UncheckedAt(track->GetHitIndex(iHit));
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

                    Char_t* measX = Form("%f %f ", X, 1. * fSigma);
                    Char_t* measY = Form("%f %f ", Y, 1. * fSigma);

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
                        Fatal("BmnGemAlignment::StartMille()", "Undefined alignment type!!!");

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
                    Fatal("BmnGemAlignment::StartMille()", "Undefined alignment type!!!");
        }
    }
}

void BmnGemAlignment::BinFilePede() {
    ifstream fout_txt;
    fout_txt.open(TString(fName + ".txt").Data(), ios::in);

    Int_t NLC = 4;
    fNGL_PER_STAT = ((fAlignmentType == "xy") ? 2 : (fAlignmentType == "xyz") ? 3 : 0);
    fStatUsed = fNstat - fFixedStats.size();
    Int_t NGL = fNGL_PER_STAT * fNstat;

    const Int_t dim = NGL;
    Int_t* Labels = new Int_t[dim];
    for (Int_t iEle = 0; iEle < dim; iEle++)
        Labels[iEle] = 1 + iEle;

    Double_t rMeasure, dMeasure;
    Int_t nTracks;
    Int_t nGem;
    fout_txt >> nTracks;
    Double_t DerGl[NGL], DerLc[NLC];

    BmnMille* Mille = new BmnMille(TString(fName + ".bin").Data(), kTRUE, kFALSE);

    for (Int_t iTrack = 0; iTrack < nTracks; iTrack++) {
        for (Int_t iPlane = 0; iPlane < 2 * fNstat; iPlane++) {
            fout_txt >> nGem;

            for (Int_t iVar = 0; iVar < NLC; iVar++)
                fout_txt >> DerLc[iVar];

            for (Int_t iVar = 0; iVar < NGL; iVar++)
                fout_txt >> DerGl[iVar];

            fout_txt >> rMeasure >> dMeasure;
            Mille->mille(NLC, DerLc, NGL, DerGl, Labels, rMeasure, fSigma * dMeasure);

            if (fDebugInfo)
                DebugInfo(nGem, NLC, NGL, DerLc, DerGl, rMeasure, fSigma * dMeasure);
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
        } else if (size == 68) {
            ss >> buff1 >> buff2 >> buff3 >> buff4 >> buff5;
            fprintf(fin, "%d %f %f\n", buff1.Atoi(), buff2.Atof(), buff3.Atof());
        } else
            cout << "Unsupported format given!";
    }
    fclose(fin);
}

void BmnGemAlignment::MakeSteerFile(Int_t iter) {
    FILE* steer = fopen(Form("steer_%d.txt", iter), "w");
    TString alignType = "alignment_" + fAlignmentType + ".bin";
    fprintf(steer, "%s\n", alignType.Data());
    fprintf(steer, "method inversion %d %f\n", fNumOfIterations, fAccuracy);
    fprintf(steer, "Parameter\n");

    for (Int_t iPar = 0; iPar < fNGL_PER_STAT * fNstat; iPar++) {
        Int_t currStat = iPar / fNGL_PER_STAT;
        fprintf(steer, "%d %f ", iPar + 1, 0.);

        for (Int_t iSize = 0; iSize < fFixedStats.size(); iSize++) {
            if (find(fFixedStats.begin(), fFixedStats.end(), currStat) != fFixedStats.end())
                fprintf(steer, "%f\n", -1.);
            else
                fprintf(steer, "%f\n", fPreSigma);
            break;
        }
    }
    fclose(steer);
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

ClassImp(BmnGemAlignment)