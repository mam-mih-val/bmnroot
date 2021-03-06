#if !defined(__CLING__) || defined(__MAKECLING__)
// ROOT includes
#include "TString.h"
#include "TStopwatch.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TPRegexp.h"
#include "TKey.h"

// FairRoot includes
#include "FairRunAna.h"
#include "FairSource.h"
#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairFileSource.h"
#include "FairRootFileSink.h"

// BM@N includes
#include "BmnFunctionSet.h"
#include "BmnFieldMap.h"
#include "BmnTrackDrawH.h"
#include "BmnGlobalTrackDraw.h"
#include "UniDbRun.h"
#include "BmnFileSource.h"
#include "BmnNewFieldMap.h"
#include "MpdEventManager.h"
#include "MpdMCPointDraw.h"
#include "MpdHitPointSetDraw.h"
#include "MpdMCTracks.h"

#include <iostream>
using namespace std;
#endif

void SetTasks(MpdEventManager* fMan, int data_source, int run_period, int run_number, bool isField, bool isTarget);

// common EVENT DISPLAY macro for simulated and experimental data
//
// data source: 0 - event display for simulatated data (it shows both MC and reconstructed simulated data):
//      sim_run_info - path to the ROOT file with MC data and detector geometry
//      reco_file - ROOT file with reconstructed data for simulated events
// data source: 1 - offline event display for reconstructed experimental data (it can work in online continious view):
//      sim_run_info - run number in 'runN-NN' format, e.g. "run3-642" to obtain BM@N geometry from the Unified Database
//      reco_file - ROOT file with reconstructed data for experimental events
// data source: 2 - online event display for raw experimental data from directory (it can work in offline mode for last modified file in the directory):
//                  it uses ATLAS TDAQ components, so TDAQ should be present on your machine (FindTDAQ.cmake script)
//      sim_run_info - run number in 'runN-NN' format, e.g. "run6-1" to obtain BM@N geometry from the Unified Database
//      reco_file - path to the directory with raw '*.data' files, e.g. "/tdaq/data/" (last slash is required)
// is_online: false (default) - use Offline Mode (manual switching of events); true - use Online Mode (continious view events)

