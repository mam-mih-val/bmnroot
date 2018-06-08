#include "BmnSiliconHitMaker.h"

#include "BmnSiliconModule.h"

#include "TClonesArray.h"
#include "TSystem.h"

static Float_t workTime = 0.0;

BmnSiliconHitMaker::BmnSiliconHitMaker()
: fHitMatching(kTRUE) {

    fInputPointsBranchName = "SiliconPoint";
    fInputDigitsBranchName = "BmnSiliconDigit";
    fInputDigitMatchesBranchName = "BmnSiliconDigitMatch";

    fOutputHitsBranchName = "BmnSiliconHit";
    fOutputHitMatchesBranchName = "BmnSiliconHitMatch";

    fVerbose = 1;

    fCurrentConfig = BmnSiliconConfiguration::None;
    StationSet = NULL;
}

BmnSiliconHitMaker::BmnSiliconHitMaker(Bool_t isExp)
: fHitMatching(kTRUE) {

    fIsExp = isExp;
    fInputPointsBranchName = "SiliconPoint";
    fInputDigitsBranchName = (!isExp) ? "BmnSiliconDigit" : "SILICON";
    fInputDigitMatchesBranchName = "BmnSiliconDigitMatch";

    fOutputHitsBranchName = "BmnSiliconHit";
    fOutputHitMatchesBranchName = "BmnSiliconHitMatch";

    fBmnEvQualityBranchName = "BmnEventQuality";

    fVerbose = 1;

    fCurrentConfig = BmnSiliconConfiguration::None;
    StationSet = NULL;
}

BmnSiliconHitMaker::~BmnSiliconHitMaker() {

}

InitStatus BmnSiliconHitMaker::Init() {

    if (fVerbose) cout << "\nBmnSiliconHitMaker::Init()\n ";

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnSiliconDigitsArray = (TClonesArray*) ioman->GetObject(fInputDigitsBranchName);
    if (!fBmnSiliconDigitsArray) {
        cout << "BmnSiliconHitMaker::Init(): branch " << fInputDigitsBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }
    fBmnSiliconDigitMatchesArray = (TClonesArray*) ioman->GetObject(fInputDigitMatchesBranchName);

    if (fVerbose) {
        if (fBmnSiliconDigitMatchesArray) cout << "  Strip matching information exists!\n";
        else cout << "  Strip matching information doesn`t exist!\n";
    }

    fBmnSiliconHitsArray = new TClonesArray(fOutputHitsBranchName);
    ioman->Register(fOutputHitsBranchName, "SILICON", fBmnSiliconHitsArray, kTRUE);

    if (fHitMatching && fBmnSiliconDigitMatchesArray) {
        fBmnSiliconHitMatchesArray = new TClonesArray("BmnMatch");
        ioman->Register(fOutputHitMatchesBranchName, "SILICON", fBmnSiliconHitMatchesArray, kTRUE);
    } else {
        fBmnSiliconHitMatchesArray = 0;
    }

    TString gPathSiliconConfig = gSystem->Getenv("VMCWORKDIR");
        gPathSiliconConfig += "/silicon/XMLConfigs/";

    //Create SILICON detector ------------------------------------------------------
    switch (fCurrentConfig) {

        case BmnSiliconConfiguration::RunSpring2017 :
            StationSet = new BmnSiliconStationSet(gPathSiliconConfig + "SiliconRunSpring2017.xml");
            if (fVerbose) cout << "   Current SILICON Configuration : RunSpring2017" << "\n";
            break;

        case BmnSiliconConfiguration::RunSpring2018 :
            StationSet = new BmnSiliconStationSet(gPathSiliconConfig + "SiliconRunSpring2018.xml");
            if (fVerbose) cout << "   Current SILICON Configuration : RunSpring2018" << "\n";
            break;

        default:
            StationSet = NULL;
    }

    const Int_t nStat = StationSet->GetNStations();
    const Int_t nParams = 3;

    corr = new Double_t**[nStat];
    for (Int_t iStat = 0; iStat < nStat; iStat++) {
        Int_t nModul = StationSet->GetSiliconStation(iStat)->GetNModules();
        corr[iStat] = new Double_t*[nModul];
        for (Int_t iMod = 0; iMod < nModul; iMod++) {
            corr[iStat][iMod] = new Double_t[nParams];
            for (Int_t iPar = 0; iPar < nParams; iPar++) {
                corr[iStat][iMod][iPar] = 0.;
            }
        }
    }

    if (fAlignCorrFileName != "") {
        ReadAlignCorrFile(fAlignCorrFileName, corr);

    }
    cout << "SI-alignment corrections in use:" << endl;
    for (Int_t iStat = 0; iStat != nStat; iStat++) {
        Int_t nModul = StationSet->GetSiliconStation(iStat)->GetNModules();
        for (Int_t iMod = 0; iMod != nModul; iMod++) {
            for (Int_t iPar = 0; iPar < nParams; iPar++)
                cout << "Stat " << iStat << " Module " << iMod << " Param. " << iPar << " Value (in cm.) " << TString::Format("% 7.4f", corr[iStat][iMod][iPar]) << endl;
        }
    }

    //--------------------------------------------------------------------------

    fBmnEvQuality = (TClonesArray*) ioman->GetObject(fBmnEvQualityBranchName);

    if (fVerbose) cout << "BmnGemStripHitMaker::Init() finished\n";

    return kSUCCESS;
}

