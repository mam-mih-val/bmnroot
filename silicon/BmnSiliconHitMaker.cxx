#include "BmnSiliconHitMaker.h"

#include "BmnSiliconModule.h"

#include "TClonesArray.h"
#include "TSystem.h"
#include "UniDbDetectorParameter.h"

static Float_t workTime = 0.0;

BmnSiliconHitMaker::BmnSiliconHitMaker()
    : fHitMatching(kTRUE) {

    fInputPointsBranchName = "SiliconPoint";
    fInputDigitsBranchName = "BmnSiliconDigit";
    fInputDigitMatchesBranchName = "BmnSiliconDigitMatch";

    fOutputHitsBranchName = "BmnSiliconHit";

    fCurrentConfig = BmnSiliconConfiguration::None;
    StationSet = NULL;
}

BmnSiliconHitMaker::BmnSiliconHitMaker(Int_t run_period, Int_t run_number, Bool_t isExp, Bool_t isSrc)
    : fHitMatching(kTRUE) {

    fIsExp = isExp;
    fIsSrc = isSrc;
    fInputPointsBranchName = "SiliconPoint";
    fInputDigitsBranchName = (!isExp) ? "BmnSiliconDigit" : "SILICON";
    fInputDigitMatchesBranchName = "BmnSiliconDigitMatch";

    fOutputHitsBranchName = "BmnSiliconHit";

    fCurrentConfig = BmnSiliconConfiguration::None;
    StationSet = NULL;

    fSignalLow = 0.;
    fSignalUp = DBL_MAX;

    switch (run_period) {
    case 6: //BM@N RUN-6
        fCurrentConfig = BmnSiliconConfiguration::RunSpring2017;
        break;
    case 7: //BM@N RUN-7 (and SRC)
        if (fIsSrc) {
            fCurrentConfig = BmnSiliconConfiguration::RunSRCSpring2018;
        } else {
            fCurrentConfig = BmnSiliconConfiguration::RunSpring2018;
        }
        break;
    case 8: //BM@N RUN-8
        fCurrentConfig = BmnSiliconConfiguration::Run8_3stations;
        break;
    }

    TString gPathSiliconConfig = gSystem->Getenv("VMCWORKDIR");
    gPathSiliconConfig += "/parameters/silicon/XMLConfigs/";

    //Create SILICON detector --------------------------------------------------
    switch (fCurrentConfig) {

    case BmnSiliconConfiguration::RunSpring2017:
        StationSet = new BmnSiliconStationSet(gPathSiliconConfig + "SiliconRunSpring2017.xml");
        if (fVerbose > 1) cout << "   Current SILICON Configuration : RunSpring2017" << "\n";
        break;

    case BmnSiliconConfiguration::RunSpring2018:
        StationSet = new BmnSiliconStationSet(gPathSiliconConfig + "SiliconRunSpring2018.xml");
        if (fVerbose > 1) cout << "   Current SILICON Configuration : RunSpring2018" << "\n";
        break;

    case BmnSiliconConfiguration::RunSRCSpring2018:
        StationSet = new BmnSiliconStationSet(gPathSiliconConfig + "SiliconRunSRCSpring2018.xml");
        if (fVerbose > 1) cout << "   Current SILICON Configuration : RunSRCSpring2018" << "\n";
        break;

    case BmnSiliconConfiguration::Run8_3stations:
        StationSet = new BmnSiliconStationSet(gPathSiliconConfig + "SiliconRun8_3stations.xml");
        if (fVerbose) cout << "   Current SILICON Configuration : SiliconRun8_3stations" << "\n";
        break;

    case BmnSiliconConfiguration::Run8_4stations:
        StationSet = new BmnSiliconStationSet(gPathSiliconConfig + "SiliconRun8_4stations.xml");
        if (fVerbose) cout << "   Current SILICON Configuration : SiliconRun8_4stations" << "\n";
        break;

    case BmnSiliconConfiguration::Run8_5stations:
        StationSet = new BmnSiliconStationSet(gPathSiliconConfig + "SiliconRun8_5stations.xml");
        if (fVerbose) cout << "   Current SILICON Configuration : SiliconRun8_5stations" << "\n";
        break;

    case BmnSiliconConfiguration::Run8_mods_6_10_14_18:
        StationSet = new BmnSiliconStationSet(gPathSiliconConfig + "SiliconRun8_mods_6_10_14_18.xml");
        if (fVerbose) cout << "   Current SILICON Configuration : SiliconRun8_mods_6_10_14_18" << "\n";
        break;

    default:
        StationSet = NULL;
    }

    if (fIsExp) {
        const Int_t nStat = StationSet->GetNStations();
        UniDbDetectorParameter* coeffAlignCorrs = UniDbDetectorParameter::GetDetectorParameter("Silicon", "alignment_shift", run_period, run_number);
        vector<UniValue*> algnShifts;
        if (coeffAlignCorrs)
            coeffAlignCorrs->GetValue(algnShifts);
        fAlignCor = new Double_t * *[nStat];
        for (Int_t iStat = 0; iStat < nStat; iStat++) {
            const Int_t nParams = 3;
            Int_t nModul = StationSet->GetSiliconStation(iStat)->GetNModules();
            fAlignCor[iStat] = new Double_t * [nModul];
            for (Int_t iMod = 0; iMod < nModul; iMod++) {
                fAlignCor[iStat][iMod] = new Double_t[nParams];
                for (Int_t iPar = 0; iPar < nParams; iPar++) {
                    fAlignCor[iStat][iMod][iPar] = 0.0;
                }
            }
        }

        for (Int_t i = 0; i < algnShifts.size(); ++i) {
            // cout << ((AlignmentValue*)algnShifts[i])->value[0] << endl;
            // cout << ((AlignmentValue*)algnShifts[i])->value[1] << endl;
            // cout << ((AlignmentValue*)algnShifts[i])->value[2] << endl;
            Int_t st = ((AlignmentValue*)algnShifts[i])->station;
            Int_t mod = ((AlignmentValue*)algnShifts[i])->module;
            fAlignCor[st][mod][0] = ((AlignmentValue*)algnShifts[i])->value[0];
            fAlignCor[st][mod][1] = ((AlignmentValue*)algnShifts[i])->value[1];
            fAlignCor[st][mod][2] = ((AlignmentValue*)algnShifts[i])->value[2];
        }
    }
}

