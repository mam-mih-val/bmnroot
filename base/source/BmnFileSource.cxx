//
//  BmnFileSource.cxx
//  BmnRoot
//
//  Created by Mohammad Al-Turany on 08/02/14. Modified by Konstantin Gertsenberger 26.05.2016
//

#include "BmnFileSource.h"
#include "BmnEventHeader.h"
#include "DigiRunHeader.h"

#include "FairLogger.h"
#include "FairRootManager.h"

#include "TROOT.h"
#include "TChainElement.h"
#include "TRandom.h"

#include <set>
#include <algorithm>

//_____________________________________________________________________________
BmnFileSource::BmnFileSource(TFile *f, const char* Title, UInt_t identifier)
  :FairSource()
  ,fInputTitle(Title)
  ,fRootFile(f)
  ,fCurrentEntryNr(0)
  ,fFriendFileList()
  ,fInputChainList()
  ,fFriendTypeList()
  ,fCheckInputBranches()
  ,fInputLevel()
  ,fRunIdInfoAll()
  ,fInChain(0)
  ,fInTree(0)
  ,fListFolder(new TObjArray(16))
  ,fRtdb(FairRuntimeDb::instance())
  ,fCbmout(0)
  ,fCbmroot(0)
  ,fSourceIdentifier(0)
  ,fNoOfEntries(-1)
  ,IsInitialized(kFALSE)
  ,fEvtHeader(0)
  ,fFileHeader(0)
  ,fEvtHeaderIsNew(kFALSE)
  ,fCurrentEntryNo(0)
  ,fTimeforEntryNo(-1)
  ,fEventTimeMin(0.)
  ,fEventTimeMax(0.)
  ,fEventTime(0.)
  ,fBeamTime(-1.)
  ,fGapTime(-1.)
  ,fEventMeanTime(0.)
  ,fTimeProb(0)
  ,fCheckFileLayout(kTRUE)
{
    if (fRootFile->IsZombie())
        LOG(fatal)<<"Error opening the Input file";

    LOG(debug)<<"BmnFileSource created------------";
}

//_____________________________________________________________________________
BmnFileSource::BmnFileSource(const TString* RootFileName, const char* Title, UInt_t identifier)
  :FairSource()
  ,fInputTitle(Title)
  ,fRootFile(0)
  ,fCurrentEntryNr(0)
  ,fFriendFileList()
  ,fInputChainList()
  ,fFriendTypeList()
  ,fCheckInputBranches()
  ,fInputLevel()
  ,fRunIdInfoAll()
  ,fInChain(0)
  ,fInTree(0)
  ,fListFolder(new TObjArray(16))
  ,fRtdb(FairRuntimeDb::instance())
  ,fCbmout(0)
  ,fCbmroot(0)
  ,fSourceIdentifier(0)
  ,fNoOfEntries(-1)
  ,IsInitialized(kFALSE)
  ,fEvtHeader(0)
  ,fFileHeader(0)
  ,fEvtHeaderIsNew(kFALSE)
  ,fCurrentEntryNo(0)
  ,fTimeforEntryNo(-1)
  ,fEventTimeMin(0.)
  ,fEventTimeMax(0.)
  ,fEventTime(0.)
  ,fBeamTime(-1.)
  ,fGapTime(-1.)
  ,fEventMeanTime(0.)
  ,fTimeProb(0)
  ,fCheckFileLayout(kTRUE)
{
    fRootFile = new TFile(RootFileName->Data());
    if (fRootFile->IsZombie())
        LOG(fatal)<<"Error opening the Input file";

    LOG(debug)<<"BmnFileSource created------------";
}

