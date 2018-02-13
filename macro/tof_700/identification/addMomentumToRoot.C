// To use this macro for run6 it is necessary to use
// appropriate configuration files for that run!

#if !defined(__CINT__) || defined(__MAKECINT__)

// CBM includes
#include "CbmKF.h"
#include "CbmKFSecondaryVertexFinder.h"
#include "CbmKFParticle.h"
#include "CbmKFTrack.h"
#include "CbmMCTrack.h"
#include "CbmStsKFTrackFitter.h"
#include "CbmStsTrack.h"
#include "CbmTrackMatch.h"
#include "CbmStsHit.h"
#include "CbmStsPoint.h"
#include "CbmStsStation.h"
#include "CbmVertex.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairTrackParam.h"
#include "BmnFileSource.h"
#include "BmnFieldConst.h"
#include "UniDbRun.h"
#include "BmnFieldMap.h"
#include "BmnNewFieldMap.h"
#include "CbmStsDigiPar.h"
#include "TKey.h"
#include "BmnTrack.h"

// ROOT includes
#include <TBranch.h>
#include <TClonesArray.h>
#include <TDatabasePDG.h>
#include <TFile.h>
#include <TGeoManager.h>
#include <TGeoCone.h>
#include <TGeoVolume.h>
#include <TH1.h>
#include <TH2.h>
#include <TMatrixD.h>
#include <TRandom.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TTree.h>
#include <TVector3.h>
#include <TVectorD.h>
#include <TCanvas.h>
#include <Riostream.h>
#include <map>
#include <set>
#include <vector>

#include <TGraph2D.h>
// #include </opt/bmnroot/globaltracking/BmnTrackExtrapolator.h>
#include <BmnTrackExtrapolator.h>
#include <BmnEventHeader.h>
#include <TVirtualFitter.h>
#include <TMinuit.h>

#endif

//#include <vector>

#define DCH1_Z 538.2
//#define DCH2_Z 722.5
#define DCH_Z 637.7

// Alignment corrections
//#define DXDCH -0.262
//#define DYDCH 2.135
//#define DTXDCH -0.002303
//#define DTYDCH -0.001813

//#define DXDCH -2.699
//#define DYDCH +1.753
//#define DTXDCH -0.006233
//#define DTYDCH -0.0003388

//#define DXDCH -2.4
//#define DYDCH +2.7
//#define DTXDCH -0.008
//#define DTYDCH +0.001

#define DXDCH 0.0
#define DYDCH 0.0
#define DTXDCH 0.0
#define DTYDCH 0.0

#define LEFT_X_Y_EDGE 0
#define RIGHT_X_Y_EDGE 100
#define RIGHT_X_Y_EDGE_DCH1 (RIGHT_X_Y_EDGE / 2)

#define LEFT_AMOUNT_EDGE 0.
#define RIGHT_AMOUNT_EDGE 1000.
//#define DAMOUNT_MIN (RIGHT_AMOUNT_EDGE + 1000.)
#define DAMOUNT_MIN 50

//#define PDG_ID -211
#define PDG_ID 2212

Bool_t GetBmnGeom(const char* fileName);
void ApplyAlignment();
Int_t GetNofModules(TGeoNode* station);

Double_t calculateTrackLength(FairTrackParam *parFirst, BmnTrackExtrapolator* extrapolator, Double_t zEndPoint) {
    Int_t numOfIterations = 1000;
    Double_t stepOfIteration = zEndPoint / numOfIterations;
    Double_t length = 0;

    //    cout << "Before extrapolator" << endl;
    //    cout << "parFirst->GetX() = " << parFirst->GetX() << endl;
    //    cout << "parFirst->GetY() = " << parFirst->GetY() << endl;
    //    cout << "parFirst->GetZ() = " << parFirst->GetZ() << endl;

//    extrapolator->RK5Order(parFirst, -22.);
//    parFirst->SetZ(-22.);

    extrapolator->RK5Order(parFirst, 0.);
    parFirst->SetZ(0.);

    //    cout << "After extrapolator" << endl;
    //    cout << "parFirst->GetX() = " << parFirst->GetX() << endl;
    //    cout << "parFirst->GetY() = " << parFirst->GetY() << endl;
    //    cout << "parFirst->GetZ() = " << parFirst->GetZ() << endl;

    Double_t x0 = parFirst->GetX();
    Double_t y0 = parFirst->GetY();
    Double_t z0 = parFirst->GetZ();

    for (Int_t i = 0; i < numOfIterations; i++) {
        extrapolator->RK5Order(parFirst, (i + 1) * stepOfIteration);
        parFirst->SetZ((i + 1) * stepOfIteration);

        //        cout << "parFirst->GetX() = " << parFirst->GetX() << endl;
        //        cout << "parFirst->GetY() = " << parFirst->GetY() << endl;
        //        cout << "parFirst->GetZ() = " << parFirst->GetZ() << endl;

        length += TMath::Sqrt((parFirst->GetX() - x0)*(parFirst->GetX() - x0) +
                (parFirst->GetY() - y0)*(parFirst->GetY() - y0) +
                (parFirst->GetZ() - z0)*(parFirst->GetZ() - z0));

        //        cout << "length = " << length << endl;

        x0 = parFirst->GetX();
        y0 = parFirst->GetY();
        z0 = parFirst->GetZ();
    }

    //    cout << "length = " << length << endl;

    return length;
}

Int_t numOfNotFound = 0;
Int_t numOfUnexpected = 0;