BmnSiliconHitMaker::~BmnSiliconHitMaker() {

    if (fIsExp) {
        for (Int_t iStat = 0; iStat < StationSet->GetNStations(); iStat++) {
            for (Int_t iMod = 0; iMod < StationSet->GetSiliconStation(iStat)->GetNModules(); iMod++)
                delete[] fAlignCor[iStat][iMod];
            delete[] fAlignCor[iStat];
        }
        delete[] fAlignCor;
    }
    if (StationSet) {
        delete StationSet;
    }
}

InitStatus BmnSiliconHitMaker::Init() {

    if (fVerbose > 1) cout << "=================== BmnSiliconHitMaker::Init() started ================" << endl;

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnSiliconDigitsArray = (TClonesArray*)ioman->GetObject(fInputDigitsBranchName);
    if (!fBmnSiliconDigitsArray) {
        cout << "BmnSiliconHitMaker::Init(): branch " << fInputDigitsBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }
    fBmnSiliconDigitMatchesArray = (TClonesArray*)ioman->GetObject(fInputDigitMatchesBranchName);

    if (fVerbose > 1) {
        if (fBmnSiliconDigitMatchesArray) cout << "  Strip matching information exists!\n";
        else cout << "  Strip matching information doesn`t exist!\n";
    }

    fBmnSiliconHitsArray = new TClonesArray(fOutputHitsBranchName);
    ioman->Register(fOutputHitsBranchName, "SILICON", fBmnSiliconHitsArray, kTRUE);
    fBmnSiliconUpperClustersArray = new TClonesArray("StripCluster");
    ioman->Register("BmnSiliconUpperCluster", "SILICON", fBmnSiliconUpperClustersArray, kTRUE);
    fBmnSiliconLowerClustersArray = new TClonesArray("StripCluster");
    ioman->Register("BmnSiliconLowerCluster", "SILICON", fBmnSiliconLowerClustersArray, kTRUE);

    if (fVerbose > 1) cout << "=================== BmnSiliconHitMaker::Init() finished ===============" << endl;

    return kSUCCESS;
}

