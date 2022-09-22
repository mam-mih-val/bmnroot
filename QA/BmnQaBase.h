/**
 * \file BmnQaBase.h
 * \brief FairTask for tracking performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \author Ilnur Gabdrakhmanov <ilnur@jinr.ru> - disentangle Exp and MC processing
 * \date 2007-2022
 */

#ifndef BMNQABASE_H_
#define BMNQABASE_H_

#include <time.h>
#include <chrono>

#include "FairTask.h"
#include <TSystem.h>

#include "BmnGemStripStationSet.h"
#include "BmnSiliconStationSet.h"
#include "BmnCSCStationSet.h"
#include "BmnMCTrackCreator.h"
#include "CbmVertex.h"
#include "BmnTrackingQaExpReport.h"

class BmnHistManager;

using std::string;

class BmnQaBase : public FairTask {
public:

    /**
     * \brief Constructor.
     */
    BmnQaBase() {};
    
    /**
     * \brief Constructor.
     */
    BmnQaBase(const char* name, Int_t iVerbose);
    
    /**
     * \brief Destructor.
     */
    virtual ~BmnQaBase();

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

    void SetMonitorMode(const Bool_t mm) {
        fMonitorMode = mm;
    }

    Bool_t GetMonitorMode() const {
        return fMonitorMode;
    }
   void SetObjServer(THttpServer * s) { fServer = s; }
   THttpServer * GetObjServer() const { return fServer; }


protected:
    /**
     * \brief Read data branches from input data files.
     */
    virtual void ReadDataBranches() = 0;

    virtual void ReadEventHeader();


    virtual void CreateHistograms() = 0;
    virtual void ProcessGlobal() = 0;
    
    void CreateH1(
            const string& name,
            const string& xTitle,
            const string& yTitle,
            Int_t nofBins,
            Double_t minBin,
            Double_t maxBin);

    void CreateH2(
            const string& name,
            const string& xTitle,
            const string& yTitle,
            const string& zTitle,
            Int_t nofBinsX,
            Double_t minBinX,
            Double_t maxBinX,
            Int_t nofBinsY,
            Double_t minBinY,
            Double_t maxBinY);


    THttpServer * fServer;
    TString fOutName;

    BmnHistManager* fHM; // Histogram manager
    string fOutputDir; // Output directory for results
    
    BmnSimulationReport* fReport;
    
    Bool_t fMonitorMode;


    
    Int_t fEventNo;  // event counter
    
   const Int_t fNItersToUpdate = 20;
   const chrono::seconds fTimeToUpdate = chrono::seconds(5); //! redraw each timeout seconds
   Int_t fNItersSinceUpdate;
   chrono::time_point<chrono::system_clock> fTicksLastUpdate; //!

    BmnQaBase(const BmnQaBase&) = delete;
    BmnQaBase& operator=(const BmnQaBase&) = delete;

    ClassDef(BmnQaBase, 1);
};

#endif /* BMNQABASE_H_ */
