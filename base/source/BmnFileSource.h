//
//  BmnFileSource.h
//  BmnRoot
//
//  Created by Mohammad Al-Turany on 08/02/14. Modified by Konstantin Gertsenberger 26.05.2016
//

#ifndef BMNFILESOURCE_H
#define BMNFILESOURCE_H

#include "FairSource.h"
#include "FairEventHeader.h"
#include "FairFileHeader.h"
#include "FairRuntimeDb.h"

#include "TFile.h"
#include "TString.h"
#include "TChain.h"
#include "TFolder.h"
#include "TF1.h"

#include <list>
using namespace std;

class BmnFileSource : public FairSource
{
public:
    BmnFileSource(TFile* f, const char* Title = "BmnRootFile", UInt_t identifier = 0);
    BmnFileSource(const TString* RootFileName, const char* Title = "BmnRootFile", UInt_t identifier = 0);
    BmnFileSource(const TString RootFileName, const char* Title = "BmnRootFile", UInt_t identifier = 0);
    BmnFileSource(const TString RootFileName, int& period_number, int& run_number);
    virtual ~BmnFileSource();
    
    Bool_t Init();
    Int_t ReadEvent(UInt_t i=0);
    void Close();
    void Reset();

    virtual Source_Type GetSourceType() {return kFILE;} //kBMNFILE
    virtual void SetParUnpackers() {}
    virtual Bool_t InitUnpackers() {return kTRUE;}
    virtual Bool_t ReInitUnpackers() {return kTRUE;}

    /**Check the maximum event number we can run to*/
    virtual Int_t CheckMaxEventNo(Int_t EvtEnd = 0);
    /**Read the tree entry on one branch**/
    virtual void ReadBranchEvent(const char* BrName);
    /**Read specific tree entry on one branch**/
    virtual void ReadBranchEvent(const char* BrName, Int_t Entry);
    virtual void FillEventHeader(FairEventHeader* feh);

    const TFile* GetRootFile() {return fRootFile;}
    /** Add a friend file (input) by name)*/
    void AddFriend(TString FileName);
    /**Add ROOT file to input, the file will be chained to already added files*/
    void AddFile(TString FileName);
    void AddFriendsToChain();
    void PrintFriendList();
    Bool_t CompareBranchList(TFile* fileHandle, TString inputLevel);
    void CheckFriendChains();
    void CreateNewFriendChain(TString inputFile, TString inputLevel);
    TTree* GetInTree() {return fInChain->GetTree();}
    TChain* GetInChain() {return fInChain;}
    TFile* GetInFile() {return fRootFile;}
    void CloseInFile() {if (fRootFile) fRootFile->Close();}
    /**Set the input tree when running on PROOF worker*/
    void SetInTree(TTree* tempTree);
    TObjArray* GetListOfFolders() {return fListFolder;}
    TFolder* GetBranchDescriptionFolder() {return fCbmroot;}
    UInt_t GetEntries() {return fNoOfEntries; }

    void SetInputFile(TString name);

    /** Set the repetition time of the beam when it can interact (beamTime) and when no interaction happen (gapTime). The total repetition time is beamTime + gapTime */
    void SetBeamTime(Double_t beamTime, Double_t gapTime);
    /** Set the min and max limit for event time in ns */
    void SetEventTimeInterval(Double_t min, Double_t max);
    /** Set the mean time for the event in ns */
    void SetEventMeanTime(Double_t mean);
    void SetEventTime();
    Double_t GetDeltaEventTime();
    void SetFileHeader(FairFileHeader* f) {fFileHeader = f;}
    Double_t GetEventTime();

    virtual Bool_t ActivateObject(TObject** obj, const char* BrName);

    /**Set the status of the EvtHeader
     *@param Status:  True: The header was creatged in this session and has to be filled
              FALSE: We use an existing header from previous data level
     */
    void SetEvtHeaderNew(Bool_t Status) {fEvtHeaderIsNew = Status;}
    Bool_t IsEvtHeaderNew() {return fEvtHeaderIsNew;}

    Bool_t SpecifyRunId() { return kTRUE; }

private:
    /** Title of input source, could be input, background or signal*/
    TString fInputTitle;
    /**ROOT file*/
    TFile* fRootFile;
    /** Current Entry number */
    Int_t fCurrentEntryNr;  //!
    /** List of all files added with AddFriend */
    list<TString> fFriendFileList; //!
    list<TString> fInputChainList;//!
    map<TString, TChain*> fFriendTypeList;//!
    map<TString, list<TString>* > fCheckInputBranches; //!
    list<TString> fInputLevel; //!
    map<TString, std::multimap<TString, TArrayI> > fRunIdInfoAll; //!
    /**Input Chain */
    TChain* fInChain;
    /**Input Tree */
    TTree* fInTree;
    /** list of folders from all input (and friends) files*/
    TObjArray* fListFolder; //!
    /** RuntimeDb*/
    FairRuntimeDb* fRtdb;
    /**folder structure of output*/
    TFolder* fCbmout;
    /**folder structure of input*/
    TFolder* fCbmroot;
    /***/
    UInt_t fSourceIdentifier;
    /**No of Entries in this source*/
    UInt_t fNoOfEntries;
    /**Initialization flag, true if initialized*/
    Bool_t IsInitialized;
    
    BmnFileSource(const BmnFileSource&) = delete;
    BmnFileSource operator=(const BmnFileSource&) = delete;

    /**Event Header*/
    FairEventHeader* fEvtHeader; //!

    /**File Header*/
    FairFileHeader* fFileHeader; //!

    /**This flag is true if the event header was created in this session
    * otherwise it is false which means the header was created in a previous data
    * level and used here (e.g. in the digi)
    */
    Bool_t fEvtHeaderIsNew; //!

    /** for internal use, to return the same event time for the same entry*/
    UInt_t fCurrentEntryNo; //!
    /** for internal use, to return the same event time for the same entry*/
    UInt_t fTimeforEntryNo; //!

    /** min time for one event (ns) */
    Double_t fEventTimeMin;  //!
    /** max time for one Event (ns) */
    Double_t fEventTimeMax;  //!
    /** Time of event since th start (ns) */
    Double_t fEventTime;     //!
    /** Time of particles in beam (ns) */
    Double_t fBeamTime; //!
    /** Time without particles in beam (gap) (ns) */
    Double_t fGapTime; //!
    /** EventMean time used (P(t)=1/fEventMeanTime*Exp(-t/fEventMeanTime) */
    Double_t fEventMeanTime; //!
    /** used to generate random numbers for event time; */
    TF1* fTimeProb;      //!
    /** True if the file layout should be checked when adding files to a chain.
     *  Default value is true.
     */
    Bool_t fCheckFileLayout; //!

    ClassDef(BmnFileSource, 0)
};

#endif /* defined(BMNFILESOURCE_H) */