void BmnSiliconHitMaker::Exec(Option_t* opt) {

    if (!IsActive())
        return;

    fBmnSiliconHitsArray->Delete();
    fBmnSiliconUpperClustersArray->Delete();
    fBmnSiliconLowerClustersArray->Delete();

    clock_t tStart = clock();

    if (fVerbose > 1) cout << "=================== BmnSiliconHitMaker::Exec() started ================" << endl;
    if (fVerbose > 1) cout << " BmnSiliconHitMaker::Exec(), Number of BmnSiliconDigits = " << fBmnSiliconDigitsArray->GetEntriesFast() << "\n";

    ProcessDigits();

    if (fVerbose > 1) cout << "=================== BmnSiliconHitMaker::Exec() finished ===============" << endl;
    clock_t tFinish = clock();
    workTime += ((Float_t)(tFinish - tStart)) / CLOCKS_PER_SEC;
}

void BmnSiliconHitMaker::ProcessDigits() {

    FairMCPoint* MCPoint;
    BmnSiliconDigit* digit;
    BmnMatch* strip_match; // MC-information for a strip

    BmnSiliconStation* station;
    BmnSiliconModule* module;

    //Loading digits ---------------------------------------------------------------
    Int_t AddedDigits = 0;
    Int_t AddedStripDigitMatches = 0;

    for (UInt_t idigit = 0; idigit < fBmnSiliconDigitsArray->GetEntriesFast(); idigit++) {
        digit = (BmnSiliconDigit*)fBmnSiliconDigitsArray->At(idigit);
        if (!digit->IsGoodDigit())
            continue;

        if (digit->GetStripSignal() < fSignalLow || digit->GetStripSignal() > fSignalUp)
            continue;

        station = StationSet->GetSiliconStation(digit->GetStation());
        module = station->GetModule(digit->GetModule());
        if (!module)
            return;

        if (module->SetStripSignalInLayerByZoneId(digit->GetStripLayer(), digit->GetStripNumber(), digit->GetStripSignal())) AddedDigits++;

        //Add a MC-match to the current strip if this MC-match array exists
        if (fBmnSiliconDigitMatchesArray) {
            strip_match = (BmnMatch*)fBmnSiliconDigitMatchesArray->At(idigit);
            if (module->SetStripMatchInLayerByZoneId(digit->GetStripLayer(), digit->GetStripNumber(), *strip_match)) AddedStripDigitMatches++;
        }

        //Add a digit number match to the current strip
        BmnMatch stripDigitNumberMatch; // digit number information for the current strip
        stripDigitNumberMatch.AddLink(1.0, idigit);
        module->SetStripDigitNumberMatchInLayerByZoneId(digit->GetStripLayer(), digit->GetStripNumber(), stripDigitNumberMatch);
    }

    if (fVerbose > 1) cout << "   Processed strip digits  : " << AddedDigits << "\n";
    if (fVerbose > 1 && fBmnSiliconDigitMatchesArray) cout << "   Added strip digit MC-matches  : " << AddedStripDigitMatches << "\n";
    //------------------------------------------------------------------------------

    //Processing digits
    StationSet->ProcessPointsInDetector();

    Int_t NCalculatedPoints = StationSet->CountNProcessedPointsInDetector();
    if (fVerbose > 1) cout << "   Calculated points  : " << NCalculatedPoints << "\n";
    if (fVerbose == 1) cout << "BmnSiliconHitMaker: " << NCalculatedPoints << " hits\n";

    Int_t clear_matched_points_cnt = 0; // points with the only one match-indexes

    for (Int_t iStation = 0; iStation < StationSet->GetNStations(); ++iStation) {
        station = StationSet->GetSiliconStation(iStation);

        for (Int_t iModule = 0; iModule < station->GetNModules(); ++iModule) {
            module = station->GetModule(iModule);
            Int_t NIntersectionPointsInModule = module->GetNIntersectionPoints();

            for (Int_t iPoint = 0; iPoint < NIntersectionPointsInModule; ++iPoint) {

                Double_t threshold = 2000000;// ADC
                Double_t sigL = module->GetIntersectionPoint_LowerLayerSripTotalSignal(iPoint);
                Double_t sigU = module->GetIntersectionPoint_UpperLayerSripTotalSignal(iPoint);

                if (sigL < 0 || sigU < 0) {
                    if (Abs(sigL - sigU) > 100) continue;
                    //                    if (Abs(sigL - sigU) / max(sigU, sigL) > 0.25) continue;
                }

                Double_t x = module->GetIntersectionPointX(iPoint);
                Double_t y = module->GetIntersectionPointY(iPoint);
                Double_t z = module->GetZPositionRegistered();
                z += fIsExp ? fAlignCor[iStation][iModule][2] : 0.; //alignment shift

                Double_t x_err = module->GetIntersectionPointXError(iPoint);
                Double_t y_err = module->GetIntersectionPointYError(iPoint);
                Double_t z_err = 0.0;

                Int_t RefMCIndex = -1;

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
                Double_t deltaX = fIsExp ? fAlignCor[iStation][iModule][0] : 0.;
                Double_t deltaY = fIsExp ? fAlignCor[iStation][iModule][1] : 0.;

                x += deltaX;
                y += deltaY;

                new ((*fBmnSiliconHitsArray)[fBmnSiliconHitsArray->GetEntriesFast()])
                    BmnSiliconHit(0, TVector3(x, y, z), TVector3(x_err, y_err, z_err), RefMCIndex);

                BmnSiliconHit* hit = (BmnSiliconHit*)fBmnSiliconHitsArray->At(fBmnSiliconHitsArray->GetEntriesFast() - 1);
                hit->SetStation(iStation);
                hit->SetModule(iModule);
                hit->SetIndex(fBmnSiliconHitsArray->GetEntriesFast() - 1);
                hit->SetDigitNumberMatch(module->GetIntersectionPointDigitNumberMatch(iPoint)); //digit number match for the hit
                //--------------------------------------------------------------

                new ((*fBmnSiliconUpperClustersArray)[fBmnSiliconUpperClustersArray->GetEntriesFast()]) StripCluster(module->GetUpperCluster(iPoint));
                new ((*fBmnSiliconLowerClustersArray)[fBmnSiliconLowerClustersArray->GetEntriesFast()]) StripCluster(module->GetLowerCluster(iPoint));

                if (fHitMatching) {
                    BmnMatch digiMatch = module->GetIntersectionPointDigitNumberMatch(iPoint);
                    Int_t idx0 = digiMatch.GetLink(0).GetIndex();
                    Int_t idx1 = digiMatch.GetLink(1).GetIndex();
                    BmnMatch* digiMatch0 = (BmnMatch*)fBmnSiliconDigitMatchesArray->At(idx0);
                    BmnMatch* digiMatch1 = (BmnMatch*)fBmnSiliconDigitMatchesArray->At(idx1);

                    Bool_t hitOk = kFALSE;
                    for (Int_t ilink = 0; ilink < digiMatch0->GetNofLinks(); ilink++) {
                        Int_t iindex = digiMatch0->GetLink(ilink).GetIndex();
                        for (Int_t jlink = 0; jlink < digiMatch1->GetNofLinks(); jlink++) {
                            Int_t jindex = digiMatch1->GetLink(jlink).GetIndex();
                            if (iindex == jindex) {
                                hitOk = kTRUE;
                                break;
                            }
                        }
                        if (hitOk) break;
                    }

                    hit->SetType(hitOk);
                    if (!hitOk) hit->SetRefIndex(-1);

                    //hit MC-matching ----------------------------------------------
                    FairRootManager::Instance()->SetUseFairLinks(kTRUE);
                    BmnMatch hitMatch = module->GetIntersectionPointMatch(iPoint);
                    for (BmnLink lnk : hitMatch.GetLinks())
                        hit->AddLink(FairLink(-1, lnk.GetIndex(), lnk.GetWeight()));
                }
                //--------------------------------------------------------------
            }
        }
    }
    if (fVerbose > 1) cout << "   N clear matches with MC-points = " << clear_matched_points_cnt << "\n";
    //------------------------------------------------------------------------------
    StationSet->Reset();
}

void BmnSiliconHitMaker::Finish() {
    if (fVerbose > 0) cout << "Work time of the Silicon hit maker: " << workTime << endl;
}

ClassImp(BmnSiliconHitMaker)