//_____________________________________________________________________________
BmnFileSource::BmnFileSource(const TString RootFileName, const char* Title, UInt_t identifier)
  :FairSource()
  ,fInputTitle(Title)
  ,fRootFile(0)
  ,fCurrentEntryNr(0)
  ,fFriendFileList()
  ,fInputChainList()
  ,fFriendTypeList()
  ,fCheckInputBranches()
  ,fInputLevel()
  ,fRunIdInfoAll()
  ,fInChain(0)
  ,fInTree(0)
  ,fListFolder(new TObjArray(16))
  ,fRtdb(FairRuntimeDb::instance())
  ,fCbmout(0)
  ,fCbmroot(0)
  ,fSourceIdentifier(0)
  ,fNoOfEntries(-1)
  ,IsInitialized(kFALSE)
  ,fEvtHeader(0)
  ,fFileHeader(0)
  ,fEvtHeaderIsNew(kFALSE)
  ,fCurrentEntryNo(0)
  ,fTimeforEntryNo(-1)
  ,fEventTimeMin(0.)
  ,fEventTimeMax(0.)
  ,fEventTime(0.)
  ,fBeamTime(-1.)
  ,fGapTime(-1.)
  ,fEventMeanTime(0.)
  ,fTimeProb(0)
  ,fCheckFileLayout(kTRUE)
{
    fRootFile = new TFile(RootFileName.Data());
    if (fRootFile->IsZombie())
        LOG(fatal)<<"Error opening the Input file";

    LOG(debug)<<"BmnFileSource created------------";
}

//_____________________________________________________________________________
BmnFileSource::BmnFileSource(const TString RootFileName, int& period_number, int& run_number)
  :FairSource()
  ,fInputTitle("BmnRootFile")
  ,fRootFile(0)
  ,fCurrentEntryNr(0)
  ,fFriendFileList()
  ,fInputChainList()
  ,fFriendTypeList()
  ,fCheckInputBranches()
  ,fInputLevel()
  ,fRunIdInfoAll()
  ,fInChain(0)
  ,fInTree(0)
  ,fListFolder(new TObjArray(16))
  ,fRtdb(FairRuntimeDb::instance())
  ,fCbmout(0)
  ,fCbmroot(0)
  ,fSourceIdentifier(0)
  ,fNoOfEntries(-1)
  ,IsInitialized(kFALSE)
  ,fEvtHeader(0)
  ,fFileHeader(0)
  ,fEvtHeaderIsNew(kFALSE)
  ,fCurrentEntryNo(0)
  ,fTimeforEntryNo(-1)
  ,fEventTimeMin(0.)
  ,fEventTimeMax(0.)
  ,fEventTime(0.)
  ,fBeamTime(-1.)
  ,fGapTime(-1.)
  ,fEventMeanTime(0.)
  ,fTimeProb(0)
  ,fCheckFileLayout(kTRUE)
{
    fRootFile = new TFile(RootFileName.Data());
    if (fRootFile->IsZombie())
    {
        LOG(fatal)<<"Error opening the Input file";
        return;
    }

    // get period and run number
    DigiRunHeader* run_header = (DigiRunHeader*) fRootFile->Get("DigiRunHeader");  // read DigiRunHeader if present
    if (run_header)
    {
        period_number = run_header->GetPeriodId();
        run_number = run_header->GetRunId();
    }
    else // temporary solution supporting run 4 - run 7
    {
        TTree* bmn_tree = (TTree*) fRootFile->Get("bmndata");
        if (!bmn_tree)
        {
            LOG(fatal)<<"ERROR: no 'bmndata' tree in file: "<<RootFileName;
            return;
        }

        BmnEventHeader* fEventHeader = NULL;
        bmn_tree->SetBranchAddress("BmnEventHeader.", &fEventHeader);
        if (bmn_tree->GetEntries() < 1)
        {
            LOG(fatal)<<"ERROR: no entries in 'bmndata' tree (file: "<<RootFileName<<")";
            return;
        }

        bmn_tree->GetEntry(0);
        if (!fEventHeader)
        {
            LOG(fatal)<<"ERROR: no 'BmnEventHeader.' in 'bmndata' tree (file: "<<RootFileName<<")";
            return;
        }
        run_number = fEventHeader->GetRunId();
        if (run_number > 2000) period_number = 7;
        else
        {
            if (run_number > 1100) period_number = 6;
            else {
                if (run_number > 400) period_number = 5;
                else period_number = 4;
            }
        }
    }
    delete run_header;
    LOG(debug)<<"BmnFileSource created------------";
}

