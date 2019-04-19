#include "BmnDigiMergeTask.h"

// RUN-6

BmnDigiMergeTask::BmnDigiMergeTask(TString in1, TString in2, TString in3) :
isRun6(kTRUE),
isRun7(kFALSE),
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

BmnDigiMergeTask::BmnDigiMergeTask(vector<TString> files, TString out) :
isRun6(kFALSE),
isRun7(kFALSE),
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

    if (!isRun6 && !isRun7)
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

    if (isRun6) {
        fT0Digits = new TClonesArray("BmnTrigDigit");
        fFdDigits = new TClonesArray("BmnTrigDigit");
    }

    if (isRun7) {
        fBC3Digits = new TClonesArray("BmnTrigDigit");
        fSiDigits = new TClonesArray("BmnTrigDigit");
        fCscDigits = new TClonesArray("BmnCSCDigit");
    }

    TString detsRun6[] = {"GEM", "SILICON", "ZDC", "T0", "BC1", "BC2", "VETO", "FD", "BD", "MWPC", "DCH", "TOF400", "TOF700", "ECAL"};
    TString detsRun7[] = {"BC1", "BC2", "BC3", "VC", "Si", "BD", "ZDC", "GEM", "SILICON", "MWPC", "DCH", "TOF400", "TOF700", "ECAL", "CSC"};

    TClonesArray * inArrsRun6[] = {fGemDigits, fSilDigits, fZdcDigits, fT0Digits, fBC1Digits, fBC2Digits, fVetoDigits, fFdDigits, fBdDigits,
        fMwpcDigits, fDchDigits, fTof400Digits, fTof700Digits, fEcalDigits};

    TClonesArray * inArrsRun7[] = {fBC1Digits, fBC2Digits, fBC3Digits, fVetoDigits, fSiDigits, fBdDigits, fZdcDigits,
        fGemDigits, fSilDigits, fMwpcDigits, fDchDigits, fTof400Digits, fTof700Digits, fEcalDigits, fCscDigits};

    fInArrs = new TClonesArray*[nDets];
    fHeaders = new TClonesArray*[nDets];
    fInFiles = new TChain*[nDets];

    for (Int_t iDet = 0; iDet < nDets; iDet++) {
        fHeaders[iDet] = new TClonesArray("BmnEventHeader");
        fInArrs[iDet] = (isRun6) ? inArrsRun6[iDet] : inArrsRun7[iDet];
        fInFiles[iDet] = new TChain("bmndata");
        fInFiles[iDet]->Add(files[iDet].Data());
        fInFiles[iDet]->SetBranchAddress((isRun6 ? detsRun6[iDet] : detsRun7[iDet]).Data(), &fInArrs[iDet]);
        fInFiles[iDet]->SetBranchAddress("EventHeader", &fHeaders[iDet]);
    }

    CreateOutputFile(out);
}

