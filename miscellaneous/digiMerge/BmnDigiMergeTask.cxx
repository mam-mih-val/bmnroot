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
}

// RUN-7

BmnDigiMergeTask::BmnDigiMergeTask(TString in1, TString in2) :
isRun6(kFALSE),
isRun7(kTRUE),
isSrc(kFALSE),
fIn1(nullptr),
fIn2(nullptr),
fHeader1(nullptr),
fHeader2(nullptr),
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

    TString brName = "EventHeader";

    // Detector branches
    if (!in1.IsNull()) {
        fIn1->Add(in1.Data());
        fIn1->SetBranchAddress("STRIPGEM", &fGemDigits);
        fIn1->SetBranchAddress("MYSILICON", &fSilDigits);
        fIn1->SetBranchAddress("MWPC", &fMwpcDigits);
        fIn1->SetBranchAddress("DCH", &fDchDigits);
        fIn1->SetBranchAddress("TOF400", &fTof400Digits);
        fIn1->SetBranchAddress("TOF700", &fTof700Digits);
        fIn1->SetBranchAddress("ECAL", &fEcalDigits);
        fIn1->SetBranchAddress(brName.Data(), &fHeader1);
    }

    if (!in2.IsNull()) {
        fIn2->Add(in2.Data());
        fIn2->SetBranchAddress("BC1", &fBC1Digits);
        fIn2->SetBranchAddress("BC2", &fBC2Digits);
        fIn2->SetBranchAddress("BC3", &fBC3Digits);
        fIn2->SetBranchAddress("VC", &fVetoDigits);
        fIn2->SetBranchAddress("Si", &fSiDigits);
        fIn2->SetBranchAddress("BD", &fBdDigits);
        fIn2->SetBranchAddress("ZDC", &fZdcDigits);
        fIn2->SetBranchAddress(brName.Data(), &fHeader2);
    }

    CreateOutputFiles();
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
fEcal(nullptr) {
    nDets = files.size();

    isRun6 = (nDets == 14) ? kTRUE : kFALSE;
    isRun7 = (nDets == 15) ? kTRUE : kFALSE;
    isSrc = (nDets == 35) ? kTRUE : kFALSE;

    if (!isRun6 && !isRun7 && !isSrc)
        throw;

    fBC1Digits = new TClonesArray("BmnTrigDigit");
    fBC2Digits = new TClonesArray("BmnTrigDigit");
    fVetoDigits = new TClonesArray("BmnTrigDigit");
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
        fBdDigits = new TClonesArray("BmnTrigDigit");
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
    TString detsRun7[] = {"BC1", "BC2", "BC3", "VC", "Si", "BD", "ZDC", "GEM", "SILICON", "MWPC", "DCH", "TOF400", "TOF700", "ECAL", "CSC"};
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
        fGemDigits, fSilDigits, fMwpcDigits, fDchDigits, fTof400Digits, fTof700Digits, fEcalDigits, fCscDigits};

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
    TString detsRun7[] = {"BC1", "BC2", "BC3", "VC", "Si", "BD", "ZDC", "GEM", "SILICON", "MWPC", "DCH", "TOF400", "TOF700", "ECAL", "CSC"};
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
    TClonesArray * arrRun7[] = {fBC1, fBC2, fBC3, fVeto, fSi, fBd, fZdc, fGem, fSil, fMwpc, fDch, fTof400, fTof700, fEcal, fCsc};
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
        fBd = new TClonesArray("BmnTrigDigit");
        fOutTree->Branch("BD", &fBd);
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

        if (fGemDigits)
            for (UInt_t iDigi = 0; iDigi < fGemDigits->GetEntriesFast(); iDigi++) {
                BmnGemStripDigit* dig = (BmnGemStripDigit*) fGemDigits->UncheckedAt(iDigi);
                if (dig->GetStation() != 11)
                    new ((*fGem)[fGem->GetEntriesFast()]) BmnGemStripDigit(*dig);
                else {
                    BmnCSCDigit digCsc(0, dig->GetModule(), dig->GetStripLayer(), dig->GetStripNumber(), dig->GetStripSignal());
                    new ((*fCsc)[fCsc->GetEntriesFast()]) BmnCSCDigit(digCsc);
                }
            }

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
    map <Int_t, TClonesArray*> detsAndOutTrees1;
    map <Int_t, TClonesArray*> detsAndOutTrees2;

    for (Int_t iDet = 7; iDet < nDets; iDet++)
        detsAndOutTrees1[iDet] = fOutArrs[iDet];

    for (Int_t iDet = 0; iDet < 7; iDet++)
        detsAndOutTrees2[iDet] = fOutArrs[iDet];

    for (Int_t iEntry = 0; iEntry < fIn1->GetEntries(); iEntry++) {
        fIn1->GetEntry(iEntry);

        if (iEntry % 10000 == 0)
            cout << "Event# " << iEntry << endl;

        for (auto it : detsAndOutTrees1)
            it.second->Delete();

        BmnEventHeader* header = (BmnEventHeader*) fHeader1->UncheckedAt(0);
        fHeaderOut->SetRunId(header->GetRunId());
        fHeaderOut->SetEventId(header->GetEventId());

        if (fGemDigits)
            for (UInt_t iDigi = 0; iDigi < fGemDigits->GetEntriesFast(); iDigi++) {
                BmnGemStripDigit* dig = (BmnGemStripDigit*) fGemDigits->UncheckedAt(iDigi);
                if (dig->GetStation() != 8)
                    new ((*fGem)[fGem->GetEntriesFast()]) BmnGemStripDigit(*dig);
                else {
                    BmnCSCDigit digCsc(0, dig->GetModule(), dig->GetStripLayer(), dig->GetStripNumber(), dig->GetStripSignal());
                    new ((*fCsc)[fCsc->GetEntriesFast()]) BmnCSCDigit(digCsc);
                }
            }

        if (fSilDigits)
            FillDetDigi <BmnSiliconDigit> (fSilDigits, fSil);

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
}

