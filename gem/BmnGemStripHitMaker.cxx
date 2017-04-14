#include <TChain.h>

#include "BmnGemStripHitMaker.h"

#include "BmnGemStripStationSet_RunSummer2016.h"
#include "BmnGemStripStationSet_RunWinter2016.h"
#include "BmnGemStripStationSet_RunSpring2017.h"
#include "BmnEventHeader.h"

static Float_t workTime = 0.0;

BmnGemStripHitMaker::BmnGemStripHitMaker()
: fHitMatching(kTRUE), fAlignCorrFileName("") {

    fInputPointsBranchName = "StsPoint";
    fInputDigitsBranchName = "BmnGemStripDigit";
    fInputDigitMatchesBranchName = "BmnGemStripDigitMatch";
    fBmnEventHeaderBranchName = "EventHeader";

    fOutputHitsBranchName = "BmnGemStripHit";
    fOutputHitMatchesBranchName = "BmnGemStripHitMatch";

    fVerbose = 1;

    fCurrentConfig = BmnGemStripConfiguration::None;
    StationSet = NULL;
}

BmnGemStripHitMaker::BmnGemStripHitMaker(Bool_t isExp)
: fHitMatching(kTRUE), fAlignCorrFileName("") {

    fInputPointsBranchName = "StsPoint";
    fInputDigitsBranchName = (!isExp) ? "BmnGemStripDigit" : "GEM";
    fIsExp = (!isExp) ? kFALSE : kTRUE;
    fBmnEventHeaderBranchName = "EventHeader";

    fInputDigitMatchesBranchName = "BmnGemStripDigitMatch";

    fOutputHitsBranchName = "BmnGemStripHit";
    fOutputHitMatchesBranchName = "BmnGemStripHitMatch";

    fVerbose = 1;

    fCurrentConfig = BmnGemStripConfiguration::None;
    StationSet = NULL;
}

BmnGemStripHitMaker::~BmnGemStripHitMaker() {

}

InitStatus BmnGemStripHitMaker::Init() {

    if (fVerbose) cout << "\nBmnGemStripHitMaker::Init()\n ";

    //if GEM configuration is not set -> return a fatal error
    if (!fCurrentConfig) Fatal("BmnGemStripHitMaker::Init()", " !!! Current GEM config is not set !!! ");

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnGemStripDigitsArray = (TClonesArray*) ioman->GetObject(fInputDigitsBranchName);
    fBmnEventHeader = (TClonesArray*) ioman->GetObject(fBmnEventHeaderBranchName);
    fBmnGemStripDigitMatchesArray = (TClonesArray*) ioman->GetObject(fInputDigitMatchesBranchName);

    if (fVerbose) {
        if (fBmnGemStripDigitMatchesArray) cout << "  Strip matching information exists!\n";
        else cout << "  Strip matching information doesn`t exist!\n";
    }

    fBmnGemStripHitsArray = new TClonesArray(fOutputHitsBranchName);
    ioman->Register(fOutputHitsBranchName, "GEM", fBmnGemStripHitsArray, kTRUE);

    if (fHitMatching && fBmnGemStripDigitMatchesArray) {
        fBmnGemStripHitMatchesArray = new TClonesArray("BmnMatch");
        ioman->Register(fOutputHitMatchesBranchName, "GEM", fBmnGemStripHitMatchesArray, kTRUE);
    } else {
        fBmnGemStripHitMatchesArray = 0;
    }

    //Create GEM detector ------------------------------------------------------
    switch (fCurrentConfig) {
        case BmnGemStripConfiguration::RunSummer2016:
            StationSet = new BmnGemStripStationSet_RunSummer2016(fCurrentConfig);
            if (fVerbose) cout << "   Current Configuration : RunSummer2016" << "\n";
            break;

        case BmnGemStripConfiguration::RunWinter2016:
            StationSet = new BmnGemStripStationSet_RunWinter2016(fCurrentConfig);
            if (fVerbose) cout << "   Current Configuration : RunWinter2016" << "\n";
            break;

        case BmnGemStripConfiguration::RunSpring2017:
            StationSet = new BmnGemStripStationSet_RunSpring2017(fCurrentConfig);
            if (fVerbose) cout << "   Current Configuration : RunSpring2017" << "\n";
            break;
        default:
            StationSet = NULL;
    }

    const Int_t nStat = StationSet->GetNStations();
    const Int_t nParams = 3;

    corr = new Double_t**[nStat];
    for (Int_t iStat = 0; iStat < nStat; iStat++) {
        Int_t nModul = StationSet->GetGemStation(iStat)->GetNModules();
        corr[iStat] = new Double_t*[nModul];
        for (Int_t iMod = 0; iMod < nModul; iMod++) {
            corr[iStat][iMod] = new Double_t[nParams];
            for (Int_t iPar = 0; iPar < nParams; iPar++) {
                corr[iStat][iMod][iPar] = 0.;
            }
        }
    }

    // NB! When fAlignCorrFileName == "", i.e. is empty string, no alinment
    // corrections are used at all, they remain to be simply zeros (just
    // immediate return from ReadAlignCorrFile); this is used when e.g. we want
    // to run alignment from scratch.
    // Anatoly.Solomin@jinr.ru 2017-02-21 15:12:07
    ReadAlignCorrFile(fAlignCorrFileName, corr);

    cout << "Obtained alignment GEM-corrections to be used: " << endl;
    for (Int_t iStat = 0; iStat < nStat; iStat++) {
        Int_t nModul = StationSet->GetGemStation(iStat)->GetNModules();
        for (Int_t iMod = 0; iMod < nModul; iMod++) {
            for (Int_t iPar = 0; iPar < nParams; iPar++)
                // print alignment corrections in similar format as Millepede does. Anatoly.Solomin@jinr.ru 2017-02-21 15:12:07 //
                cout << "Stat " << iStat << " Module " << iMod << " Param. " << iPar << " Value (in cm.) " << TString::Format("% 7.4f", corr[iStat][iMod][iPar]) << endl; //
        }
    }

    //--------------------------------------------------------------------------

    if (fVerbose) cout << "BmnGemStripHitMaker::Init() finished\n";

    return kSUCCESS;
}

