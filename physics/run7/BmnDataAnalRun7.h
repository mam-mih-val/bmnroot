using namespace std;

#include <FairTrackParam.h>
#include <BmnEfficiency.h>
#include <BmnMassSpectrumAnal.h>

#include <TGeoManager.h>

#ifndef BMNDATAANALRUN7_H
#define BMNDATAANALRUN7_H 1

class BmnDataAnalRun7 : public BmnEfficiency {
public:

    BmnDataAnalRun7() {
        ;
    }

    BmnDataAnalRun7(TString);

    void SetTarget(TString t) {
        fTarget = t;
    }
    
    void SetGeometryFile(TString file) {
        fGeomFile = file;    
    }
    
    void SetNEventsInFile(Int_t nEvs) {
        fNEvents = nEvs;
    }
    
    void SetNFilesInList(Int_t nFiles) {
        fNFiles = nFiles;    
    }
    
    // Anals. to be done ...
    void doResidAnal();    
    
    virtual ~BmnDataAnalRun7() {
        if (mAnal)
            delete mAnal; 
        
     
    }



private:
    BmnMassSpectrumAnal* mAnal;
    FairRunAna* fAna;
        
    TString fTarget;
    TString fGeomFile;
    
    Int_t fNFiles;
    
private:
    void doKalman(BmnGlobalTrack*, Double_t, Double_t&, Double_t&);
    void doKalman(BmnGlobalTrack*, vector <BmnHit>, Double_t, Double_t&, Double_t&);
    
    void DoNormalization(TH1F*);

    ClassDef(BmnDataAnalRun7, 0)
};

#endif