void BmnDigiMergeTask::ProcessEvents() {
    vector <UInt_t> nums;

    for (Int_t iDet = 0; iDet < nDets; iDet++)
        nums.push_back(fInFiles[iDet]->GetEntries());

    UInt_t maxEvNums = *max_element(nums.begin(), nums.end());
    fNevsInSample = 50; // FIXME
    const Int_t sampleFactor = Int_t(maxEvNums / fNevsInSample);

    TString detsRun6[] = {"GEM", "SILICON", "ZDC", "T0", "BC1", "BC2", "VETO", "FD", "BD", "MWPC", "DCH", "TOF400", "TOF700", "ECAL"};
    TString detsRun7[] = {"BC1", "BC2", "BC3", "VC", "Si", "BD", "ZDC", "GEM", "SILICON", "MWPC", "DCH", "TOF400", "TOF700", "ECAL", "CSC"};
    TString detsRun7Src[] = {"GEM", "SILICON", "CSC",
        "BC1", "BC2", "BC3", "BC4",
        "TQDC_BC1", "TQDC_BC2", "TQDC_BC3", "TQDC_BC4",
        "X1L", "X2L", "Y1L", "Y2L", "X1R", "X2R", "Y1R", "Y2R",
        "TQDC_X1L", "TQDC_X2L", "TQDC_Y1L", "TQDC_Y2L", "TQDC_X1R", "TQDC_X2R", "TQDC_Y1R", "TQDC_Y2R",
        "VETO", "TQDC_VETO",
        "TOF700", "ZDC",
        "MWPC", "DCH", "TOF400", "ECAL"};

    for (Int_t iSample = 0; iSample < sampleFactor; iSample++) {
        Int_t start = iSample * fNevsInSample;
        Int_t finish = (iSample + 1) * fNevsInSample;

        fCont = new BmnDigiContainer();

        Bool_t isEventHeaderMissedInSample = kFALSE;

        for (UInt_t iEntry = start; iEntry < finish; iEntry++) {
            if (iEntry % 1000 == 0)
                cout << "Event# " << iEntry << endl;

            // Get entry all in files
            for (Int_t iDet = 0; iDet < nDets; iDet++)
                fInFiles[iDet]->GetEntry(iEntry);

            // All triggers
            vector <BmnTrigDigit> bc1, bc2, bc3, bc4, vc, si, bd, t0, fd, x1l, x2l, y1l, y2l, x1r, x2r, y1r, y2r;
            vector <BmnTrigWaveDigit> tqdc_bc1, tqdc_bc2, tqdc_bc3, tqdc_bc4, tqdc_vc, tqdc_x1l, tqdc_x2l, tqdc_y1l, tqdc_y2l, tqdc_x1r, tqdc_x2r, tqdc_y1r, tqdc_y2r;

            // All detectors
            vector <BmnZDCDigit> zdc;
            vector <BmnGemStripDigit> gem;
            vector <BmnCSCDigit> csc;
            vector <BmnSiliconDigit> silicon;
            vector <BmnMwpcDigit> mwpc;
            vector <BmnDchDigit> dch;
            vector <BmnTof1Digit> tof400;
            vector <BmnTof2Digit> tof700;
            vector <BmnECALDigit> ecal;

            for (Int_t iDet = 0; iDet < nDets; iDet++) {
                if (fInArrs[iDet] && fInFiles[iDet]->GetEntries() != 0) {                   
                    if (iDet == 0)
                        PushDigiVectors <BmnTrigDigit, BmnGemStripDigit, BmnGemStripDigit> (iDet, bc1, gem, gem); // run7BM@N -> run6BM@N -> run7SRC
                    else if (iDet == 1)
                        PushDigiVectors <BmnTrigDigit, BmnSiliconDigit, BmnSiliconDigit> (iDet, bc2, silicon, silicon);
                    else if (iDet == 2)
                        PushDigiVectors <BmnTrigDigit, BmnZDCDigit, BmnCSCDigit> (iDet, bc3, zdc, csc);
                    else if (iDet == 3)
                        PushDigiVectors <BmnTrigDigit, BmnTrigDigit, BmnTrigDigit> (iDet, vc, t0, bc1);
                    else if (iDet == 4)
                        PushDigiVectors <BmnTrigDigit, BmnTrigDigit, BmnTrigDigit> (iDet, si, bc1, bc2);
                    else if (iDet == 5)
                        PushDigiVectors <BmnTrigDigit, BmnTrigDigit, BmnTrigDigit> (iDet, bd, bc2, bc3);
                    else if (iDet == 6)
                        PushDigiVectors <BmnZDCDigit, BmnTrigDigit, BmnTrigDigit> (iDet, zdc, vc, bc4);
                    else if (iDet == 7)
                        PushDigiVectors <BmnGemStripDigit, BmnTrigDigit, BmnTrigWaveDigit> (iDet, gem, fd, tqdc_bc1);
                    else if (iDet == 8)
                        PushDigiVectors <BmnSiliconDigit, BmnTrigDigit, BmnTrigWaveDigit> (iDet, silicon, bd, tqdc_bc2);
                    else if (iDet == 9)
                        PushDigiVectors <BmnMwpcDigit, BmnMwpcDigit, BmnTrigWaveDigit> (iDet, mwpc, mwpc, tqdc_bc3);
                    else if (iDet == 10)
                        PushDigiVectors <BmnDchDigit, BmnDchDigit, BmnTrigWaveDigit> (iDet, dch, dch, tqdc_bc4);
                    else if (iDet == 11)
                        PushDigiVectors <BmnTof1Digit, BmnTof1Digit, BmnTrigDigit> (iDet, tof400, tof400, x1l);
                    else if (iDet == 12)
                        PushDigiVectors <BmnTof2Digit, BmnTof2Digit, BmnTrigDigit> (iDet, tof700, tof700, x2l);
                    else if (iDet == 13)
                        PushDigiVectors <BmnECALDigit, BmnECALDigit, BmnTrigDigit> (iDet, ecal, ecal, y1l);
                    else if (iDet == 14)
                        PushDigiVectors <BmnCSCDigit, BmnTrigDigit> (iDet, csc, y2l);
                    else if (iDet == 15)
                        PushDigiVectors <BmnTrigDigit> (iDet, x1r);
                    else if (iDet == 16)
                        PushDigiVectors <BmnTrigDigit> (iDet, x2r);
                    else if (iDet == 17)
                        PushDigiVectors <BmnTrigDigit> (iDet, y1r);
                    else if (iDet == 18)
                        PushDigiVectors <BmnTrigDigit> (iDet, y2r);
                    else if (iDet == 19)
                        PushDigiVectors <BmnTrigWaveDigit> (iDet, tqdc_x1l);
                    else if (iDet == 20)
                        PushDigiVectors <BmnTrigWaveDigit> (iDet, tqdc_x2l);
                    else if (iDet == 21)
                        PushDigiVectors <BmnTrigWaveDigit> (iDet, tqdc_y1l);
                    else if (iDet == 22)
                        PushDigiVectors <BmnTrigWaveDigit> (iDet, tqdc_y2l);
                    else if (iDet == 23)
                        PushDigiVectors <BmnTrigWaveDigit> (iDet, tqdc_x1r);
                    else if (iDet == 24)
                        PushDigiVectors <BmnTrigWaveDigit> (iDet, tqdc_x2r);
                    else if (iDet == 25)
                        PushDigiVectors <BmnTrigWaveDigit> (iDet, tqdc_y1r);
                    else if (iDet == 26)
                        PushDigiVectors <BmnTrigWaveDigit> (iDet, tqdc_y2r);
                    else if (iDet == 27)
                        PushDigiVectors <BmnTrigDigit> (iDet, vc);
                    else if (iDet == 28)
                        PushDigiVectors <BmnTrigWaveDigit> (iDet, tqdc_vc);
                    else if (iDet == 29)
                        PushDigiVectors <BmnTof2Digit> (iDet, tof700);
                    else if (iDet == 30)
                        PushDigiVectors <BmnZDCDigit> (iDet, zdc);
                    else if (iDet == 31)
                        PushDigiVectors <BmnMwpcDigit> (iDet, mwpc);
                    else if (iDet == 32)
                        PushDigiVectors <BmnDchDigit> (iDet, dch);
                    else if (iDet == 33)
                        PushDigiVectors <BmnTof1Digit> (iDet, tof400);
                    else if (iDet == 34)
                        PushDigiVectors <BmnECALDigit> (iDet, ecal);
                }
            }

            for (Int_t iDet = 0; iDet < nDets; iDet++) {
                BmnEventHeader* header = fHeaders[iDet];
                if (!fInArrs[iDet] || !header) {
                    isEventHeaderMissedInSample = kTRUE;
                    continue;
                }
                TString currDet = (isRun7 && !isSrc) ? detsRun7[iDet] : (isRun6 && !isSrc) ? detsRun6[iDet] : detsRun7Src[iDet];
                fCont->SetEventHeadersPerEachDetector(currDet, header->GetEventId(), *header);
                fCont->SetDigi(currDet, header->GetEventId(),
                        bc1, bc2, bc3, bc4, vc, si, bd, t0, fd, x1l, x2l, y1l, y2l, x1r, x2r, y1r, y2r,
                        tqdc_bc1, tqdc_bc2, tqdc_bc3, tqdc_bc4, tqdc_vc, tqdc_x1l, tqdc_x2l, tqdc_y1l, tqdc_y2l, tqdc_x1r, tqdc_x2r, tqdc_y1r, tqdc_y2r,
                        zdc, gem, csc, silicon, mwpc, dch, tof400, tof700, ecal);
            }
        }
        GlueEventsFromInputFiles(isEventHeaderMissedInSample, start, finish + 1);
        delete fCont;
    }
}

