//--------------------------------------------------------------------------------------------------------------------------------------
#include<assert.h>
#include<map>
#include <iostream>


#include <TRandom2.h>
#include <TGeoManager.h>
#include <TGeoBBox.h>
#include <TGeoMatrix.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TEfficiency.h>
#include <TVector3.h>

#include "FairLogger.h"

#include "CbmMCTrack.h"
#include "BmnTofHit.h"
#include "BmnTOF1Point.h"

//#include "BmnTof1GeoUtils.h"

#include "BmnTof1HitProducer.h"

using namespace std;

static Float_t workTime = 0.0;

ClassImp(BmnTof1HitProducer)
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTof1HitProducer::BmnTof1HitProducer(const char *name, Bool_t useMCdata, Int_t verbose, Bool_t test)
: BmnTof1HitProducerIdeal(name, useMCdata, verbose, test), fTimeSigma(0.100), fErrX(1. / sqrt(12.)), fErrY(0.5), pRandom(new TRandom2), h2TestStrips(nullptr), h1TestDistance(nullptr), h2TestNeighborPair(nullptr),
fDoINL(true), fDoSlewing(true), fSignalVelosity(0.060) {
    pGeoUtils = new BmnTof1GeoUtils;

    if (fDoTest) {
        fTestFlnm = "test.BmnTof400HitProducer.root";
        effTestEfficiencySingleHit = new TEfficiency("effSingleHit", "Efficiency single hit;R, cm;Side", 10000, -0.1, 1.);
        fList.Add(effTestEfficiencySingleHit);
        effTestEfficiencyDoubleHit = new TEfficiency("effDoubleHit", "Efficiency double hit;R, cm;Side", 10000, -0.1, 1.);
        fList.Add(effTestEfficiencyDoubleHit);

        h1TestDistance = new TH1D("TestDistance", "Distance between strips;M, cm;Side", 1000, 0., 100.);
        fList.Add(h1TestDistance);
        h2TestStrips = new TH2D("TestStrips", ";Z, cm;#phi, rads", 2000, -300., 300., 500, -3.5, 3.5);
        fList.Add(h2TestStrips);

        h2TestNeighborPair = new TH2D("TestNeighborPair", "Neighbor strip pairs test;stripID1;stripID2", 100, -0.5, 49.5, 100, -0.5, 49.5);
        fList.Add(h2TestNeighborPair);
        h2TestXYSmeared = new TH2D("TestXYSmeared", "Smeared XY (single hit) test;#DeltaX, cm;#DeltaY, cm", 1000, -1., 1., 1000, -2., 2.);
        fList.Add(h2TestXYSmeared);
        h2TestXYSmeared2 = new TH2D("TestXYSmeared2", "Smeared XY (single hit) test;X, cm;Y, cm", 1000, -180., 180., 1000, -180., 180.);
        fList.Add(h2TestXYSmeared2);
        h2TestXYSmearedDouble = new TH2D("TestXYSmearedDouble", "Smeared XY (double hit) test;#DeltaX, cm;#DeltaY, cm", 1000, -2., 2., 1000, -2., 2.);
        fList.Add(h2TestXYSmearedDouble);
        h2TestXYSmearedDouble2 = new TH2D("TestXYSmearedDouble2", "Smeared XY (double hit) test;X, cm;Y, cm", 1000, -180., 180., 1000, -180., 180.);
        fList.Add(h2TestXYSmearedDouble2);

        h2TestEtaPhi = new TH2D("TestEtaPhi", ";#eta;#phi, degree", 1000, -1.6, 1.6, 1000, -181., 181.);
        fList.Add(h2TestEtaPhi);
        h2TestRZ = new TH2D("TestRZ", ";X, cm;Y, cm", 1000, -300., 300., 1000, -200., 200.);
        fList.Add(h2TestRZ);
        h2TdetIdStripId = new TH2D("TdetIdStripId", ";stripId;detId", 100, -0.5, 99.5, 21, -0.5, 20.5);
        fList.Add(h2TdetIdStripId);
    }
}
//--------------------------------------------------------------------------------------------------------------------------------------

BmnTof1HitProducer::~BmnTof1HitProducer() {
    delete pRandom;
    delete pGeoUtils;
}
//--------------------------------------------------------------------------------------------------------------------------------------