//_____________________________________________________________________________
BmnFileSource::~BmnFileSource()
{
}

//_____________________________________________________________________________
Bool_t BmnFileSource::Init()
{
    if (IsInitialized)
    {
        LOG(info)<<"BmnFileSource already initialized";
        return kTRUE;
    }

    if (!fInChain)
    {
        fInChain = new TChain(FairRootManager::GetTreeName(), "/cbmroot");
        LOG(debug)<<"BmnFileSource::Init() chain created";

        FairRootManager::Instance()->SetInChain(fInChain);
    }
    fInChain->Add(fRootFile->GetName());

    // Get The list of branches from the input file and add it to the
    // actual list of existing branches.
    // Add this list of branches also to the map of input trees, which
    // stores the information which branches belong to which input tree.
    // There is at least one primary input tree, but there can be many
    // additional friend trees.
    // This information is needed to add new files to the correct friend
    // tree. With this information it is also possible to check if the
    // input files which are added to the input chain all have the same
    // branch structure. Without this check it is possible to add trees
    // with a different branch structure but the same tree name. ROOT
    // probably only checks if the name of the tree is the same.
    TString chainName = fInputTitle;
    fInputLevel.push_back(chainName);
    fCheckInputBranches[chainName] = new list<TString>;


    TObjArray* fBranchList = fInChain->GetListOfBranches();
    // if no any branches then exit (some errors occured)
    if (fBranchList == NULL)
        return kFALSE;

    LOG(debug)<<"Entries in the chain "<<fBranchList->GetEntries();

    TObject** ppObj = new TObject*[fBranchList->GetEntries()];
    for (int i = 0; i < fBranchList->GetEntries(); i++)
    {
        TBranch* pBranch = (TBranch*) fBranchList->At(i);
        TString ObjName = pBranch->GetName();
        LOG(debug)<<"Branch name "<<ObjName.Data();

        fCheckInputBranches[chainName]->push_back(ObjName.Data());
        FairRootManager::Instance()->AddBranchToList(ObjName.Data());

        ppObj[i] = NULL;
        //ActivateObject(&(ppObj[i]), ObjName);
        fInChain->SetBranchAddress(ObjName, &ppObj[i]);
        FairRootManager::Instance()->RegisterInputObject(ObjName, ppObj[i]);
    }
    
    // Add all additional input files to the input chain and do a
    // consitency check
    list<TString>::const_iterator iter;
    for (iter = fInputChainList.begin(); iter != fInputChainList.end(); iter++)
    {
        // Store global gFile pointer for safety reasons.
        // Set gFile to old value at the end of the routine.R
        TFile* temp = gFile;
        
        // Temporarily open the input file to extract information which
        // is needed to bring the friend trees in the correct order
        TFile* inputFile = new TFile(*iter);
        if (inputFile->IsZombie())
            LOG(fatal)<<"Error opening the file "<<(*iter).Data()<<" which should be added to the input chain or as friend chain";
        
        // Check if the branchlist is the same as for the first input file.
        Bool_t isOk = CompareBranchList(inputFile, chainName);
        if (!isOk)
        {
            LOG(fatal)<<"Branch structure of the input file "<<fRootFile->GetName()<<" and the file to be added "<<(*iter).Data()<<" are different.";
            return kFALSE;
        }
        
        // Add the file to the input chain
        fInChain->Add(*iter);
        
        // Close the temporarly file and restore the gFile pointer.
        inputFile->Close();
        gFile = temp;
    }

    fNoOfEntries = fInChain->GetEntries(); 
    LOG(debug)<<"Entries in this Source "<<fNoOfEntries;

    AddFriendsToChain();

    return kTRUE;
}

