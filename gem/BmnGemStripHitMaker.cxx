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
#include "UniDbDetectorParameter.h"

static Float_t workTime = 0.0;

BmnGemStripHitMaker::BmnGemStripHitMaker()
: fHitMatching(kTRUE) {

    fInputPointsBranchName = "StsPoint";
    fInputDigitsBranchName = "BmnGemStripDigit";
    fInputDigitMatchesBranchName = "BmnGemStripDigitMatch";

    fOutputHitsBranchName = "BmnGemStripHit";
    fOutputHitMatchesBranchName = "BmnGemStripHitMatch";

    fField = NULL;
    fFieldScale = 0.0;

    fCurrentConfig = BmnGemStripConfiguration::None;
    StationSet = nullptr;
    TransfSet = nullptr;
}

BmnGemStripHitMaker::BmnGemStripHitMaker(Int_t run_period, Int_t run_number, Bool_t isExp, Bool_t isSrc)
: fHitMatching(kTRUE) {

    fInputPointsBranchName = "StsPoint";
    fInputDigitsBranchName = (!isExp) ? "BmnGemStripDigit" : "GEM";
    fIsExp = isExp;
    fIsSrc = isSrc;

    fSignalLow = 0.;
    fSignalUp = DBL_MAX;

    fInputDigitMatchesBranchName = "BmnGemStripDigitMatch";

    fOutputHitsBranchName = "BmnGemStripHit";
    fOutputHitMatchesBranchName = "BmnGemStripHitMatch";

    fBmnEvQualityBranchName = "BmnEventQuality";

    fField = NULL;
    fFieldScale = 0.0;

    fCurrentConfig = BmnGemStripConfiguration::None;
    StationSet = nullptr;
    TransfSet = nullptr;

    switch(run_period) {
        case 5: //BM@N RUN-5
            fCurrentConfig = BmnGemStripConfiguration::RunWinter2016;
            break;
        case 6: //BM@N RUN-6
            fCurrentConfig = BmnGemStripConfiguration::RunSpring2017;
            break;
        case 7: //BM@N RUN-7 (and SRC)
            if(fIsSrc) {
                fCurrentConfig = BmnGemStripConfiguration::RunSRCSpring2018;
            } else {
                fCurrentConfig = BmnGemStripConfiguration::RunSpring2018;
            }
            break;
        case 8: //BM@N RUN-8
            fCurrentConfig = BmnGemStripConfiguration::FutureConfig2020;
            break;
    }
    
    TString gPathGemConfig = gSystem->Getenv("VMCWORKDIR");
    gPathGemConfig += "/parameters/gem/XMLConfigs/";
    
    //Create GEM detector ------------------------------------------------------
    switch (fCurrentConfig) {
        case BmnGemStripConfiguration::RunSummer2016:
            StationSet = new BmnGemStripStationSet_RunSummer2016(fCurrentConfig);
            if (fVerbose > 1) cout << "   Current GEM Configuration : RunSummer2016" << "\n";
            break;

        case BmnGemStripConfiguration::RunWinter2016:
            StationSet = new BmnGemStripStationSet_RunWinter2016(fCurrentConfig);
            if (fVerbose > 1) cout << "   Current GEM Configuration : RunWinter2016" << "\n";
            break;

        case BmnGemStripConfiguration::RunSpring2017:
            StationSet = new BmnGemStripStationSet_RunSpring2017(fCurrentConfig);
            //StationSet = new BmnGemStripStationSet(gPathGemConfig + "GemRunSpring2017.xml");
            if (fVerbose > 1) cout << "   Current GEM Configuration : RunSpring2017" << "\n";
            break;

        case BmnGemStripConfiguration::RunSpring2018:
            StationSet = new BmnGemStripStationSet(gPathGemConfig + "GemRunSpring2018.xml");
            if (fVerbose > 1) cout << "   Current GEM Configuration : RunSpring2018" << "\n";
            break;

        case BmnGemStripConfiguration::RunSRCSpring2018 :
            StationSet = new BmnGemStripStationSet(gPathGemConfig + "GemRunSRCSpring2018.xml");
            TransfSet = new BmnGemStripTransform();
            TransfSet->LoadFromXMLFile(gPathGemConfig + "GemRunSRCSpring2018.xml");
            if (fVerbose > 1) cout << "   Current GEM Configuration : GemRunSRCSpring2018" << "\n";
            break;

        case BmnGemStripConfiguration::FutureConfig2020:
            StationSet = new BmnGemStripStationSet(gPathGemConfig + "GemFutureConfig2020.xml");
            TransfSet = new BmnGemStripTransform();
            TransfSet->LoadFromXMLFile(gPathGemConfig + "GemFutureConfig2020.xml");
            if (fVerbose) cout << "   Current GEM Configuration : GemFutureConfig2020" << "\n";
            break;

        default:
            StationSet = NULL;
    }
    
    if (fIsExp) {
        const Int_t nStat = StationSet->GetNStations();
        UniDbDetectorParameter* coeffLorCorrs = UniDbDetectorParameter::GetDetectorParameter("GEM", "lorentz_shift", run_period, run_number);
        vector<UniValue*> shifts;
        if (coeffLorCorrs)
            coeffLorCorrs->GetValue(shifts);

        fLorCor = new Double_t * [nStat];
        for (Int_t iStat = 0; iStat < nStat; iStat++) {
            const Int_t nParams = 3;
            fLorCor[iStat] = new Double_t[nParams];
            for (Int_t iPar = 0; iPar < nParams; iPar++) {
                fLorCor[iStat][iPar] = (coeffLorCorrs) ? ((LorentzShiftValue*)shifts[iStat])->ls[iPar] : 0.0;
            }
        }


        UniDbDetectorParameter* coeffAlignCorrs = UniDbDetectorParameter::GetDetectorParameter("GEM", "alignment_shift", run_period, run_number);
        vector<UniValue*> algnShifts;
        if (coeffAlignCorrs)
            coeffAlignCorrs->GetValue(algnShifts);
        fAlignCor = new Double_t * *[nStat];
        for (Int_t iStat = 0; iStat < nStat; iStat++) {
            const Int_t nParams = 3;
            Int_t nModul = StationSet->GetGemStation(iStat)->GetNModules();
            fAlignCor[iStat] = new Double_t * [nModul];
            for (Int_t iMod = 0; iMod < nModul; iMod++) {
                fAlignCor[iStat][iMod] = new Double_t[nParams];
                for (Int_t iPar = 0; iPar < nParams; iPar++) {
                    //cout << iStat << " " << iMod << ": " << ((AlignmentValue*)algnShifts[iStat * nModul + iMod])->value[iPar] << endl;
                    fAlignCor[iStat][iMod][iPar] = (coeffAlignCorrs) ? ((AlignmentValue*)algnShifts[iStat * nModul + iMod])->value[iPar] : 0.0;
                }
            }
        }
    }
}