//void eventdisplay(char* sim_run_info = "run6-1220", char* reco_file = "$VMCWORKDIR/macro/run/bmn_run1220.root", int data_source = 1, bool is_online = false)
//void eventdisplay(char* sim_run_info = "run6-1220", char* reco_file = "/tdaq/data/", int data_source = 2, bool is_online = true)
void eventdisplay(const char* sim_run_info = "$VMCWORKDIR/macro/run/bmnsim.root", const char* reco_file = "$VMCWORKDIR/macro/run/bmndst.root", int data_source = 0, bool is_online = false)
{
    gDebug = 0;

    // CREATE FairRunAna
    FairRunAna* fRunAna = new FairRunAna();

    int run_period = -1, run_number = -1;
    bool isTarget = false, isField = true;
    FairSource* fFileSource = NULL;

    TString strSimRunInfo(sim_run_info), strRecoFile(reco_file);
    // FOR SIMULATION : set source of events to display and addtiional parameters
    if (data_source == 0)
    {
        // check file existence with MC data and detector geometry
        if (!BmnFunctionSet::CheckFileExist(strSimRunInfo))
        {
            cout<<endl<<"ERROR: Simulation file with detector geometry wasn't found!"<<endl;
            return;
        }

        fFileSource = new FairFileSource(strSimRunInfo);

        // set parameter file with MC data and detector geometry
        FairRuntimeDb* rtdb = fRunAna->GetRuntimeDb();
        FairParRootFileIo* parIo1 = new FairParRootFileIo();
        parIo1->open(strSimRunInfo.Data());
        rtdb->setFirstInput(parIo1);
        rtdb->setOutput(parIo1);
        rtdb->saveOutput();

        // add file with reconstructed data as a friend
        if (BmnFunctionSet::CheckFileExist(strRecoFile))
            ((FairFileSource*)fFileSource)->AddFriend(strRecoFile);
        else
            cout<<endl<<"Warning: File with reconstructed data wasn't found!"<<endl;
    }
    // FOR EXPERIMENTAL DATA
    // FROM RECONSTRUCTED ROOT FILE (data_source == 1), FROM DIRECTORY WITH RAW .DATA FILES (data_source == 2)
    else
    {
        Ssiz_t indDash = strSimRunInfo.First('-');
        if ((indDash > 0) && (strSimRunInfo.BeginsWith("run")))
        {
            // get run period
            run_period = TString(strSimRunInfo(3, indDash - 3)).Atoi();
            // get run number
            run_number = TString(strSimRunInfo(indDash+1, strSimRunInfo.Length() - indDash-1)).Atoi();

            // get geometry for run
            TString root_file_path = "current_geo_file.root";
            Int_t res_code = UniDbRun::ReadGeometryFile(run_period, run_number, (char*)root_file_path.Data());
            if (res_code != 0)
            {
                cout << "\nGeometry couldn't' be read from the database" << endl;
                return;
            }

            // get gGeoManager from ROOT file (if required)
            TFile* geoFile = new TFile(root_file_path, "READ");
            if (!geoFile->IsOpen())
            {
                cout<<"Error: could not open ROOT file with geometry!"<<endl;
                return;
            }
            TList* keyList = geoFile->GetListOfKeys();
            TIter next(keyList);
            TKey* key = (TKey*) next();
            TString className(key->GetClassName());
            if (className.BeginsWith("TGeoManager"))
                key->ReadObj();
            else
            {
                cout<<"Error: TGeoManager isn't top element in geometry file "<<root_file_path<<endl;
                return;
            }

            // set magnet field with factor corresponding the given run (for GEANE)
            UniDbRun* pCurrentRun = UniDbRun::GetRun(run_period, run_number);
            if (pCurrentRun == 0)
                return;

            Double_t fieldScale = 0.0;
            Double_t map_current  = 55.87;
            Double_t* field_voltage = pCurrentRun->GetFieldVoltage();
            if ((field_voltage == NULL) || (*field_voltage < 10))
            {
                fieldScale = 0;
                isField = kFALSE;
            }
            else
                fieldScale = (*field_voltage) / map_current;
            BmnFieldMap* magField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.dat");
            magField->SetScale(fieldScale);
            magField->Init();
            fRunAna->SetField(magField);
            TString targ = "-", beam = pCurrentRun->GetBeamParticle();
            if (pCurrentRun->GetTargetParticle() != NULL)
            {
                isTarget = kTRUE;
                targ = *(pCurrentRun->GetTargetParticle());
            }

            cout << "\n\n|||||||||||||||| EXPERIMENTAL RUN SUMMARY ||||||||||||||||" << endl;
            cout << "||\t\tPeriod:\t\t" << run_period << "\t\t\t||" << endl;
            cout << "||\t\tNumber:\t\t" << run_number << "\t\t\t||" << endl;
            cout << "||\t\tBeam:\t\t" << beam << "\t\t\t||" << endl;
            cout << "||\t\tTarget:\t\t" << targ << "\t\t\t||" << endl;
            cout << "||\t\tField scale:\t" << setprecision(4) << fieldScale << "\t\t\t||" << endl;
            cout << "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n\n" << endl;
        }
        else
        {
            cout<<"Error: run info wasn't found!"<<endl;
            return;
        }

        if (!BmnFunctionSet::CheckFileExist(strRecoFile)) return;

        // set source as raw data file
        if (data_source == 1)
            fFileSource = new BmnFileSource(strRecoFile);
        // set source as TDAQ Event Monitor
        if (data_source == 2)
        {
            // load TDAQ libraries
            gSystem->Load("libemon");
            gSystem->Load("libemon-dal");
            gSystem->Load("libcmdline");
            gSystem->Load("libipc");
            gSystem->Load("libowl");
            gSystem->Load("libomniORB4");
            gSystem->Load("libomnithread");
            gSystem->Load("libers");

            //fFileSource = new BmnTdaqSource("bmn", "raw", "file", 2);
        }
    }

    if (fFileSource != NULL) fRunAna->SetSource(fFileSource);

    // Create Event Manager
    MpdEventManager* fMan = new MpdEventManager();
    fMan->isOnline = is_online;
    fMan->iDataSource = data_source;

    // set output file
    fRunAna->SetSink(new FairRootFileSink("ed_out.root"));

    // set tasks to draw
    SetTasks(fMan, data_source, run_period, run_number, isField, isTarget);

    // light background color by default
    fMan->background_color = 17;
    fMan->isDarkColor = false;

    // MpdEventManager::Init(Int_t visopt = 1, Int_t vislvl = 3, Int_t maxvisnds = 10000)
    fMan->Init();
}

