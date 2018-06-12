#include <TChain.h>
#include <zlib.h>
#include <TRandom.h>
#include "TSystem.h"

#include "BmnGemStripHitMaker.h"

#include "BmnGemStripStationSet_RunSummer2016.h"
#include "BmnGemStripStationSet_RunWinter2016.h"
#include "BmnGemStripStationSet_RunSpring2017.h"
#include "BmnEventHeader.h"
#include "FairRunAna.h"

static Float_t workTime = 0.0;

BmnGemStripHitMaker::BmnGemStripHitMaker()
: fHitMatching(kTRUE), fAlignCorrFileName(""), fRunId(-1) {

    fInputPointsBranchName = "StsPoint";
    fInputDigitsBranchName = "BmnGemStripDigit";
    fInputDigitMatchesBranchName = "BmnGemStripDigitMatch";

    fOutputHitsBranchName = "BmnGemStripHit";
    fOutputHitMatchesBranchName = "BmnGemStripHitMatch";

    fVerbose = 1;
    fField = NULL;

    fCurrentConfig = BmnGemStripConfiguration::None;
    StationSet = NULL;
}

BmnGemStripHitMaker::BmnGemStripHitMaker(Int_t run_period, Bool_t isExp)
: fHitMatching(kTRUE), fAlignCorrFileName(""), fRunId(-1), fPeriodId(run_period) {

    fInputPointsBranchName = "StsPoint";
    fInputDigitsBranchName = (!isExp) ? "BmnGemStripDigit" : "GEM";
    fIsExp = isExp;

    fInputDigitMatchesBranchName = "BmnGemStripDigitMatch";

    fOutputHitsBranchName = "BmnGemStripHit";
    fOutputHitMatchesBranchName = "BmnGemStripHitMatch";

    fBmnEvQualityBranchName = "BmnEventQuality";

    fVerbose = 1;
    fField = NULL;

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
    if (!fBmnGemStripDigitsArray) {
        cout << "BmnGemStripHitMaker::Init(): branch " << fInputDigitsBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

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

    TString gPathGemConfig = gSystem->Getenv("VMCWORKDIR");
    gPathGemConfig += "/gem/XMLConfigs/";

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
            //StationSet = new BmnGemStripStationSet(gPathGemConfig + "GemRunSpring2017.xml");
            if (fVerbose) cout << "   Current Configuration : RunSpring2017" << "\n";
            break;

        case BmnGemStripConfiguration::RunSpring2018:
            StationSet = new BmnGemStripStationSet(gPathGemConfig + "GemRunSpring2018.xml");
            cout << "   Current Configuration : RunSpring2018" << "\n";
            break;

        default:
            StationSet = NULL;
    }

    const Int_t nStat = StationSet->GetNStations();
    const Int_t nParams = 3;

    TRandom* rand = new TRandom();
    rand->SetSeed(2);

    corr = new Double_t**[nStat];
    misAlign = new Double_t**[nStat];
    for (Int_t iStat = 0; iStat < nStat; iStat++) {
        Int_t nModul = StationSet->GetGemStation(iStat)->GetNModules();
        corr[iStat] = new Double_t*[nModul];
        misAlign[iStat] = new Double_t*[nModul];
        for (Int_t iMod = 0; iMod < nModul; iMod++) {
            corr[iStat][iMod] = new Double_t[nParams];
            misAlign[iStat][iMod] = new Double_t[nParams];
            for (Int_t iPar = 0; iPar < nParams; iPar++) {
                corr[iStat][iMod][iPar] = 0.;
                if (!fIsExp)
                    misAlign[iStat][iMod][iPar] = rand->Gaus(0., (iPar == 0) ? 0.03 : (iPar == 1) ? 0.05 : 0.);
            }
        }
    }

    delete rand;

    if (fAlignCorrFileName != "")
        ReadAlignCorrFile(fAlignCorrFileName, corr);

    if (fIsExp)
        cout << "GEM-alignment corrections in use: " << endl;
    else
        cout << "Remain GEM-misalignment in use: " << endl;
    for (Int_t iStat = 0; iStat != nStat; iStat++) {
        Int_t nModul = StationSet->GetGemStation(iStat)->GetNModules();
        for (Int_t iMod = 0; iMod != nModul; iMod++) {
            for (Int_t iPar = 0; iPar != nParams; iPar++)
                cout << "Stat " << iStat << " Module " << iMod << " Param. " << iPar << " Value (in cm.) " <<
                    TString::Format("% 7.4f", (fIsExp) ? corr[iStat][iMod][iPar] : misAlign[iStat][iMod][iPar]) << endl; //
        }
    }
    fField = FairRunAna::Instance()->GetField();
    if (!fField) Fatal("Init", "No Magnetic Field found");

    // Initialize coefficients to be used when the Lorentz corrections calculating ...
    if (Abs(fField->GetBy(0., 0., 0.)) > FLT_EPSILON) {
        lorCorrsCoeff = new Double_t*[nStat];
        UniDbDetectorParameter* coeffLorCorrs = UniDbDetectorParameter::GetDetectorParameter("GEM", "lorentz_shift", fPeriodId, fRunId);
        LorentzShiftStructure* shifts;
        Int_t element_count = 0;
        if (coeffLorCorrs)
            coeffLorCorrs->GetLorentzShiftArray(shifts, element_count);
        for (Int_t iEle = 0; iEle < nStat; iEle++) {
            const Int_t nParams = 3; // Parabolic approximation is used
            lorCorrsCoeff[iEle] = new Double_t[nParams];
            for (Int_t iParam = 0; iParam < nParams; iParam++)
                lorCorrsCoeff[iEle][iParam] = (coeffLorCorrs) ? shifts[iEle].ls[iParam] : 0.;
        }
    }

    //--------------------------------------------------------------------------

    fBmnEvQuality = (TClonesArray*) ioman->GetObject(fBmnEvQualityBranchName);

    if (fVerbose) cout << "BmnGemStripHitMaker::Init() finished\n";

    return kSUCCESS;
}

