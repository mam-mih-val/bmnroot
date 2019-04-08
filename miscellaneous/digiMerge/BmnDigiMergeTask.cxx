#include "BmnDigiMergeTask.h"

BmnDigiMergeTask::BmnDigiMergeTask(TString in1, TString in2, TString in3, TString out) :
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

    // Detector branches 
    fIn1->Add(in1.Data());
    fIn1->SetBranchAddress("STRIPGEM", &fGemDigits);
    fIn1->SetBranchAddress("MYSILICON", &fSilDigits);

    fIn2->Add(in2.Data());
    fIn2->SetBranchAddress("ZDC", &fZdcDigits);

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

    // Event headers
    TString brName = "EventHeader";
    fIn1->SetBranchAddress(brName.Data(), &fHeader1);
    fIn2->SetBranchAddress(brName.Data(), &fHeader2);
    fIn3->SetBranchAddress(brName.Data(), &fHeader3);

    fNevsInSample = 100;
    fNevsToBeProcessed = 0;

    // Create output tree to write digis
    CreateOutputFile(out);
}

void BmnDigiMergeTask::CreateOutputFile(TString out) {
    fOutFile = new TFile(out.Data(), "recreate");
    fOutTree = new TTree("bmndata", "bmndata");

    fHeaderOut = new TClonesArray("BmnEventHeader");
    fGem = new TClonesArray("BmnGemStripDigit");
    fSil = new TClonesArray("BmnSiliconDigit");
    fZdc = new TClonesArray("BmnZDCDigit");
    fT0 = new TClonesArray("BmnTrigDigit");
    fBC1 = new TClonesArray("BmnTrigDigit");
    fBC2 = new TClonesArray("BmnTrigDigit");
    fVeto = new TClonesArray("BmnTrigDigit");
    fFd = new TClonesArray("BmnTrigDigit");
    fBd = new TClonesArray("BmnTrigDigit");
    fMwpc = new TClonesArray("BmnMwpcDigit");
    fDch = new TClonesArray("BmnDchDigit");
    fTof400 = new TClonesArray("BmnTof1Digit");
    fTof700 = new TClonesArray("BmnTof2Digit");
    fEcal = new TClonesArray("BmnECALDigit");

    fOutTree->Branch("EventHeader", &fHeaderOut);
    fOutTree->Branch("GEM", &fGem);
    fOutTree->Branch("SILICON", &fSil);
    fOutTree->Branch("ZDC", &fZdc);
    fOutTree->Branch("T0", &fT0);
    fOutTree->Branch("BC1", &fBC1);
    fOutTree->Branch("BC2", &fBC2);
    fOutTree->Branch("VETO", &fVeto);
    fOutTree->Branch("FD", &fFd);
    fOutTree->Branch("BD", &fBd);
    fOutTree->Branch("MWPC", &fMwpc);
    fOutTree->Branch("DCH", &fDch);
    fOutTree->Branch("TOF400", &fTof400);
    fOutTree->Branch("TOF700", &fTof700);
    fOutTree->Branch("ECAL", &fEcal);
}

void BmnDigiMergeTask::ProcessEvents() {
    const UInt_t minEvsNumber = (fNevsToBeProcessed == 0) ? Min(fIn1->GetEntries(), Min(fIn2->GetEntries(), fIn3->GetEntries())) : fNevsToBeProcessed;
    const Int_t sampleFactor = Int_t(minEvsNumber / fNevsInSample);

    for (Int_t iSample = 0; iSample < sampleFactor; iSample++) {
        Int_t start = iSample * fNevsInSample;
        Int_t finish = (iSample + 1) * fNevsInSample;

        fCont = new BmnDigiContainer();
        FillDigiContainer(start, finish);

        GlueEventsFromInputFiles();
        delete fCont;
    }
}

