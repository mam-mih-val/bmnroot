#include "BmnDigiMergeTask.h"

// RUN-6

BmnDigiMergeTask::BmnDigiMergeTask(TString in1, TString in2, TString in3) :
isRun6(kTRUE),
isRun7(kFALSE),
isSrc(kFALSE),
fOutFile(nullptr),
fOutTree(nullptr),
fIn1(nullptr),
fIn2(nullptr),
fIn3(nullptr),
fHeaderOut(nullptr),
fGem(nullptr),
fSil(nullptr),
fZdc(nullptr),
fT0(nullptr),
fBC1(nullptr),
fBC2(nullptr),
fVeto(nullptr),
fFd(nullptr),
fBd(nullptr),
fMwpc(nullptr),
fDch(nullptr),
fTof400(nullptr),
fTof700(nullptr),
fEcal(nullptr),
fHeader1(nullptr),
fHeader2(nullptr),
fHeader3(nullptr),
fGemDigits(nullptr),
fSilDigits(nullptr),
fZdcDigits(nullptr),
fT0Digits(nullptr),
fBC1Digits(nullptr),
fBC2Digits(nullptr),
fVetoDigits(nullptr),
fFdDigits(nullptr),
fBdDigits(nullptr),
fMwpcDigits(nullptr),
fDchDigits(nullptr),
fTof400Digits(nullptr),
fTof700Digits(nullptr),
fEcalDigits(nullptr),
fCont(nullptr) {
    fIn1 = new TChain("BMN_DIGIT");
    fIn2 = new TChain("cbmsim");
    fIn3 = new TChain("cbmsim");

    // Event headers
    TString brName = "EventHeader";

    // Detector branches
    if (!in1.IsNull()) {
        fIn1->Add(in1.Data());
        fIn1->SetBranchAddress("STRIPGEM", &fGemDigits);
        fIn1->SetBranchAddress("MYSILICON", &fSilDigits);
        fIn1->SetBranchAddress(brName.Data(), &fHeader1);
    }

    if (!in2.IsNull()) {
        fIn2->Add(in2.Data());
        fIn2->SetBranchAddress("ZDC", &fZdcDigits);
        fIn2->SetBranchAddress(brName.Data(), &fHeader2);
    }

    if (!in3.IsNull()) {
        fIn3->Add(in3.Data());
        fIn3->SetBranchAddress("T0", &fT0Digits);
        fIn3->SetBranchAddress("BC1", &fBC1Digits);
        fIn3->SetBranchAddress("BC2", &fBC2Digits);
        fIn3->SetBranchAddress("VETO", &fVetoDigits);
        fIn3->SetBranchAddress("FD", &fFdDigits);
        fIn3->SetBranchAddress("BD", &fBdDigits);
        fIn3->SetBranchAddress("MWPC", &fMwpcDigits);
        fIn3->SetBranchAddress("DCH", &fDchDigits);
        fIn3->SetBranchAddress("TOF400", &fTof400Digits);
        fIn3->SetBranchAddress("TOF700", &fTof700Digits);
        fIn3->SetBranchAddress("ECAL", &fEcalDigits);
        fIn3->SetBranchAddress(brName.Data(), &fHeader3);
    }
    CreateOutputFiles();
    CreateGeometries();
}

// RUN-7

BmnDigiMergeTask::BmnDigiMergeTask(TString* in) :
isRun6(kFALSE),
isRun7(kTRUE),
isSrc(kFALSE),
fIn1(nullptr),
fIn2(nullptr),
fIn3(nullptr),
fHeader1(nullptr),
fHeader2(nullptr),
fHeader3Obj(nullptr),
fGemDigits(nullptr),
fSilDigits(nullptr),
fBC1Digits(nullptr),
fBC2Digits(nullptr),
fBC3Digits(nullptr),
fVetoDigits(nullptr),
fSiDigits(nullptr),
fBdDigits(nullptr),
fMwpcDigits(nullptr),
fDchDigits(nullptr),
fTof400Digits(nullptr),
fTof700Digits(nullptr),
fEcalDigits(nullptr),
fZdcDigits(nullptr),
fOutFile(nullptr),
fOutTree(nullptr),
fOutFiles(nullptr),
fOutTrees(nullptr),
fGem(nullptr),
fCsc(nullptr),
fSil(nullptr),
fZdc(nullptr),
fT0(nullptr),
fBC1(nullptr),
fBC2(nullptr),
fBC3(nullptr),
fSi(nullptr),
fVeto(nullptr),
fFd(nullptr),
fBd(nullptr),
fMwpc(nullptr),
fDch(nullptr),
fTof400(nullptr),
fTof700(nullptr),
fEcal(nullptr),
fHeaderOut(nullptr) {
    fIn1 = new TChain("BMN_DIGIT");
    fIn2 = new TChain("cbmsim");
    fIn3 = new TChain("bmndata");

    TString brName = "EventHeader";

    // Detector branches
    if (!in[0].IsNull()) {
        fIn1->Add(in[0].Data());
        fIn1->SetBranchAddress("STRIPGEM", &fGemDigits);
        fIn1->SetBranchAddress("MYSILICON", &fSilDigits);
        fIn1->SetBranchAddress("MWPC", &fMwpcDigits);
        fIn1->SetBranchAddress("DCH", &fDchDigits);
        fIn1->SetBranchAddress("TOF400", &fTof400Digits);
        fIn1->SetBranchAddress("ECAL", &fEcalDigits);
        fIn1->SetBranchAddress(brName.Data(), &fHeader1);
    }

    if (!in[1].IsNull()) {
        fIn2->Add(in[1].Data());
        fIn2->SetBranchAddress("BC1", &fBC1Digits);
        fIn2->SetBranchAddress("BC2", &fBC2Digits);
        fIn2->SetBranchAddress("BC3", &fBC3Digits);
        fIn2->SetBranchAddress("VC", &fVetoDigits);
        fIn2->SetBranchAddress("Si", &fSiDigits);
        fIn2->SetBranchAddress("BD", &fBdDigits);
        fIn2->SetBranchAddress("ZDC", &fZdcDigits);
        fIn2->SetBranchAddress(brName.Data(), &fHeader2);
    }

    if (!in[2].IsNull()) {
        fIn3->Add(in[2].Data());
        fIn3->SetBranchAddress("TOF700", &fTof700Digits);
        fIn3->SetBranchAddress(brName.Data(), &fHeader3Obj);
    }

    CreateOutputFiles();
    CreateGeometries();
}

BmnDigiMergeTask::BmnDigiMergeTask(vector<TString> files) :
isRun6(kFALSE),
isRun7(kTRUE),
isSrc(kTRUE),
fIn1(nullptr),
fIn2(nullptr),
fIn3(nullptr),

// in1
fGemDigits(nullptr),
fSilDigits(nullptr),

// in2
fBC1Digits(nullptr),
fBC2Digits(nullptr),
fBC3Digits(nullptr),
fBC4Digits(nullptr),
fBC1TQDCDigits(nullptr),
fBC2TQDCDigits(nullptr),
fBC3TQDCDigits(nullptr),
fBC4TQDCDigits(nullptr),
fX1LDigits(nullptr),
fX2LDigits(nullptr),
fY1LDigits(nullptr),
fY2LDigits(nullptr),
fX1RDigits(nullptr),
fX2RDigits(nullptr),
fY1RDigits(nullptr),
fY2RDigits(nullptr),
fX1LTQDCDigits(nullptr),
fX2LTQDCDigits(nullptr),
fY1LTQDCDigits(nullptr),
fY2LTQDCDigits(nullptr),
fX1RTQDCDigits(nullptr),
fX2RTQDCDigits(nullptr),
fY1RTQDCDigits(nullptr),
fY2RTQDCDigits(nullptr),
fVetoDigits(nullptr),
fVetoTQDCDigits(nullptr),
fTof700Digits(nullptr),
fZdcDigits(nullptr),

// in3
fMwpcDigits(nullptr),
fDchDigits(nullptr),
fTof400Digits(nullptr),
fEcalDigits(nullptr),