//_____________________________________________________________________________
void BmnFileSource::SetInTree(TTree* tempTree)
{
    fInTree = tempTree;
    fRootFile = (TFile*)tempTree->GetCurrentFile();
    fInChain->Reset();
    IsInitialized = kFALSE;

    Init();
}

//_____________________________________________________________________________
Int_t BmnFileSource::ReadEvent(UInt_t i)
{
    fCurrentEntryNo = i;
    SetEventTime();

    if (fInChain->GetEntry(i))
        return 0;

    return 1;
}

//_____________________________________________________________________________
void BmnFileSource::Close()
{
    CloseInFile();
}
//_____________________________________________________________________________
void BmnFileSource::Reset()
{
}

//_____________________________________________________________________________
void BmnFileSource::AddFriend(TString fName)
{
    fFriendFileList.push_back(fName);
}

//_____________________________________________________________________________
void BmnFileSource::AddFile(TString FileName)
{
    fInputChainList.push_back(FileName);
}
//_____________________________________________________________________________
void BmnFileSource::AddFriendsToChain()
{
    // Loop over all Friend files and extract the type. The type is defined by
    // the tree which is stored in the file. If there is already a chain of with
    // this type of tree then the file will be added to this chain.
    // If there is no such chain it will be created.
    //    
    // In the old way it was needed sometimes to add a freind file more
    // than once. This is not needed any longer, so we remove deuplicates
    // from the list and display a warning.
    list<TString> friendList;
    list<TString>::iterator iter1;
    for (iter1 = fFriendFileList.begin(); iter1 != fFriendFileList.end(); iter1++)
    {
        if (find(friendList.begin(), friendList.end(), *iter1) == friendList.end())
            friendList.push_back(*iter1);
    }
    
    TFile* temp = gFile;
    
    Int_t friendType = 1;
    // Loop over all files which have been added as friends
    for (iter1 = friendList.begin(); iter1 != friendList.end(); iter1++)
    {
        list<TString>::iterator iter;
        TString inputLevel;
        // Loop over all already defined input levels to check if this type
        // of friend tree is already added.
        // If this type of friend tree already exist add the file to the
        // then already existing friend chain. If this type of friend tree
        // does not exist already create a new friend chain and add the file.
        Bool_t inputLevelFound = kFALSE;
        TFile* inputFile;
        for (iter = fInputLevel.begin(); iter != fInputLevel.end(); iter++)
        {
            inputLevel = (*iter);
            
            inputFile = new TFile((*iter1));
            if (inputFile->IsZombie())
                LOG(fatal)<<"Error opening the file "<<(*iter).Data()<<" which should be added to the input chain or as friend chain";
            
            // Check if the branchlist is already stored in the map. If it is
            // already stored add the file to the chain.
            Bool_t isOk = CompareBranchList(inputFile, inputLevel);
            if (isOk)
            {
                inputLevelFound = kTRUE;
                inputFile->Close();
                continue;
            }
            inputFile->Close();
        }
        if (!inputLevelFound)
        {
            inputLevel = Form("FriendTree_%i",friendType);
            CreateNewFriendChain(*iter1, inputLevel);
            friendType++;
        }
        
        TChain* chain = static_cast<TChain*>(fFriendTypeList[inputLevel]);
        chain->AddFile((*iter1), 1234567890, FairRootManager::GetTreeName());
    }
    gFile=temp;
    
    // Add all the friend chains which have been created to the
    // main input chain
    map< TString, TChain* >::iterator mapIterator;
    for (mapIterator = fFriendTypeList.begin(); mapIterator != fFriendTypeList.end(); mapIterator++)
    {
        TChain* chain = static_cast<TChain*>(mapIterator->second);
        fInChain->AddFriend(chain);
    }
    
    // Print some output about the input structure
    PrintFriendList();
}