void BmnGemStripHitMaker::Exec(Option_t* opt) {
    clock_t tStart = clock();

    if (fIsExp) {
        BmnEventHeader* evHeader = (BmnEventHeader*) fBmnEventHeader->At(0);
        if (evHeader && evHeader->GetTripWord())
            return;
    }

    fBmnGemStripHitsArray->Clear();

    if (fHitMatching && fBmnGemStripHitMatchesArray) {
        fBmnGemStripHitMatchesArray->Clear();
    }

    if (!fBmnGemStripDigitsArray) {
        Error("BmnGemStripHitMaker::Exec()", " !!! Unknown branch name !!! ");
        return;
    }

    if (fVerbose) cout << " BmnGemStripHitMaker::Exec(), Number of BmnGemStripDigits = " << fBmnGemStripDigitsArray->GetEntriesFast() << "\n";

    ProcessDigits();

    if (fVerbose) cout << " BmnGemStripHitMaker::Exec() finished\n";
    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}

void BmnGemStripHitMaker::ProcessDigits() {

    FairMCPoint* MCPoint;
    BmnGemStripDigit* digit;
    BmnMatch *strip_match;

    BmnGemStripStation* station;
    BmnGemStripModule* module;

    //Loading digits ---------------------------------------------------------------
    Int_t AddedDigits = 0;
    Int_t AddedStripDigitMatches = 0;

    for (UInt_t idigit = 0; idigit < fBmnGemStripDigitsArray->GetEntriesFast(); idigit++) {
        digit = (BmnGemStripDigit*) fBmnGemStripDigitsArray->At(idigit);
        station = StationSet->GetGemStation(digit->GetStation());
        module = station->GetModule(digit->GetModule());

        if (module->SetStripSignalInLayer(digit->GetStripLayer(), digit->GetStripNumber(), digit->GetStripSignal())) AddedDigits++;

        if (fBmnGemStripDigitMatchesArray) {
            strip_match = (BmnMatch*) fBmnGemStripDigitMatchesArray->At(idigit);
            if (module->SetStripMatchInLayer(digit->GetStripLayer(), digit->GetStripNumber(), *strip_match)) AddedStripDigitMatches++;
        }
    }

    if (fVerbose) cout << "   Processed strip digits  : " << AddedDigits << "\n";
    if (fVerbose && fBmnGemStripDigitMatchesArray) cout << "   Added strip digit matches  : " << AddedStripDigitMatches << "\n";
    //------------------------------------------------------------------------------

    //Processing digits
    StationSet->ProcessPointsInDetector();

    Int_t NCalculatedPoints = StationSet->CountNProcessedPointsInDetector();
    if (fVerbose) cout << "   Calculated points  : " << NCalculatedPoints << "\n";

    Int_t clear_matched_points_cnt = 0; // points with the only one match-index

    for (Int_t iStation = 0; iStation < StationSet->GetNStations(); ++iStation) {
        BmnGemStripStation *station = StationSet->GetGemStation(iStation);

        for (Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            BmnGemStripModule *module = station->GetModule(iModule);
            Double_t z = module->GetZPositionRegistered();
            z += corr[iStation][iModule][2]; //alignment implementation

            Int_t NIntersectionPointsInModule = module->GetNIntersectionPoints();

            for (Int_t iPoint = 0; iPoint < NIntersectionPointsInModule; ++iPoint) {
                Double_t x = module->GetIntersectionPointX(iPoint);
                Double_t y = module->GetIntersectionPointY(iPoint);

                Double_t x_err = module->GetIntersectionPointXError(iPoint);
                Double_t y_err = module->GetIntersectionPointYError(iPoint);
                Double_t z_err = 0.0;

                Int_t RefMCIndex = 0;

                //hit matching (define RefMCIndex)) ----------------------------
                BmnMatch match = module->GetIntersectionPointMatch(iPoint);

                Int_t most_probably_index = -1;
                Double_t max_weight = 0;

                Int_t n_links = match.GetNofLinks();
                if (n_links == 1) clear_matched_points_cnt++;
                for (Int_t ilink = 0; ilink < n_links; ilink++) {
                    Int_t index = match.GetLink(ilink).GetIndex();
                    Double_t weight = match.GetLink(ilink).GetWeight();
                    if (weight > max_weight) {
                        max_weight = weight;
                        most_probably_index = index;
                    }
                }

                RefMCIndex = most_probably_index;
                //--------------------------------------------------------------

                //Add hit ------------------------------------------------------
                x *= -1; // invert to global X

                x += corr[iStation][iModule][0];
                y += corr[iStation][iModule][1];

                new ((*fBmnGemStripHitsArray)[fBmnGemStripHitsArray->GetEntriesFast()])
                        BmnGemStripHit(0, TVector3(x, y, z), TVector3(x_err, y_err, z_err), RefMCIndex);

                BmnGemStripHit* hit = (BmnGemStripHit*) fBmnGemStripHitsArray->At(fBmnGemStripHitsArray->GetEntriesFast() - 1);
                hit->SetStation(iStation);
                hit->SetModule(iModule);
                hit->SetIndex(fBmnGemStripHitsArray->GetEntriesFast() - 1);
                hit->SetClusterSizeInLowerLayer(module->GetIntersectionPoint_LowerLayerClusterSize(iPoint)); //cluster size (lower layer |||)
                hit->SetClusterSizeInUpperLayer(module->GetIntersectionPoint_UpperLayerClusterSize(iPoint)); //cluster size (upper layer ///or\\\)
                hit->SetStripPositionInLowerLayer(module->GetIntersectionPoint_LowerLayerSripPosition(iPoint)); //strip position (lower layer |||)
                hit->SetStripPositionInUpperLayer(module->GetIntersectionPoint_UpperLayerSripPosition(iPoint)); //strip position (upper layer ///or\\\)
                //--------------------------------------------------------------

                //hit matching -------------------------------------------------
                if (fHitMatching && fBmnGemStripHitMatchesArray) {
                    new ((*fBmnGemStripHitMatchesArray)[fBmnGemStripHitMatchesArray->GetEntriesFast()])
                            BmnMatch(module->GetIntersectionPointMatch(iPoint));
                }
                //--------------------------------------------------------------
            }
        }
    }
    if (fVerbose) cout << "   N clear matches with MC-points = " << clear_matched_points_cnt << "\n";
    //------------------------------------------------------------------------------
    StationSet->Reset();
}

