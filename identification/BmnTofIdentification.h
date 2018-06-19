#ifndef BMNTOFIDENTIFICATION_H
#define BMNTOFIDENTIFICATION_H 1

// STL
#include <iostream>
// FairSoft
#include <TNamed.h>
#include <TChain.h>
#include <TDatabasePDG.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TStopwatch.h>
//BmnRoot
#include <BmnTofHit.h>
#include <BmnGlobalTracking.h>

class BmnTofIdentification : public TObject {
public:
    BmnTofIdentification();
    virtual ~BmnTofIdentification();
    
    static void MakePlots(TString NameFileIn = "bmndst.root", Int_t nEvForRead = 0);
private:

    ClassDef(BmnTofIdentification, 1)
};

#endif /* BMNTOFIDENTIFICATION_H */