BmnGemStripHitMaker::~BmnGemStripHitMaker() {
    if (fIsExp) {
        for (Int_t iStat = 0; iStat < StationSet->GetNStations(); iStat++) {
            for (Int_t iMod = 0; iMod < StationSet->GetGemStation(iStat)->GetNModules(); iMod++) {
                delete[] fAlignCor[iStat][iMod];
            }
            delete[] fAlignCor[iStat];
            delete[] fLorCor[iStat];
        }
        delete[] fAlignCor;
        delete[] fLorCor;
    }

    delete StationSet;
}

InitStatus BmnGemStripHitMaker::Init() {

    if (fVerbose > 1) cout << "=================== BmnGemStripHitMaker::Init() started ===============" << endl;

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

    if (fVerbose > 1) {
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

    fField = FairRunAna::Instance()->GetField();
    if (!fField)
        Fatal("Init", "No Magnetic Field found");

    //--------------------------------------------------------------------------
    
    fBmnEvQuality = (TClonesArray*) ioman->GetObject(fBmnEvQualityBranchName);

    if (fVerbose > 1) cout << "=================== BmnGemStripHitMaker::Init() finished ==============" << endl;

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

    if (!IsActive())
        return;

    if (fVerbose > 1) cout << "=================== BmnGemStripHitMaker::Exec() started ===============" << endl;
    clock_t tStart = clock();

    fField = FairRunAna::Instance()->GetField();

    if (fVerbose > 1) cout << " BmnGemStripHitMaker::Exec(), Number of BmnGemStripDigits = " << fBmnGemStripDigitsArray->GetEntriesFast() << "\n";

    ProcessDigits();

    if (fVerbose > 1) cout << "=================== BmnGemStripHitMaker::Exec() finished ==============" << endl;
    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}

void BmnGemStripHitMaker::ProcessDigits() {

    FairMCPoint* MCPoint;
    BmnGemStripDigit* digit;
    BmnMatch *strip_match; // MC-information for a strip

    BmnGemStripStation* station;
    BmnGemStripModule* module;

    //Loading digits ---------------------------------------------------------------
    Int_t AddedDigits = 0;
    Int_t AddedStripDigitMatches = 0;

    for (UInt_t idigit = 0; idigit < fBmnGemStripDigitsArray->GetEntriesFast(); idigit++) {
        digit = (BmnGemStripDigit*) fBmnGemStripDigitsArray->At(idigit);
        if (!digit->IsGoodDigit())
            continue;
        
        if (digit->GetStripSignal() < fSignalLow || digit->GetStripSignal() > fSignalUp)
            continue;
        
        station = StationSet->GetGemStation(digit->GetStation());
        module = station->GetModule(digit->GetModule());

        if (module->SetStripSignalInLayer(digit->GetStripLayer(), digit->GetStripNumber(), digit->GetStripSignal())) AddedDigits++;

        //Add a MC-match to the current strip if this MC-match array exists
        if (fBmnGemStripDigitMatchesArray) {
            strip_match = (BmnMatch*) fBmnGemStripDigitMatchesArray->At(idigit);
            if (module->SetStripMatchInLayer(digit->GetStripLayer(), digit->GetStripNumber(), *strip_match)) AddedStripDigitMatches++;
        }

        //Add a digit number match to the current strip
        BmnMatch stripDigitNumberMatch; // digit number information for the current strip
        stripDigitNumberMatch.AddLink(1.0, idigit);
        module->SetStripDigitNumberMatchInLayer(digit->GetStripLayer(), digit->GetStripNumber(), stripDigitNumberMatch);
    }

    if (fVerbose > 1) cout << "   Processed strip digits  : " << AddedDigits << "\n";
    if (fVerbose > 1 && fBmnGemStripDigitMatchesArray) cout << "   Added strip digit MC-matches  : " << AddedStripDigitMatches << "\n";
    //------------------------------------------------------------------------------

    //Processing digits
    StationSet->ProcessPointsInDetector();

    Int_t NCalculatedPoints = StationSet->CountNProcessedPointsInDetector();
    if (fVerbose > 1) cout << "   Calculated points  : " << NCalculatedPoints << "\n";
    if (fVerbose == 1) cout << "BmnGemStripHitMaker: " << NCalculatedPoints << " hits\n";

    Int_t clear_matched_points_cnt = 0; // points with the only one match-index

    //for SRC only!!! //Temporary! FIXME: move into DB
    const Int_t nSt = 10;
    Double_t alignX[nSt] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    Double_t alignY[nSt] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    // Double_t alignX[nSt] = {0.0, 0.0, 0.0, 0.0, +0.163, -0.060, -0.036, -0.037, +0.006, +0.071};
    // Double_t alignY[nSt] = {0.0, 0.0, 0.0, 0.0, -0.008, -0.082, +0.072, +0.009, +0.030, -0.082};
    const Int_t nFields = 6;
    Double_t dX_ls_fieldScale2[nSt] = {0.0, 0.0, 0.0, 0.0, -0.033, +0.183, -0.231, +0.167, -0.252, +0.156};
    Double_t dY_ls_fieldScale2[nSt] = {0.0, 0.0, 0.0, 0.0, +0.000, -0.027, +0.067, +0.035, -0.001, +0.023};

    // Double_t dX_ls[nSt][nFields] = {
    //     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    //     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    //     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    //     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    //     {0.0, -0.05, -0.04, -0.02, -0.033, -0.04},
    //     {0.0, +0.14, +0.21, +0.30, +0.183, +0.45},
    //     {0.0, -0.08, -0.11, -0.10, -0.231, -0.11},
    //     {0.0, +0.10, +0.13, +0.20, +0.167, +0.31},
    //     {0.0, -0.13, -0.19, -0.22, -0.252, -0.30},
    //     {0.0, +0.03, +0.02, +0.03, +0.156, +0.04}};

    // Double_t dY_ls[nSt][nFields] = {
    //     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    //     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    //     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    //     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    //     {0.0, +0.00, -0.00, -0.00, +0.000, -0.01},
    //     {0.0, +0.01, +0.02, +0.06, -0.027, +0.04},
    //     {0.0, -0.02, -0.01, +0.03, +0.067, -0.02},
    //     {0.0, -0.00, +0.02, +0.07, +0.035, +0.01},
    //     {0.0, -0.02, -0.00, +0.03, -0.001, -0.07},
    //     {0.0, +0.05, +0.10, +0.17, +0.023, +0.09}};

    // Int_t fieldFactors[nFields] = {0, 900, 1200, 1500, 1800, 2100};
    // Int_t fieldScales[nFields] = {0, 1.0, 4.0/3, 5.0/3, 2.0, 7.0/3};
    // Int_t iField = 4;

    for (Int_t iStation = 0; iStation < StationSet->GetNStations(); ++iStation) {
        BmnGemStripStation *station = StationSet->GetGemStation(iStation);

        for (Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            BmnGemStripModule *module = station->GetModule(iModule);
            Int_t NIntersectionPointsInModule = module->GetNIntersectionPoints();

            for (Int_t iPoint = 0; iPoint < NIntersectionPointsInModule; ++iPoint) {

                Double_t threshold = 2000000; // ADC
                Double_t sigL = module->GetIntersectionPoint_LowerLayerSripTotalSignal(iPoint);
                Double_t sigU = module->GetIntersectionPoint_UpperLayerSripTotalSignal(iPoint);

                Double_t x = module->GetIntersectionPointX(iPoint);
                Double_t y = module->GetIntersectionPointY(iPoint);
                Double_t z = module->GetZPositionRegistered();
                if (fIsExp && !fIsSrc)
                    z += fAlignCor[iStation][iModule][2]; //alignment implementation

                Double_t x_err = module->GetIntersectionPointXError(iPoint);
                Double_t y_err = module->GetIntersectionPointYError(iPoint);
                Double_t z_err = 0.0;

                 //Transform hit coordinates from local coordinate system of GEM-planes to global
                if(TransfSet) {
                    Plane3D::Point glob_point = TransfSet->ApplyTransforms(Plane3D::Point(-x, y, z), iStation, iModule);
                    x = -glob_point.X();
                    y = glob_point.Y();
                    z = glob_point.Z();
                }

                Int_t RefMCIndex = 0;

                //MC-matching for the current hit (define RefMCIndex)) ---------
                BmnMatch mc_match_hit = module->GetIntersectionPointMatch(iPoint);

                Int_t most_probably_index = -1;
                Double_t max_weight = 0;

                Int_t n_links = mc_match_hit.GetNofLinks();
                if (n_links == 1) clear_matched_points_cnt++;
                for (Int_t ilink = 0; ilink < n_links; ilink++) {
                    Int_t index = mc_match_hit.GetLink(ilink).GetIndex();
                    Double_t weight = mc_match_hit.GetLink(ilink).GetWeight();
                    if (weight > max_weight) {
                        max_weight = weight;
                        most_probably_index = index;
                    }
                }

                RefMCIndex = most_probably_index;
                //--------------------------------------------------------------

                //Add hit ------------------------------------------------------
                x *= -1; // invert to global X

                if (fIsExp) {
                    Double_t deltaX = fIsSrc ? alignX[iStation] : fAlignCor[iStation][iModule][0];
                    Double_t deltaY = fIsSrc ? alignY[iStation] : fAlignCor[iStation][iModule][1];

                    x += deltaX;
                    y += deltaY;

                    if (Abs(fField->GetBy(0., 0., 0.)) > FLT_EPSILON) {
                        Double_t Bx = Abs(fField->GetBx(x, y, z));
                        Double_t By = Abs(fField->GetBy(x, y, z));
                        Double_t yCor = fLorCor[iStation][0] + fLorCor[iStation][1] * Bx + fLorCor[iStation][2] * Bx * Bx;
                        Double_t xCor = fLorCor[iStation][0] + fLorCor[iStation][1] * By + fLorCor[iStation][2] * By * By;
                        Int_t sign = (module->GetElectronDriftDirection() == ForwardZAxisEDrift) ? +1 : -1;
                        Double_t lsX = fIsSrc ? dX_ls_fieldScale2[iStation] / 2.0 * fFieldScale : xCor * sign;
                        x += lsX;
                        Double_t lsY = fIsSrc ? dY_ls_fieldScale2[iStation] / 2.0 * fFieldScale : yCor * sign;
                        y += lsY;
                    }

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
                hit->SetStripTotalSignalInLowerLayer(sigL);
                hit->SetStripTotalSignalInUpperLayer(sigU);
                hit->SetDigitNumberMatch(module->GetIntersectionPointDigitNumberMatch(iPoint)); //digit number match for the hit
                //--------------------------------------------------------------

                //hit MC-matching ----------------------------------------------
                if (fHitMatching && fBmnGemStripHitMatchesArray) {
                    new ((*fBmnGemStripHitMatchesArray)[fBmnGemStripHitMatchesArray->GetEntriesFast()])
                            BmnMatch(module->GetIntersectionPointMatch(iPoint));
                }
                //--------------------------------------------------------------
            }
        }
    }
    if (fVerbose > 1) cout << "   N clear matches with MC-points = " << clear_matched_points_cnt << "\n";
    //------------------------------------------------------------------------------
    StationSet->Reset();
}

void BmnGemStripHitMaker::Finish() {
    if (StationSet) {
        delete StationSet;
        StationSet = nullptr;
    }

    if(TransfSet) {
        delete TransfSet;
        TransfSet = nullptr;
    }

    cout << "Work time of the GEM hit maker: " << workTime << endl;
}

ClassImp(BmnGemStripHitMaker)
