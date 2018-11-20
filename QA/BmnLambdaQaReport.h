/**
 * \file BmnLambdaQaReport.h
 * \brief visualising simulated lambda reconstruction performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \author Alexander Lytaev <sas-lyt@ya.ru> - modifications for BMN experiment 
 * \date 2018 July
 */

#ifndef BMNLAMBDAQAREPORT_H_
#define BMNLAMBDAQAREPORT_H_

#include "report/BmnSimulationReport.h"
#include "BmnGemStripStationSet.h"
#include "BmnGemStripStationSet_RunSpring2017.h"
#include <TClonesArray.h>
#include <string>
using std::string;

class BmnLambdaQaReport : public BmnSimulationReport {
public:
    /**
     * \brief Constructor.
     */
    BmnLambdaQaReport(Bool_t fUseMCFile = kTRUE, Bool_t fUseRecoFile = kFALSE, Short_t key2 = 'a', TString name = "lambda_qa", TString keyAddition = "", Bool_t drawPoints = kTRUE, TClonesArray* particlePairsInfo = nullptr);

    /**
     * \brief Destructor.
     */
    virtual ~BmnLambdaQaReport();

private:

    virtual void Create();
    virtual void Draw();
    void PrintEventInfo(); 
    void DrawNumberOfReconstructableLambdaHistograms(const string& canvasName);
    void DrawNumberOfLambdaHistograms(const string& canvasName);
    void DrawGeomEfficiencyHistograms(const string& canvasName);
    void DrawTwoDimensionalRecGeomEfficiencyHistograms(const string& canvasName);
    void DrawNumberOfLambdaDecayProtonsHistograms( const string& canvasName );
    void DrawTwoDimensionalRecProtonsRecEfficiencyHistograms(const string& canvasName);
    void DrawRecProtonsGeomEffHistograms( const string& canvasName );
    void DrawNumberOfLambdaDecayMesonsHistograms( const string& canvasName );
    void DrawRecMesonsGeomRecEffHistograms( const string& canvasName );
    void DrawTwoDimensionalRecMesonsGeomRecEfficiencyHistograms(const string& canvasName);
    void DrawNumberOfReconstructedLambdaHistograms(const string& canvasName);
    void DrawReconstructionEfficiencyHistograms(const string& canvasName);
    void DrawTwoDimensionalRealRecEfficiencyHistograms( const string& canvasName );
    void DrawNumberOfLambdaDecayProtonsReconstructedHistograms( const string& canvasName );
    void DrawTwoDimensionalRealRecEfficiencyRecProtonsHistograms( const string& canvasName );
    void DrawRecProtonsReconstructionEfficiencyHistograms(const string& canvasName);
    void DrawNumberOfLambdaDecayMesonsReconstructedHistograms( const string& canvasName );
    void DrawTwoDimensionalRealRecEfficiencyRecMesonsHistograms( const string& canvasName );
    void DrawRecMesonsReconstructionEfficiencyHistograms(const string& canvasName);
    void DrawNumberOfNotReconstructedLambdasHistograms(const string& canvasName);
    void DrawReconstructedFromMCLambdasHistograms(const string& canvasName);
    void DrawTwoDimensinalReconstructedFromMCLambdasHistograms(const string& canvasName);
    void DrawReconstructedFromMCWOCutsLambdasHistograms(const string& canvasName);
    void DrawTwoDimensinalReconstructedFromMCWOCutsLambdasHistograms(const string& canvasName);
    void DrawReconstructedLambdasHistograms(const string& canvasName);
    void DrawTwoDimensinalReconstructedLambdasHistograms(const string& canvasName);
    void DrawReconstructedLambdasWOCutsHistograms(const string& canvasName);
    void DrawTwoDimensinalReconstructedLambdasWOCutsHistograms(const string& canvasName);
       
    TString fOutName;
    Bool_t fUseMCFile;
    Bool_t fUseRecoFile;
    Bool_t fDrawPoints;
    Char_t fKey2;
    TString fKeyAddition;
    TString drawPointsOpt;
    TClonesArray* fParticlePairsInfo;
    
    ClassDef(BmnLambdaQaReport, 1)
};

#endif /* BMNLAMBDAQAREPORT_H_ */