void BmnGemStripHitMaker::Finish() {
    if (StationSet) {
        for (Int_t iStat = 0; iStat < StationSet->GetNStations(); iStat++) {
            Int_t nModul = StationSet->GetGemStation(iStat)->GetNModules();
            for (Int_t iMod = 0; iMod < nModul; iMod++) {
                delete corr[iStat][iMod];
            }
            delete[] corr[iStat];
        }
        delete[] corr;

        delete StationSet;
        StationSet = NULL;
    }
    cout << "Work time of the GEM hit maker: " << workTime << endl;
}

void BmnGemStripHitMaker::ReadAlignCorrFile(TString fname, Double_t*** corr) {
    if (fname == "") // case when we do not want to use any alinment corrections
        return;

    Int_t coeff = 0; // -1 for RunWinter2016, +1 for RunSpring2017 and in the future
    TString branchName = "";
    switch (fCurrentConfig) {
        case BmnGemStripConfiguration::RunWinter2016:
            coeff = -1;
            branchName = "BmnGemAlignmentCorrections";
            break;

        case BmnGemStripConfiguration::RunSpring2017:
            coeff = 1;
            branchName = "BmnGemAlignCorrections";
            break;
        default:
            StationSet = NULL;
    }

    TFile* f = new TFile(fname.Data());
    TTree* t = (TTree*) f->Get("cbmsim");
    TClonesArray* corrs = NULL;
    t->SetBranchAddress(branchName.Data(), &corrs);

    for (Int_t iEntry = 0; iEntry < t->GetEntries(); iEntry++) {
        t->GetEntry(iEntry);
        for (Int_t iCorr = 0; iCorr < corrs->GetEntriesFast(); iCorr++) {
            if (coeff == -1) { // To be removed in future
                BmnGemAlignmentCorrections* align = (BmnGemAlignmentCorrections*) corrs->UncheckedAt(iCorr);
                Int_t iStat = align->GetStation();
                Int_t iMod = align->GetModule();
                corr[iStat][iMod][0] = coeff * align->GetCorrections().X();
                corr[iStat][iMod][1] = coeff * align->GetCorrections().Y();
                corr[iStat][iMod][2] = coeff * align->GetCorrections().Z();
            } else {
                BmnGemAlignCorrections* align = (BmnGemAlignCorrections*) corrs->UncheckedAt(iCorr);
                Int_t iStat = align->GetStation();
                Int_t iMod = align->GetModule();
                corr[iStat][iMod][0] = coeff * align->GetCorrections().X();
                corr[iStat][iMod][1] = coeff * align->GetCorrections().Y();
                corr[iStat][iMod][2] = coeff * align->GetCorrections().Z();
            }
        }
    }
    delete f;
}

ClassImp(BmnGemStripHitMaker)