void BmnDigiMergeTask::CreateOutputFiles() {
    fIn1->GetEntry(0);
    BmnEventHeader* evHead = (BmnEventHeader*) fHeader1->UncheckedAt(0);
    UInt_t runId = evHead->GetRunId();

    nDets = isRun7 ? 15 : isRun6 ? 14 : throw;

    TString detsRun6[] = {"GEM", "SILICON", "ZDC", "T0", "BC1", "BC2", "VETO", "FD", "BD", "MWPC", "DCH", "TOF400", "TOF700", "ECAL"};
    TString detsRun7[] = {"BC1", "BC2", "BC3", "VC", "Si", "BD", "ZDC", "GEM", "SILICON", "MWPC", "DCH", "TOF400", "TOF700", "ECAL", "CSC"};

    fOutFiles = new TFile*[nDets];
    fOutTrees = new TTree*[nDets];

    fBC1 = new TClonesArray("BmnTrigDigit");
    fBC2 = new TClonesArray("BmnTrigDigit");
    fVeto = new TClonesArray("BmnTrigDigit");
    fBd = new TClonesArray("BmnTrigDigit");
    fGem = new TClonesArray("BmnGemStripDigit");
    fSil = new TClonesArray("BmnSiliconDigit");
    fMwpc = new TClonesArray("BmnMwpcDigit");
    fDch = new TClonesArray("BmnDchDigit");
    fTof400 = new TClonesArray("BmnTof1Digit");
    fTof700 = new TClonesArray("BmnTof2Digit");
    fEcal = new TClonesArray("BmnECALDigit");
    fZdc = new TClonesArray("BmnZDCDigit");

    if (isRun6) {
        fT0 = new TClonesArray("BmnTrigDigit");
        fFd = new TClonesArray("BmnTrigDigit");
    }

    if (isRun7) {
        fBC3 = new TClonesArray("BmnTrigDigit");
        fSi = new TClonesArray("BmnTrigDigit");
        fCsc = new TClonesArray("BmnCSCDigit");
    }

    fHeaderOut = new TClonesArray("BmnEventHeader");

    TClonesArray * arrRun6[] = {fGem, fSil, fZdc, fT0, fBC1, fBC2, fVeto, fFd, fBd, fMwpc, fDch, fTof400, fTof700, fEcal};
    TClonesArray * arrRun7[] = {fBC1, fBC2, fBC3, fVeto, fSi, fBd, fZdc, fGem, fSil, fMwpc, fDch, fTof400, fTof700, fEcal, fCsc};

    fOutArrs = new TClonesArray*[nDets];
    for (Int_t iDet = 0; iDet < nDets; iDet++)
        fOutArrs[iDet] = isRun6 ? arrRun6[iDet] : arrRun7[iDet];

    for (Int_t iDet = 0; iDet < nDets; iDet++) {
        fOutFiles[iDet] = new TFile(TString((isRun6 ? detsRun6[iDet] : detsRun7[iDet]) + TString::Format("_%d.root", runId)).Data(), "recreate");
        fOutTrees[iDet] = new TTree("bmndata", "bmndata");
        fOutTrees[iDet]->Branch("EventHeader", &fHeaderOut);
        fOutTrees[iDet]->Branch((isRun6 ? detsRun6[iDet] : detsRun7[iDet]).Data(), &fOutArrs[iDet]);
    }
}