InitStatus BmnTof1HitProducer::Init() {
    //FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "Begin [BmnTof1HitProducer::Init].");

    if (fVerbose) cout << endl << "BmnTof1HitProducer::Init(): Start" << endl;

    if (fOnlyPrimary) cout << " Only primary particles are processed!!! \n"; // FIXME NOT used now ADDD

    if (fUseMCData) {
        aMcPoints = (TClonesArray*) FairRootManager::Instance()->GetObject("TOF1Point");
        if (!aMcPoints) {
            cout << "BmnTof1HitProducer::Init(): branch TOF1Point not found! Task will be deactivated" << endl;
            SetActive(kFALSE);
            return kERROR;
        }
        aMcTracks = (TClonesArray*) FairRootManager::Instance()->GetObject("MCTrack");
        if (!aMcTracks) {
            cout << "BmnTof1HitProducer::Init(): branch MCTrack not found! Task will be deactivated" << endl;
            SetActive(kFALSE);
            return kERROR;
        }
    } else {
        aExpDigits = (TClonesArray*) FairRootManager::Instance()->GetObject("TOF400");
        if (!aExpDigits) {
            cout << "BmnTof1HitProducer::Init(): branch TOF400 not found! Task will be deactivated" << endl;
            SetActive(kFALSE);
            return kERROR;
        }

        //looking for T0 branch
        TString NameT0Branch;
        if (fVerbose) cout << "BmnTof1HitProducer::Init(): looking for branch " << NameT0Branch << "for start" << endl;
        if (fPeriod == 6) NameT0Branch = "T0";
        if (fPeriod == 7) NameT0Branch = "BC2";
        aExpDigitsT0 = (TClonesArray*) FairRootManager::Instance()->GetObject(NameT0Branch.Data());
        if (!aExpDigitsT0) {
            cout << "BmnTof1HitProducer::Init(): branch T0 not found! Task will be deactivated" << endl;
            SetActive(kFALSE);
            return kERROR;
        }
    }

    // Create and register output array
    aTofHits = new TClonesArray("BmnTofHit");
    FairRootManager::Instance()->Register("BmnTof1Hit", "TOF1", aTofHits, kTRUE);

    //Parsing geometry
    fNDetectors = pGeoUtils->ParseTGeoManager(fUseMCData, h2TestStrips, true);
    if (fVerbose) cout << "BmnTof1HitProducer::Init(): number of TOF400 Detectors from geometry file = " << fNDetectors << endl;
    pGeoUtils->FindNeighborStrips(h1TestDistance, h2TestNeighborPair, fDoTest);

    // Init BmnTOF1Detectors 
    if (!fUseMCData) {

        if (!SetCorrFiles()) {
            cout << "BmnTof1HitProducer::Init(): No corrections for the current run! Task will be deactivated" << endl;
            SetActive(kFALSE);
            return kERROR;
        }

        //        NameFileLRcorrection = Form("TOF400_LRCorr_RUN%i_v2.dat", fPeriod);
        //        Bool_t FlagFileLRcorrection = IsFile(NameFileLRcorrection);
        //        if (!FlagFileLRcorrection) {
        //            cout << endl << "File " << NameFileLRcorrection.Data() << " for LR correction is not found" << endl;
        //            cout << "Check /input folder for file" << endl;
        //        }
        //
        //        NameFileSlewingCorrection = Form("TOF400_SlewingCorr_BMN_RUN%i_v2.root", fPeriod);
        //        Bool_t FlagFileSlewingCorrection = IsFile(NameFileSlewingCorrection);
        //        if (!FlagFileSlewingCorrection) {
        //            cout << endl << "File " << NameFileSlewingCorrection.Data() << " for Slewing correction is not found" << endl;
        //            cout << "Check /input folder for file" << endl;
        //        }
        //
        //        NameFileTimeShiftCorrection = Form("TOF400_TimeShiftCorr_BMN_RUN%i_v3.dat", fPeriod);
        //        Bool_t FlagFileTimeShiftCorrection = IsFile(NameFileTimeShiftCorrection);
        //        if (!FlagFileTimeShiftCorrection) {
        //            cout << "File " << NameFileTimeShiftCorrection.Data() << " for Time shift correction is not found" << endl;
        //            cout << "Check /input folder for file" << endl;
        //        }
        pDetector = new BmnTOF1Detector *[fNDetectors];
        for (Int_t i = 0; i < fNDetectors; i++) {
            Int_t DoTestForDetector = 0; // For developers only. Level of Histograms filling (0-don't fill, 1-low, 2-high). 
            pDetector[i] = new BmnTOF1Detector(i, DoTestForDetector, fVerbose);

            if (FlagFileLRcorrection) pDetector[i]->SetCorrLR(NameFileLRcorrection);
            if (FlagFileSlewingCorrection) pDetector[i]->SetCorrSlewing(NameFileSlewingCorrection);
            if (FlagFileTimeShiftCorrection) pDetector[i]->SetCorrTimeShift(NameFileTimeShiftCorrection);
            pDetector[i]->SetGeo(pGeoUtils);

            if (fPeriod == 6) {
                //cout << "!!!!!!!!!!!!!!!! Kill Strip !!!!!!!!!!!!!!!!!!" << endl;
                pDetector[i]->KillStrip(0);
                pDetector[i]->KillStrip(47);
            }
        }
    }

    if (fVerbose) cout << "BmnTof1HitProducer::Init(): Initialization finished succesfully." << endl;

    //FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "Initialization [BmnTof1HitProducer::Init] finished succesfully.");

    return kSUCCESS;
}
//--------------------------------------------------------------------------------------------------------------------------------------