// set FairRunAna drawing tasks depending from data source and on/offline mode
void SetTasks(MpdEventManager* fMan, int data_source, int run_period, int run_number, bool isField, bool isTarget)
{
    Style_t pointMarker = kFullDotSmall;
    Color_t mcPointColor = kRed, recoPointColor = kBlack, expPointColor = kRed;

    // FOR SIMULATION
    if (data_source == 0)
    {
        // draw MC Points
        MpdMCPointDraw* ArmTrigPoint = new MpdMCPointDraw("ArmTrigPoint", mcPointColor, pointMarker);
        fMan->AddTask(ArmTrigPoint);
        MpdMCPointDraw* BCPoint = new MpdMCPointDraw("BCPoint", mcPointColor, pointMarker);
        fMan->AddTask(BCPoint);
        MpdMCPointDraw* FDPoint = new MpdMCPointDraw("FDPoint", mcPointColor, pointMarker);
        fMan->AddTask(FDPoint);
        MpdMCPointDraw* MWPCPoint = new MpdMCPointDraw("MWPCPoint", mcPointColor, pointMarker);
        fMan->AddTask(MWPCPoint);
        MpdMCPointDraw* BdPoint = new MpdMCPointDraw("BdPoint", mcPointColor, pointMarker);
        fMan->AddTask(BdPoint);
        MpdMCPointDraw* SiliconPoint = new MpdMCPointDraw("SiliconPoint", mcPointColor, pointMarker);
        fMan->AddTask(SiliconPoint);
        MpdMCPointDraw* StsPoint = new MpdMCPointDraw("StsPoint", mcPointColor, pointMarker); // GEM
        fMan->AddTask(StsPoint);
        MpdMCPointDraw* CSCPoint = new MpdMCPointDraw("CSCPoint", mcPointColor, pointMarker);
        fMan->AddTask(CSCPoint);
        MpdMCPointDraw* TOF1Point = new MpdMCPointDraw("TOF400Point", mcPointColor, pointMarker);
        fMan->AddTask(TOF1Point);
        MpdMCPointDraw* DCHPoint = new MpdMCPointDraw("DCHPoint", mcPointColor, pointMarker);
        fMan->AddTask(DCHPoint);
        MpdMCPointDraw* TofPoint = new MpdMCPointDraw("TOF700Point", mcPointColor, pointMarker);
        fMan->AddTask(TofPoint);
        MpdMCPointDraw* EcalPoint = new MpdMCPointDraw("EcalPoint", mcPointColor, pointMarker);
        fMan->AddTask(EcalPoint);
        MpdMCPointDraw* ZdcPoint = new MpdMCPointDraw("ZdcPoint", mcPointColor, pointMarker);
        fMan->AddTask(ZdcPoint);
        MpdMCPointDraw* SSDPoint = new MpdMCPointDraw("SsdPoint", mcPointColor, pointMarker);
        fMan->AddTask(SSDPoint);

        // draw MC Geometry Tracks
        MpdMCTracks* GeoTrack = new MpdMCTracks("GeoTracks");
        fMan->AddTask(GeoTrack);
        // OR draw MC tracks by Geane - not implemented yet
        //MpdMCStack* MC      Track = new MpdMCStack("MCTrack");
        //fMan->AddTask(MCTrack);

        // draw Reconstructed Detector Hits
        MpdHitPointSetDraw* BmnMwpcHit = new MpdHitPointSetDraw("BmnMwpcHit", expPointColor, pointMarker);
        fMan->AddTask(BmnMwpcHit);
        MpdHitPointSetDraw* BmnArmTrigHit = new MpdHitPointSetDraw("BmnArmTrigHit", recoPointColor, pointMarker);
        fMan->AddTask(BmnArmTrigHit);
        MpdHitPointSetDraw* BmnBCHit = new MpdHitPointSetDraw("BmnBCHit", recoPointColor, pointMarker);
        fMan->AddTask(BmnBCHit);
        MpdHitPointSetDraw* BmnGemHit = new MpdHitPointSetDraw("BmnGemStripHit", recoPointColor, pointMarker); // new MpdHitDraw("BmnGemStripHit", 1); //in box view
        fMan->AddTask(BmnGemHit);
        MpdHitPointSetDraw* BmnTof1Hit = new MpdHitPointSetDraw("BmnTof400Hit", recoPointColor, pointMarker);
        fMan->AddTask(BmnTof1Hit);
        MpdHitPointSetDraw* BmnDchHit = new MpdHitPointSetDraw("BmnDchHit", recoPointColor, pointMarker);
        fMan->AddTask(BmnDchHit);
        MpdHitPointSetDraw* BmnTof2Hit = new MpdHitPointSetDraw("BmnTof700Hit", recoPointColor, pointMarker);
        fMan->AddTask(BmnTof2Hit);
        MpdHitPointSetDraw* BmnSiliconHit = new MpdHitPointSetDraw("BmnSiliconHit", recoPointColor, pointMarker);
        fMan->AddTask(BmnSiliconHit);
        MpdHitPointSetDraw* BmnCSCHit = new MpdHitPointSetDraw("BmnCSCHit", recoPointColor, pointMarker);
        fMan->AddTask(BmnCSCHit);
        MpdHitPointSetDraw* BmnUpstreamHit = new MpdHitPointSetDraw("BmnUpstreamHit", recoPointColor, pointMarker);
        fMan->AddTask(BmnUpstreamHit);

        // draw Reconstructed Global Tracks
        BmnGlobalTrackDraw* BmnGlobalTrack = new BmnGlobalTrackDraw("BmnGlobalTrack");
        fMan->AddTask(BmnGlobalTrack);

        // save EventDisplay Screenshot
        //MpdWebScreenshots* WebScreenshots = new MpdWebScreenshots("WebScreenshots", "/var/www/html/events"); // for WEB-page
        //MpdWebScreenshots* WebScreenshots = new MpdWebScreenshots("WebScreenshots","screenshots"); // folder to save the screenshots
        //WebScreenshots->SetFormatFiles(0);    // 0 -.png, 1 -.jpg, 2 -.jpg and .png
        //WebScreenshots->SetMultiFiles(0);     // 0 - the same file (event.png), 1 - multiple files (event_nnn.png)
        //WebScreenshots->SetPort(8016);        // 8016 by default
        //fMan->AddTask(WebScreenshots);

        return;
    }

    // FOR EXPERIMENTAL DATA FROM RECONSTRUCTED ROOT FILE
    if (data_source == 1)
    {
        // draw MWPC Hits
        MpdHitPointSetDraw* BmnMwpcHit = new MpdHitPointSetDraw("BmnMwpcHit", expPointColor, pointMarker);
        fMan->AddTask(BmnMwpcHit);
        // draw Silicon Hits
        MpdHitPointSetDraw* BmnSiliconHit = new MpdHitPointSetDraw("BmnSiliconHit", expPointColor, pointMarker);
        fMan->AddTask(BmnSiliconHit);
        // draw GEM Hits
        MpdHitPointSetDraw* BmnGemHit = new MpdHitPointSetDraw("BmnGemStripHit", expPointColor, pointMarker);
        fMan->AddTask(BmnGemHit);
        // draw DCH Hits
        MpdHitPointSetDraw* BmnDchHit = new MpdHitPointSetDraw("BmnDchHit", expPointColor, pointMarker);
        fMan->AddTask(BmnDchHit);
        // draw TOF1 Hits
        MpdHitPointSetDraw* BmnTof1Hit = new MpdHitPointSetDraw("BmnTof400Hit", expPointColor, pointMarker);
        fMan->AddTask(BmnTof1Hit);
        // draw TOF2 Hits
        MpdHitPointSetDraw* BmnTof2Hit = new MpdHitPointSetDraw("BmnTof700Hit", expPointColor, pointMarker);
        fMan->AddTask(BmnTof2Hit);
        // draw CSC Hits
        MpdHitPointSetDraw* BmnCSCHit = new MpdHitPointSetDraw("BmnCSCHit", expPointColor, pointMarker);
        fMan->AddTask(BmnCSCHit);
        // draw Upstream Hits
        MpdHitPointSetDraw* BmnUpstreamHit = new MpdHitPointSetDraw("BmnUpstreamHit", expPointColor, pointMarker);
        fMan->AddTask(BmnUpstreamHit);

        // draw MWPC Tracks
        BmnTrackDrawH* BmnMwpcTrack = new BmnTrackDrawH("BmnMwpcTrack", "BmnMwpcHit");
        fMan->AddTask(BmnMwpcTrack);
        // draw GEM Tracks
        BmnTrackDrawH* BmnGemTrack = new BmnTrackDrawH("BmnGemTrack", "BmnGemStripHit");
        fMan->AddTask(BmnGemTrack);
        // draw DCH Tracks
        // BmnTrackDrawH* DchTrack = new BmnTrackDrawH("BmnDchTrack", "BmnDchHit");
        // fMan->AddTask(DchTrack);

        // draw Reconstructed Global Tracks
        BmnGlobalTrackDraw* BmnGlobalTrack = new BmnGlobalTrackDraw("BmnGlobalTrack");
        fMan->AddTask(BmnGlobalTrack);

        //FairGeane* Geane = new FairGeane();
        //fMan->AddTask(Geane);
        //CbmTrackDraw* MwpcTrack = new CbmTrackDraw("MwpcMatchedTracks");
        //fMan->AddTask(MwpcTrack);

        return;
    }
/*
    // FOR EXPERIMENTAL DATA FROM DIRECTORY WITH .DATA FILES
    if (data_source == 2)
    {
        // GEM hit finder
        BmnGemStripConfiguration::GEM_CONFIG gem_config;
        if (run_period == 6)
            gem_config = BmnGemStripConfiguration::RunSpring2017;
        else if (run_period == 5)
            gem_config = BmnGemStripConfiguration::RunWinter2016;
        BmnGemStripHitMaker* gemHM = new BmnGemStripHitMaker(true);
        gemHM->SetCurrentConfig(gem_config);
        gemHM->SetAlignmentCorrectionsFileName(run_period, run_number);
        gemHM->SetHitMatching(kTRUE);
        gemHM->SetVerbose(0);
        fMan->AddTask(gemHM);

        // Tracking GEM
        BmnGemTracking* gemTF = new BmnGemTracking();
        gemTF->SetTarget(isTarget);
        gemTF->SetField(isField);
        TVector3 vAppr = TVector3(0.0, -3.5, -21.7);
        gemTF->SetRoughVertex(vAppr);
        gemTF->SetVerbose(1);
        fMan->AddTask(gemTF);

        // TOF-400 hit finder
        BmnTof1HitProducer* tof1HP = new BmnTof1HitProducer("TOF1", false, 0, kTRUE);
        fMan->AddTask(tof1HP);

        // draw GEM hits
        MpdHitPointSetDraw* GemHit = new MpdHitPointSetDraw("BmnGemStripHit", expPointColor, pointMarker);
        GemHit->SetVerbose(1);
        fMan->AddTask(GemHit);

        // draw TOF-400 hits
        MpdHitPointSetDraw* Tof1Hit = new MpdHitPointSetDraw("BmnTof400Hit", expPointColor, pointMarker);
        Tof1Hit->SetVerbose(1);
        fMan->AddTask(Tof1Hit);

        // draw GEM seeds or tracks
        BmnTrackDrawH* GemTrack = new BmnTrackDrawH("BmnGemTrack", "BmnGemStripHit");
        GemTrack->SetVerbose(1);
        fMan->AddTask(GemTrack);

        return;
    }*/
}

int main(int argc, char** arg)
{
   eventdisplay();
}
