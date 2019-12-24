// -------------------------------------------------------------------------
// -----                MpdLAQGSMGeneratorExt header file                 -----
// -------------------------------------------------------------------------

/** MpdLAQGSMGeneratorExt
 * Extracts specific particle from sim files and stores momentum to the branch
 * Or reads events as ordinary simulator depending on passed parameters
 * 
 *@author Elena Litvinenko  <litvin@nf.jinr.ru>
 *@author Ilnur Gabdrakhmanov <ilnur@jinr.ru>
 *@version 9.12.2019
 *
 ** The MpdLAQGSMGeneratorExt uses the ASCII input 
 ** provided by K.Gudima LAQGSM event generator.
 **/

#ifndef MPDLAQGSMGENERATOREXT_H
#define MPDLAQGSMGENERATOREX_H 1

#include "MpdLAQGSMGenerator.h"

#include <fstream>
#include <map>
#include <vector>

#include <TVector3.h>
#include <TClonesArray.h>

#include <BmnFairRunSim.h>

#ifndef __CINT__
#include <zlib.h>
#endif

using namespace std;

class MpdLAQGSMGeneratorExt : public MpdLAQGSMGenerator {
public:

    /** Default constructor without arguments should not be used. **/
    MpdLAQGSMGeneratorExt();

    /** Standard constructor. 
     ** @param fileName The input file name vector
     ** @param use_collider_system 
     ** @param QGSM_format_ID
     ** @param Max_Event_Number
     ** @param pdg PDG code to extract
     ** @param extract_from_dir true - Do extraction from text files to the branch
     * false - read events from previously extracted
     **/
    MpdLAQGSMGeneratorExt(
            const vector<TString> &fileNames,
            const Bool_t use_collider_system = kTRUE,
            Int_t QGSM_format_ID = 0,
            Int_t Max_Event_Number = 0,
            Int_t pdg = 3122,
            Bool_t extract_from_dir = kTRUE);

    /** Destructor. **/
    virtual ~MpdLAQGSMGeneratorExt();

    //  static la_tab_t la_tab[78];    //!  list of light particles known for MpdLAQGSMGeneratorExt

    /** Reads on event from the input file and pushes the tracks onto
     ** the stack. Abstract method in base class.
     ** @param primGen  pointer to the FairPrimaryGenerator
     **/
    Bool_t ReadEvent(FairPrimaryGenerator* primGen);
    Bool_t(MpdLAQGSMGeneratorExt::*ReadEventImpl)(FairPrimaryGenerator* primGen);
    Bool_t ReadEventFromTxt(FairPrimaryGenerator* primGen);
    Bool_t ExtractEventFromTxt(FairPrimaryGenerator* primGen);
    Bool_t ReadEventFromFiltered(FairPrimaryGenerator* primGen);
    Bool_t SkipEvents(Int_t nSkip); //AZ
    
    BmnFairRunSim* GetRunSimInst() {
        return fRunSimInst;
    }
    void SetRunSimInst(BmnFairRunSim* v) {
        fRunSimInst = v;
    }

protected:
    Bool_t fExtract_from_dir;
    Int_t iFile;
    Int_t fSelPDG; ///<- particle to extract
    vector<TString> fFileVec; ///<- files to cycle
    TClonesArray *fMom;//!
    TString fMomBranchName;
    BmnFairRunSim * fRunSimInst;//!
    
    TTree * fMTree;//!
    TFile *fSourceFile;//!
    Long64_t fNEvents;
    Long64_t fIEvent;
    
    Bool_t OpenNext();

    ClassDef(MpdLAQGSMGeneratorExt, 1);

};

#endif