void BmnGemStripHitMaker::Exec(Option_t* opt) {
    // Event separation by triggers ...
    if (fIsExp && fBmnEvQuality) {
        BmnEventQuality* evQual = (BmnEventQuality*) fBmnEvQuality->UncheckedAt(0);
        if (!evQual->GetIsGoodEvent())
            return;
    }
    fBmnGemStripHitsArray->Delete();

    if (fHitMatching && fBmnGemStripHitMatchesArray) {
        fBmnGemStripHitMatchesArray->Delete();
    }

    Int_t kDigitCut = (fPeriodId == 7) ? 1000 : 400;
    if (!IsActive() || fBmnGemStripDigitsArray->GetEntriesFast() > kDigitCut)
        return;

    if (fVerbose) cout << "\nBmnGemStripHitMaker::Exec()\n ";
    clock_t tStart = clock();

    fField = FairRunAna::Instance()->GetField();

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
        if (!digit->IsGoodDigit())
            continue;
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
                Double_t deltaX = fIsExp ? corr[iStation][iModule][0] : misAlign[iStation][iModule][0];
                Double_t deltaY = fIsExp ? corr[iStation][iModule][1] : misAlign[iStation][iModule][1];

                x += deltaX;
                y += deltaY;

                if (Abs(fField->GetBy(0., 0., 0.)) > FLT_EPSILON) {
                    Int_t sign = (module->GetElectronDriftDirection() == ForwardZAxisEDrift) ? +1 : -1;
                    Double_t ls = GetLorentzByField(Abs(fField->GetBy(x, y, z)), iStation) * sign;
                    x += ls;
                }

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
    // NB! normally, the following should never be used, except, maybe some tests:
    //if (fAlignCorrFileName != "")
    //    system(TString("rm "+fAlignCorrFileName).Data());
    if (StationSet) {
        for (Int_t iStat = 0; iStat < StationSet->GetNStations(); iStat++) {
            Int_t nModul = StationSet->GetGemStation(iStat)->GetNModules();
            for (Int_t iMod = 0; iMod < nModul; iMod++) {
                delete [] corr[iStat][iMod];
            }
            delete [] corr[iStat];
        }
        delete [] corr;

        delete StationSet;
        StationSet = NULL;
    }

    if (fIsExp && Abs(fField->GetBy(0., 0., 0.)) > FLT_EPSILON)
        delete [] lorCorrsCoeff;

    cout << "Work time of the GEM hit maker: " << workTime << endl;
}

void BmnGemStripHitMaker::ReadAlignCorrFile(TString fname, Double_t*** corr) {
    Int_t coeff = 0; // -1 for RunWinter2016, +1 for RunSpring2017 and in the future
    TString branchName = "";
    switch (fCurrentConfig) {
        case BmnGemStripConfiguration::RunWinter2016:
            coeff = -1;
            branchName = "BmnGemAlignmentCorrections";
            break;

        default:
            coeff = 1;
            branchName = "BmnGemAlignCorrections";
            break;
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
