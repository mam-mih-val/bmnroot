#ifndef BMNTOF1HITPRODUCER_H
#define BMNTOF1HITPRODUCER_H 1

#include "BmnTOF1Detector.h"
#include "BmnTof1GeoUtils.h"
#include "BmnTof1HitProducerIdeal.h"
#include "BmnTrigDigit.h"

#include <TRandom2.h>
#include <TEfficiency.h>
#include <TH1D.h>
#include <TH2D.h>


class BmnTof1HitProducer : public BmnTof1HitProducerIdeal
{
    Double_t fTimeSigma;    // Uncertainties of time, gaus sigma [ns],  default: 100 ps
    Double_t fErrX, fErrY;  // Uncertainties of coordinates, gaus sigma [cm], dX= 10./sqrt(12.) mm, default: dY= 5 mm.
    TRandom2 *pRandom;      //!

    Int_t fPeriod, fRun;

    // QA test histos
    TEfficiency *effTestEfficiencySingleHit, *effTestEfficiencyDoubleHit;  //!
    TH1D *htR, *h1TestDistance;                                            //!
    TH2D *h2TestNeighborPair, *h2TestXYSmeared, *h2TestXYSmeared2, *h2TestXYSmearedDouble, *h2TestXYSmearedDouble2,
        *h2TestEtaPhi, *h2TestStrips, *h2TestRZ, *h2TdetIdStripId;  //!

    Bool_t HitExist(Double_t val);
    Bool_t DoubleHitExist(Double_t val);

    BmnTof1GeoUtils *pGeoUtils;  //!

    BmnTOF1Detector **pDetector;  //!
    Int_t fNDetectors;

    const double fSignalVelosity;  // [ns/cm]

    TString NameFileLRcorrection, NameFileSlewingCorrection, NameFileTimeShiftCorrection;
    Bool_t FlagFileLRcorrection, FlagFileSlewingCorrection, FlagFileTimeShiftCorrection;

    BmnTrigDigit *FingT0Digit();
    Bool_t IsFile(TString NameFile);
    Bool_t SetCorrFiles();
    Bool_t OutOfRange(Int_t iPlane);

    InitStatus LoadDetectorConfiguration();

  public:
    BmnTof1HitProducer(const char *name = "TOF1 HitProducer", Bool_t useMCdata = true, Int_t verbose = 1, Bool_t DoTest = false);
    virtual ~BmnTof1HitProducer();

    virtual InitStatus Init();
    virtual void Exec(Option_t *opt);
    virtual void Finish();

    virtual InitStatus OnlineInit();
    virtual InitStatus OnlineRead(const std::unique_ptr<TTree> &dataTree, const std::unique_ptr<TTree> &resultTree);
    virtual void OnlineWrite(const std::unique_ptr<TTree> &dataTree);

    void SetTimeResolution(Double_t sigma) { fTimeSigma = sigma; };
    void SetAlongStripResolution(Double_t Xerr) { fErrX = Xerr; };
    void SetSeed(UInt_t seed = 0);

    TString GetParameters() {
        char s[32];
        TString buf = "\n Run parameters: fTimeSigma="; sprintf(s, "%.5g", fTimeSigma); buf+=(s);
        buf+=" ns, fErrX="; sprintf(s, "%.4g", fErrX); buf+=(s);
        buf+=" cm, fErrY="; sprintf(s, "%.4g", fErrY); buf+=(s);
        buf+=" cm, fDoTest="; buf+=fDoTest; buf+=", fDoMergeHits="; buf+=fDoMergeHits;
        return buf;
    }

    void SetPeriodRun(Int_t p, Int_t r) {
        fPeriod = p;
        fRun = r;
    }

  ClassDef(BmnTof1HitProducer, 3);
};

#endif