Int_t getGemEntryId(Int_t iEv, TClonesArray *gemEv, TClonesArray *dchEv, TChain* gemChain, TChain* dchChain) {
    static Int_t iEvGem = 0;
    UInt_t dchEvId;
    UInt_t gemEvId = 0;

    if (!((BmnEventHeader*) dchEv->UncheckedAt(0))) {
        //            cout << "GEM event have not found" << endl;
        numOfNotFound++;
        return -1;
    }
    dchEvId = ((BmnEventHeader*) dchEv->UncheckedAt(0))->GetEventId();

    //    cout << "dchEvId = " << dchEvId << endl;
    //    cout << "gemChain->GetEntries() = " << gemChain->GetEntries() << endl;
    while (iEvGem < dchChain->GetEntries()) {
        gemChain->GetEntry(iEvGem);
        //        cout << "((BmnEventHeader*) gemEv->UncheckedAt(0))->GetEventId() = " << ((BmnEventHeader*) gemEv->UncheckedAt(0))->GetEventId() << endl;
        gemEvId = ((BmnEventHeader*) gemEv->UncheckedAt(0))->GetEventId();
        if (gemEvId > dchEvId) {
            //            cout << "GEM event have not found" << endl;
            numOfNotFound++;
            return -1;
        }
        iEvGem++;
        if (gemEvId == dchEvId) {
            //            cout << "EvId = " << dchEvId - 1 << endl;
            return iEvGem - 1;
        }
    }
    if (iEvGem == dchChain->GetEntries()) {
        //        cout << "GEM event have not found" << endl;
        numOfNotFound++;
        return -1;
    }

    //    cout << "Unexpected result for getGemEntryId()" << endl;
    numOfUnexpected++;
    return iEvGem;
}

// Algorithm of removing track duplicates could be changed. On first iteration
// we can find mean of amount residual. After that from all dch tracks for specific
// gem track we can select track with minimum deviation from mean of amount residual.

Bool_t isInVector(vector<Int_t> vector, Int_t pattern) {
    for (Int_t i = 0; i < (int)vector.size(); i++) {
        if (vector[i] == pattern)
            return kTRUE;
    }
    return kFALSE;
}