fOutFile(nullptr),
fOutTree(nullptr),
fOutFiles(nullptr),
fOutTrees(nullptr),

// out file content
fGem(nullptr),
fCsc(nullptr),
fSil(nullptr),
fZdc(nullptr),
fMwpc(nullptr),
fDch(nullptr),
fTof400(nullptr),
fTof700(nullptr),
fEcal(nullptr),

fBC1(nullptr),
fBC2(nullptr),
fBC3(nullptr),
fBC4(nullptr),
fBC1TQDC(nullptr),
fBC2TQDC(nullptr),
fBC3TQDC(nullptr),
fBC4TQDC(nullptr),
fVeto(nullptr),
fVetoTQDC(nullptr),
fX1L(nullptr),
fX2L(nullptr),
fY1L(nullptr),
fY2L(nullptr),
fX1R(nullptr),
fX2R(nullptr),
fY1R(nullptr),
fY2R(nullptr),
fX1LTQDC(nullptr),
fX2LTQDC(nullptr),
fY1LTQDC(nullptr),
fY2LTQDC(nullptr),
fX1RTQDC(nullptr),
fX2RTQDC(nullptr),
fY1RTQDC(nullptr),
fY2RTQDC(nullptr),
fHeader1(nullptr),
fHeader2Obj(nullptr),
fHeader3(nullptr),
fHeaderOut(nullptr) {
    fIn1 = new TChain("BMN_DIGIT");
    fIn2 = new TChain("bmndata");
    fIn3 = new TChain("bmndata");

    if (!files[0].IsNull()) {
        fIn1->Add(files[0].Data());
        fIn1->SetBranchAddress("EventHeader", &fHeader1);
        fIn1->SetBranchAddress("STRIPGEM", &fGemDigits);
        fIn1->SetBranchAddress("MYSILICON", &fSilDigits);
    }

    if (!files[1].IsNull()) {
        fIn2->Add(files[1].Data());
        fIn2->SetBranchAddress("BmnEventHeader.", &fHeader2Obj);
        fIn2->SetBranchAddress("BC1", &fBC1Digits);
        fIn2->SetBranchAddress("BC2", &fBC2Digits);
        fIn2->SetBranchAddress("BC3", &fBC3Digits);
        fIn2->SetBranchAddress("BC4", &fBC4Digits);
        fIn2->SetBranchAddress("TQDC_BC1", &fBC1TQDCDigits);
        fIn2->SetBranchAddress("TQDC_BC2", &fBC2TQDCDigits);
        fIn2->SetBranchAddress("TQDC_BC3", &fBC3TQDCDigits);
        fIn2->SetBranchAddress("TQDC_BC4", &fBC4TQDCDigits);

        fIn2->SetBranchAddress("X1_Left", &fX1LDigits);
        fIn2->SetBranchAddress("X2_Left", &fX2LDigits);
        fIn2->SetBranchAddress("Y1_Left", &fY1LDigits);
        fIn2->SetBranchAddress("Y2_Left", &fY2LDigits);
        fIn2->SetBranchAddress("X1_Right", &fX1RDigits);
        fIn2->SetBranchAddress("X2_Right", &fX2RDigits);
        fIn2->SetBranchAddress("Y1_Right", &fY1RDigits);
        fIn2->SetBranchAddress("Y2_Right", &fY2RDigits);
        fIn2->SetBranchAddress("TQDC_X1_Left", &fX1LTQDCDigits);
        fIn2->SetBranchAddress("TQDC_X2_Left", &fX2LTQDCDigits);
        fIn2->SetBranchAddress("TQDC_Y1_Left", &fY1LTQDCDigits);
        fIn2->SetBranchAddress("TQDC_Y2_Left", &fY2LTQDCDigits);
        fIn2->SetBranchAddress("TQDC_X1_Right", &fX1RTQDCDigits);
        fIn2->SetBranchAddress("TQDC_X2_Right", &fX2RTQDCDigits);
        fIn2->SetBranchAddress("TQDC_Y1_Right", &fY1RTQDCDigits);
        fIn2->SetBranchAddress("TQDC_Y2_Right", &fY2RTQDCDigits);

        fIn2->SetBranchAddress("VC", &fVetoDigits);
        fIn2->SetBranchAddress("TQDC_VC", &fVetoTQDCDigits);

        fIn2->SetBranchAddress("ZDC", &fZdcDigits);
        fIn2->SetBranchAddress("TOF700", &fTof700Digits);
    }

    if (!files[2].IsNull()) {
        fIn3->Add(files[2].Data());
        fIn3->SetBranchAddress("EventHeader", &fHeader3);
        fIn3->SetBranchAddress("MWPC", &fMwpcDigits);
        fIn3->SetBranchAddress("DCH", &fDchDigits);
        fIn3->SetBranchAddress("ECAL", &fEcalDigits);
        fIn3->SetBranchAddress("TOF400", &fTof400Digits);
    }

    CreateOutputFiles();
    CreateGeometries();
}

