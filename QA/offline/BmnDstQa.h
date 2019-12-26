#include <TNamed.h>
#include <TH1F.h>
#include <TH2F.h>
#include "BmnQaHistoManager.h"

#include <BmnOfflineQaSteering.h>

#ifndef BMNDSTQA_H
#define BMNDSTQA_H 1

using namespace std;

class BmnDstQa : public TNamed {
public:

    /** Default constructor **/
    BmnDstQa() {}
    BmnDstQa(UInt_t);
    //BmnDstQa(TString);

    /** Destructor **/
    virtual ~BmnDstQa() {
    };

    // Setters

    // Getters

    BmnQaHistoManager* GetManager() {
        return fHistoManager;
    }

private:
    BmnOfflineQaSteering* fSteering;
    BmnQaHistoManager* fHistoManager;
    
    BmnGemStripStationSet* fDetGem;
    BmnCSCStationSet* fDetCsc;
    BmnSiliconStationSet* fDetSilicon;
    
    TString prefix;
      
    void Hits();
    void Tracks();
    void Matching();
    void PID();
     
    void BasicTrackDistributions();
    void HitsDistributions();   
    void MatchingHistos();
    void EfficiencyHistos();
    void ResidualsPullsHistos();
    void OccupancyHistos();        
    void AverageStripValuePerHit(); 
    void AverageStripValuePerTrack();

    ClassDef(BmnDstQa, 1);
    
};

#endif