Bool_t BmnTof1HitProducer::HitExist(Double_t val) // val - distance to the pad edge [cm]
{
    const static Double_t slope = (0.98 - 0.95) / 0.2;
    Double_t efficiency = (val > 0.2) ? 0.98 : (0.95 + slope * val);

    //-------------------------------------
    // 99% ---------
    //              \
    //               \
    //                \
    // 95%             \ 
    //  <-----------|--|
    //            0.2  0.
    //-------------------------------------

    if (pRandom->Rndm() < efficiency) return true;
    return false;
}
//------------------------------------------------------------------------------------------------------------------------

Bool_t BmnTof1HitProducer::DoubleHitExist(Double_t val) // val - distance to the pad edge  [cm]
{
    const static Double_t slope = (0.3 - 0.0) / 0.5;
    Double_t efficiency = (val > 0.5) ? 0. : (0.3 - slope * val);

    //-------------------------------------
    // 30%               /
    //                  /
    //                 / 
    //                /
    // 0%            /  
    //  <-----------|----|
    //            0.5    0.
    //-------------------------------------

    if (efficiency == 0.) return false;

    if (pRandom->Rndm() < efficiency) return HitExist(val);
    return false;
}
//--------------------------------------------------------------------------------------------------------------------------------------

void BmnTof1HitProducer::Exec(Option_t* opt) {
    if (!IsActive())
        return;
    clock_t tStart = clock();

    if (fVerbose) cout << endl << "======================== TOF400 exec started ====================" << endl;
    static const TVector3 XYZ_err(fErrX, fErrY, 0.);

    aTofHits->Clear();

    Int_t UID, trackID;
    TVector3 pos, XYZ_smeared;
    int nSingleHits = 0, nDoubleHits = 0;

    if (fUseMCData) {
        for (Int_t pointIndex = 0, nTofPoint = aMcPoints->GetEntriesFast(); pointIndex < nTofPoint; pointIndex++) // cycle by TOF points
        {
            BmnTOF1Point *pPoint = (BmnTOF1Point*) aMcPoints->UncheckedAt(pointIndex);

            if (fVerbose > 2) pPoint->Print("");

            trackID = pPoint->GetTrackID();
            UID = pPoint->GetDetectorID();
            Double_t time = pRandom->Gaus(pPoint->GetTime(), fTimeSigma); // 100 ps		
            pPoint->Position(pos);

            const LStrip1 *pStrip = pGeoUtils->FindStrip(UID);

            XYZ_smeared.SetXYZ(pStrip->center.X(), pRandom->Gaus(pos.Y(), fErrY), pStrip->center.Z());

            LStrip1::Side_t side;
            Double_t distance = pStrip->MinDistanceToEdge(&pos, side); // [cm]

            bool passed;
            if (passed = HitExist(distance)) // check efficiency 
            {
                AddHit(UID, XYZ_smeared, XYZ_err, pointIndex, trackID, time);
                nSingleHits++;

                if (fDoTest) {
                    h2TestXYSmeared->Fill(pos.X() - XYZ_smeared.X(), pos.Y() - XYZ_smeared.Y());
                    h2TestXYSmeared2->Fill(XYZ_smeared.X(), XYZ_smeared.Y());
                    h2TestEtaPhi->Fill(pos.Eta(), pos.Phi() * TMath::RadToDeg());
                    h2TestRZ->Fill(pos.X(), pos.Y());
                }
            }

            if (fDoTest) effTestEfficiencySingleHit->Fill(passed, distance);

            if (passed = DoubleHitExist(distance)) // check cross hit
            {
                Int_t CrossUID = (side == LStrip1::kRight) ? pStrip->neighboring[LStrip1::kRight] : pStrip->neighboring[LStrip1::kLeft];

                if (LStrip1::kInvalid == CrossUID) continue; // last strip on module

                pStrip = pGeoUtils->FindStrip(CrossUID);
                XYZ_smeared.SetXYZ(pStrip->center.X(), pRandom->Gaus(pos.Y(), fErrY), pStrip->center.Z());

                AddHit(CrossUID, XYZ_smeared, XYZ_err, pointIndex, trackID, time);
                nDoubleHits++;

                if (fDoTest) {
                    h2TestXYSmearedDouble->Fill((pos - XYZ_smeared).Mag(), pos.Z() - XYZ_smeared.Z());
                    h2TestXYSmearedDouble2->Fill(XYZ_smeared.X(), XYZ_smeared.Y());
                }
            }

            if (fDoTest) effTestEfficiencyDoubleHit->Fill(passed, distance);

        } // cycle by the TOF points
    } else {
        BmnTrigDigit* digT0 = FingT0Digit();

        for (Int_t i = 0; i < fNDetectors; i++)
            pDetector[i]->Clear();

        for (Int_t iDig = 0; iDig < aExpDigits->GetEntriesFast(); ++iDig) {
            BmnTof1Digit* digTof = (BmnTof1Digit*) aExpDigits->At(iDig);
            //cout << "SETTING PLANE " << digTof->GetPlane() << "\n";
            pDetector[digTof->GetPlane()]->SetDigit(digTof);
        }

        for (Int_t i = 0; i < fNDetectors; i++)
            nSingleHits += pDetector[i] -> FindHits(digT0, aTofHits);

    }


    MergeHitsOnStrip(); // save only the fastest hit in the strip. Used for MC only

    int nFinally = CompressHits(); // remove blank slotes

    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;

    if (fVerbose) cout << "Tof400  single hits= " << nSingleHits << ", double hits= " << nDoubleHits << ", final hits= " << nFinally << endl;
    if (fVerbose) cout << "======================== TOF400 exec finished ====================" << endl;
}
//--------------------------------------------------------------------------------------------------------------------------------------