void BmnDigiMergeTask::GlueEventsFromInputFiles(Bool_t flag, UInt_t s, UInt_t f) {
    map <pair <UInt_t, TString>, BmnEventHeader> headMap = fCont->GetEventHeaderMap();

    TString detWithEventHeader = flag ? "GEM" : (isRun7) ? "BC1" : "ZDC";

    for (UInt_t iEvId = s - Int_t(fNevsInSample / 2); iEvId < f + Int_t(fNevsInSample / 2); iEvId++) {
        fBC1->Delete();
        fBC2->Delete();
        fVeto->Delete();
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
            fBd->Delete();
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

        // Get EventHeader of the latest version
        for (auto it : headMap) {
            if (it.first.first != iEvId)
                continue;

            if (it.first.second.Contains(detWithEventHeader.Data())) {
                BmnEventHeader head = it.second;
                fHeaderOut->SetRunId(head.GetRunId());
                fHeaderOut->SetEventId(it.first.first);
            }
        }

        for (Int_t iDet = 0; iDet < nDets; iDet++) {
            if (iDet == 0)
                FillDigisOverRuns <BmnTrigDigit, BmnGemStripDigit, BmnGemStripDigit>
                    (iEvId, fCont->GetTrigDigi("BC1"), fCont->GetGemDigi(), fCont->GetGemDigi(), fBC1, fGem, fGem);

            else if (iDet == 1)
                FillDigisOverRuns <BmnTrigDigit, BmnSiliconDigit, BmnSiliconDigit>
                    (iEvId, fCont->GetTrigDigi("BC2"), fCont->GetSiliconDigi(), fCont->GetSiliconDigi(), fBC2, fSil, fSil);

            else if (iDet == 2)
                FillDigisOverRuns <BmnTrigDigit, BmnZDCDigit, BmnCSCDigit>
                    (iEvId, fCont->GetTrigDigi("BC3"), fCont->GetZdcDigi(), fCont->GetCscDigi(), fBC3, fZdc, fCsc);

            else if (iDet == 3)
                FillDigisOverRuns <BmnTrigDigit, BmnTrigDigit, BmnTrigDigit>
                    (iEvId, fCont->GetTrigDigi("VETO"), fCont->GetTrigDigi("T0"), fCont->GetTrigDigi("BC1"), fVeto, fT0, fBC1);

            else if (iDet == 4)
                FillDigisOverRuns <BmnTrigDigit, BmnTrigDigit, BmnTrigDigit>
                    (iEvId, fCont->GetTrigDigi("SI"), fCont->GetTrigDigi("BC1"), fCont->GetTrigDigi("BC2"), fSi, fBC1, fBC2);

            else if (iDet == 5)
                FillDigisOverRuns <BmnTrigDigit, BmnTrigDigit, BmnTrigDigit>
                    (iEvId, fCont->GetTrigDigi("BD"), fCont->GetTrigDigi("BC2"), fCont->GetTrigDigi("BC3"), fBd, fBC2, fBC3);

            else if (iDet == 6)
                FillDigisOverRuns <BmnZDCDigit, BmnTrigDigit, BmnTrigDigit>
                    (iEvId, fCont->GetZdcDigi(), fCont->GetTrigDigi("VETO"), fCont->GetTrigDigi("BC4"), fZdc, fVeto, fBC4);

            else if (iDet == 7)
                FillDigisOverRuns <BmnGemStripDigit, BmnTrigDigit, BmnTrigWaveDigit>
                    (iEvId, fCont->GetGemDigi(), fCont->GetTrigDigi("FD"), fCont->GetTqdcTrigDigi("BC1"), fGem, fFd, fBC1TQDC);

            else if (iDet == 8)
                FillDigisOverRuns <BmnSiliconDigit, BmnTrigDigit, BmnTrigWaveDigit>
                    (iEvId, fCont->GetSiliconDigi(), fCont->GetTrigDigi("BD"), fCont->GetTqdcTrigDigi("BC2"), fSil, fBd, fBC2TQDC);

            else if (iDet == 9)
                FillDigisOverRuns <BmnMwpcDigit, BmnMwpcDigit, BmnTrigWaveDigit>
                    (iEvId, fCont->GetMwpcDigi(), fCont->GetMwpcDigi(), fCont->GetTqdcTrigDigi("BC3"), fMwpc, fMwpc, fBC3TQDC);

            else if (iDet == 10)
                FillDigisOverRuns <BmnDchDigit, BmnDchDigit, BmnTrigWaveDigit>
                    (iEvId, fCont->GetDchDigi(), fCont->GetDchDigi(), fCont->GetTqdcTrigDigi("BC4"), fDch, fDch, fBC4TQDC);

            else if (iDet == 11)
                FillDigisOverRuns <BmnTof1Digit, BmnTof1Digit, BmnTrigDigit>
                    (iEvId, fCont->GetTof400Digi(), fCont->GetTof400Digi(), fCont->GetTrigDigi("X1L"), fTof400, fTof400, fX1L);

            else if (iDet == 12)
                FillDigisOverRuns <BmnTof2Digit, BmnTof2Digit, BmnTrigDigit>
                    (iEvId, fCont->GetTof700Digi(), fCont->GetTof700Digi(), fCont->GetTrigDigi("X2L"), fTof700, fTof700, fX2L);

            else if (iDet == 13)
                FillDigisOverRuns <BmnECALDigit, BmnECALDigit, BmnTrigDigit>
                    (iEvId, fCont->GetEcalDigi(), fCont->GetEcalDigi(), fCont->GetTrigDigi("Y1L"), fEcal, fEcal, fY1L);
            else if (iDet == 14)
                FillDigisOverRuns <BmnCSCDigit, BmnTrigDigit>
                    (iEvId, fCont->GetCscDigi(), fCont->GetTrigDigi("Y2L"), fCsc, fY2L);
            else if (iDet == 15)
                FillDetDigi <BmnTrigDigit>
                    (iEvId, fCont->GetTrigDigi("X1R"), fX1R);
            else if (iDet == 16)
                FillDetDigi <BmnTrigDigit>
                    (iEvId, fCont->GetTrigDigi("X2R"), fX2R);
            else if (iDet == 17)
                FillDetDigi <BmnTrigDigit>
                    (iEvId, fCont->GetTrigDigi("Y1R"), fY1R);
            else if (iDet == 18)
                FillDetDigi <BmnTrigDigit>
                    (iEvId, fCont->GetTrigDigi("Y2R"), fY2R);
            else if (iDet == 19)
                FillDetDigi <BmnTrigWaveDigit>
                    (iEvId, fCont->GetTqdcTrigDigi("X1L"), fX1LTQDC);
            else if (iDet == 20)
                FillDetDigi <BmnTrigWaveDigit>
                    (iEvId, fCont->GetTqdcTrigDigi("X2L"), fX2LTQDC);
            else if (iDet == 21)
                FillDetDigi <BmnTrigWaveDigit>
                    (iEvId, fCont->GetTqdcTrigDigi("Y1L"), fY1LTQDC);
            else if (iDet == 22)
                FillDetDigi <BmnTrigWaveDigit>
                    (iEvId, fCont->GetTqdcTrigDigi("Y2L"), fY2LTQDC);
            else if (iDet == 23)
                FillDetDigi <BmnTrigWaveDigit>
                    (iEvId, fCont->GetTqdcTrigDigi("X1R"), fX1RTQDC);
            else if (iDet == 24)
                FillDetDigi <BmnTrigWaveDigit>
                    (iEvId, fCont->GetTqdcTrigDigi("X2R"), fX2RTQDC);
            else if (iDet == 25)
                FillDetDigi <BmnTrigWaveDigit>
                    (iEvId, fCont->GetTqdcTrigDigi("Y1R"), fY1RTQDC);
            else if (iDet == 26)
                FillDetDigi <BmnTrigWaveDigit>
                    (iEvId, fCont->GetTqdcTrigDigi("Y2R"), fY2RTQDC);
            else if (iDet == 27)
                FillDetDigi <BmnTrigDigit>
                    (iEvId, fCont->GetTrigDigi("VETO"), fVeto);
            else if (iDet == 28)
                FillDetDigi <BmnTrigWaveDigit>
                    (iEvId, fCont->GetTqdcTrigDigi("VETO"), fVetoTQDC);
            else if (iDet == 29)
                FillDetDigi <BmnTof2Digit>
                    (iEvId, fCont->GetTof700Digi(), fTof700);
            else if (iDet == 30)
                FillDetDigi <BmnZDCDigit>
                    (iEvId, fCont->GetZdcDigi(), fZdc);
            else if (iDet == 31)
                FillDetDigi <BmnMwpcDigit>
                    (iEvId, fCont->GetMwpcDigi(), fMwpc);
            else if (iDet == 32)
                FillDetDigi <BmnDchDigit>
                    (iEvId, fCont->GetDchDigi(), fDch);
            else if (iDet == 33)
                FillDetDigi <BmnTof1Digit>
                    (iEvId, fCont->GetTof400Digi(), fTof400);
            else if (iDet == 34)
                FillDetDigi <BmnECALDigit>
                    (iEvId, fCont->GetEcalDigi(), fEcal);
        }

        if (IsArraysEmpty())
            continue;

        fOutTree->Fill();
    }
}

