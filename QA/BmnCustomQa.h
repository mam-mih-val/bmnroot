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
    
    void Redraw();
    
    /**
     * \brief Read data branches from input data files.
     */
    void ReadDataBranches();

    void CreateHistograms();
    void ProcessGlobal();
    
    string fPadConfFileName;
    BmnPadGenerator * fPadGenerator;
    BmnPadBranch * fPadTree;
    
    TTree* fTreeTemp;
    vector<TObject*> fNamVecIn;
    vector<TObject*> fNamVec;
    vector<TClonesArray*> fArrVecIn;
    vector<TClonesArray*> fArrVec;
    
    TCanvas * can = nullptr;


    BmnCustomQa(const BmnCustomQa&) = delete;
    BmnCustomQa& operator=(const BmnCustomQa&) = delete;

    ClassDef(BmnCustomQa, 1);
};

#endif /* BMNCUSTOMQA_H */
