/**
 *  updateContent.C 
 * 
 *  An example macro how to copy raw root data and make first events        
 *  as pedestal ones                                                      
 * 
 */

int updateContent(TString fileName = "", TString destName = "", TString treeName = "BMN_RAW", TString brName = "ADC32") {
    Int_t nPedsMax = 100;
    const TString EHDigiName = "BmnEventHeader.";
    vector<TString> digiNamesOther;
    vector<TClass*> digiClassesOther;
    vector<TClonesArray*> digiBaseArsOther;
    vector<TClonesArray*> digiDestArsOther;
    if (fileName == "") {
        cout << "ERROR: File path was not specified" << endl;
        return -1;
    }

    gSystem->ExpandPathName(fileName);

    /*****************************/
    /** Open input base digits **/
    /*****************************/
    TFile* rootFile = new TFile(fileName.Data(), "READ");
    if (!rootFile) {
        cout << "ERROR: File was not found: " << fileName << endl;
        return -2;
    }

    TTree* fInTreeBase = (TTree*) rootFile->Get(treeName.Data());
    if (!fInTreeBase) {
        cout << "ERROR: Tree was not found: " << treeName << endl;
        return -3;
    }
    UInt_t fNEventSource = fInTreeBase->GetEntries();
    TObjArray* digiBaseBrsFull = fInTreeBase->GetListOfBranches();
    TIterator* it = digiBaseBrsFull->MakeIterator();
    it->Reset();
    TBranch* b = NULL;
    while ((b = (TBranch*) it->Next())) {
        if (strcmp(b->GetName(), EHDigiName.Data()) == 0)
            continue;
        TClonesArray* arDigi = nullptr;
        fInTreeBase->SetBranchAddress(b->GetName(), &arDigi);
        digiNamesOther.push_back(TString(b->GetName()));
        digiBaseArsOther.push_back(arDigi);
        digiClassesOther.push_back(arDigi->GetClass());
    }
    BmnEventHeader * baseEH = nullptr;
    fInTreeBase->SetBranchAddress(EHDigiName.Data(), &baseEH);

    /*****************************/
    /** Open  dest digits **/
    /*****************************/
    TFile *fDestHitsFile = new TFile(destName, "RECREATE");
    if (fDestHitsFile->IsOpen() == false) {
        printf("\n!!!!\ncannot open file %s !\n", destName.Data());
        return kBMNERROR;
    }
    printf("\n DEST FILE: ");
    printf("%s\n", destName.Data());
    TTree* fDestTree = new TTree(treeName.Data(), treeName.Data());
    for (Int_t i = 0; i < digiBaseArsOther.size(); i++) {
        TClonesArray* arDigi = new TClonesArray(digiClassesOther[i]);
        fDestTree->Branch(digiNamesOther[i], &arDigi);
        digiDestArsOther.push_back(arDigi);
    }
    BmnEventHeader* destEH = new BmnEventHeader();
    fDestTree->Branch("BmnEventHeader.", &destEH);
    UInt_t fNArs = digiNamesOther.size();


    // fill events
    Int_t nPedsAdded = 0;
    for (UInt_t iEv = 0; iEv < fNEventSource; ++iEv) {
        DrawBar(iEv, fNEventSource);
        fInTreeBase->GetEntry(iEv);
        destEH->Clear();
        for (UInt_t iBr = 0; iBr < digiDestArsOther.size(); iBr++) {
            digiDestArsOther[iBr]->Clear("C");
            digiDestArsOther[iBr]->AbsorbObjects(digiBaseArsOther[iBr]);
        }
        destEH->Clear();
        destEH->SetEventId(baseEH->GetEventId());
        destEH->SetEventTime(baseEH->GetEventTime());
        destEH->SetEventTimeTS(baseEH->GetEventTimeTS());
        if (nPedsAdded++ < nPedsMax)
            destEH->SetEventTime(kBMNPEDESTAL);
        else
            destEH->SetEventType(baseEH->GetEventType());
        destEH->SetPeriodId(baseEH->GetPeriodId());
        destEH->SetRunId(baseEH->GetRunId());
        fDestTree->Fill();
    }
    fDestTree->Write();
    //    fDestHitsFile->WriteObject(rhBase, RHDigiName.Data());
    fDestHitsFile->Write();
    if (fDestHitsFile)
        fDestHitsFile->Close();

    rootFile->Close();
    delete rootFile;
    return 0;
}