BmnDigiMergeTask::BmnDigiMergeTask(vector<TString> files, TString out) :
isRun6(kFALSE),
isRun7(kFALSE),
isSrc(kFALSE),
fOutFile(nullptr),
fOutTree(nullptr),
fOutFiles(nullptr),
fOutTrees(nullptr),
fInFiles(nullptr),
fBC1Digits(nullptr),
fBC2Digits(nullptr),
fBC3Digits(nullptr),
fVetoDigits(nullptr),
fSiDigits(nullptr),
fBdDigits(nullptr),
fZdcDigits(nullptr),
fGemDigits(nullptr),
fCscDigits(nullptr),
fSilDigits(nullptr),
fMwpcDigits(nullptr),
fDchDigits(nullptr),
fTof400Digits(nullptr),
fTof700Digits(nullptr),
fEcalDigits(nullptr),
fHeaderOut(nullptr),
fHeaders(nullptr),
fGem(nullptr),
fCsc(nullptr),
fSil(nullptr),
fZdc(nullptr),
fT0(nullptr),
fBC1(nullptr),
fBC2(nullptr),
fBC3(nullptr),
fSi(nullptr),
fVeto(nullptr),
fFd(nullptr),
fBd(nullptr),
fMwpc(nullptr),
fDch(nullptr),
fTof400(nullptr),
fTof700(nullptr),
fEcal(nullptr),
evId_evNum(nullptr) {
    nDets = files.size();

    isRun6 = (nDets == 14) ? kTRUE : kFALSE;
    isRun7 = (nDets == 15) ? kTRUE : kFALSE;
    isSrc = (nDets == 35) ? kTRUE : kFALSE;

    if (!isRun6 && !isRun7 && !isSrc)
        throw;

    fBC1Digits = new TClonesArray("BmnTrigDigit");
    fBC2Digits = new TClonesArray("BmnTrigDigit");
    fVetoDigits = new TClonesArray("BmnTrigDigit");
    fBdDigits = new TClonesArray("BmnTrigDigit");
    fZdcDigits = new TClonesArray("BmnZDCDigit");
    fGemDigits = new TClonesArray("BmnGemStripDigit");
    fSilDigits = new TClonesArray("BmnSiliconDigit");
    fMwpcDigits = new TClonesArray("BmnMwpcDigit");
    fDchDigits = new TClonesArray("BmnDchDigit");
    fTof400Digits = new TClonesArray("BmnTof1Digit");
    fTof700Digits = new TClonesArray("BmnTof2Digit");
    fEcalDigits = new TClonesArray("BmnECALDigit");

    if (isRun6 && !isSrc) {
        fT0Digits = new TClonesArray("BmnTrigDigit");
        fFdDigits = new TClonesArray("BmnTrigDigit");
    }

    if (isRun7 && !isSrc) {
        fBC3Digits = new TClonesArray("BmnTrigDigit");
        fSiDigits = new TClonesArray("BmnTrigDigit");
        fCscDigits = new TClonesArray("BmnCSCDigit");
    }

    if (isSrc) {
        fCscDigits = new TClonesArray("BmnCSCDigit");
        fBC3Digits = new TClonesArray("BmnTrigDigit");
        fBC4Digits = new TClonesArray("BmnTrigDigit");

        fVetoTQDCDigits = new TClonesArray("BmnTrigWaveDigit");

        fX1LDigits = new TClonesArray("BmnTrigDigit");
        fX2LDigits = new TClonesArray("BmnTrigDigit");
        fY1LDigits = new TClonesArray("BmnTrigDigit");
        fY2LDigits = new TClonesArray("BmnTrigDigit");
        fX1RDigits = new TClonesArray("BmnTrigDigit");
        fX2RDigits = new TClonesArray("BmnTrigDigit");
        fY1RDigits = new TClonesArray("BmnTrigDigit");
        fY2RDigits = new TClonesArray("BmnTrigDigit");

        fX1LTQDCDigits = new TClonesArray("BmnTrigWaveDigit");
        fX2LTQDCDigits = new TClonesArray("BmnTrigWaveDigit");
        fY1LTQDCDigits = new TClonesArray("BmnTrigWaveDigit");
        fY2LTQDCDigits = new TClonesArray("BmnTrigWaveDigit");
        fX1RTQDCDigits = new TClonesArray("BmnTrigWaveDigit");
        fX2RTQDCDigits = new TClonesArray("BmnTrigWaveDigit");
        fY1RTQDCDigits = new TClonesArray("BmnTrigWaveDigit");
        fY2RTQDCDigits = new TClonesArray("BmnTrigWaveDigit");

        fBC1TQDCDigits = new TClonesArray("BmnTrigWaveDigit");
        fBC2TQDCDigits = new TClonesArray("BmnTrigWaveDigit");
        fBC3TQDCDigits = new TClonesArray("BmnTrigWaveDigit");
        fBC4TQDCDigits = new TClonesArray("BmnTrigWaveDigit");
    }

    TString detsRun6[] = {"GEM", "SILICON", "ZDC", "T0", "BC1", "BC2", "VETO", "FD", "BD", "MWPC", "DCH", "TOF400", "TOF700", "ECAL"};
    TString detsRun7[] = {"BC1", "BC2", "BC3", "VC", "Si", "BD", "ZDC", "GEM", "SILICON", "MWPC", "DCH", "TOF400", "ECAL", "CSC", "TOF700"};
    TString detsRun7Src[] = {"GEM", "SILICON", "CSC",
        "BC1", "BC2", "BC3", "BC4",
        "TQDC_BC1", "TQDC_BC2", "TQDC_BC3", "TQDC_BC4",
        "X1L", "X2L", "Y1L", "Y2L", "X1R", "X2R", "Y1R", "Y2R",
        "TQDC_X1L", "TQDC_X2L", "TQDC_Y1L", "TQDC_Y2L", "TQDC_X1R", "TQDC_X2R", "TQDC_Y1R", "TQDC_Y2R",
        "VETO", "TQDC_VETO",
        "TOF700", "ZDC",
        "MWPC", "DCH", "TOF400", "ECAL"};

    TClonesArray * inArrsRun6[] = {fGemDigits, fSilDigits, fZdcDigits, fT0Digits, fBC1Digits, fBC2Digits, fVetoDigits, fFdDigits, fBdDigits,
        fMwpcDigits, fDchDigits, fTof400Digits, fTof700Digits, fEcalDigits};

    TClonesArray * inArrsRun7[] = {fBC1Digits, fBC2Digits, fBC3Digits, fVetoDigits, fSiDigits, fBdDigits, fZdcDigits,
        fGemDigits, fSilDigits, fMwpcDigits, fDchDigits, fTof400Digits, fEcalDigits, fCscDigits, fTof700Digits};

    TClonesArray * inArrsRun7Src[] = {fGemDigits, fSilDigits, fCscDigits,
        fBC1Digits, fBC2Digits, fBC3Digits, fBC4Digits,
        fBC1TQDCDigits, fBC2TQDCDigits, fBC3TQDCDigits, fBC4TQDCDigits,
        fX1LDigits, fX2LDigits, fY1LDigits, fY2LDigits, fX1RDigits, fX2RDigits, fY1RDigits, fY2RDigits,
        fX1LTQDCDigits, fX2LTQDCDigits, fY1LTQDCDigits, fY2LTQDCDigits, fX1RTQDCDigits, fX2RTQDCDigits, fY1RTQDCDigits, fY2RTQDCDigits,
        fVetoDigits, fVetoTQDCDigits,
        fTof700Digits, fZdcDigits, fMwpcDigits, fDchDigits, fTof400Digits, fEcalDigits};

    fInArrs = new TClonesArray*[nDets];
    fHeaders = new BmnEventHeader*[nDets];
    fInFiles = new TChain*[nDets];

    for (Int_t iDet = 0; iDet < nDets; iDet++) {
        fHeaders[iDet] = new BmnEventHeader();
        if (!isSrc)
            fInArrs[iDet] = (isRun6) ? inArrsRun6[iDet] : inArrsRun7[iDet];
        else
            fInArrs[iDet] = inArrsRun7Src[iDet];
        fInFiles[iDet] = new TChain("bmndata");
        fInFiles[iDet]->Add(files[iDet].Data());
        if (!isSrc)
            fInFiles[iDet]->SetBranchAddress((isRun6 ? detsRun6[iDet] : detsRun7[iDet]).Data(), &fInArrs[iDet]);
        else
            fInFiles[iDet]->SetBranchAddress(detsRun7Src[iDet].Data(), &fInArrs[iDet]);
        fInFiles[iDet]->SetBranchAddress("BmnEventHeader.", &fHeaders[iDet]);
    }

    CreateOutputFile(out);
}

