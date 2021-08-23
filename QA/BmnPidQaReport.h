/**
 * \file BmnTrackingQaReport.h
 * \brief Create report for tracking QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef BMNPIDQAREPORT_H_
#define BMNPIDQAREPORT_H_

#include "TParticlePDG.h"
#include "report/BmnSimulationReport.h"
#include <string>
#include <vector>
#include <unordered_map>
#include "TEfficiency.h"
#include "report/BmnDrawOnline.h"
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
    BmnPidQaReport(TString name,  TString storageName, vector<TParticlePDG*> particles, unordered_map<Double_t, std::pair<string, vector<Int_t>>> massTable);

    /**
     * \brief Destructor.
     */
    virtual ~BmnPidQaReport();

protected:
    /**
     * \brief Inherited from BmnSimulationReport.
     */
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
    void DrawOneBanana(const TString canvasName, const TString name1,
                        const vector<TGraph*>& graphs, const vector<string>& graphLabels);
    void DrawTwoBananas(const TString canvasName, const TString name1, const TString name2, 
                        const vector<TGraph*>& graphs, const vector<string>& graphLabels);
    void DrawThreeH1(const TString canvasName, const TString name1, const TString name2, const TString name3);

    void DrawEffCont(const TString canvasName, TEfficiency*, TEfficiency*);

    void DrawEventsInfo(const TString canvasName);

    void PrepareBananaGraphs();

    string PrintEventInfo();

    vector<string> fGlobalTrackVariants;


    vector<TParticlePDG*> fParticles;
    vector<TGraph*> fGraphs;
    vector<string> fGraphLabels;


    //{Mass: [Particle name: (pdgCode1, pdgCode2, ...)], ...}
    unordered_map<Double_t, std::pair<string, vector<Int_t> > > fMassTable; 

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