void BmnDigiMergeTask::FillDigiContainer(UInt_t iStart, UInt_t iFinish) {
    // cout << "in1 processing ..." << endl;
    for (UInt_t iEve = iStart; iEve < iFinish; iEve++) {
        fIn1->GetEntry(iEve);
        BmnEventHeader* h = (BmnEventHeader*) fHeader1->UncheckedAt(0);

        // Gem digis ...
        for (UInt_t iDigi = 0; iDigi < fGemDigits->GetEntriesFast(); iDigi++) {
            BmnGemStripDigit* dig = (BmnGemStripDigit*) fGemDigits->UncheckedAt(iDigi);
            fCont->SetGemDigi(h->GetEventId(), *dig);
        }

        // Silicon digis ...
        for (UInt_t iDigi = 0; iDigi < fSilDigits->GetEntriesFast(); iDigi++) {
            BmnSiliconDigit* dig = (BmnSiliconDigit*) fSilDigits->UncheckedAt(iDigi);
            fCont->SetSiliconDigi(h->GetEventId(), *dig);
        }
    }

    // cout << "in2 processing ..." << endl;
    for (Int_t iEve = iStart; iEve < iFinish; iEve++) {
        fIn2->GetEntry(iEve);
        BmnEventHeader* h = (BmnEventHeader*) fHeader2->UncheckedAt(0);
        fCont->SetEventHeader(h->GetEventId(), *h);

        // Zdc digis 
        for (UInt_t iDigi = 0; iDigi < fZdcDigits->GetEntriesFast(); iDigi++) {
            BmnZDCDigit* dig = (BmnZDCDigit*) fZdcDigits->UncheckedAt(iDigi);
            fCont->SetZdcDigi(h->GetEventId(), *dig);
        }
    }

    // cout << "in3 processing ..." << endl;
    for (UInt_t iEve = iStart; iEve < iFinish; iEve++) {
        fIn3->GetEntry(iEve);
        BmnEventHeader* h = (BmnEventHeader*) fHeader3->UncheckedAt(0);

        for (UInt_t iDigi = 0; iDigi < fT0Digits->GetEntriesFast(); iDigi++) {
            BmnTrigDigit* dig = (BmnTrigDigit*) fT0Digits->UncheckedAt(iDigi);
            fCont->SetT0Digi(h->GetEventId(), *dig);
        }

        for (UInt_t iDigi = 0; iDigi < fBC1Digits->GetEntriesFast(); iDigi++) {
            BmnTrigDigit* dig = (BmnTrigDigit*) fBC1Digits->UncheckedAt(iDigi);
            fCont->SetBc1Digi(h->GetEventId(), *dig);
        }

        for (UInt_t iDigi = 0; iDigi < fBC2Digits->GetEntriesFast(); iDigi++) {
            BmnTrigDigit* dig = (BmnTrigDigit*) fBC2Digits->UncheckedAt(iDigi);
            fCont->SetBc2Digi(h->GetEventId(), *dig);
        }

        for (UInt_t iDigi = 0; iDigi < fBdDigits->GetEntriesFast(); iDigi++) {
            BmnTrigDigit* dig = (BmnTrigDigit*) fBdDigits->UncheckedAt(iDigi);
            fCont->SetBdDigi(h->GetEventId(), *dig);
        }

        for (UInt_t iDigi = 0; iDigi < fFdDigits->GetEntriesFast(); iDigi++) {
            BmnTrigDigit* dig = (BmnTrigDigit*) fFdDigits->UncheckedAt(iDigi);
            fCont->SetFdDigi(h->GetEventId(), *dig);
        }

        for (UInt_t iDigi = 0; iDigi < fVetoDigits->GetEntriesFast(); iDigi++) {
            BmnTrigDigit* dig = (BmnTrigDigit*) fVetoDigits->UncheckedAt(iDigi);
            fCont->SetVetoDigi(h->GetEventId(), *dig);
        }

        for (UInt_t iDigi = 0; iDigi < fMwpcDigits->GetEntriesFast(); iDigi++) {
            BmnMwpcDigit* dig = (BmnMwpcDigit*) fMwpcDigits->UncheckedAt(iDigi);
            fCont->SetMwpcDigi(h->GetEventId(), *dig);
        }

        for (UInt_t iDigi = 0; iDigi < fDchDigits->GetEntriesFast(); iDigi++) {
            BmnDchDigit* dig = (BmnDchDigit*) fDchDigits->UncheckedAt(iDigi);
            fCont->SetDchDigi(h->GetEventId(), *dig);
        }

        for (UInt_t iDigi = 0; iDigi < fTof400Digits->GetEntriesFast(); iDigi++) {
            BmnTof1Digit* dig = (BmnTof1Digit*) fTof400Digits->UncheckedAt(iDigi);
            fCont->SetTof400Digi(h->GetEventId(), *dig);
        }

        for (UInt_t iDigi = 0; iDigi < fTof700Digits->GetEntriesFast(); iDigi++) {
            BmnTof2Digit* dig = (BmnTof2Digit*) fTof700Digits->UncheckedAt(iDigi);
            fCont->SetTof700Digi(h->GetEventId(), *dig);
        }

        for (UInt_t iDigi = 0; iDigi < fEcalDigits->GetEntriesFast(); iDigi++) {
            BmnECALDigit* dig = (BmnECALDigit*) fEcalDigits->UncheckedAt(iDigi);
            fCont->SetEcalDigi(h->GetEventId(), *dig);
        }
    }
}