void BmnDigiMergeTask::CreateOutputFiles() {
    fIn1->GetEntry(0);
    BmnEventHeader* evHead = (BmnEventHeader*) fHeader1->UncheckedAt(0);
    UInt_t runId = evHead->GetRunId();

    if (!isSrc)
        nDets = isRun7 ? 15 : isRun6 ? 14 : throw;
    else
        nDets = 9 + 26; // dets + triggers

    TString detsRun6[] = {"GEM", "SILICON", "ZDC", "T0", "BC1", "BC2", "VETO", "FD", "BD", "MWPC", "DCH", "TOF400", "TOF700", "ECAL"};
    TString detsRun7[] = {"BC1", "BC2", "BC3", "VC", "Si", "BD", "ZDC", "GEM", "SILICON", "MWPC", "DCH", "TOF400", "ECAL", "CSC", "TOF700"};
    TString detsRun7Src[] = {"GEM", "SILICON", "CSC",
        "BC1", "BC2", "BC3", "BC4",
        "TQDC_BC1", "TQDC_BC2", "TQDC_BC3", "TQDC_BC4",
        "X1L", "X2L", "Y1L", "Y2L", "X1R", "X2R", "Y1R", "Y2R",
        "TQDC_X1L", "TQDC_X2L", "TQDC_Y1L", "TQDC_Y2L", "TQDC_X1R", "TQDC_X2R", "TQDC_Y1R", "TQDC_Y2R",
        "VETO", "TQDC_VETO",
        "TOF700", "ZDC",
        "MWPC", "DCH", "TOF400", "ECAL"};

    fOutFiles = new TFile*[nDets];
    fOutTrees = new TTree*[nDets];

    // Common triggers (BM@N + SRC)
    fBC1 = new TClonesArray("BmnTrigDigit");
    fBC2 = new TClonesArray("BmnTrigDigit");
    fBC3 = new TClonesArray("BmnTrigDigit");
    fVeto = new TClonesArray("BmnTrigDigit");

    // Common detectors (BM@N + SRC)
    fGem = new TClonesArray("BmnGemStripDigit");
    fSil = new TClonesArray("BmnSiliconDigit");
    fMwpc = new TClonesArray("BmnMwpcDigit");
    fDch = new TClonesArray("BmnDchDigit");
    fTof400 = new TClonesArray("BmnTof1Digit");
    fTof700 = new TClonesArray("BmnTof2Digit");
    fEcal = new TClonesArray("BmnECALDigit");
    fZdc = new TClonesArray("BmnZDCDigit");

    if (!isSrc) {
        fBd = new TClonesArray("BmnTrigDigit");
        if (isRun6) {
            fT0 = new TClonesArray("BmnTrigDigit");
            fFd = new TClonesArray("BmnTrigDigit");
        }

        if (isRun7) {
            fSi = new TClonesArray("BmnTrigDigit");
            fCsc = new TClonesArray("BmnCSCDigit");
        }
    } else {
        // Triggers
        fBC4 = new TClonesArray("BmnTrigDigit");

        fBC1TQDC = new TClonesArray("BmnTrigWaveDigit");
        fBC2TQDC = new TClonesArray("BmnTrigWaveDigit");
        fBC3TQDC = new TClonesArray("BmnTrigWaveDigit");
        fBC4TQDC = new TClonesArray("BmnTrigWaveDigit");

        fX1L = new TClonesArray("BmnTrigDigit");
        fX2L = new TClonesArray("BmnTrigDigit");
        fY1L = new TClonesArray("BmnTrigDigit");
        fY2L = new TClonesArray("BmnTrigDigit");
        fX1R = new TClonesArray("BmnTrigDigit");
        fX2R = new TClonesArray("BmnTrigDigit");
        fY1R = new TClonesArray("BmnTrigDigit");
        fY2R = new TClonesArray("BmnTrigDigit");

        fX1LTQDC = new TClonesArray("BmnTrigWaveDigit");
        fX2LTQDC = new TClonesArray("BmnTrigWaveDigit");
        fY1LTQDC = new TClonesArray("BmnTrigWaveDigit");
        fY2LTQDC = new TClonesArray("BmnTrigWaveDigit");
        fX1RTQDC = new TClonesArray("BmnTrigWaveDigit");
        fX2RTQDC = new TClonesArray("BmnTrigWaveDigit");
        fY1RTQDC = new TClonesArray("BmnTrigWaveDigit");
        fY2RTQDC = new TClonesArray("BmnTrigWaveDigit");

        fVetoTQDC = new TClonesArray("BmnTrigWaveDigit");

        // Detectors
        fCsc = new TClonesArray("BmnCSCDigit");
    }

    fHeaderOut = new BmnEventHeader();

    TClonesArray * arrRun6[] = {fGem, fSil, fZdc, fT0, fBC1, fBC2, fVeto, fFd, fBd, fMwpc, fDch, fTof400, fTof700, fEcal};
    TClonesArray * arrRun7[] = {fBC1, fBC2, fBC3, fVeto, fSi, fBd, fZdc, fGem, fSil, fMwpc, fDch, fTof400, fEcal, fCsc, fTof700};
    TClonesArray * arrRun7Src[] = {fGem, fSil, fCsc, fBC1, fBC2, fBC3, fBC4, fBC1TQDC, fBC2TQDC, fBC3TQDC, fBC4TQDC,
        fX1L, fX2L, fY1L, fY2L, fX1R, fX2R, fY1R, fY2R, fX1LTQDC, fX2LTQDC, fY1LTQDC, fY2LTQDC, fX1RTQDC, fX2RTQDC, fY1RTQDC, fY2RTQDC,
        fVeto, fVetoTQDC, fTof700, fZdc, fMwpc, fDch, fTof400, fEcal};

    fOutArrs = new TClonesArray*[nDets];
    for (Int_t iDet = 0; iDet < nDets; iDet++) {
        if (!isSrc)
            fOutArrs[iDet] = isRun6 ? arrRun6[iDet] : arrRun7[iDet];
        else
            fOutArrs[iDet] = arrRun7Src[iDet];
    }

    for (Int_t iDet = 0; iDet < nDets; iDet++) {
        if (!isSrc)
            fOutFiles[iDet] = new TFile(TString((isRun6 ? detsRun6[iDet] : detsRun7[iDet]) + TString::Format("_%d.root", runId)).Data(), "recreate");
        else
            fOutFiles[iDet] = new TFile(TString(detsRun7Src[iDet] + TString::Format("_%d.root", runId)).Data(), "recreate");
        fOutTrees[iDet] = new TTree("bmndata", "bmndata");
        fOutTrees[iDet]->Branch("BmnEventHeader.", &fHeaderOut);
        if (!isSrc)
            fOutTrees[iDet]->Branch((isRun6 ? detsRun6[iDet] : detsRun7[iDet]).Data(), &fOutArrs[iDet]);
        else
            fOutTrees[iDet]->Branch(detsRun7Src[iDet].Data(), &fOutArrs[iDet]);
    }
}

void BmnDigiMergeTask::CreateOutputFile(TString out) {
    fOutFile = new TFile(out.Data(), "recreate");
    fOutTree = new TTree("bmndata", "bmndata");

    fHeaderOut = new BmnEventHeader();
    fOutTree->Branch("BmnEventHeader.", &fHeaderOut);

    fGem = new TClonesArray("BmnGemStripDigit");
    fOutTree->Branch("GEM", &fGem);

    fSil = new TClonesArray("BmnSiliconDigit");
    fOutTree->Branch("SILICON", &fSil);

    fZdc = new TClonesArray("BmnZDCDigit");
    fOutTree->Branch("ZDC", &fZdc);

    fBC1 = new TClonesArray("BmnTrigDigit");
    fOutTree->Branch("BC1", &fBC1);

    fBC2 = new TClonesArray("BmnTrigDigit");
    fOutTree->Branch("BC2", &fBC2);

    fVeto = new TClonesArray("BmnTrigDigit");
    fOutTree->Branch("VETO", &fVeto);

    fBd = new TClonesArray("BmnTrigDigit");
    fOutTree->Branch("BD", &fBd);

    fMwpc = new TClonesArray("BmnMwpcDigit");
    fDch = new TClonesArray("BmnDchDigit");
    fTof400 = new TClonesArray("BmnTof1Digit");
    fTof700 = new TClonesArray("BmnTof2Digit");
    fEcal = new TClonesArray("BmnECALDigit");
    fOutTree->Branch("MWPC", &fMwpc);
    fOutTree->Branch("DCH", &fDch);
    fOutTree->Branch("TOF400", &fTof400);
    fOutTree->Branch("TOF700", &fTof700);
    fOutTree->Branch("ECAL", &fEcal);

    if (isRun6 && !isSrc) {
        fT0 = new TClonesArray("BmnTrigDigit");
        fOutTree->Branch("T0", &fT0);
        fFd = new TClonesArray("BmnTrigDigit");
        fOutTree->Branch("FD", &fFd);
    }

    if (isRun7 && !isSrc) {
        fCsc = new TClonesArray("BmnCSCDigit");
        fOutTree->Branch("CSC", &fCsc);
        fBC3 = new TClonesArray("BmnTrigDigit");
        fOutTree->Branch("BC3", &fBC3);
        fSi = new TClonesArray("BmnTrigDigit");
        fOutTree->Branch("Si", &fSi);
    }

    if (isSrc) {
        fCsc = new TClonesArray("BmnCSCDigit");
        fOutTree->Branch("CSC", &fCsc);

        fBC3 = new TClonesArray("BmnTrigDigit");
        fOutTree->Branch("BC3", &fBC3);

        fBC4 = new TClonesArray("BmnTrigDigit");
        fOutTree->Branch("BC4", &fBC4);

        fVetoTQDC = new TClonesArray("BmnTrigWaveDigit");
        fOutTree->Branch("TQDC_VETO", &fVetoTQDC);

        fX1L = new TClonesArray("BmnTrigDigit");
        fOutTree->Branch("X1L", &fX1L);
        fX2L = new TClonesArray("BmnTrigDigit");
        fOutTree->Branch("X2L", &fX2L);
        fY1L = new TClonesArray("BmnTrigDigit");
        fOutTree->Branch("Y1L", &fY1L);
        fY2L = new TClonesArray("BmnTrigDigit");
        fOutTree->Branch("Y2L", &fY2L);
        fX1R = new TClonesArray("BmnTrigDigit");
        fOutTree->Branch("X1R", &fX1R);
        fX2R = new TClonesArray("BmnTrigDigit");
        fOutTree->Branch("X2R", &fX2R);
        fY1R = new TClonesArray("BmnTrigDigit");
        fOutTree->Branch("Y1R", &fY1R);
        fY2R = new TClonesArray("BmnTrigDigit");
        fOutTree->Branch("Y2R", &fY2R);

        fX1LTQDC = new TClonesArray("BmnTrigWaveDigit");
        fOutTree->Branch("TQDC_X1L", &fX1LTQDC);
        fX2LTQDC = new TClonesArray("BmnTrigWaveDigit");
        fOutTree->Branch("TQDC_X2L", &fX2LTQDC);
        fY1LTQDC = new TClonesArray("BmnTrigWaveDigit");
        fOutTree->Branch("TQDC_Y1L", &fY1LTQDC);
        fY2LTQDC = new TClonesArray("BmnTrigWaveDigit");
        fOutTree->Branch("TQDC_Y2L", &fY2LTQDC);
        fX1RTQDC = new TClonesArray("BmnTrigWaveDigit");
        fOutTree->Branch("TQDC_X1R", &fX1RTQDC);
        fX2RTQDC = new TClonesArray("BmnTrigWaveDigit");
        fOutTree->Branch("TQDC_X2R", &fX2RTQDC);
        fY1RTQDC = new TClonesArray("BmnTrigWaveDigit");
        fOutTree->Branch("TQDC_Y1R", &fY1RTQDC);
        fY2RTQDC = new TClonesArray("BmnTrigWaveDigit");
        fOutTree->Branch("TQDC_Y2R", &fY2RTQDC);

        fBC1TQDC = new TClonesArray("BmnTrigWaveDigit");
        fOutTree->Branch("TQDC_BC1", &fBC1TQDC);
        fBC2TQDC = new TClonesArray("BmnTrigWaveDigit");
        fOutTree->Branch("TQDC_BC2", &fBC2TQDC);
        fBC3TQDC = new TClonesArray("BmnTrigWaveDigit");
        fOutTree->Branch("TQDC_BC3", &fBC3TQDC);
        fBC4TQDC = new TClonesArray("BmnTrigWaveDigit");
        fOutTree->Branch("TQDC_BC4", &fBC4TQDC);
    }

    TClonesArray * arrRun6[] = {fGem, fSil, fZdc, fT0, fBC1, fBC2, fVeto, fFd, fBd, fMwpc, fDch, fTof400, fTof700, fEcal};
    TClonesArray * arrRun7[] = {fBC1, fBC2, fBC3, fVeto, fSi, fBd, fZdc, fGem, fSil, fMwpc, fDch, fTof400, fEcal, fCsc, fTof700};
    TClonesArray * arrRun7Src[] = {fGem, fSil, fCsc, fBC1, fBC2, fBC3, fBC4, fBC1TQDC, fBC2TQDC, fBC3TQDC, fBC4TQDC,
        fX1L, fX2L, fY1L, fY2L, fX1R, fX2R, fY1R, fY2R, fX1LTQDC, fX2LTQDC, fY1LTQDC, fY2LTQDC, fX1RTQDC, fX2RTQDC, fY1RTQDC, fY2RTQDC,
        fVeto, fVetoTQDC, fTof700, fZdc, fMwpc, fDch, fTof400, fEcal};

    fOutArrs = new TClonesArray*[nDets];
    for (Int_t iDet = 0; iDet < nDets; iDet++) {
        if (!isSrc)
            fOutArrs[iDet] = isRun6 ? arrRun6[iDet] : arrRun7[iDet];
        else
            fOutArrs[iDet] = arrRun7Src[iDet];
    }
}