void addMomentumToRoot(Int_t runId = 1889) {

//void addMomentumToRoot(TString inFileGem = "", TString inFileDch = "") {

    TString inFileGem = Form("../../run/bmndst_%d.root", runId);
    TString inFileDch = Form("bmn_reco_DCH_run%d.root", runId);

    if ((inFileGem == "") || (inFileDch == "")) {
        cout << "Please, enter full path to root file for GEM and full path to root file for DCH" << endl;
        return;
    }

    Double_t fieldScale;
    TString dataFile;

    //    cout << "Enter field data file name" << endl;
    //    cout << "It could be for example field_sp41v3_ascii_Extrap.dat or field_sp41v2_ascii_noExtrap.dat " << endl;
    //    cin >> dataFile;
    //    dataFile = "field_sp41v2_ascii_noExtrap.dat";
    dataFile = "field_sp41v4_ascii_Extrap.dat";
    //    dataFile = "nofield.dat";
    //    cout << "Enter fieldScale. fieldScale = 1. for 900A" << endl;
    //    cin >> fieldScale;
    //    fieldScale = 2. / 3.;
    fieldScale = 1200. / 900.;
    BmnFieldMap* magField = new BmnNewFieldMap(dataFile);
    //    BmnFieldMap* magField = new BmnNewFieldMap("field_const_8_5_gauss.dat");
    magField->SetScale(fieldScale);
    magField->Init();
    magField->Print();

    TString outFile = "aaa3.root";

#ifdef USE_DOFIT
    GetBmnGeom("GEMS_RunWinter2016_old.root");
    ApplyAlignment();
#endif

    FairRunAna *fRun = new FairRunAna();
    fRun->SetField(magField);

#ifdef USE_DOFIT
    fRun->SetSource(new BmnFileSource(inFileGem));
    fRun->SetOutputFile(outFile);

    TString parFile = inFileGem;
    TList *parFileList = new TList();
    TObjString stsDigiFile = "sts_v17BMN_gem.digi.par"; //sts_v16BMN_gem.digi.par
    parFileList->Add(&stsDigiFile);

    FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo();
    FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
    parIo1->open(parFile.Data());
    parIo2->open(parFileList, "in");
    rtdb->setFirstInput(parIo1);
    rtdb->setSecondInput(parIo2);
    rtdb->setOutput(parIo1);
    rtdb->saveOutput();

    FairTask *pKF = new CbmKF();
    fRun->AddTask(pKF);
    fRun->Init();

    TChain *rec = FairRootManager::Instance()->GetInChain();
    TClonesArray *tracks = (TClonesArray *) FairRootManager::Instance()->GetObject("StsTrack");
    TClonesArray *hits = (TClonesArray *) FairRootManager::Instance()->GetObject("StsHit");
    TClonesArray *eventHeaderGem = (TClonesArray *) FairRootManager::Instance()->GetObject("BmnEventHeader");
#endif

    //TChain* in1 = new TChain("BMN_ALLDIGIT");
    TChain* in1 = new TChain("cbmsim");
    TChain* in2 = new TChain("cbmsim");

    //    cout << "Enter bmndst root file name" << endl;
    //    cin >> rootFile;

    in1->Add(inFileGem);
    //    in2->Add("/home/plot/BmnExamples/8_AddMomentumForToF/bmndst_526.root");
    in2->Add(inFileDch);
    cout << "#recorded entries BMN_ALLDIGIT = " << in1->GetEntries() << endl;
    cout << "#recorded entries cbmsim = " << in2->GetEntries() << endl;

    //TClonesArray *eventHeaderGem = new TClonesArray("FairEventHeader");

#ifndef USE_DOFIT
    TClonesArray* hits = NULL;
    TClonesArray* tracks = NULL;
    TClonesArray* eventHeaderGem = NULL;
#endif
    TClonesArray* eventHeader = new TClonesArray("BmnEventHeader");
    TClonesArray* tof700 = new TClonesArray("BmnTof2Digit");
    TClonesArray* tof400 = new TClonesArray("BmnTof1Digit");
    TClonesArray* dchHits = new TClonesArray("BmnDchHit");
    TClonesArray* dchTracks = new TClonesArray("BmnTrack");

    TClonesArray* Tracks = NULL;
    TClonesArray* TracksOut = NULL;
    TClonesArray* TracksGemPar = NULL;

    TClonesArray* dchMatchedTracks = NULL;
    TClonesArray* dchMatchedTracksOut = new TClonesArray("BmnTrack");
    TClonesArray* dchMatchedTracksGemPar = new TClonesArray("BmnTrack");
    TClonesArray* dch1TracksOut = new TClonesArray("BmnTrack");
    TClonesArray* dch1TracksGemPar = new TClonesArray("BmnTrack");

    //in1->SetBranchAddress("StsHit", &hits);
    in1->SetBranchAddress("BmnGemStripHit", &hits);//AC
    //in1->SetBranchAddress("StsTrack", &tracks);
    //in1->SetBranchAddress("ExtrapCbmStsTrackSM", &tracks);
    in1->SetBranchAddress("BmnGemTrack", &tracks);//AC
    //in1->SetBranchAddress("EventHeader.", &eventHeaderGem);
    in1->SetBranchAddress("EventHeaderBmn", &eventHeaderGem);
    in2->SetBranchAddress("EventHeader", &eventHeader);
    in2->SetBranchAddress("TOF700", &tof700);
    in2->SetBranchAddress("TOF400", &tof400);
    in2->SetBranchAddress("BmnDchHit", &dchHits);
    in2->SetBranchAddress("DchTracks", &dchTracks);
    in2->SetBranchAddress("DchMatchedTracks", &dchMatchedTracks);

    TString outName = Form("bmn_reco_DCH_GEM_run%d.root", runId);
    TFile* fReco = new TFile(outName, "RECREATE"); //the root file where the dch hits and tracks will be filled
    TTree* tReco = new TTree("cbmsim", "test_bmn");

    tReco->Branch("EventHeader", &eventHeader);
    tReco->Branch("TOF700", &tof700);
    tReco->Branch("TOF400", &tof400);
    tReco->Branch("BmnDchHit", &dchHits);
    tReco->Branch("DchTracks", &dchTracks);
    tReco->Branch("DchMatchedTracks", &dchMatchedTracksOut);
    tReco->Branch("DchMatchedTracksGemParameters", &dchMatchedTracksGemPar);
    tReco->Branch("Dch1Tracks", &dch1TracksOut);
    tReco->Branch("Dch1TracksGemParameters", &dch1TracksGemPar);

#if 0
    return;
#endif


    const Int_t nEv1 = in1->GetEntries();
    const Int_t nEv2 = in2->GetEntries();
    const Int_t nEv = min(nEv1,nEv2);
    //    const Int_t nEv = 10000;
    //    vector<Int_t> evVector;
    vector < Double_t> dxVector;
    vector < Double_t> dyVector;
    vector < Double_t> dtxVector;
    vector < Double_t> dtyVector;
    vector < Double_t> sxVector;
    vector < Double_t> syVector;
    vector < Double_t> stxVector;
    vector < Double_t> styVector;
    vector < Double_t> sxVectorGem;
    vector < Double_t> syVectorGem;
    vector < Double_t> stxVectorGem;
    vector < Double_t> styVectorGem;
    vector < Double_t> dx1Vector;
    vector < Double_t> dy1Vector;
    vector < Double_t> dtx1Vector;
    vector < Double_t> dty1Vector;
    vector < Double_t> damountVector;
    vector < Double_t> dxVectorDch1;
    vector < Double_t> dyVectorDch1;
    vector < Double_t> dtxVectorDch1;
    vector < Double_t> dtyVectorDch1;
    vector < Double_t> damountVectorDch1;
    vector < Int_t> usedTracksVector;
    vector < Int_t> usedTracksVectorGem;
    vector < Int_t> usedTracksVectorDch1;
    vector <Int_t> combiVector;

    BmnTrackExtrapolator* extrapolator = new BmnTrackExtrapolator();
    CbmStsKFTrackFitter fitter;
    FairTrackParam fairTrackParam = FairTrackParam();
    FairTrackParam outPar;
    FairTrackParam outParDch1;
    Double_t adamount[100][100] = {1000000.};

    fitter.Init();
    Double_t dch_z = DCH_Z;


    Int_t nTracks_matched = 0;
    for (Int_t iEv = 0; iEv < nEv; iEv++) {
//    for (Int_t iEv = 0; iEv < 21000; iEv++) {

        Int_t iEvGem = 0;
        // Choose event to analize
        //        if (iEv != 56)
        //            continue;

        eventHeader->Clear();
        tof700->Clear();
        tof400->Clear();
        dchHits->Clear();
        dchTracks->Clear();
        dchMatchedTracks->Clear();

        in2->GetEntry(iEv);

	if (((BmnEventHeader *)(eventHeader->At(0)))->GetType() != 1) continue;

        iEvGem = getGemEntryId(iEv, eventHeaderGem, eventHeader, in1, in2);
        if (iEvGem == -1)
            continue;

        dchMatchedTracksOut->Clear();
        dchMatchedTracksGemPar->Clear();
        dch1TracksOut->Clear();
        dch1TracksGemPar->Clear();

        if (iEv % 1000 == 0)
            cout << "iEv = " << iEv << endl;
        //        if (tracks->GetEntriesFast() > 1) // TODO: Remove to use all tracks
        //            continue;


	for (Int_t dchpos = 0; dchpos<2; dchpos++)
	{
        usedTracksVector.clear();
        usedTracksVectorGem.clear();
	if (dchpos == 0)
	{
	    Tracks = dchMatchedTracks;
	    TracksOut = dchMatchedTracksOut;
	    TracksGemPar = dchMatchedTracksGemPar;
	    dch_z = DCH_Z;
	}
	else
	{
	    Tracks = dchTracks;
	    TracksOut = dch1TracksOut;
	    TracksGemPar = dch1TracksGemPar;
	    dch_z = DCH1_Z;
	}
	Int_t nGem = tracks->GetEntriesFast();
	Int_t nDch = Tracks->GetEntriesFast();
        for (Int_t iTrGem = 0; iTrGem < nGem; ++iTrGem) {
cout<< "first loop on gem tracks" <<endl;
            CbmStsTrack *track = (CbmStsTrack*) tracks->UncheckedAt(iTrGem);

#ifdef USE_DOFIT
            cout << "Before track->GetParamFirst()->GetQp() = " << track->GetParamFirst()->GetQp() << endl;

            // Fit track
            fitter.DoFit(track, PDG_ID);
            cout << "After track->GetParamFirst()->GetQp() = " << track->GetParamFirst()->GetQp() << endl;
#endif

            fairTrackParam = *(track->GetParamLast());

            //            if (track->GetParamFirst()->GetQp() < 0) // TODO: Take only positivly charged particles
            //                continue;

            outPar = FairTrackParam(fairTrackParam);
//	    extrapolator->RK5Order(&outPar, dch_z);

            //            cout << "outPar.GetQp() = " << outPar.GetQp() << endl;
            // TODO: Scan by DCH tracks and try to find best matched

            //            if (dchMatchedTracks->GetEntriesFast() > 1) // TODO: Remove to use all tracks
            //                break;
            //            if ((tracks->GetEntriesFast() != 1) || (dchMatchedTracks->GetEntriesFast() != 1)) // TODO: Remove to use all tracks
            //                break;

            BmnTrack* trackDch = NULL;

            for (Int_t iTrack = 0; iTrack < nDch; ++iTrack) {

                trackDch = (BmnTrack*) Tracks->At(iTrack);

		if (dchpos == 1 && trackDch->GetParamFirst()->GetZ() > 600.)
		{
		    adamount[iTrGem][iTrack] = 10000000.;
		    continue;
		}

    		Double_t xGem = 0;
    		Double_t yGem = 0;
		if (dchpos == 0)
		{
    		    xGem = fairTrackParam.GetX();
    		    yGem = fairTrackParam.GetY();
		}
		else
		{
    		    xGem = fairTrackParam.GetX() + (DCH1_Z - fairTrackParam.GetZ())*fairTrackParam.GetTx();
    		    yGem = fairTrackParam.GetY() + (DCH1_Z - fairTrackParam.GetZ())*fairTrackParam.GetTy();
		}

                Double_t x = trackDch->GetParamFirst()->GetX() + DXDCH;
                Double_t tx = trackDch->GetParamFirst()->GetTx() + DYDCH;
                Double_t y = trackDch->GetParamFirst()->GetY() + DTXDCH;
                Double_t ty = trackDch->GetParamFirst()->GetTy() + DTYDCH;

                Double_t dx = xGem - x;
                Double_t dtx = outPar.GetTx() - tx;
                Double_t dy = yGem - y;
                Double_t dty = outPar.GetTy() - ty;
                Double_t ddxgem = outPar.GetCovariance(0,0);
                Double_t ddtxgem = outPar.GetCovariance(2,2);
                Double_t ddygem = outPar.GetCovariance(1,1);
                Double_t ddtygem = outPar.GetCovariance(3,3);
                Double_t ddxdch = trackDch->GetParamFirst()->GetCovariance(0,0);
                Double_t ddtxdch = trackDch->GetParamFirst()->GetCovariance(2,2);
                Double_t ddydch = trackDch->GetParamFirst()->GetCovariance(1,1);
                Double_t ddtydch = trackDch->GetParamFirst()->GetCovariance(3,3);

                Double_t damount = dx*dx/(ddxgem+ddxdch) + dy*dy/(ddygem+ddydch) + dtx*dtx/(ddtxgem+ddtxdch) + dty*dty/(ddtygem+ddtydch);

		adamount[iTrGem][iTrack] = damount;
	    }
	}

	while(1)
	{
	  Int_t iTrackMin = -1, iTrGemMin = -1;
	  Double_t dam = 1000000.;
//	  Int_t nGem = tracks->GetEntriesFast();
//	  Int_t nDch = Tracks->GetEntriesFast();
          for (Int_t iTrGem = 0; iTrGem < nGem; ++iTrGem) {
            if (isInVector(usedTracksVectorGem, iTrGem))
                continue;

            for (Int_t iTrack = 0; iTrack < nDch; ++iTrack) {
                if (isInVector(usedTracksVector, iTrack))
                    continue;
		if (adamount[iTrGem][iTrack] < dam)
		{
		    dam = adamount[iTrGem][iTrack];
		    iTrackMin = iTrack;
		    iTrGemMin = iTrGem;

		}
	    }
	  }
	  if (iTrackMin < 0) break;
    	  usedTracksVectorGem.push_back(iTrGemMin);
          usedTracksVector.push_back(iTrackMin);
	  if (dam > DAMOUNT_MIN) break;
	  nTracks_matched++;
//	}
//	if (iTrackMin < 0) continue;
//	if (dam > DAMOUNT_MIN) continue;


        CbmStsTrack *track = (CbmStsTrack*) tracks->UncheckedAt(iTrGemMin);

#ifdef USE_DOFIT
        cout << "Before track->GetParamLast()->GetQp() = " << track->GetParamLast()->GetQp() << endl;

            // Fit track
        fitter.DoFit(track, PDG_ID);
        cout << "After track->GetParamLast()->GetQp() = " << track->GetParamLast()->GetQp() << endl;
#endif

        fairTrackParam = *(track->GetParamLast());

        //            if (track->GetParamFirst()->GetQp() < 0) // TODO: Take only positivly charged particles
        //                continue;

        outPar = FairTrackParam(fairTrackParam);
	//extrapolator->RK5Order(&outPar, dch_z);

        Double_t damount_min = DAMOUNT_MIN;
        BmnTrack* trackDch = NULL;
        Double_t x0 = 0, xGem0 = 0;
        Double_t y0 = 0, yGem0 = 0;
        Double_t z0 = 0, zGem0 = 0;
        Double_t tx0 = 0, txGem0 = 0;
        Double_t ty0 = 0, tyGem0 = 0;
        Double_t qp0 = 0, qpGem0 = 0;
        Float_t chi20 = 0;
        Double_t chi2Gem0 = 0;
        Int_t nhits0 = 0, nhitsGem0 = 0;
        Int_t iTrack0 = 0;

        Double_t chidx0 = 0;
        Double_t chidtx0 = 0;
        Double_t chidy0 = 0;
        Double_t chidty0 = 0;

        Double_t dx0 = 0;
        Double_t dtx0 = 0;
        Double_t dy0 = 0;
        Double_t dty0 = 0;
        Double_t ddxgem0 = 0;
        Double_t ddtxgem0 = 0;
        Double_t ddygem0 = 0;
        Double_t ddtygem0 = 0;
        Double_t ddxdch0 = 0;
        Double_t ddtxdch0 = 0;
        Double_t ddydch0 = 0;
        Double_t ddtydch0 = 0;
        txGem0 = fairTrackParam.GetTx();
        tyGem0 = fairTrackParam.GetTy();
	if (dchpos == 0)
	{
    	    xGem0 = fairTrackParam.GetX();
    	    yGem0 = fairTrackParam.GetY();
    	    zGem0 = fairTrackParam.GetZ();
	}
	else
	{
    	    xGem0 = fairTrackParam.GetX() + (DCH1_Z - fairTrackParam.GetZ())*txGem0;
    	    yGem0 = fairTrackParam.GetY() + (DCH1_Z - fairTrackParam.GetZ())*tyGem0;
    	    zGem0 = DCH1_Z;
	}
        qpGem0 = fairTrackParam.GetQp();
        chi2Gem0 = track->GetChi2();
        nhitsGem0 = track->GetNStsHits();
        qp0 = outPar.GetQp();

        BmnTrack* segDch1 = NULL;
        BmnTrack* segDch2 = NULL;
        BmnTrack* segDch = NULL;

        trackDch = (BmnTrack*) Tracks->At(iTrackMin);

        Double_t x = trackDch->GetParamFirst()->GetX() + DXDCH;
        Double_t tx = trackDch->GetParamFirst()->GetTx() + DYDCH;
        Double_t y = trackDch->GetParamFirst()->GetY() + DTXDCH;
        Double_t ty = trackDch->GetParamFirst()->GetTy() + DTYDCH;

        Double_t dx = xGem0 - x;
        Double_t dtx = outPar.GetTx() - tx;
        Double_t dy = yGem0 - y;
        Double_t dty = outPar.GetTy() - ty;
        Double_t ddxgem = outPar.GetCovariance(0,0);
        Double_t ddtxgem = outPar.GetCovariance(2,2);
        Double_t ddygem = outPar.GetCovariance(1,1);
        Double_t ddtygem = outPar.GetCovariance(3,3);
        Double_t ddxdch = trackDch->GetParamFirst()->GetCovariance(0,0);
        Double_t ddtxdch = trackDch->GetParamFirst()->GetCovariance(2,2);
        Double_t ddydch = trackDch->GetParamFirst()->GetCovariance(1,1);
        Double_t ddtydch = trackDch->GetParamFirst()->GetCovariance(3,3);

        Double_t cdx = dx*dx;
        Double_t cdtx = dtx*dtx;
        Double_t cdy = dy*dy;
        Double_t cdty = dty*dty;

        Double_t chidx = dx/sqrt(ddxgem+ddxdch);
        Double_t chidtx = dtx/sqrt(ddtxgem+ddtxdch);
        Double_t chidy = dy/sqrt(ddygem+ddydch);
        Double_t chidty = dty/sqrt(ddtygem+ddtydch);

        Double_t damount = dx*dx/(ddxgem+ddxdch) + dy*dy/(ddygem+ddydch) + dtx*dtx/(ddtxgem+ddtxdch) + dty*dty/(ddtygem+ddtydch);

        damount_min = damount;

        x0 = trackDch->GetParamFirst()->GetX() + DXDCH;
        y0 = trackDch->GetParamFirst()->GetY() + DYDCH;
        z0 = trackDch->GetParamFirst()->GetZ();
        tx0 = (trackDch->GetParamFirst()->GetTx() + DTXDCH);// / (1 - DTXDCH * trackDch->GetParamFirst()->GetTx());
        ty0 = (trackDch->GetParamFirst()->GetTy() + DTYDCH);// / (1 - DTYDCH * trackDch->GetParamFirst()->GetTy());
        chi20 = trackDch->GetChi2();
        nhits0 = trackDch->GetNHits();

	chidx0 = chidx;
	chidy0 = chidy;
	chidtx0 = chidtx;
	chidty0 = chidty;

	dx0 = dx;
	dy0 = dy;
	dtx0 = dtx;
	dty0 = dty;

        ddxgem0 = ddxgem;
        ddtxgem0 = ddtxgem;
        ddygem0 = ddygem;
        ddtygem0 = ddtygem;
        ddxdch0 = ddxdch;
        ddtxdch0 = ddxdch;
        ddydch0 = ddydch;
        ddtydch0 = ddtydch;
        iTrack0 = iTrackMin;

//        if (damount_min < DAMOUNT_MIN) {
                // TODO: Fill root tree
                // Add momentum
        FairTrackParam ParamsTrackDch1;
//        cout << " dchpos " << dchpos << " Z DCH " << z0 << endl;
        ParamsTrackDch1.SetPosition(TVector3(x0, y0, z0));
        ParamsTrackDch1.SetTx(tx0);
        ParamsTrackDch1.SetTy(ty0);
        ParamsTrackDch1.SetQp(qp0);
        BmnTrack* tr_dch1 = new((*TracksOut)[TracksOut->GetEntriesFast()]) BmnTrack();
        tr_dch1->SetChi2(chi20);
        tr_dch1->SetNHits(nhits0);
        tr_dch1->SetParamFirst(ParamsTrackDch1);

        // Calculate track length
        FairTrackParam parForExtrap = FairTrackParam(ParamsTrackDch1);
        tr_dch1->SetLength(calculateTrackLength(&parForExtrap, extrapolator, dch_z));

//        cout << " dchpos " << dchpos << " Z GEM " << zGem0 << endl;
        ParamsTrackDch1.SetPosition(TVector3(xGem0, yGem0, zGem0));
        ParamsTrackDch1.SetTx(txGem0);
        ParamsTrackDch1.SetTy(tyGem0);
        ParamsTrackDch1.SetQp(qpGem0);
        BmnTrack* tr_dch2 = new((*TracksGemPar)[TracksGemPar->GetEntriesFast()]) BmnTrack();
        tr_dch2->SetChi2(chi2Gem0);
        tr_dch2->SetNHits(nhitsGem0);
        tr_dch2->SetParamFirst(ParamsTrackDch1);

        // Calculate track length
        parForExtrap = FairTrackParam(ParamsTrackDch1);
        tr_dch2->SetLength(calculateTrackLength(&parForExtrap, extrapolator, dch_z));

	if (dchpos == 0)
	{
	    dxVector.push_back(chidx0);
	    dyVector.push_back(chidy0);
	    dtxVector.push_back(chidtx0);
	    dtyVector.push_back(chidty0);
	    dx1Vector.push_back(dx0);
            dy1Vector.push_back(dy0);
            dtx1Vector.push_back(dtx0);
            dty1Vector.push_back(dty0);
            sxVector.push_back(sqrt(ddxdch0));
	    syVector.push_back(sqrt(ddydch0));
            stxVector.push_back(sqrt(ddtxdch0));
            styVector.push_back(sqrt(ddtydch0));
            sxVectorGem.push_back(sqrt(ddxgem0));
            syVectorGem.push_back(sqrt(ddygem0));
            stxVectorGem.push_back(sqrt(ddtxgem0));
            styVectorGem.push_back(sqrt(ddtygem0));
            damountVector.push_back(damount_min);
	}
	else
	{
            dxVectorDch1.push_back(chidx0);
            dyVectorDch1.push_back(chidy0);
            dtxVectorDch1.push_back(chidtx0);
            dtyVectorDch1.push_back(chidty0);
            damountVectorDch1.push_back(damount_min);
	}

//	} // if (damount_min ....

	} // while(1)

        usedTracksVector.clear();
        usedTracksVectorGem.clear();

	} // dchpos

        if ((tracks->GetEntriesFast() > 0) && (dchMatchedTracks->GetEntriesFast() > 0))
            if (dchMatchedTracks->GetEntriesFast() > tracks->GetEntriesFast())
                combiVector.push_back(-1 * dchMatchedTracks->GetEntriesFast() * tracks->GetEntriesFast());
            else
                combiVector.push_back(dchMatchedTracks->GetEntriesFast() * tracks->GetEntriesFast());

        tReco->Fill();
    } // events

    cout << " N tracks matched " << nTracks_matched << endl;

    delete extrapolator;

    fReco->cd();
    tReco->Write();
    fReco->Close();

    cout << "numOfNotFound = " << numOfNotFound << endl;
    cout << "numOfUnexpected = " << numOfUnexpected << endl;

    TString outHist = Form("bmn_reco_DCH_GEM_run%d_hist.root",runId);
    TFile* fHist = new TFile(outHist, "RECREATE"); //the root file where the hists
#if 1
    // TODO: Prepare output tree
    //    TFile* outFile1 = new TFile("PLsFcn.root", "recreate");
    //    TTree* outTree = new TTree("cbmsim", "cbmsim");
    //    TClonesArray* outClass = new TClonesArray("BmnTrackWithLorenzShift");
    //    outTree->Branch("BmnTrackWithLorenzShift", &outClass);

    // Build P histo and tree
    const Int_t nCTracks = dxVector.size();
    TString str1 = Form("Chi2x");
    TString str2 = Form("Chi2y");
    TString str3 = Form("Chi2tx");
    TString str4 = Form("Chi2ty");
    TString str5 = Form("combi");
    TString str6 = Form("Chi2");
    TString str7= Form("dx");
    TString str8= Form("dy");
    TString str9 = Form("dtx");
    TString str0 = Form("dty");
    TString str11 = Form("SigmaX");
    TString str12 = Form("SigmaY");
    TString str13 = Form("SigmaTx");
    TString str14 = Form("SigmaTy");
    TString str15 = Form("SigmaX_Gem");
    TString str16 = Form("SigmaY_Gem");
    TString str17= Form("SigmaTx_Gem");
    TString str18= Form("SigmaTy_Gem");
    TH1D *xHits = new TH1D(str1, str1, 100, -20, 20);
    TH1D *yHits = new TH1D(str2, str2, 100, -20, 20);
    TH1D *txHits = new TH1D(str3, str3, 100, -20, 20);
    TH1D *tyHits = new TH1D(str4, str4, 100, -20, 20);
    TH1D *combi = new TH1D(str5, str5, 100, -50, 50);
    TH1D *damountHist = new TH1D(str6, str6, 1000, 0, 200.);
    TH1D *xHits1 = new TH1D(str7, str7, 100, -50, 50);
    TH1D *yHits1 = new TH1D(str8, str8, 100, -50, 50);
    TH1D *txHits1 = new TH1D(str9, str9, 100, -1, 1);
    TH1D *tyHits1 = new TH1D(str0, str0, 100, -1, 1);

    TH1D *sxHits = new TH1D(str11, str11, 100, -50, 50);
    TH1D *syHits = new TH1D(str12, str12, 100, -50, 50);
    TH1D *stxHits = new TH1D(str13, str13, 100, -1, 1);
    TH1D *styHits = new TH1D(str14, str14, 100, -1, 1);
    TH1D *sxHitsGem = new TH1D(str15, str15, 100, -50, 50);
    TH1D *syHitsGem = new TH1D(str16, str16, 100, -50, 50);
    TH1D *stxHitsGem = new TH1D(str17, str17, 100, -1, 1);
    TH1D *styHitsGem = new TH1D(str18, str18, 100, -1, 1);
//    xHits->GetXaxis()->SetTitle("");
//    yHits->GetXaxis()->SetTitle("cm");
//    damountHist->GetXaxis()->SetTitle("cm^2");
    for (int i = 0; i < nCTracks; i++) {
        xHits->Fill(dxVector[i]);
        yHits->Fill(dyVector[i]);
        txHits->Fill(dtxVector[i]);
        tyHits->Fill(dtyVector[i]);
        xHits1->Fill(dx1Vector[i]);
        yHits1->Fill(dy1Vector[i]);
        txHits1->Fill(dtx1Vector[i]);
        tyHits1->Fill(dty1Vector[i]);
        damountHist->Fill(damountVector[i]);
        sxHits->Fill(sxVector[i]);
        syHits->Fill(syVector[i]);
        stxHits->Fill(stxVector[i]);
        styHits->Fill(styVector[i]);
        sxHitsGem->Fill(sxVectorGem[i]);
        syHitsGem->Fill(syVectorGem[i]);
        stxHitsGem->Fill(stxVectorGem[i]);
        styHitsGem->Fill(styVectorGem[i]);
        //            new ((*outClass)[outClass->GetEntriesFast()])BmnTrackWithLorenzShift(1 / Qp[i], Ls[i], Fcn[i]);
    }
    for (int i = 0; i < (int)combiVector.size(); i++)
        combi->Fill(combiVector[i]);

    // Draw histo
    TCanvas *c1 = new TCanvas("c1", "c1", 800, 1000);
    c1->Divide(2, 5);
    c1->cd(1);
    xHits->Draw();
    c1->cd(2);
    yHits->Draw();
    c1->cd(3);
    txHits->Draw();
    c1->cd(4);
    tyHits->Draw();
    c1->cd(5);
    combi->Draw();
    c1->cd(6);
    damountHist->Draw();
    c1->cd(7);
    xHits1->Draw();
    c1->cd(8);
    yHits1->Draw();
    c1->cd(9);
    txHits1->Draw();
    c1->cd(10);
    tyHits1->Draw();

    TCanvas *c2 = new TCanvas("c2", "c2", 800, 1000);
    c2->Divide(2, 4);
    c2->cd(1);
    sxHits->Draw();
    c2->cd(2);
    syHits->Draw();
    c2->cd(3);
    stxHits->Draw();
    c2->cd(4);
    styHits->Draw();
    c2->cd(5);
    sxHitsGem->Draw();
    c2->cd(6);
    syHitsGem->Draw();
    c2->cd(7);
    stxHitsGem->Draw();
    c2->cd(8);
    styHitsGem->Draw();

    // Write to root-file
    //    outTree->Fill();
    //    outTree->Write();
    //    outFile1->Close();
#endif

#if 1
    // Build P histo and tree
    const Int_t nCTracksDch1 = dxVectorDch1.size();
    TCanvas *c2Dch1 = new TCanvas("c3", "c3", 800, 1000);
    c2Dch1->Divide(2, 3);
    TString str1Dch1 = Form("chi2x_dch1");
    TString str2Dch1 = Form("chi2y_dch1");
    TString str3Dch1 = Form("chi2tx_dch1");
    TString str4Dch1 = Form("chi2ty_dch1");
    TString str6Dch1 = Form("chi2_dch1");
    TH1D *xHitsDch1 = new TH1D(str1Dch1, str1Dch1, 100, -20, 20);
    TH1D *yHitsDch1 = new TH1D(str2Dch1, str2Dch1, 100, -20, 20);
    TH1D *txHitsDch1 = new TH1D(str3Dch1, str3Dch1, 100, -20, 20);
    TH1D *tyHitsDch1 = new TH1D(str4Dch1, str4Dch1, 100, -20, 20);
    TH1D *damountHistDch1 = new TH1D(str6Dch1, str6Dch1, 100, 0, 100.);
    xHitsDch1->GetXaxis()->SetTitle("cm");
    yHitsDch1->GetXaxis()->SetTitle("cm");
    damountHistDch1->GetXaxis()->SetTitle("cm^2");
    for (int i = 0; i < nCTracksDch1; i++) {
        xHitsDch1->Fill(dxVectorDch1[i]);
        yHitsDch1->Fill(dyVectorDch1[i]);
        txHitsDch1->Fill(dtxVectorDch1[i]);
        tyHitsDch1->Fill(dtyVectorDch1[i]);
        damountHistDch1->Fill(damountVectorDch1[i]);
    }

    // Draw histo
    c2Dch1->cd(1);
    xHitsDch1->Draw();
    c2Dch1->cd(2);
    yHitsDch1->Draw();
    c2Dch1->cd(3);
    txHitsDch1->Draw();
    c2Dch1->cd(4);
    tyHitsDch1->Draw();
    c2Dch1->cd(5);
    damountHistDch1->Draw();
#endif
    fHist->Write();
}