//_____________________________________________________________________________
void BmnFileSource::PrintFriendList()
{
    // Print information about the input structure
    // List files from the input chain together with all files of all friend chains
    LOG(info)<<"The input consists out of the following trees and files: \n"<<" - "<<fInChain->GetName();
    TObjArray* fileElements = fInChain->GetListOfFiles();

    TIter next(fileElements);
    TChainElement* chEl = 0;
    while (chEl = (TChainElement*)next())
        LOG(info)<<"    - "<<chEl->GetTitle();
    
    map<TString, TChain*>::iterator mapIterator;
    for (mapIterator = fFriendTypeList.begin(); mapIterator != fFriendTypeList.end(); mapIterator++)
    {
        TChain* chain = (TChain*) mapIterator->second;
        LOG(info)<<" - "<<chain->GetName();
        fileElements = chain->GetListOfFiles();
        TIter next1(fileElements);
        chEl = 0;
        while (chEl = (TChainElement*)next1())
            LOG(info)<<"    - "<<chEl->GetTitle();
    }    
}

//_____________________________________________________________________________
void BmnFileSource::CheckFriendChains()
{
    multimap< TString, multimap<TString,TArrayI> >::iterator it1;
    multimap<TString,TArrayI> map1;

    // Get the structure from the input chain
    it1 = fRunIdInfoAll.find("InputChain");
    map1 = it1->second;
    vector<Int_t> runid;
    vector<Int_t> events;
    multimap<TString,TArrayI>::iterator it;
    for (it=map1.begin(); it != map1.end(); it++)
    {
        TArrayI bla = (*it).second;
        runid.push_back(bla[0]);
        events.push_back(bla[1]);
    }

    // Now loop over all chains except the input chain and comapare the
    // runids and event numbers.
    // If there is a mismatch stop the execution.
    Int_t errorFlag = 0;
    TString inputLevel;
    list<TString>::iterator listit;
    for (listit=fInputLevel.begin(); listit != fInputLevel.end(); listit++)
    {
        inputLevel = (*listit);
        if (!inputLevel.Contains("InputChain"))
        {
            it1 = fRunIdInfoAll.find(inputLevel);
            map1 = it1->second;
            if (runid.size() != map1.size())
            {
                errorFlag = 1;
                //        goto error_label;
                break;
            }
            Int_t counter = 0;
            for (it=map1.begin(); it != map1.end(); it++)
            {
                TArrayI bla = (*it).second;
                if ((bla[0] != runid[counter]) || (bla[1] != events[counter]))
                {
                    errorFlag = 2;
                    //          goto error_label;
                    break;
                }
                counter++;
            }
            if (errorFlag>0)
                break;
        }
    }

    // Use goto to leave double loop at once in case of error
    // error_label:
    if (errorFlag>0)
    {
        LOG(error)<<"The input chain and the friend chain "<<inputLevel.Data()<<" have a different structure:";
        if (errorFlag == 1)
        {
            LOG(error)<<"The input chain has the following runids and event numbers:";
            for (UInt_t i=0; i < runid.size(); i++)
                LOG(error)<<" - Runid " << runid[i]<<" with "<<events[i]<<" events";
            LOG(error)<<"The "<<inputLevel.Data()<<" chain has the following runids and event numbers:";
            for (it=map1.begin(); it != map1.end(); it++)
            {
                TArrayI bla = (*it).second;
                LOG(error)<<" - Runid "<<bla[0]<<" with "<<bla[1]<<" events";
            }
        }
        if (errorFlag == 2)
        {
            Int_t counter = 0;
            for (it=map1.begin(); it != map1.end(); it++)
            {
                TArrayI bla = (*it).second;
                LOG(error)<<"Runid Input Chain, "<<inputLevel.Data()<<" chain: "<<bla[0]<<", "<<runid[counter];
                LOG(error)<<"Event number Input Chain, "<<inputLevel.Data()<<" chain: "<<bla[1]<<", "<<events[counter];
                counter++;
            }
        }
        LOG(fatal)<<"Event structure mismatch";
    }
}