void BmnDigiMergeTask::SplitToDetectorsSrc() {
    map <Int_t, TClonesArray*> detsAndOutTrees1;
    map <Int_t, TClonesArray*> detsAndOutTrees2;
    map <Int_t, TClonesArray*> detsAndOutTrees3;

    for (Int_t iDet = 0; iDet < 3; iDet++)
        detsAndOutTrees1[iDet] = fOutArrs[iDet];

    for (Int_t iDet = 3; iDet < 31; iDet++)
        detsAndOutTrees2[iDet] = fOutArrs[iDet];

    for (Int_t iDet = 31; iDet < nDets; iDet++)
        detsAndOutTrees3[iDet] = fOutArrs[iDet];

    for (Int_t iEntry = 0; iEntry < fIn1->GetEntries(); iEntry++) {
        fIn1->GetEntry(iEntry);

        if (iEntry % 10000 == 0)
            cout << "Event# " << iEntry << endl;

        for (auto it : detsAndOutTrees1)
            it.second->Delete();

        BmnEventHeader* header = (BmnEventHeader*) fHeader1->UncheckedAt(0);
        fHeaderOut->SetRunId(header->GetRunId());
        fHeaderOut->SetEventId(header->GetEventId());

        // Prepare arrays for GEM- and SI-converters and make
        Int_t* statsGem = new Int_t[fDetectorGEM->GetNStations()];
        Int_t* statsSil = new Int_t[fDetectorSI->GetNStations()];

        Int_t* statsGemPermut = new Int_t[fDetectorGEM->GetNStations()];
        Int_t* statsSilPermut = new Int_t[fDetectorSI->GetNStations()];

        for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++) {
            statsGem[iStat] = -1;
            statsGemPermut[iStat] = -1;
        }

        for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++) {
            statsSil[iStat] = -1;
            statsSilPermut[iStat] = -1;
        }

        Run7(statsGem, statsSil, statsGemPermut, statsSilPermut); // FIXME!

        for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
            fGemStats[statsGem[iStat]] = statsGemPermut[iStat];

        for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++)
            fSilStats[statsSil[iStat]] = statsSilPermut[iStat];

        if (fGemDigits)
            for (UInt_t iDigi = 0; iDigi < fGemDigits->GetEntriesFast(); iDigi++) {
                BmnGemStripDigit* dig = (BmnGemStripDigit*) fGemDigits->UncheckedAt(iDigi);

                if (dig->GetStation() != 11) {
                    Int_t stat = dig->GetStation();
                    Int_t strip = dig->GetStripNumber() - 1; // strips should be enumerated from zero
                    Double_t signal = dig->GetStripSignal();
                    dig->SetStation(GemStatPermutation(stat));
                    stat = dig->GetStation(); // Stations permuted already!

                    if (stat == 5 || stat == 7 || stat == 9) {
                        if (dig->GetModule() == 0)
                            dig->SetModule(1);
                        else if (dig->GetModule() == 1)
                            dig->SetModule(0);
                        else {
                            cout << "Something went wrong!" << endl;
                            throw;
                        }
                    }
                    new((*fGem)[fGem->GetEntriesFast()]) BmnGemStripDigit(stat, dig->GetModule(), dig->GetStripLayer(), strip, signal);
                } else {
                    // Create CSC-digi without converter
                    BmnCSCDigit digCsc(0, dig->GetModule(), dig->GetStripLayer(), dig->GetStripNumber(), dig->GetStripSignal());

                    Int_t strip = digCsc.GetStripNumber() - 1; // strips should be enumerated from zero
                    Double_t signal = digCsc.GetStripSignal();

                    // Modules should be permutated! (mod0 <--> mod1)
                    if (digCsc.GetModule() == 0) {
                        digCsc.SetModule(1);
                        // Layers should be permutated in mod0 only
                        if (digCsc.GetStripLayer() == 2)
                            digCsc.SetStripLayer(0);
                        else if (digCsc.GetStripLayer() == 3)
                            digCsc.SetStripLayer(1);
                        else if (digCsc.GetStripLayer() == 0)
                            digCsc.SetStripLayer(2);
                        else if (digCsc.GetStripLayer() == 1)
                            digCsc.SetStripLayer(3);
                        else {
                            cout << "Something went wrong!" << endl;
                            throw;
                        }
                    } else if (digCsc.GetModule() == 1)
                        digCsc.SetModule(0);

                    else {
                        cout << "Something went wrong!" << endl;
                        throw;
                    }
                    new ((*fCsc)[fCsc->GetEntriesFast()]) BmnCSCDigit(0, digCsc.GetModule(), digCsc.GetStripLayer(), strip, signal);
                }
            }

        if (fSilDigits)
            for (UInt_t iDigi = 0; iDigi < fSilDigits->GetEntriesFast(); iDigi++) {
                BmnSiliconDigit* dig = (BmnSiliconDigit*) fSilDigits->UncheckedAt(iDigi);
                Int_t stat = dig->GetStation();
                dig->SetStation(SiliconStatPermutation(stat));
                stat = dig->GetStation(); // Stations permuted already!

                Int_t strip = dig->GetStripNumber() - 1; // strips should be enumerated from zero
                Int_t signal = dig->GetStripSignal();

                Int_t mod = dig->GetModule();
                Int_t layer = dig->GetStripLayer();

                new((*fSil)[fSil->GetEntriesFast()]) BmnSiliconDigit(stat, mod, layer, strip, signal);
            }

        for (auto it : detsAndOutTrees1)
            fOutTrees[it.first]->Fill();
    }

    for (Int_t iEntry = 0; iEntry < fIn2->GetEntries(); iEntry++) {
        fIn2->GetEntry(iEntry);

        if (iEntry % 10000 == 0)
            cout << "Event# " << iEntry << endl;

        for (auto it : detsAndOutTrees2)
            it.second->Delete();

        fHeaderOut->SetRunId(fHeader2Obj->GetRunId());
        fHeaderOut->SetEventId(fHeader2Obj->GetEventId());

        // Detectors
        if (fTof700Digits)
            FillDetDigi <BmnTof2Digit> (fTof700Digits, fTof700);

        if (fZdcDigits)
            FillDetDigi <BmnZDCDigit> (fZdcDigits, fZdc);

        // Triggers
        const Int_t nTrigs = 13;
        const Int_t nTqdcTrigs = 13;

        TClonesArray * trigsIn[nTrigs] = {fBC1Digits, fBC2Digits, fBC3Digits, fBC4Digits,
            fX1LDigits, fX2LDigits, fY1LDigits, fY2LDigits, fX1RDigits, fX2RDigits, fY1RDigits, fY2RDigits, fVetoDigits};
        TClonesArray * trigsOut[nTrigs] = {fBC1, fBC2, fBC3, fBC4, fX1L, fX2L, fY1L, fY2L, fX1R, fX2R, fY1R, fY2R, fVeto};
        TClonesArray * tqdcTrigsIn[nTrigs] = {fBC1TQDCDigits, fBC2TQDCDigits, fBC3TQDCDigits, fBC4TQDCDigits,
            fX1LTQDCDigits, fX2LTQDCDigits, fY1LTQDCDigits, fY2LTQDCDigits, fX1RTQDCDigits, fX2RTQDCDigits, fY1RTQDCDigits, fY2RTQDCDigits, fVetoTQDCDigits};
        TClonesArray * tqdcTrigsOut[nTrigs] = {fBC1TQDC, fBC2TQDC, fBC3TQDC, fBC4TQDC,
            fX1LTQDC, fX2LTQDC, fY1LTQDC, fY2LTQDC, fX1RTQDC, fX2RTQDC, fY1RTQDC, fY2RTQDC, fVetoTQDC};

        for (Int_t iTrig = 0; iTrig < nTrigs; iTrig++)
            if (trigsIn[iTrig])
                FillDetDigi <BmnTrigDigit> (trigsIn[iTrig], trigsOut[iTrig]);

        for (Int_t iTqdcTrig = 0; iTqdcTrig < nTqdcTrigs; iTqdcTrig++)
            if (tqdcTrigsIn[iTqdcTrig])
                FillDetDigi <BmnTrigWaveDigit> (tqdcTrigsIn[iTqdcTrig], tqdcTrigsOut[iTqdcTrig]);

        for (auto it : detsAndOutTrees2)
            fOutTrees[it.first]->Fill();
    }

    for (Int_t iEntry = 0; iEntry < fIn3->GetEntries(); iEntry++) {
        fIn3->GetEntry(iEntry);

        if (iEntry % 10000 == 0)
            cout << "Event# " << iEntry << endl;

        for (auto it : detsAndOutTrees3)
            it.second->Delete();

        BmnEventHeader* header = (BmnEventHeader*) fHeader3->UncheckedAt(0);
        fHeaderOut->SetRunId(header->GetRunId());
        fHeaderOut->SetEventId(header->GetEventId());

        if (fMwpcDigits)
            FillDetDigi <BmnMwpcDigit> (fMwpcDigits, fMwpc);

        if (fDchDigits)
            FillDetDigi <BmnDchDigit> (fDchDigits, fDch);

        if (fTof400Digits)
            FillDetDigi <BmnTof1Digit> (fTof400Digits, fTof400);

        if (fEcalDigits)
            FillDetDigi <BmnECALDigit> (fEcalDigits, fEcal);

        for (auto it : detsAndOutTrees3)
            fOutTrees[it.first]->Fill();
    }
}