Bool_t GetBmnGeom(const char* fileName) {
    // ---> Get TGeoManager and top node (TOP)
    TFile* geoFile = new TFile(fileName, "READ");
    if (!geoFile->IsOpen()) {
        cout << "-E- Could not open file!" << endl;
        return kFALSE;
    }

    // ---> Get TOP node from file
    TList* keyList = geoFile->GetListOfKeys();
    TIter next(keyList);
    TKey* key = NULL;
    TGeoVolume* top = NULL;
    while ((key = (TKey*) next())) {
        TString className(key->GetClassName());
        if (className.BeginsWith("TGeoVolume")) {
            top = dynamic_cast<TGeoVolume*> (key->ReadObj());
            std::cout << "Found volume " << top->GetName() << endl;
            break;
        }
    }
    if (!top) {
        cout << "-E- Could not find volume object in file" << endl;
        return kFALSE;
    }
    gGeoManager->GetListOfVolumes()->ls();
    gGeoManager->SetTopVolume(top);
    return kTRUE;
}

void ApplyAlignment() {
    // Apply alignment
    TGeoNode* st_node = NULL;
    // Get STS node
    TGeoNode* sts = NULL;
    gGeoManager->CdTop();
    TGeoNode* cave = gGeoManager->GetCurrentNode();
    for (Int_t iNode = 0; iNode < cave->GetNdaughters(); iNode++) {
        TGeoNode* node = cave->GetDaughter(iNode);
        TString name = node->GetName();
        if (name.Contains("GEMS")) {
            sts = node;
            gGeoManager->CdDown(iNode);
            break;
        }
    }
    if (!sts) {
        cout << "-E- CbmBmnStsDigitize::ApplyAlignment: Cannot find top GEM node"
                << endl;
        //return kFALSE;
    }

    // Loop over stations in STS
    Int_t statNr = 0;
    Int_t nStat0 = sts->GetNdaughters();
    Double_t xAlign[15] = {0, 0, -0.2, 0, 27.94, -34.38, 41.26, -40.44, 41.59, -40.12};
    //0.24, 0.32,     0.32, 0.50,      0.83, 0.64};//-1.5, -1.70, -1.5,   27.94,-34.38,      -40.44,41.26,      41.59,-40.12};
    Double_t yAlign[15] = {0, 0, 0.20, 0, 0.19, -0.625, 0.31, 0.13, 0.276, 0.369};
    //0.19, -0.625,     0.13, 0.31,    0.267, 0.369 };//0,    0.20,   0,    0.19,-0.425,      -2.27,-2.09,      -2.133,-2.031};
    Double_t zAlign2[15] = {0, 0.9, 1.6, 0.9, 1.9, 3.9, 2.2, 1.7, 2.7, 1.7};

    Double_t zAlign[15] = {0, 31.8, 63.8, 95.8, 127.8, 159.8, 191.8};

    //for (Int_t iNode = 1; iNode < sts->GetNdaughters(); iNode++) {
    for (Int_t iNode = 1; iNode < nStat0; iNode++) {
        // Go to station node
        gGeoManager->CdDown(iNode);
        TGeoNode* stationNode = gGeoManager->GetCurrentNode();
        TString statName = stationNode->GetName();
        if (!statName.Contains("station")) {
            gGeoManager->CdUp();
            continue;
        }
        ++statNr; //AZ

        TGeoHMatrix *matr = gGeoManager->GetCurrentMatrix();
        Double_t* statTrans = matr->GetTranslation();

        statTrans[2] = zAlign[statNr];
        matr->SetTranslation(statTrans);
        TGeoHMatrix *matr0 = new TGeoHMatrix(*matr);
        matr0->RegisterYourself();

        //  Int_t nModules = stationNode->GetNdaughters();
        Int_t nModules = GetNofModules(stationNode);

        sts->GetVolume()->AddNode((TGeoVolumeAssembly*) stationNode->GetVolume(), 0, matr0); //AZ
        st_node = sts->GetVolume()->GetNode(iNode + 6);
        Double_t statZ = statTrans[2];

        //-----------------------module translate-------------
        Int_t moduleNr = 0;
        Int_t copy_no = 0;
        // ---> Large sensors
        for (Int_t iStatD = 0; iStatD < nModules; iStatD++) {
            gGeoManager->CdDown(iStatD);
            TGeoNode* module = gGeoManager->GetCurrentNode();
            if (!TString(module->GetName()).Contains("module")) {
                gGeoManager->CdUp();
                continue;
            }

            if (TString(module->GetName()).Contains("Senso")) {
                if (iNode <= 3) moduleNr = 0;
                else {
                    if (iStatD == 0) moduleNr = 0;
                    if (iStatD > 0) moduleNr = 1;
                }

                Int_t ipNr = 0;
                if (statNr <= 3) ipNr = statNr;
                else ipNr = statNr * 2 - 4 + moduleNr;

                TGeoHMatrix *matrMod = gGeoManager->GetCurrentMatrix();
                Double_t* modTrans = matrMod->GetTranslation();
                modTrans[0] = xAlign[ipNr];
                modTrans[1] = yAlign[ipNr];
                modTrans[2] = zAlign2[ipNr];
                matrMod->SetTranslation(modTrans);
                TGeoHMatrix *matr0Mod = new TGeoHMatrix(*matrMod);
                matr0Mod->RegisterYourself();

                Double_t* sensTrans = matrMod->GetTranslation();
                st_node->GetVolume()->AddNode((TGeoVolumeAssembly*) module->GetVolume(), copy_no, matr0Mod);
                copy_no++;
            }
            gGeoManager->CdUp(); // back to module
        }
        //----------------------end module translate----------
        gGeoManager->CdUp(); // to sts
    } // station loop

    for (Int_t iNode = 1; iNode < nStat0; iNode++) {
        // Go to station node
        gGeoManager->CdDown(1);
        TGeoNode* stationNode = gGeoManager->GetCurrentNode();
        sts->GetVolume()->RemoveNode(stationNode); //AZ
        gGeoManager->CdUp(); // to sts
    }

    for (Int_t iNode = 1; iNode < nStat0; iNode++) {
        // Go to station node
        gGeoManager->CdDown(iNode);
        TGeoNode* stationNode = gGeoManager->GetCurrentNode();
        TString statName = stationNode->GetName();
        if (!statName.Contains("station")) {
            gGeoManager->CdUp();
            continue;
        }

        Int_t nMod = 1;
        if (iNode > 3) nMod = 2;
        for (Int_t iStatD = 0; iStatD < nMod; iStatD++) {
            gGeoManager->CdDown(0);
            TGeoNode* module = gGeoManager->GetCurrentNode();
            stationNode->GetVolume()->RemoveNode(module); //AZ
            gGeoManager->CdUp(); // to sts
        }
        gGeoManager->CdUp();
    }

    for (Int_t iNode = 1; iNode < sts->GetNdaughters(); iNode++) {
        // Go to station node
        gGeoManager->CdDown(iNode);
        TGeoNode* stationNode = gGeoManager->GetCurrentNode();
        for (Int_t iModule = 0; iModule < stationNode->GetNdaughters(); iModule++) {
            gGeoManager->CdDown(iModule);
            TGeoNode* moduleNode = gGeoManager->GetCurrentNode();
            Double_t* sensTrans = gGeoManager->GetCurrentMatrix()->GetTranslation();
            gGeoManager->CdUp();
        }
        gGeoManager->CdUp(); // to sts
    }
}

Int_t GetNofModules(TGeoNode* station) {
    Int_t nModules = 0; //station->GetNdaughters();
    // --- Modules
    for (Int_t iModule = 0; iModule < station->GetNdaughters(); iModule++) {
        TGeoNode* module = station->GetDaughter(iModule);
        if (TString(module->GetName()).Contains("module")) nModules++;
    }
    return nModules;
}