Bool_t BmnDigiMergeTask::IsArraysEmpty() {
    if (fBC1->GetEntriesFast() == 0 &&
            fBC2->GetEntriesFast() == 0 &&
            fVeto->GetEntriesFast() == 0 &&
            fZdc->GetEntriesFast() == 0 &&
            fGem->GetEntriesFast() == 0 &&
            fSil->GetEntriesFast() == 0 &&
            fMwpc->GetEntriesFast() == 0 &&
            fDch->GetEntriesFast() == 0 &&
            fTof400->GetEntriesFast() == 0 &&
            fTof700->GetEntriesFast() == 0 &&
            fEcal->GetEntriesFast() == 0 &&

            (isRun6 && !isSrc) ? (fT0->GetEntriesFast() == 0 && fFd->GetEntriesFast() == 0) :
            (isRun7 && !isSrc) ? (fBC3->GetEntriesFast() == 0 && fBd->GetEntriesFast() == 0 && fSi->GetEntriesFast() == 0 && fCsc->GetEntriesFast() == 0) :

            (fCsc->GetEntriesFast() == 0 &&
            fBC3->GetEntriesFast() == 0 &&
            fBC4->GetEntriesFast() == 0 &&

            fVetoTQDC->GetEntriesFast() == 0 &&

            fX1L->GetEntriesFast() == 0 &&
            fX2L->GetEntriesFast() == 0 &&
            fY1L->GetEntriesFast() == 0 &&
            fY2L->GetEntriesFast() == 0 &&
            fX1R->GetEntriesFast() == 0 &&
            fX2R->GetEntriesFast() == 0 &&
            fY1R->GetEntriesFast() == 0 &&
            fY2R->GetEntriesFast() == 0 &&

            fX1LTQDC->GetEntriesFast() == 0 &&
            fX2LTQDC->GetEntriesFast() == 0 &&
            fY1LTQDC->GetEntriesFast() == 0 &&
            fY2LTQDC->GetEntriesFast() == 0 &&
            fX1RTQDC->GetEntriesFast() == 0 &&
            fX2RTQDC->GetEntriesFast() == 0 &&
            fY1RTQDC->GetEntriesFast() == 0 &&
            fY2RTQDC->GetEntriesFast() == 0 &&

            fBC1TQDC->GetEntriesFast() == 0 &&
            fBC2TQDC->GetEntriesFast() == 0 &&
            fBC3TQDC->GetEntriesFast() == 0 &&
            fBC4TQDC->GetEntriesFast() == 0))
        return kTRUE;
    else
        return kFALSE;
}
