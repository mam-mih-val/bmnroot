/**
 * \file BmnPidQaReport.h
 * \brief Create report for tracking QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2021
 */
#ifndef BMNPIDQAREPORT_H_
#define BMNPIDQAREPORT_H_

#include "TParticlePDG.h"
#include "BmnSimulationReport.h"
#include <string>
#include <vector>
#include <unordered_map>
#include "TEfficiency.h"
#include "BmnDrawOnline.h"
using std::string;
using std::vector;
using std::unordered_map;
class TH1;

/**
 * \class BmnPidQaReport
 * \brief Create report for pid QA.
 
 */
class BmnPidQaReport : public BmnSimulationReport {
public:
    /**
     * \brief Constructor.
     */
    BmnPidQaReport();

    /**
     * \brief Constructor with parameters.
     */
    BmnPidQaReport(TString name,  TString storageName, unordered_map<Double_t, string> massTable);

    /**
     * \brief Destructor.
     */
    virtual ~BmnPidQaReport();

protected:

virtual void Create();
    /**
     * \brief Inherited from BmnSimulationReport.
     */
    virtual void Draw();

   
    void DrawOneH1(const TString canvasName, const TString name1, const TString drawOpt);
    void DrawTwoH1(const TString canvasName, const TString name1, const TString name2, const TString drawOpt);
    void DrawTwoH2(const TString canvasName, const TString name1, const TString name2);
    void DrawThreeH2(const TString canvasName, const TString name1, const TString name2, const TString name3);
    void DrawOneH2(const TString canvasName, const TString name1);
    void DrawTwoBananas(const TString canvasName, const TString name1, const TString name2);
    void DrawThreeH1(const TString canvasName, const TString name1, const TString name2, const TString name3);

    void DrawEffCont(const TString canvasName, TEfficiency*, TEfficiency*);

    vector<string> fGlobalTrackVariants;
  
    //{Mass: Particle name}
    unordered_map<Double_t, string> fMassTable; 

    TString fPrefix;
    TString fStorageName;

private:
	 BmnDrawOnline* drawHist;
     TEfficiency* effTof400;
     TEfficiency* contTof400;
     TEfficiency* effTof700;
     TEfficiency* contTof700;
     TEfficiency* effHitsTof400;
     TEfficiency* contHitsTof400;
     TEfficiency* effHitsTof700;
     TEfficiency* contHitsTof700;


    ClassDef(BmnPidQaReport, 1)
};

#endif /* BMNPIDQAREPORT_H_ */