//_____________________________________________________________________________
void BmnFileSource::CreateNewFriendChain(TString inputFile, TString inputLevel)
{
    TFile* temp = gFile;

    TFile* f = new TFile(inputFile);
    TString chainName = inputLevel;
    fInputLevel.push_back(chainName);
    fCheckInputBranches[chainName] = new list<TString>;

    /**Get The list of branches from the friend file and add it to the actual list*/
    TChain* chain = new TChain(inputLevel);
    chain->Add(inputFile);
    TObjArray* fBranchList = chain->GetListOfBranches();
    for (int i = 0; i < fBranchList->GetEntries(); i++)
    {
        TString ObjName = ((TNamed*)fBranchList->At(i))->GetName();
        fCheckInputBranches[chainName]->push_back(ObjName.Data());

        FairRootManager::Instance()->AddBranchToList(ObjName.Data());
    }

    fFriendTypeList[inputLevel] = chain;
    
    f->Close();
    gFile = temp;
}

//_____________________________________________________________________________
Bool_t BmnFileSource::CompareBranchList(TFile* fileHandle, TString inputLevel)
{
    // fill a set with the original branch structure
    // This allows to use functions find and erase
    set<TString> branches;
    list<TString>::const_iterator iter;
    for (iter = fCheckInputBranches[inputLevel]->begin(); iter != fCheckInputBranches[inputLevel]->end(); iter++)
        branches.insert(*iter);

    // To do so we have to loop over the branches in the file and to compare
    // the branches. If both lists are equal everything is okay

    // Get The list of branches from the input file one by one and compare
    // it to the reference list of branches which is defined for this tree.
    // If a branch with the same name is found, this branch is removed from
    // the list. If in the end no branch is left in the list everything is fine
    set<TString>::iterator iter1;
    TChain* fileTree = new TChain(FairRootManager::GetTreeName(), "/cbmroot");
    fileTree->Add(fileHandle->GetName());
    TObjArray* fBranchList = fileTree->GetListOfBranches();
    for (int i = 0; i < fBranchList->GetEntries(); i++)
    {
        TString ObjName = ((TNamed*)fBranchList->At(i))->GetName();
        iter1 = branches.find(ObjName.Data());

        if (iter1 != branches.end()) branches.erase(iter1);
        else
          // Not found is an error because branch structure is
          // different. It is impossible to add to tree with a
          // different branch structure
          return kFALSE;
    }

    // If the size of branches is !=0 after removing all branches also in the
    // reference list, this is also a sign that both branch list are not the
    // same
    if (branches.size() != 0)
    {
        LOG(info)<<"Compare Branch List will return kFALSE. The list has "<< branches.size()<<" branches:";
        for (set<TString>::iterator it = branches.begin(); it != branches.end(); it++)
            LOG(info)<<"  -> "<<*it;
        return kFALSE;
    }

    return kTRUE;
}

//_____________________________________________________________________________
Bool_t BmnFileSource::ActivateObject(TObject** obj, const char* BrName)
{
    if (fInTree)
    {
        fInTree->SetBranchStatus(BrName, 1);
        fInTree->SetBranchAddress(BrName, obj);
    }

    if (fInChain)
    {
        fInChain->SetBranchStatus(BrName, 1);
        fInChain->SetBranchAddress(BrName, obj);
    }
    
    return kTRUE;
}

//_____________________________________________________________________________
void BmnFileSource::SetInputFile(TString name)
{
    fRootFile = new TFile(name.Data());

    if (fRootFile->IsZombie())
        LOG(fatal)<<"Error opening the Input file";

    LOG(info)<<"BmnFileSource set------------";
}

//_____________________________________________________________________________
Int_t BmnFileSource::CheckMaxEventNo(Int_t EvtEnd)
{
    Int_t MaxEventNo = 0;
    if (EvtEnd != 0)
        MaxEventNo = EvtEnd;

    MaxEventNo = fInChain->GetEntries();
    return MaxEventNo;
}