void BmnDigiMergeTask::CreateOutputFile(TString out) {
    fOutFile = new TFile(out.Data(), "recreate");
    fOutTree = new TTree("bmndata", "bmndata");

    fHeaderOut = new TClonesArray("BmnEventHeader");
    fOutTree->Branch("EventHeader", &fHeaderOut);

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


    if (isRun6) {
        fT0 = new TClonesArray("BmnTrigDigit");
        fOutTree->Branch("T0", &fT0);
        fFd = new TClonesArray("BmnTrigDigit");
        fOutTree->Branch("FD", &fFd);
    }

    if (isRun7) {
        fCsc = new TClonesArray("BmnCSCDigit");
        fOutTree->Branch("CSC", &fCsc);
        fBC3 = new TClonesArray("BmnTrigDigit");
        fOutTree->Branch("BC3", &fBC3);
        fSi = new TClonesArray("BmnTrigDigit");
        fOutTree->Branch("Si", &fSi);
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

        fHeaderOut->Delete();

        BmnEventHeader* header = (BmnEventHeader*) fHeader1->UncheckedAt(0);
        new ((*fHeaderOut)[fHeaderOut->GetEntriesFast()]) BmnEventHeader(*header);

        if (fGemDigits)
            for (UInt_t iDigi = 0; iDigi < fGemDigits->GetEntriesFast(); iDigi++) {
                BmnGemStripDigit* dig = (BmnGemStripDigit*) fGemDigits->UncheckedAt(iDigi);
                new ((*fGem)[fGem->GetEntriesFast()]) BmnGemStripDigit(*dig);
            }

        if (fSilDigits)
            for (UInt_t iDigi = 0; iDigi < fSilDigits->GetEntriesFast(); iDigi++) {
                BmnSiliconDigit* dig = (BmnSiliconDigit*) fSilDigits->UncheckedAt(iDigi);
                new ((*fSil)[fSil->GetEntriesFast()]) BmnSiliconDigit(*dig);
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

        fHeaderOut->Delete();

        BmnEventHeader* header = (BmnEventHeader*) fHeader2->UncheckedAt(0);
        new ((*fHeaderOut)[fHeaderOut->GetEntriesFast()]) BmnEventHeader(*header);

        if (fZdcDigits)
            for (UInt_t iDigi = 0; iDigi < fZdcDigits->GetEntriesFast(); iDigi++) {
                BmnZDCDigit* dig = (BmnZDCDigit*) fZdcDigits->UncheckedAt(iDigi);
                new ((*fZdc)[fZdc->GetEntriesFast()]) BmnZDCDigit(*dig);
            }
        for (auto it : detsAndOutTrees2)
            fOutTrees[it.first]->Fill();
    }

    for (Int_t iEntry = 0; iEntry < fIn3->GetEntries(); iEntry++) {
        fIn3->GetEntry(iEntry);

        if (iEntry % 10000 == 0)
            cout << "Event# " << iEntry << endl;

        for (auto it : detsAndOutTrees3)
            it.second->Delete();

        fHeaderOut->Delete();

        BmnEventHeader* header = (BmnEventHeader*) fHeader3->UncheckedAt(0);
        new ((*fHeaderOut)[fHeaderOut->GetEntriesFast()]) BmnEventHeader(*header);

        if (fT0Digits)
            for (UInt_t iDigi = 0; iDigi < fT0Digits->GetEntriesFast(); iDigi++) {
                BmnTrigDigit* dig = (BmnTrigDigit*) fT0Digits->UncheckedAt(iDigi);
                new ((*fT0)[fT0->GetEntriesFast()]) BmnTrigDigit(*dig);
            }

        if (fFdDigits)
            for (UInt_t iDigi = 0; iDigi < fFdDigits->GetEntriesFast(); iDigi++) {
                BmnTrigDigit* dig = (BmnTrigDigit*) fFdDigits->UncheckedAt(iDigi);
                new ((*fFd)[fFd->GetEntriesFast()]) BmnTrigDigit(*dig);
            }

        if (fBC1Digits)
            for (UInt_t iDigi = 0; iDigi < fBC1Digits->GetEntriesFast(); iDigi++) {
                BmnTrigDigit* dig = (BmnTrigDigit*) fBC1Digits->UncheckedAt(iDigi);
                new ((*fBC1)[fBC1->GetEntriesFast()]) BmnTrigDigit(*dig);
            }

        if (fBC2Digits)
            for (UInt_t iDigi = 0; iDigi < fBC2Digits->GetEntriesFast(); iDigi++) {
                BmnTrigDigit* dig = (BmnTrigDigit*) fBC2Digits->UncheckedAt(iDigi);
                new ((*fBC2)[fBC2->GetEntriesFast()]) BmnTrigDigit(*dig);
            }

        if (fVetoDigits)
            for (UInt_t iDigi = 0; iDigi < fVetoDigits->GetEntriesFast(); iDigi++) {
                BmnTrigDigit* dig = (BmnTrigDigit*) fVetoDigits->UncheckedAt(iDigi);
                new ((*fVeto)[fVeto->GetEntriesFast()]) BmnTrigDigit(*dig);
            }

        if (fBdDigits)
            for (UInt_t iDigi = 0; iDigi < fBdDigits->GetEntriesFast(); iDigi++) {
                BmnTrigDigit* dig = (BmnTrigDigit*) fBdDigits->UncheckedAt(iDigi);
                new ((*fBd)[fBd->GetEntriesFast()]) BmnTrigDigit(*dig);
            }

        if (fMwpcDigits)
            for (UInt_t iDigi = 0; iDigi < fMwpcDigits->GetEntriesFast(); iDigi++) {
                BmnMwpcDigit* dig = (BmnMwpcDigit*) fMwpcDigits->UncheckedAt(iDigi);
                new ((*fMwpc)[fMwpc->GetEntriesFast()]) BmnMwpcDigit(*dig);
            }

        if (fDchDigits)
            for (UInt_t iDigi = 0; iDigi < fDchDigits->GetEntriesFast(); iDigi++) {
                BmnDchDigit* dig = (BmnDchDigit*) fDchDigits->UncheckedAt(iDigi);
                new ((*fDch)[fDch->GetEntriesFast()]) BmnDchDigit(*dig);
            }

        if (fTof400Digits)
            for (UInt_t iDigi = 0; iDigi < fTof400Digits->GetEntriesFast(); iDigi++) {
                BmnTof1Digit* dig = (BmnTof1Digit*) fTof400Digits->UncheckedAt(iDigi);
                new ((*fTof400)[fTof400->GetEntriesFast()]) BmnTof1Digit(*dig);
            }

        if (fTof700Digits)
            for (UInt_t iDigi = 0; iDigi < fTof700Digits->GetEntriesFast(); iDigi++) {
                BmnTof2Digit* dig = (BmnTof2Digit*) fTof700Digits->UncheckedAt(iDigi);
                new ((*fTof700)[fTof700->GetEntriesFast()]) BmnTof2Digit(*dig);
            }

        if (fEcalDigits)
            for (UInt_t iDigi = 0; iDigi < fEcalDigits->GetEntriesFast(); iDigi++) {
                BmnECALDigit* dig = (BmnECALDigit*) fEcalDigits->UncheckedAt(iDigi);
                new ((*fEcal)[fEcal->GetEntriesFast()]) BmnECALDigit(*dig);
            }
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

        fHeaderOut->Delete();

        BmnEventHeader* header = (BmnEventHeader*) fHeader1->UncheckedAt(0);
        new ((*fHeaderOut)[fHeaderOut->GetEntriesFast()]) BmnEventHeader(*header);

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
            for (UInt_t iDigi = 0; iDigi < fSilDigits->GetEntriesFast(); iDigi++) {
                BmnSiliconDigit* dig = (BmnSiliconDigit*) fSilDigits->UncheckedAt(iDigi);
                new ((*fSil)[fSil->GetEntriesFast()]) BmnSiliconDigit(*dig);
            }

        if (fMwpcDigits)
            for (UInt_t iDigi = 0; iDigi < fMwpcDigits->GetEntriesFast(); iDigi++) {
                BmnMwpcDigit* dig = (BmnMwpcDigit*) fMwpcDigits->UncheckedAt(iDigi);
                new ((*fMwpc)[fMwpc->GetEntriesFast()]) BmnMwpcDigit(*dig);
            }

        if (fDchDigits)
            for (UInt_t iDigi = 0; iDigi < fDchDigits->GetEntriesFast(); iDigi++) {
                BmnDchDigit* dig = (BmnDchDigit*) fDchDigits->UncheckedAt(iDigi);
                new ((*fDch)[fDch->GetEntriesFast()]) BmnDchDigit(*dig);
            }

        if (fTof400Digits)
            for (UInt_t iDigi = 0; iDigi < fTof400Digits->GetEntriesFast(); iDigi++) {
                BmnTof1Digit* dig = (BmnTof1Digit*) fTof400Digits->UncheckedAt(iDigi);
                new ((*fTof400)[fTof400->GetEntriesFast()]) BmnTof1Digit(*dig);
            }

        if (fTof700Digits)
            for (UInt_t iDigi = 0; iDigi < fTof700Digits->GetEntriesFast(); iDigi++) {
                BmnTof2Digit* dig = (BmnTof2Digit*) fTof700Digits->UncheckedAt(iDigi);
                new ((*fTof700)[fTof700->GetEntriesFast()]) BmnTof2Digit(*dig);
            }

        if (fEcalDigits)
            for (UInt_t iDigi = 0; iDigi < fEcalDigits->GetEntriesFast(); iDigi++) {
                BmnECALDigit* dig = (BmnECALDigit*) fEcalDigits->UncheckedAt(iDigi);
                new ((*fEcal)[fEcal->GetEntriesFast()]) BmnECALDigit(*dig);
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

        fHeaderOut->Delete();

        BmnEventHeader* header = (BmnEventHeader*) fHeader2->UncheckedAt(0);
        // cout << header << endl;
        new ((*fHeaderOut)[fHeaderOut->GetEntriesFast()]) BmnEventHeader(*header);
        // h->SetRunId(header->GetRunId());
        // h->SetEventId(header.GetEventId());
        // h->SetEventTimeTS(header.GetEventTimeTS());
        //h->SetEventTime(header.GetEventTime());
        //h->SetType(header.GetType());
        //h->SetTripWord(header.GetTripWord());
        // h->SetTrigInfo(head.GetTrigInfo());
        //h->SetTimeShift(header.GetTimeShift());

        if (fBC1Digits)
            for (UInt_t iDigi = 0; iDigi < fBC1Digits->GetEntriesFast(); iDigi++) {
                BmnTrigDigit* dig = (BmnTrigDigit*) fBC1Digits->UncheckedAt(iDigi);
                new ((*fBC1)[fBC1->GetEntriesFast()]) BmnTrigDigit(*dig);
            }

        if (fBC2Digits)
            for (UInt_t iDigi = 0; iDigi < fBC2Digits->GetEntriesFast(); iDigi++) {
                BmnTrigDigit* dig = (BmnTrigDigit*) fBC2Digits->UncheckedAt(iDigi);
                new ((*fBC2)[fBC2->GetEntriesFast()]) BmnTrigDigit(*dig);
            }

        if (fBC3Digits)
            for (UInt_t iDigi = 0; iDigi < fBC3Digits->GetEntriesFast(); iDigi++) {
                BmnTrigDigit* dig = (BmnTrigDigit*) fBC3Digits->UncheckedAt(iDigi);
                new ((*fBC3)[fBC3->GetEntriesFast()]) BmnTrigDigit(*dig);
            }

        if (fVetoDigits)
            for (UInt_t iDigi = 0; iDigi < fVetoDigits->GetEntriesFast(); iDigi++) {
                BmnTrigDigit* dig = (BmnTrigDigit*) fVetoDigits->UncheckedAt(iDigi);
                new ((*fVeto)[fVeto->GetEntriesFast()]) BmnTrigDigit(*dig);
            }

        if (fSiDigits)
            for (UInt_t iDigi = 0; iDigi < fSiDigits->GetEntriesFast(); iDigi++) {
                BmnTrigDigit* dig = (BmnTrigDigit*) fSiDigits->UncheckedAt(iDigi);
                new ((*fSi)[fSi->GetEntriesFast()]) BmnTrigDigit(*dig);
            }

        if (fBdDigits)
            for (UInt_t iDigi = 0; iDigi < fBdDigits->GetEntriesFast(); iDigi++) {
                BmnTrigDigit* dig = (BmnTrigDigit*) fBdDigits->UncheckedAt(iDigi);
                new ((*fBd)[fBd->GetEntriesFast()]) BmnTrigDigit(*dig);
            }

        if (fZdcDigits)
            for (UInt_t iDigi = 0; iDigi < fZdcDigits->GetEntriesFast(); iDigi++) {
                BmnZDCDigit* dig = (BmnZDCDigit*) fZdcDigits->UncheckedAt(iDigi);
                new ((*fZdc)[fZdc->GetEntriesFast()]) BmnZDCDigit(*dig);
            }

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

            vector <BmnTrigDigit> bc1;
            vector <BmnTrigDigit> bc2;
            vector <BmnTrigDigit> bc3;
            vector <BmnTrigDigit> vc;
            vector <BmnTrigDigit> si;
            vector <BmnTrigDigit> bd;
            vector <BmnZDCDigit> zdc;
            vector <BmnGemStripDigit> gem;
            vector <BmnCSCDigit> csc;
            vector <BmnSiliconDigit> silicon;
            vector <BmnMwpcDigit> mwpc;
            vector <BmnDchDigit> dch;
            vector <BmnTof1Digit> tof400;
            vector <BmnTof2Digit> tof700;
            vector <BmnECALDigit> ecal;
            vector <BmnTrigDigit> t0;
            vector <BmnTrigDigit> fd;

            for (Int_t iDet = 0; iDet < nDets; iDet++) {
                if (fInArrs[iDet] && fInFiles[iDet]->GetEntries() != 0)
                    for (UInt_t iDigi = 0; iDigi < fInArrs[iDet]->GetEntriesFast(); iDigi++) {
                        if (iDet == 0) {
                            if (isRun7) {
                                BmnTrigDigit* dig = (BmnTrigDigit*) fInArrs[iDet]->UncheckedAt(iDigi);
                                bc1.push_back(*dig);
                            } else {
                                BmnGemStripDigit* dig = (BmnGemStripDigit*) fInArrs[iDet]->UncheckedAt(iDigi);
                                gem.push_back(*dig);
                            }
                        } else if (iDet == 1) {
                            if (isRun7) {
                                BmnTrigDigit* dig = (BmnTrigDigit*) fInArrs[iDet]->UncheckedAt(iDigi);
                                bc2.push_back(*dig);
                            } else {
                                BmnSiliconDigit* dig = (BmnSiliconDigit*) fInArrs[iDet]->UncheckedAt(iDigi);
                                silicon.push_back(*dig);
                            }
                        } else if (iDet == 2) {
                            if (isRun7) {
                                BmnTrigDigit* dig = (BmnTrigDigit*) fInArrs[iDet]->UncheckedAt(iDigi);
                                bc3.push_back(*dig);
                            } else {
                                BmnZDCDigit* dig = (BmnZDCDigit*) fInArrs[iDet]->UncheckedAt(iDigi);
                                zdc.push_back(*dig);
                            }
                        } else if (iDet == 3) {
                            if (isRun7) {
                                BmnTrigDigit* dig = (BmnTrigDigit*) fInArrs[iDet]->UncheckedAt(iDigi);
                                vc.push_back(*dig);
                            } else {
                                BmnTrigDigit* dig = (BmnTrigDigit*) fInArrs[iDet]->UncheckedAt(iDigi);
                                t0.push_back(*dig);
                            }
                        } else if (iDet == 4) {
                            if (isRun7) {
                                BmnTrigDigit* dig = (BmnTrigDigit*) fInArrs[iDet]->UncheckedAt(iDigi);
                                si.push_back(*dig);
                            } else {
                                BmnTrigDigit* dig = (BmnTrigDigit*) fInArrs[iDet]->UncheckedAt(iDigi);
                                bc1.push_back(*dig);
                            }
                        } else if (iDet == 5) {
                            if (isRun7) {
                                BmnTrigDigit* dig = (BmnTrigDigit*) fInArrs[iDet]->UncheckedAt(iDigi);
                                bd.push_back(*dig);
                            } else {
                                BmnTrigDigit* dig = (BmnTrigDigit*) fInArrs[iDet]->UncheckedAt(iDigi);
                                bc2.push_back(*dig);
                            }
                        } else if (iDet == 6) {
                            if (isRun7) {
                                BmnZDCDigit* dig = (BmnZDCDigit*) fInArrs[iDet]->UncheckedAt(iDigi);
                                zdc.push_back(*dig);
                            } else {
                                BmnTrigDigit* dig = (BmnTrigDigit*) fInArrs[iDet]->UncheckedAt(iDigi);
                                vc.push_back(*dig);
                            }
                        } else if (iDet == 7) {
                            if (isRun7) {
                                BmnGemStripDigit* dig = (BmnGemStripDigit*) fInArrs[iDet]->UncheckedAt(iDigi);
                                gem.push_back(*dig);
                            } else {
                                BmnTrigDigit* dig = (BmnTrigDigit*) fInArrs[iDet]->UncheckedAt(iDigi);
                                fd.push_back(*dig);
                            }
                        } else if (iDet == 8) {
                            if (isRun7) {
                                BmnSiliconDigit* dig = (BmnSiliconDigit*) fInArrs[iDet]->UncheckedAt(iDigi);
                                silicon.push_back(*dig);
                            } else {
                                BmnTrigDigit* dig = (BmnTrigDigit*) fInArrs[iDet]->UncheckedAt(iDigi);
                                bd.push_back(*dig);
                            }
                        } else if (iDet == 9) {
                            BmnMwpcDigit* dig = (BmnMwpcDigit*) fInArrs[iDet]->UncheckedAt(iDigi);
                            mwpc.push_back(*dig);
                        } else if (iDet == 10) {
                            BmnDchDigit* dig = (BmnDchDigit*) fInArrs[iDet]->UncheckedAt(iDigi);
                            dch.push_back(*dig);
                        } else if (iDet == 11) {
                            BmnTof1Digit* dig = (BmnTof1Digit*) fInArrs[iDet]->UncheckedAt(iDigi);
                            tof400.push_back(*dig);
                        } else if (iDet == 12) {
                            BmnTof2Digit* dig = (BmnTof2Digit*) fInArrs[iDet]->UncheckedAt(iDigi);
                            tof700.push_back(*dig);
                        } else if (iDet == 13) {
                            BmnECALDigit* dig = (BmnECALDigit*) fInArrs[iDet]->UncheckedAt(iDigi);
                            ecal.push_back(*dig);
                        } else if (iDet == 14) {
                            BmnCSCDigit* dig = (BmnCSCDigit*) fInArrs[iDet]->UncheckedAt(iDigi);
                            csc.push_back(*dig);
                        }
                    }
            }

            for (Int_t iDet = 0; iDet < nDets; iDet++) {
                BmnEventHeader* header = (BmnEventHeader*) fHeaders[iDet]->UncheckedAt(0);
                if (!fInArrs[iDet] || !header) {
                    isEventHeaderMissedInSample = kTRUE;
                    continue;
                }
                fCont->SetEventHeadersPerEachDetector(isRun7 ? detsRun7[iDet] : detsRun6[iDet], header->GetEventId(), *header);
                fCont->SetDigi(isRun7 ? detsRun7[iDet] : detsRun6[iDet], header->GetEventId(),
                        gem, csc, silicon, zdc, bc1, bc2, bc3, vc, bd, si, mwpc, dch, tof400, tof700, ecal, t0, fd);
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
        fHeaderOut->Delete();
        fGem->Delete();
        fSil->Delete();
        fZdc->Delete();
        fBC1->Delete();
        fBC2->Delete();
        fVeto->Delete();
        fBd->Delete();
        fMwpc->Delete();
        fDch->Delete();
        fTof400->Delete();
        fTof700->Delete();
        fEcal->Delete();

        if (isRun6) {
            fT0->Delete();
            fFd->Delete();
        }

        if (isRun7) {
            fBC3->Delete();
            fSi->Delete();
            fCsc->Delete();
        }

        // Get EventHeader of the latest version
        for (auto it : headMap) {
            if (it.first.first != iEvId)
                continue;

            if (it.first.second.Contains(detWithEventHeader.Data())) {
                BmnEventHeader head = it.second;
                BmnEventHeader* h = new ((*fHeaderOut)[fHeaderOut->GetEntriesFast()]) BmnEventHeader();
                h->SetRunId(head.GetRunId());
                h->SetEventId(it.first.first);
            }
        }

        for (Int_t iDet = 0; iDet < nDets; iDet++) {
            if (iDet == 0) {
                if (isRun7) {
                    for (auto it : fCont->GetBc1Digi()) {
                        if (it.first != iEvId)
                            continue;

                        for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                            new ((*fBC1)[fBC1->GetEntriesFast()]) BmnTrigDigit(it.second[iDig]);
                    }
                } else {
                    for (auto it : fCont->GetGemDigi()) {
                        if (it.first != iEvId)
                            continue;

                        for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                            new ((*fGem)[fGem->GetEntriesFast()]) BmnGemStripDigit(it.second[iDig]);
                    }
                }
            } else if (iDet == 1) {
                if (isRun7) {
                    for (auto it : fCont->GetBc2Digi()) {
                        if (it.first != iEvId)
                            continue;

                        for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                            new ((*fBC2)[fBC2->GetEntriesFast()]) BmnTrigDigit(it.second[iDig]);
                    }
                } else {
                    for (auto it : fCont->GetSiliconDigi()) {
                        if (it.first != iEvId)
                            continue;

                        for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                            new ((*fSil)[fSil->GetEntriesFast()]) BmnSiliconDigit(it.second[iDig]);
                    }
                }
            } else if (iDet == 2) {
                if (isRun7) {
                    for (auto it : fCont->GetBc3Digi()) {
                        if (it.first != iEvId)
                            continue;

                        for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                            new ((*fBC3)[fBC3->GetEntriesFast()]) BmnTrigDigit(it.second[iDig]);
                    }
                } else {
                    for (auto it : fCont->GetZdcDigi()) {
                        if (it.first != iEvId)
                            continue;

                        for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                            new ((*fZdc)[fZdc->GetEntriesFast()]) BmnZDCDigit(it.second[iDig]);
                    }
                }
            } else if (iDet == 3) {
                if (isRun7) {
                    for (auto it : fCont->GetVetoDigi()) {
                        if (it.first != iEvId)
                            continue;

                        for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                            new ((*fVeto)[fVeto->GetEntriesFast()]) BmnTrigDigit(it.second[iDig]);
                    }
                } else {
                    for (auto it : fCont->GetT0Digi()) {
                        if (it.first != iEvId)
                            continue;

                        for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                            new ((*fT0)[fT0->GetEntriesFast()]) BmnTrigDigit(it.second[iDig]);
                    }
                }
            } else if (iDet == 4) {
                if (isRun7) {
                    for (auto it : fCont->GetSiDigi()) {
                        if (it.first != iEvId)
                            continue;

                        for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                            new ((*fSi)[fSi->GetEntriesFast()]) BmnTrigDigit(it.second[iDig]);
                    }
                } else {
                    for (auto it : fCont->GetBc1Digi()) {
                        if (it.first != iEvId)
                            continue;

                        for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                            new ((*fBC1)[fBC1->GetEntriesFast()]) BmnTrigDigit(it.second[iDig]);
                    }
                }
            } else if (iDet == 5) {
                if (isRun7) {
                    for (auto it : fCont->GetBdDigi()) {
                        if (it.first != iEvId)
                            continue;

                        for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                            new ((*fBd)[fBd->GetEntriesFast()]) BmnTrigDigit(it.second[iDig]);
                    }
                } else {
                    for (auto it : fCont->GetBc2Digi()) {
                        if (it.first != iEvId)
                            continue;

                        for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                            new ((*fBC2)[fBC2->GetEntriesFast()]) BmnTrigDigit(it.second[iDig]);
                    }
                }
            } else if (iDet == 6) {
                if (isRun7) {
                    for (auto it : fCont->GetZdcDigi()) {
                        if (it.first != iEvId)
                            continue;

                        for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                            new ((*fZdc)[fZdc->GetEntriesFast()]) BmnZDCDigit(it.second[iDig]);
                    }
                } else {
                    for (auto it : fCont->GetVetoDigi()) {
                        if (it.first != iEvId)
                            continue;

                        for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                            new ((*fVeto)[fVeto->GetEntriesFast()]) BmnTrigDigit(it.second[iDig]);
                    }
                }
            } else if (iDet == 7) {
                if (isRun7) {
                    for (auto it : fCont->GetGemDigi()) {
                        if (it.first != iEvId)
                            continue;

                        for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                            new ((*fGem)[fGem->GetEntriesFast()]) BmnGemStripDigit(it.second[iDig]);
                    }
                } else {
                    for (auto it : fCont->GetFdDigi()) {
                        if (it.first != iEvId)
                            continue;

                        for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                            new ((*fFd)[fFd->GetEntriesFast()]) BmnTrigDigit(it.second[iDig]);
                    }
                }
            } else if (iDet == 8) {
                if (isRun7) {
                    for (auto it : fCont->GetSiliconDigi()) {
                        if (it.first != iEvId)
                            continue;

                        for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                            new ((*fSil)[fSil->GetEntriesFast()]) BmnSiliconDigit(it.second[iDig]);
                    }
                } else {
                    for (auto it : fCont->GetBdDigi()) {
                        if (it.first != iEvId)
                            continue;

                        for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                            new ((*fBd)[fBd->GetEntriesFast()]) BmnTrigDigit(it.second[iDig]);
                    }
                }
            } else if (iDet == 9)
                for (auto it : fCont->GetMwpcDigi()) {
                    if (it.first != iEvId)
                        continue;

                    for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                        new ((*fMwpc)[fMwpc->GetEntriesFast()]) BmnMwpcDigit(it.second[iDig]);
                } else if (iDet == 10)
                for (auto it : fCont->GetDchDigi()) {
                    if (it.first != iEvId)
                        continue;

                    for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                        new ((*fDch)[fDch->GetEntriesFast()]) BmnDchDigit(it.second[iDig]);
                } else if (iDet == 11)
                for (auto it : fCont->GetTof400Digi()) {
                    if (it.first != iEvId)
                        continue;

                    for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                        new ((*fTof400)[fTof400->GetEntriesFast()]) BmnTof1Digit(it.second[iDig]);
                } else if (iDet == 12)
                for (auto it : fCont->GetTof700Digi()) {
                    if (it.first != iEvId)
                        continue;

                    for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                        new ((*fTof700)[fTof700->GetEntriesFast()]) BmnTof2Digit(it.second[iDig]);
                } else if (iDet == 13)
                for (auto it : fCont->GetEcalDigi()) {
                    if (it.first != iEvId)
                        continue;

                    for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                        new ((*fEcal)[fEcal->GetEntriesFast()]) BmnECALDigit(it.second[iDig]);
                } else if (iDet == 14)
                for (auto it : fCont->GetCscDigi()) {
                    if (it.first != iEvId)
                        continue;

                    for (Int_t iDig = 0; iDig < it.second.size(); iDig++)
                        new ((*fCsc)[fCsc->GetEntriesFast()]) BmnCSCDigit(it.second[iDig]);
                }
        }

        if (IsArraysEmpty())
            continue;

        fOutTree->Fill();
    }
}

Bool_t BmnDigiMergeTask::IsArraysEmpty() {
    if (fGem->GetEntriesFast() == 0 &&
            fSil->GetEntriesFast() == 0 &&
            fZdc->GetEntriesFast() == 0 &&
            fBC1->GetEntriesFast() == 0 &&
            fBC2->GetEntriesFast() == 0 &&
            fVeto->GetEntriesFast() == 0 &&
            fBd->GetEntriesFast() == 0 &&
            fMwpc->GetEntriesFast() == 0 &&
            fDch->GetEntriesFast() == 0 &&
            fTof400->GetEntriesFast() == 0 &&
            fTof700->GetEntriesFast() == 0 &&
            fEcal->GetEntriesFast() == 0 &&
            (isRun6 ? (fT0->GetEntriesFast() == 0 && fFd->GetEntriesFast() == 0) : (fBC3->GetEntriesFast() == 0 && fSi->GetEntriesFast() == 0 && fCsc->GetEntriesFast() == 0)))
        return kTRUE;
    else
        return kFALSE;
}