pair <UInt_t, UInt_t> BmnDigiMergeTask::FindStartFinishEventId() {
    vector <UInt_t> minEventIds; // Minimum event id from three input files
    vector <UInt_t> maxEventIds; // Maximum event id from three input files

    multimap <UInt_t, BmnGemStripDigit> gem = fCont->GetGemDigi();
    multimap <UInt_t, BmnSiliconDigit> silicon = fCont->GetSiliconDigi();
    multimap <UInt_t, BmnZDCDigit> zdc = fCont->GetZdcDigi();
    multimap <UInt_t, BmnTrigDigit> t0 = fCont->GetT0Digi();
    multimap <UInt_t, BmnTrigDigit> bc1 = fCont->GetBc1Digi();
    multimap <UInt_t, BmnTrigDigit> bc2 = fCont->GetBc2Digi();
    multimap <UInt_t, BmnTrigDigit> veto = fCont->GetVetoDigi();
    multimap <UInt_t, BmnTrigDigit> fd = fCont->GetFdDigi();
    multimap <UInt_t, BmnTrigDigit> bd = fCont->GetBdDigi();
    multimap <UInt_t, BmnMwpcDigit> mwpc = fCont->GetMwpcDigi();
    multimap <UInt_t, BmnDchDigit> dch = fCont->GetDchDigi();
    multimap <UInt_t, BmnTof1Digit> tof400 = fCont->GetTof400Digi();
    multimap <UInt_t, BmnTof2Digit> tof700 = fCont->GetTof700Digi();
    multimap <UInt_t, BmnECALDigit> ecal = fCont->GetEcalDigi();

    if (!gem.empty()) {
        minEventIds.push_back(gem.begin()->first);
        maxEventIds.push_back(gem.rbegin()->first);
    }

    if (!silicon.empty()) {
        minEventIds.push_back(silicon.begin()->first);
        maxEventIds.push_back(silicon.rbegin()->first);
    }

    if (!zdc.empty()) {
        minEventIds.push_back(zdc.begin()->first);
        maxEventIds.push_back(zdc.rbegin()->first);
    }

    if (!t0.empty()) {
        minEventIds.push_back(t0.begin()->first);
        maxEventIds.push_back(t0.rbegin()->first);
    }

    if (!bc1.empty()) {
        minEventIds.push_back(bc1.begin()->first);
        maxEventIds.push_back(bc1.rbegin()->first);
    }

    if (!bc2.empty()) {
        minEventIds.push_back(bc2.begin()->first);
        maxEventIds.push_back(bc2.rbegin()->first);
    }

    if (!veto.empty()) {
        minEventIds.push_back(veto.begin()->first);
        maxEventIds.push_back(veto.rbegin()->first);
    }

    if (!fd.empty()) {
        minEventIds.push_back(fd.begin()->first);
        maxEventIds.push_back(fd.rbegin()->first);
    }

    if (!bd.empty()) {
        minEventIds.push_back(bd.begin()->first);
        maxEventIds.push_back(bd.rbegin()->first);
    }

    if (!mwpc.empty()) {
        minEventIds.push_back(mwpc.begin()->first);
        maxEventIds.push_back(mwpc.rbegin()->first);
    }

    if (!dch.empty()) {
        minEventIds.push_back(dch.begin()->first);
        maxEventIds.push_back(dch.rbegin()->first);
    }

    if (!tof400.empty()) {
        minEventIds.push_back(tof400.begin()->first);
        maxEventIds.push_back(tof400.rbegin()->first);
    }

    if (!tof700.empty()) {
        minEventIds.push_back(tof700.begin()->first);
        maxEventIds.push_back(tof700.rbegin()->first);
    }

    if (!ecal.empty()) {
        minEventIds.push_back(ecal.begin()->first);
        maxEventIds.push_back(ecal.rbegin()->first);
    }

    return pair <UInt_t, UInt_t>
            (*min_element(minEventIds.begin(), minEventIds.end()),
            *max_element(maxEventIds.begin(), maxEventIds.end()));
}