//_____________________________________________________________________________
void  BmnFileSource::SetEventMeanTime(Double_t mean)
{
    fEventMeanTime = mean;

    fTimeProb = new TF1("TimeProb","(1/[0])*exp(-x/[0])", 0., mean*10);
    fTimeProb->SetParameter(0, mean);
    fTimeProb->GetRandom();
}

//_____________________________________________________________________________
void BmnFileSource::SetEventTimeInterval(Double_t min, Double_t max)
{
    fEventTimeMin = min;
    fEventTimeMax = max;
    fEventMeanTime = (fEventTimeMin + fEventTimeMax) / 2;
}

//_____________________________________________________________________________
void BmnFileSource::SetBeamTime(Double_t beamTime, Double_t gapTime)
{
    fBeamTime = beamTime;
    fGapTime = gapTime;
}

//_____________________________________________________________________________
void BmnFileSource::SetEventTime()
{
    LOG(debug)<<"Set event time for Entry = "<<fTimeforEntryNo<<" , where the current entry is "<<fCurrentEntryNo<<" and eventTime is "<<fEventTime;

    if (fBeamTime < 0)
        fEventTime += GetDeltaEventTime();
    else
    {
        do
        {
            fEventTime += GetDeltaEventTime();
        } while (fmod(fEventTime, fBeamTime + fGapTime) > fBeamTime);
    }

    LOG(debug)<<"New time = "<<fEventTime;
    fTimeforEntryNo = fCurrentEntryNo;
}

//_____________________________________________________________________________
Double_t BmnFileSource::GetDeltaEventTime()
{
    Double_t deltaTime = 0;
    if (fTimeProb != 0)
    {
        deltaTime = fTimeProb->GetRandom();
        LOG(debug)<<"Time set via sampling method : "<<deltaTime;
    }
    else
    {
        deltaTime = gRandom->Uniform(fEventTimeMin, fEventTimeMax);
        LOG(debug)<<"Time set via Uniform Random : "<<deltaTime;
    }

    return deltaTime;
}

//_____________________________________________________________________________
Double_t BmnFileSource::GetEventTime()
{
    LOG(debug)<<"-- Get Event Time --";

    if (!fEvtHeaderIsNew && fEvtHeader!=0)
    {
        Double_t EvtTime = fEvtHeader->GetEventTime();

        if(!(EvtTime < 0))
            return EvtTime;
    }

    if (fTimeforEntryNo != fCurrentEntryNo)
        SetEventTime();

    LOG(debug)<<"Calculate event time from user input : "<<fEventTime<<" ns";

    return fEventTime;
}

//_____________________________________________________________________________
void BmnFileSource::ReadBranchEvent(const char* BrName)
{
    /**fill the object with content if the other branches in this tree entry were already read**/
    if (fEvtHeader == 0)
        return; //No event header, Reading will start later

    if (fInTree) fInTree->FindBranch(BrName)->GetEntry(fEvtHeader->GetMCEntryNumber());
    else
        if (fInChain) fInChain->FindBranch(BrName)->GetEntry(fEvtHeader->GetMCEntryNumber());

    return;
}

//_____________________________________________________________________________
void BmnFileSource::ReadBranchEvent(const char* BrName, Int_t Entry)
{
    if (fInTree) fInTree->FindBranch(BrName)->GetEntry(Entry);
    else
        if (fInChain) fInChain->FindBranch(BrName)->GetEntry(Entry);

    return;
}

//_____________________________________________________________________________
void BmnFileSource::FillEventHeader(FairEventHeader* feh)
{
    feh->SetEventTime(fEventTime);

    if (fEvtHeader)
    {
        feh->SetRunId(fEvtHeader->GetRunId());
        feh->SetMCEntryNumber(fEvtHeader->GetMCEntryNumber());
    }

    feh->SetInputFileId(0);

    return;
}

ClassImp(BmnFileSource)