void BmnSiliconHitMaker::Exec(Option_t* opt) {
    // Event separation by triggers ...
    if (fIsExp && fBmnEvQuality) {
        BmnEventQuality* evQual = (BmnEventQuality*) fBmnEvQuality->UncheckedAt(0);
        if (!evQual->GetIsGoodEvent())
            return;
    }
    fBmnSiliconHitsArray->Delete();

    if (fHitMatching && fBmnSiliconHitMatchesArray) {
        fBmnSiliconHitMatchesArray->Delete();
    }

    if (!IsActive())
        return;
    clock_t tStart = clock();

    if (fVerbose) cout << " BmnSiliconHitMaker::Exec(), Number of BmnSiliconDigits = " << fBmnSiliconDigitsArray->GetEntriesFast() << "\n";

    ProcessDigits();

    if (fVerbose) cout << " BmnSiliconHitMaker::Exec() finished\n";
    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}

void BmnSiliconHitMaker::ProcessDigits() {

    FairMCPoint* MCPoint;
    BmnSiliconDigit* digit;
    BmnMatch *strip_match;

    BmnSiliconStation* station;
    BmnSiliconModule* module;

    //Loading digits ---------------------------------------------------------------
    Int_t AddedDigits = 0;
    Int_t AddedStripDigitMatches = 0;

    for (UInt_t idigit = 0; idigit < fBmnSiliconDigitsArray->GetEntriesFast(); idigit++) {
        digit = (BmnSiliconDigit*) fBmnSiliconDigitsArray->At(idigit);
        station = StationSet->GetSiliconStation(digit->GetStation());
        module = station->GetModule(digit->GetModule());
        if (!module)
            return;

        if (module->SetStripSignalInLayerByZoneId(digit->GetStripLayer(), digit->GetStripNumber(), digit->GetStripSignal())) AddedDigits++;

        if (fBmnSiliconDigitMatchesArray) {
            strip_match = (BmnMatch*) fBmnSiliconDigitMatchesArray->At(idigit);
            if (module->SetStripMatchInLayerByZoneId(digit->GetStripLayer(), digit->GetStripNumber(), *strip_match)) AddedStripDigitMatches++;
        }
    }

    if (fVerbose) cout << "   Processed strip digits  : " << AddedDigits << "\n";
    if (fVerbose && fBmnSiliconDigitMatchesArray) cout << "   Added strip digit matches  : " << AddedStripDigitMatches << "\n";
    //------------------------------------------------------------------------------

    //Processing digits
    StationSet->ProcessPointsInDetector();

    Int_t NCalculatedPoints = StationSet->CountNProcessedPointsInDetector();
    if (fVerbose) cout << "   Calculated points  : " << NCalculatedPoints << "\n";

    Int_t clear_matched_points_cnt = 0; // points with the only one match-indexes

    for (Int_t iStation = 0; iStation < StationSet->GetNStations(); ++iStation) {
        station = StationSet->GetSiliconStation(iStation);

        for (Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            module = station->GetModule(iModule);
            Double_t z = module->GetZPositionRegistered();
            z += corr[iStation][iModule][2]; //alignment shift

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

                new ((*fBmnSiliconHitsArray)[fBmnSiliconHitsArray->GetEntriesFast()])
                        BmnSiliconHit(0, TVector3(x, y, z), TVector3(x_err, y_err, z_err), RefMCIndex);

                BmnSiliconHit* hit = (BmnSiliconHit*) fBmnSiliconHitsArray->At(fBmnSiliconHitsArray->GetEntriesFast() - 1);
                hit->SetStation(iStation);
                hit->SetModule(iModule);
                hit->SetIndex(fBmnSiliconHitsArray->GetEntriesFast() - 1);
                hit->SetClusterSizeInLowerLayer(module->GetIntersectionPoint_LowerLayerClusterSize(iPoint)); //cluster size (lower layer |||)
                hit->SetClusterSizeInUpperLayer(module->GetIntersectionPoint_UpperLayerClusterSize(iPoint)); //cluster size (upper layer ///or\\\)
                hit->SetStripPositionInLowerLayer(module->GetIntersectionPoint_LowerLayerSripPosition(iPoint)); //strip position (lower layer |||)
                hit->SetStripPositionInUpperLayer(module->GetIntersectionPoint_UpperLayerSripPosition(iPoint)); //strip position (upper layer ///or\\\)
                //--------------------------------------------------------------

                //hit matching -------------------------------------------------
                if (fHitMatching && fBmnSiliconHitMatchesArray) {
                    new ((*fBmnSiliconHitMatchesArray)[fBmnSiliconHitMatchesArray->GetEntriesFast()])
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

void BmnSiliconHitMaker::Finish() {
    if (StationSet) {
        for (Int_t iStat = 0; iStat < StationSet->GetNStations(); iStat++) {
            Int_t nModul = StationSet->GetSiliconStation(iStat)->GetNModules();
            for (Int_t iMod = 0; iMod < nModul; iMod++)
                delete [] corr[iStat][iMod];
            delete [] corr[iStat];
        }
        delete [] corr;
        delete StationSet;
        StationSet = NULL;
    }
    cout << "Work time of the Silicon hit maker: " << workTime << endl;
}

void BmnSiliconHitMaker::ReadAlignCorrFile(TString fname, Double_t*** Corr) {
    TString branchName = "BmnSiliconAlignCorrections";

    TFile* f = new TFile(fname.Data());
    TTree* t = (TTree*) f->Get("cbmsim");
    TClonesArray* corrs = NULL;
    t->SetBranchAddress(branchName.Data(), &corrs);

    for (Int_t iEntry = 0; iEntry < t->GetEntries(); iEntry++) {
        t->GetEntry(iEntry);
        for (Int_t iCorr = 0; iCorr < corrs->GetEntriesFast(); iCorr++) {
            BmnSiliconAlignCorrections* align = (BmnSiliconAlignCorrections*) corrs->UncheckedAt(iCorr);
            Int_t iStat = align->GetStation();
            Int_t iMod = align->GetModule();
            Corr[iStat][iMod][0] = align->GetCorrections().X();
            Corr[iStat][iMod][1] = align->GetCorrections().Y();
            Corr[iStat][iMod][2] = align->GetCorrections().Z();
        }
    }
    delete f;
}

ClassImp(BmnSiliconHitMaker)