void BmnDigiMergeTask::SplitToDetectorsRun6() {
    map <Int_t, TClonesArray*> detsAndOutTrees1;
    map <Int_t, TClonesArray*> detsAndOutTrees2;
    map <Int_t, TClonesArray*> detsAndOutTrees3;

    for (Int_t iDet = 0; iDet < 2; iDet++)
        detsAndOutTrees1[iDet] = fOutArrs[iDet];

    for (Int_t iDet = 2; iDet < 3; iDet++)
        detsAndOutTrees2[iDet] = fOutArrs[iDet];

    for (Int_t iDet = 3; iDet < nDets; iDet++)
        detsAndOutTrees3[iDet] = fOutArrs[iDet];

    for (Int_t iEntry = 0; iEntry < fIn1->GetEntries(); iEntry++) {
        fIn1->GetEntry(iEntry);

        if (iEntry % 10000 == 0)
            cout << "Event# " << iEntry << endl;

        for (auto it : detsAndOutTrees1)
            it.second->Delete();

        BmnEventHeader* header = (BmnEventHeader*) fHeader1->UncheckedAt(0);
        fHeaderOut->SetRunId(header->GetRunId());
        fHeaderOut->SetEventId(header->GetEventId());
        // if (header->GetEventType() == 0) cout << header->GetEventType() << endl;
        fHeaderOut->SetEventType(header->GetEventType());

        if (fGemDigits)
            FillDetDigi <BmnGemStripDigit> (fGemDigits, fGem);

        if (fSilDigits)
            FillDetDigi <BmnSiliconDigit> (fSilDigits, fSil);

        for (auto it : detsAndOutTrees1)
            fOutTrees[it.first]->Fill();
    }

    for (Int_t iEntry = 0; iEntry < fIn2->GetEntries(); iEntry++) {
        fIn2->GetEntry(iEntry);

        if (iEntry % 10000 == 0)
            cout << "Event# " << iEntry << endl;

        for (auto it : detsAndOutTrees2)
            it.second->Delete();

        BmnEventHeader* header = (BmnEventHeader*) fHeader2->UncheckedAt(0);
        fHeaderOut->SetRunId(header->GetRunId());
        fHeaderOut->SetEventId(header->GetEventId());

        if (fZdcDigits)
            FillDetDigi <BmnZDCDigit> (fZdcDigits, fZdc);

        for (auto it : detsAndOutTrees2)
            fOutTrees[it.first]->Fill();
    }

    for (Int_t iEntry = 0; iEntry < fIn3->GetEntries(); iEntry++) {
        fIn3->GetEntry(iEntry);

        if (iEntry % 10000 == 0)
            cout << "Event# " << iEntry << endl;

        for (auto it : detsAndOutTrees3)
            it.second->Delete();

        BmnEventHeader* header = (BmnEventHeader*) fHeader3->UncheckedAt(0);
        fHeaderOut->SetRunId(header->GetRunId());
        fHeaderOut->SetEventId(header->GetEventId());

        // Triggers
        const Int_t nTrigs = 6;
        TClonesArray * trigsIn[nTrigs] = {fT0Digits, fFdDigits, fBC1Digits, fBC2Digits, fVetoDigits, fBdDigits};
        TClonesArray * trigsOut[nTrigs] = {fT0, fFd, fBC1, fBC2, fVeto, fBd};

        for (Int_t iTrig = 0; iTrig < nTrigs; iTrig++)
            if (trigsIn[iTrig])
                FillDetDigi <BmnTrigDigit> (trigsIn[iTrig], trigsOut[iTrig]);

        if (fMwpcDigits)
            FillDetDigi <BmnMwpcDigit> (fMwpcDigits, fMwpc);

        if (fDchDigits)
            FillDetDigi <BmnDchDigit> (fDchDigits, fDch);

        if (fTof400Digits)
            FillDetDigi <BmnTof1Digit> (fTof400Digits, fTof400);

        if (fTof700Digits)
            FillDetDigi <BmnTof2Digit> (fTof700Digits, fTof700);

        if (fEcalDigits)
            FillDetDigi <BmnECALDigit> (fEcalDigits, fEcal);

        for (auto it : detsAndOutTrees3)
            fOutTrees[it.first]->Fill();
    }
}