void BmnDigiMergeTask::GlueEventsFromInputFiles() {
    UInt_t min = FindStartFinishEventId().first;
    UInt_t max = FindStartFinishEventId().second + 1;

    for (UInt_t iEvId = min; iEvId < max; iEvId++) {
        if (iEvId % 100 == 0)
            cout << "Event# " << iEvId << endl;

        fHeaderOut->Delete();
        fGem->Delete();
        fSil->Delete();
        fZdc->Delete();
        fT0->Delete();
        fBC1->Delete();
        fBC2->Delete();
        fVeto->Delete();
        fFd->Delete();
        fBd->Delete();
        fMwpc->Delete();
        fDch->Delete();
        fTof400->Delete();
        fTof700->Delete();
        fEcal->Delete();

        for (auto it : fCont->GetEventHeader()) {
            if (it.first != iEvId)
                continue;
            BmnEventHeader head = it.second;
         
            BmnEventHeader* h = new ((*fHeaderOut)[fHeaderOut->GetEntriesFast()]) BmnEventHeader();
            h->SetRunId(head.GetRunId());
            h->SetEventId(head.GetEventId());
            h->SetEventTimeTS(head.GetEventTimeTS());
            h->SetEventTime(head.GetEventTime());
            h->SetType(head.GetType());
            h->SetTripWord(head.GetTripWord());
            // h->SetTrigInfo(head.GetTrigInfo());
            h->SetTimeShift(head.GetTimeShift());
        }

        for (auto it : fCont->GetGemDigi()) {
            if (it.first != iEvId)
                continue;

            new ((*fGem)[fGem->GetEntriesFast()]) BmnGemStripDigit(it.second);
        }

        for (auto it : fCont->GetSiliconDigi()) {
            if (it.first != iEvId)
                continue;

            new ((*fSil)[fSil->GetEntriesFast()]) BmnSiliconDigit(it.second);
        }

        for (auto it : fCont->GetZdcDigi()) {
            if (it.first != iEvId)
                continue;

            new ((*fZdc)[fZdc->GetEntriesFast()]) BmnZDCDigit(it.second);
        }

        for (auto it : fCont->GetT0Digi()) {
            if (it.first != iEvId)
                continue;

            new ((*fT0)[fT0->GetEntriesFast()]) BmnTrigDigit(it.second);
        }

        for (auto it : fCont->GetBc1Digi()) {
            if (it.first != iEvId)
                continue;

            new ((*fBC1)[fBC1->GetEntriesFast()]) BmnTrigDigit(it.second);
        }

        for (auto it : fCont->GetBc2Digi()) {
            if (it.first != iEvId)
                continue;

            new ((*fBC2)[fBC2->GetEntriesFast()]) BmnTrigDigit(it.second);
        }

        for (auto it : fCont->GetVetoDigi()) {
            if (it.first != iEvId)
                continue;

            new ((*fVeto)[fVeto->GetEntriesFast()]) BmnTrigDigit(it.second);
        }

        for (auto it : fCont->GetFdDigi()) {
            if (it.first != iEvId)
                continue;

            new ((*fFd)[fFd->GetEntriesFast()]) BmnTrigDigit(it.second);
        }

        for (auto it : fCont->GetBdDigi()) {
            if (it.first != iEvId)
                continue;

            new ((*fBd)[fBd->GetEntriesFast()]) BmnTrigDigit(it.second);
        }

        for (auto it : fCont->GetMwpcDigi()) {
            if (it.first != iEvId)
                continue;

            new ((*fMwpc)[fMwpc->GetEntriesFast()]) BmnMwpcDigit(it.second);
        }

        for (auto it : fCont->GetDchDigi()) {
            if (it.first != iEvId)
                continue;

            new ((*fDch)[fDch->GetEntriesFast()]) BmnDchDigit(it.second);
        }

        for (auto it : fCont->GetTof400Digi()) {
            if (it.first != iEvId)
                continue;

            new ((*fTof400)[fTof400->GetEntriesFast()]) BmnTof1Digit(it.second);
        }

        for (auto it : fCont->GetTof700Digi()) {
            if (it.first != iEvId)
                continue;

            new ((*fTof700)[fTof700->GetEntriesFast()]) BmnTof2Digit(it.second);
        }

        for (auto it : fCont->GetEcalDigi()) {
            if (it.first != iEvId)
                continue;

            new ((*fEcal)[fEcal->GetEntriesFast()]) BmnECALDigit(it.second);
        }

        fOutTree->Fill();
    }
}