void BmnTof1HitProducer::Finish() {

    if (fDoTest) {
        //FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "[BmnTof1HitProducer::Finish] Update  %s file. ", fTestFlnm.Data());
        TFile *ptr = gFile;
        TFile file(fTestFlnm.Data(), "RECREATE");
        fList.Write();
        file.Close();
        gFile = ptr;
        if (!fUseMCData)
            for (Int_t i = 0; i < fNDetectors; i++)
                pDetector[i] -> SaveHistToFile(fTestFlnm.Data());
    }

    cout << "Work time of the TOF-400 hit finder: " << workTime << endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------

void BmnTof1HitProducer::SetSeed(UInt_t seed) {
    pRandom->SetSeed(seed);
}
//--------------------------------------------------------------------------------------------------------------------------------------

BmnTrigDigit* BmnTof1HitProducer::FingT0Digit() {
    BmnTrigDigit* digT0 = NULL;
    for (Int_t i = 0; i < aExpDigitsT0->GetEntriesFast(); i++) {
        digT0 = (BmnTrigDigit*) aExpDigitsT0->At(i);
        if (digT0->GetMod() == 0) {
            //if (fVerbose) 
            //    cout << "BmnTof1HitProducer::FingT0Digit(): T0 digit is found, Time = " << digT0->GetTime() << endl;
            return digT0; // take first T0 digit with Mod == 0. needed for ToF calculation.
        }
    }
    return NULL;
}
//--------------------------------------------------------------------------------------------------------------------------------------

Bool_t BmnTof1HitProducer::IsFile(TString NameFile = "") {
    NameFile = Form("%s%s%s", getenv("VMCWORKDIR"), "/input/", NameFile.Data());
    ifstream Temp;
    Temp.open(NameFile, ios::in /*| ios::nocreate*/);
    return Temp.is_open();
}
//--------------------------------------------------------------------------------------------------------------------------------------

Bool_t BmnTof1HitProducer::SetCorrFiles() {

    Bool_t temp = false;
    FlagFileLRcorrection = false;
    FlagFileSlewingCorrection = false;
    FlagFileTimeShiftCorrection = false;

    // Run 6 (03.2017)
    if (fPeriod == 6) {
        NameFileLRcorrection = Form("TOF400_LRCorr_RUN%i.dat", fPeriod);
        NameFileSlewingCorrection = Form("TOF400_SlewingCorr_RUN%i.root", fPeriod);
        NameFileTimeShiftCorrection = Form("TOF400_TimeShiftCorr_RUN%i.dat", fPeriod);

        FlagFileLRcorrection = true;
        FlagFileSlewingCorrection = true;
        FlagFileTimeShiftCorrection = true;

        temp = true;
    }

    // Run 7 (03.2018 - 04.2018)
    if (fPeriod == 7) {

        //SRC
        if (fRun >= 2013 && fRun <= 3588) {
            
            //for first time will be used correction from BM@N
            NameFileLRcorrection = Form("TOF400_LRCorr_RUN%i_BMN.dat", fPeriod); 
            NameFileSlewingCorrection = Form("TOF400_SlewingCorr_RUN%i_Bmn.root", fPeriod);
            
            //NameFileLRcorrection = Form("TOF400_LRCorr_RUN%i_SRC.dat", fPeriod);
            //NameFileSlewingCorrection = Form("TOF400_SlewingCorr_RUN%i_SRC.root", fPeriod);
            
            NameFileTimeShiftCorrection = Form("TOF400_TimeShiftCorr_RUN%i_SRC.dat", fPeriod);

            FlagFileLRcorrection = true;
            FlagFileSlewingCorrection = true;
            FlagFileTimeShiftCorrection = true;

            temp = true;
        }

        //BM@N Ar beam
        if (fRun >= 3589 && fRun <= 4707) {
            NameFileLRcorrection = Form("TOF400_LRCorr_RUN%i_BMN.dat", fPeriod);
            NameFileSlewingCorrection = Form("TOF400_SlewingCorr_RUN%i_BMN.root", fPeriod);
            NameFileTimeShiftCorrection = Form("TOF400_TimeShiftCorr_RUN%i_BMN_Ar.dat", fPeriod);

            FlagFileLRcorrection = true;
            FlagFileSlewingCorrection = true;
            FlagFileTimeShiftCorrection = true;

            temp = true;
        }

        //BM@N Kr beam
        if (fRun >= 4747 && fRun <= 5185) {
            NameFileLRcorrection = Form("TOF400_LRCorr_RUN%i_BMN.dat", fPeriod);
            NameFileSlewingCorrection = Form("TOF400_SlewingCorr_RUN%i_BMN.root", fPeriod);
            NameFileTimeShiftCorrection = Form("TOF400_TimeShiftCorr_RUN%i_BMN_Kr.dat", fPeriod);

            FlagFileLRcorrection = true;
            FlagFileSlewingCorrection = true;
            FlagFileTimeShiftCorrection = true;

            temp = true;
        }
    }

    if (temp) {

        //check all files exist 
        if (!IsFile(NameFileLRcorrection)) {
            FlagFileLRcorrection = false;
            if (fVerbose) {
                cout << endl << "BmnTof1HitProducer::Init(): File " << NameFileLRcorrection.Data() << " for LR correction is not found" << endl;
                cout << "Check /input folder for file" << endl;
            }
        }

        if (!IsFile(NameFileSlewingCorrection)) {
            FlagFileSlewingCorrection = false;
            if (fVerbose) {
                cout << endl << "BmnTof1HitProducer::Init(): File " << NameFileSlewingCorrection.Data() << " for Slewing correction is not found" << endl;
                cout << "Check /input folder for file" << endl;
            }
        }

        if (!IsFile(NameFileTimeShiftCorrection)) {
            FlagFileTimeShiftCorrection = false;
            if (fVerbose) {
                cout << endl << "BmnTof1HitProducer::Init(): File " << NameFileTimeShiftCorrection.Data() << " for TimeShift correction is not found" << endl;
                cout << "Check /input folder for file" << endl;
            }
        }

    }

    // return "true" in case the run is physical and correction files are found.
    if (FlagFileLRcorrection && FlagFileSlewingCorrection && FlagFileTimeShiftCorrection)
        return kTRUE;

    // return "false" in case the run is outside physical runs or correction files are not found.
    return kFALSE;
}