void BmnDigiMergeTask::SplitToDetectorsRun7() {
    // fBC1, fBC2, fBC3, fVeto, fSi, fBd, fZdc, fGem, fSil, fMwpc, fDch, fTof400, fEcal, fCsc, fTof700
    map <Int_t, TClonesArray*> detsAndOutTrees1;
    map <Int_t, TClonesArray*> detsAndOutTrees2;
    map <Int_t, TClonesArray*> detsAndOutTrees3;

    for (Int_t iDet = 7; iDet < 14; iDet++)
        detsAndOutTrees1[iDet] = fOutArrs[iDet];

    for (Int_t iDet = 0; iDet < 7; iDet++)
        detsAndOutTrees2[iDet] = fOutArrs[iDet];

    for (Int_t iDet = 14; iDet < nDets; iDet++)
        detsAndOutTrees3[iDet] = fOutArrs[iDet];

    for (Int_t iEntry = 0; iEntry < fIn1->GetEntries(); iEntry++) {
        fIn1->GetEntry(iEntry);

        if (iEntry % 10000 == 0)
            cout << "Event# " << iEntry << endl;

        for (auto it : detsAndOutTrees1)
            it.second->Delete();

        BmnEventHeader* header = (BmnEventHeader*) fHeader1->UncheckedAt(0);
        fHeaderOut->SetRunId(header->GetRunId());
        fHeaderOut->SetEventId(header->GetEventId());

        // Prepare arrays for GEM- and SI-converters and make
        Int_t* statsGem = new Int_t[fDetectorGEM->GetNStations()];
        Int_t* statsSil = new Int_t[fDetectorSI->GetNStations()];

        Int_t* statsGemPermut = new Int_t[fDetectorGEM->GetNStations()];
        Int_t* statsSilPermut = new Int_t[fDetectorSI->GetNStations()];

        for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++) {
            statsGem[iStat] = -1;
            statsGemPermut[iStat] = -1;
        }

        for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++) {
            statsSil[iStat] = -1;
            statsSilPermut[iStat] = -1;
        }

        Run7(statsGem, statsSil, statsGemPermut, statsSilPermut); // FIXME!

        for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
            fGemStats[statsGem[iStat]] = statsGemPermut[iStat];

        for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++)
            fSilStats[statsSil[iStat]] = statsSilPermut[iStat];

        if (fGemDigits)
            for (UInt_t iDigi = 0; iDigi < fGemDigits->GetEntriesFast(); iDigi++) {
                BmnGemStripDigit* dig = (BmnGemStripDigit*) fGemDigits->UncheckedAt(iDigi);

                if (dig->GetStation() == 3) // Stat3 did not work, so skipped
                    continue;

                if (dig->GetStation() != 8) {
                    Int_t stat = dig->GetStation();
                    Int_t strip = dig->GetStripNumber() - 1; // strips should be enumerated from zero
                    Double_t signal = dig->GetStripSignal();
                    dig->SetStation(GemStatPermutation(stat));
                    stat = dig->GetStation(); // Stations permuted already!

                    if (stat == 0 || stat == 3 || stat == 5) {
                        if (dig->GetModule() == 0)
                            dig->SetModule(1);
                        else if (dig->GetModule() == 1)
                            dig->SetModule(0);
                        else {
                            cout << "Something went wrong!" << endl;
                            throw;
                        }
                    }
                    new((*fGem)[fGem->GetEntriesFast()]) BmnGemStripDigit(stat, dig->GetModule(), dig->GetStripLayer(), strip, signal);
                } else {
                    // Create CSC-digi without converter
                    BmnCSCDigit digCsc(0, dig->GetModule(), dig->GetStripLayer(), dig->GetStripNumber(), dig->GetStripSignal());

                    Int_t strip = digCsc.GetStripNumber() - 1; // strips should be enumerated from zero
                    Double_t signal = digCsc.GetStripSignal();

                    // Modules should be permutated! (mod0 <--> mod1)
                    if (digCsc.GetModule() == 0) {
                        digCsc.SetModule(1);
                        // Layers should be permutated in mod0 only
                        if (digCsc.GetStripLayer() == 2)
                            digCsc.SetStripLayer(0);
                        else if (digCsc.GetStripLayer() == 3)
                            digCsc.SetStripLayer(1);
                        else if (digCsc.GetStripLayer() == 0)
                            digCsc.SetStripLayer(2);
                        else if (digCsc.GetStripLayer() == 1)
                            digCsc.SetStripLayer(3);
                        else {
                            cout << "Something went wrong!" << endl;
                            throw;
                        }
                    } else if (digCsc.GetModule() == 1)
                        digCsc.SetModule(0);

                    else {
                        cout << "Something went wrong!" << endl;
                        throw;
                    }
                    new ((*fCsc)[fCsc->GetEntriesFast()]) BmnCSCDigit(0, digCsc.GetModule(), digCsc.GetStripLayer(), strip, signal);
                }
            }

        if (fSilDigits)
            for (UInt_t iDigi = 0; iDigi < fSilDigits->GetEntriesFast(); iDigi++) {
                BmnSiliconDigit* dig = (BmnSiliconDigit*) fSilDigits->UncheckedAt(iDigi);
                Int_t stat = dig->GetStation();
                dig->SetStation(SiliconStatPermutation(stat));
                stat = dig->GetStation(); // Stations permuted already!

                Int_t strip = dig->GetStripNumber() - 1; // strips should be enumerated from zero
                Int_t signal = dig->GetStripSignal();

                Int_t mod = dig->GetModule();
                Int_t layer = dig->GetStripLayer();
                
                BmnSiliconDigit* outDig = new((*fSil)[fSil->GetEntriesFast()]) BmnSiliconDigit(stat, mod, layer, strip, signal);
                outDig->SetIsGoodDigit(dig->IsGoodDigit());
            }

        if (fMwpcDigits)
            FillDetDigi <BmnMwpcDigit> (fMwpcDigits, fMwpc);

        if (fDchDigits)
            FillDetDigi <BmnDchDigit> (fDchDigits, fDch);

        if (fTof400Digits)
            FillDetDigi <BmnTof1Digit> (fTof400Digits, fTof400);

        if (fEcalDigits)
            FillDetDigi <BmnECALDigit> (fEcalDigits, fEcal);

        for (auto it : detsAndOutTrees1)
            fOutTrees[it.first]->Fill();
    }

    for (Int_t iEntry = 0; iEntry < fIn2->GetEntries(); iEntry++) {
        fIn2->GetEntry(iEntry);
        if (iEntry % 10000 == 0)
            cout << "Event# " << iEntry << endl;

        for (auto it : detsAndOutTrees2)
            it.second->Delete();

        BmnEventHeader* header = (BmnEventHeader*) fHeader2->UncheckedAt(0);
        fHeaderOut->SetRunId(header->GetRunId());
        fHeaderOut->SetEventId(header->GetEventId());

        // Triggers
        const Int_t nTrigs = 6;
        TClonesArray * trigsIn[nTrigs] = {fBC1Digits, fBC2Digits, fBC3Digits, fVetoDigits, fSiDigits, fBdDigits};
        TClonesArray * trigsOut[nTrigs] = {fBC1, fBC2, fBC3, fVeto, fSi, fBd};

        for (Int_t iTrig = 0; iTrig < nTrigs; iTrig++)
            if (trigsIn[iTrig])
                FillDetDigi <BmnTrigDigit> (trigsIn[iTrig], trigsOut[iTrig]);

        if (fZdcDigits)
            FillDetDigi <BmnZDCDigit> (fZdcDigits, fZdc);

        for (auto it : detsAndOutTrees2)
            fOutTrees[it.first]->Fill();
    }

    for (Int_t iEntry = 0; iEntry < fIn3->GetEntries(); iEntry++) {
        fIn3->GetEntry(iEntry);

        if (iEntry % 10000 == 0)
            cout << "Event# " << iEntry << endl;

        for (auto it : detsAndOutTrees3)
            it.second->Delete();

        fHeaderOut->SetRunId(fHeader3Obj->GetRunId());
        fHeaderOut->SetEventId(fHeader3Obj->GetEventId());

        if (fTof700Digits)
            FillDetDigi <BmnTof2Digit> (fTof700Digits, fTof700);

        for (auto it : detsAndOutTrees3)
            fOutTrees[it.first]->Fill();
    }
}

