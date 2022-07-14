/**
 * \file BmnCustomQa.h
 * \brief FairTask for tracking performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \author Ilnur Gabdrakhmanov <ilnur@jinr.ru> - disentangle Exp and MC processing
 * \date 2007-2022
 */

#ifndef BMNCUSTOMQA_H
#define BMNCUSTOMQA_H

#include <time.h>
#include <chrono>

#include <TTree.h>
#include <TClass.h>
#include <TBranch.h>
#include <TObjString.h>


#include "FairTask.h"

#include "DstEventHeader.h"
#include "BmnGemStripStationSet.h"
#include "BmnSiliconStationSet.h"
#include "BmnCSCStationSet.h"
#include "BmnMCTrackCreator.h"
#include "CbmVertex.h"
#include "BmnQaBase.h"

#include "BmnHist.h"
#include "BmnPadBranch.h"
#include "BmnPadGenerator.h"
//#include "BmnCustomQaReport.h"

class BmnHistManager;
class BmnTrackMatch;
class BmnMCTrackCreator;
class BmnGlobalElectronId;

using std::string;
using std::vector;

class BmnCustomQa : public BmnQaBase {
public:

    /**
     * \brief Constructor.
     */
    BmnCustomQa() {};
    
    /**
     * \brief Constructor.
     */
    BmnCustomQa(string name, string padConfFile);

    /**
     * \brief Destructor.
     */
    virtual ~BmnCustomQa();

    /**
     * \brief Derived from FairTask.
     */
    virtual InitStatus Init();

    /**
     * \brief Derived from FairTask.
     */
    virtual void Exec(
            Option_t* opt);

    /**
     * \brief Derived from FairTask.
     */
    virtual void Finish();

    void SetOutputDir(const std::string& dir) {
        fOutputDir = dir;
    }


private:
    /**
     * \brief Read data branches from input data files.
     */
    void ReadDataBranches();


    void ReadEventHeader();


    void CreateHistograms();
    void ProcessGlobal();

    string fPadConfFileName;
    BmnPadGenerator * fPadGenerator;
    BmnPadBranch * fPadTree;
    
    TTree* fTreeTemp;
    std::vector<TNamed*> fNamVecIn;
    std::vector<TNamed*> fNamVec;
    std::vector<TClonesArray*> fArrVecIn;
    std::vector<TClonesArray*> fArrVec;
    
    TCanvas * can = nullptr;


    BmnCustomQa(const BmnCustomQa&) = delete;
    BmnCustomQa& operator=(const BmnCustomQa&) = delete;

    ClassDef(BmnCustomQa, 1);
};

#endif /* BMNCUSTOMQA_H */