void BmnDigiMergeTask::ProcessEvents() {
    vector <UInt_t> nums;
    for (Int_t iDet = 0; iDet < nDets; iDet++)
        nums.push_back(fInFiles[iDet]->GetEntries());

    maxEvId = *max_element(nums.begin(), nums.end()) + 100;

    evId_evNum = new Long_t*[maxEvId];
    for (UInt_t iEle = 0; iEle < maxEvId; iEle++)
        evId_evNum[iEle] = new Long_t[nDets];

    for (Long_t iEvId = 0; iEvId < maxEvId; iEvId++)
        for (Int_t iDet = 0; iDet < nDets; iDet++)
            evId_evNum[iEvId][iDet] = -1;

    for (Int_t iDet = 0; iDet < nDets; iDet++) {
        BmnEventHeader* header = fHeaders[iDet];
        for (Int_t iEntry = 0; iEntry < fInFiles[iDet]->GetEntries(); iEntry++) {
            fInFiles[iDet]->GetEntry(iEntry);
            //cout << iDet << " " << header->GetEventId() << endl;
            evId_evNum[header->GetEventId()][iDet] = iEntry;
        }
    }
    GlueEventsFromInputFiles();
}

void BmnDigiMergeTask::GlueEventsFromInputFiles() {
    for (UInt_t iEvId = 0; iEvId < maxEvId; iEvId++) {
        fBC1->Delete();
        fBC2->Delete();
        fVeto->Delete();
        fBd->Delete();
        fZdc->Delete();
        fGem->Delete();
        fSil->Delete();
        fMwpc->Delete();
        fDch->Delete();
        fTof400->Delete();
        fTof700->Delete();
        fEcal->Delete();

        if (isRun6 && !isSrc) {
            fT0->Delete();
            fFd->Delete();
        }

        if (isRun7 && !isSrc) {
            fBC3->Delete();
            fSi->Delete();
            fCsc->Delete();
        }

        if (isSrc) {
            fCsc->Delete();
            fBC3->Delete();
            fBC4->Delete();

            fVetoTQDC->Delete();

            fX1L->Delete();
            fX2L->Delete();
            fY1L->Delete();
            fY2L->Delete();
            fX1R->Delete();
            fX2R->Delete();
            fY1R->Delete();
            fY2R->Delete();

            fX1LTQDC->Delete();
            fX2LTQDC->Delete();
            fY1LTQDC->Delete();
            fY2LTQDC->Delete();
            fX1RTQDC->Delete();
            fX2RTQDC->Delete();
            fY1RTQDC->Delete();
            fY2RTQDC->Delete();

            fBC1TQDC->Delete();
            fBC2TQDC->Delete();
            fBC3TQDC->Delete();
            fBC4TQDC->Delete();
        }

        for (Int_t iDet = 0; iDet < nDets; iDet++) {
            fHeaderOut->SetRunId(fHeaders[iDet]->GetRunId());
            fHeaderOut->SetEventId(fHeaders[iDet]->GetEventId());

            if (evId_evNum[iEvId][iDet] % 1000 == 0)
                cout << "Det# " << iDet << " Event# " << evId_evNum[iEvId][iDet] << endl;

            fInFiles[iDet]->GetEntry(evId_evNum[iEvId][iDet]);          
            
            fOutArrs[iDet]->AbsorbObjects(fInArrs[iDet]);
        }

        fOutTree->Fill();
    }
}

void BmnDigiMergeTask::Run7(Int_t* statsGem, Int_t* statsSil, Int_t* statsGemPermut, Int_t* statsSilPermut) {
    // Setup is valid for two modes (BM@N and SRC)
    // To be moved to the UniDb
    if (!isSrc) {
        statsGem[0] = 1; // MK-numeration
        statsGem[1] = 2; // MK-numeration
        statsGem[2] = 4; // MK-numeration
        statsGem[3] = 5; // MK-numeration
        statsGem[4] = 6; // MK-numeration
        statsGem[5] = 7; // MK-numeration

        statsGemPermut[0] = 0;
        statsGemPermut[1] = 1;
        statsGemPermut[2] = 2;
        statsGemPermut[3] = 3;
        statsGemPermut[4] = 4;
        statsGemPermut[5] = 5;

        statsSil[0] = 1; // MK-numeration
        statsSil[1] = 2; // MK-numeration
        statsSil[2] = 3; // MK-numeration

        statsSilPermut[0] = 0;
        statsSilPermut[1] = 1;
        statsSilPermut[2] = 2;

    } else if (isSrc) {
        statsGem[0] = 1; // MK-numeration
        statsGem[1] = 2; // MK-numeration
        statsGem[2] = 3; // MK-numeration
        statsGem[3] = 4; // MK-numeration
        statsGem[4] = 5; // MK-numeration
        statsGem[5] = 6; // MK-numeration
        statsGem[6] = 7; // MK-numeration
        statsGem[7] = 8; // MK-numeration
        statsGem[8] = 9; // MK-numeration
        statsGem[9] = 10; // MK-numeration

        statsGemPermut[0] = 0;
        statsGemPermut[1] = 1;
        statsGemPermut[2] = 2;
        statsGemPermut[3] = 3;
        statsGemPermut[4] = 4;
        statsGemPermut[5] = 5;
        statsGemPermut[6] = 6;
        statsGemPermut[7] = 7;
        statsGemPermut[8] = 8;
        statsGemPermut[9] = 9;

        statsSil[0] = 1; // MK-numeration
        statsSil[1] = 2; // MK-numeration
        statsSil[2] = 3; // MK-numeration

        statsSilPermut[0] = 0;
        statsSilPermut[1] = 1;
        statsSilPermut[2] = 2;
    } else {
        cout << "Configuration not defined!" << endl;
        throw;
    }
}

void BmnDigiMergeTask::CreateGeometries() {
    // Initialize det. geometries for converter
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString confSi = (!isSrc) ? "SiliconRunSpring2018.xml" : isSrc ? "SiliconRunSRCSpring2018.xml" : "";
    TString confGem = (!isSrc) ? "GemRunSpring2018.xml" : isSrc ? "GemRunSRCSpring2018.xml" : "";
    TString confCsc = "CSCRunSpring2018.xml";

    // SI
    TString gPathSiliconConfig = gPathConfig + "/parameters/silicon/XMLConfigs/";
    fDetectorSI = new BmnSiliconStationSet(gPathSiliconConfig + confSi);

    // GEM
    TString gPathGemConfig = gPathConfig + "/parameters/gem/XMLConfigs/";
    fDetectorGEM = new BmnGemStripStationSet(gPathGemConfig + confGem);

    // CSC
    TString gPathCscConfig = gPathConfig + "/parameters/csc/XMLConfigs/";
    fDetectorCSC = new BmnCSCStationSet(gPathCscConfig + confCsc);
}

