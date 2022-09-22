//--------------------------------------------------------------------------------------------------------------------------------------
#ifndef __BmnTofHitProducer_H
#define __BmnTofHitProducer_H 1

#include <TClonesArray.h>
#include <FairTask.h>

#include <map>
#include <math.h>

#include "BmnTofHitProducerIdeal.h"
#include "BmnTofGeoUtils.h"
#include "BmnTof2Digit.h"
#include "BmnTrigDigit.h"
#include "BmnTof2Raw2DigitNew.h"

#define TOF2_MAX_CHAMBERS 59
#define TOF2_MAX_STRIPS_IN_CHAMBER 32

#define STRIP_CORRECTIONS 1
#define LIST_CHAMBER_CORRECTIONS 0
#define LIST_STRIP_CORRECTIONS 0
// CVERS=0 -  use last calibration tables, 1 - use *;1 version
#define CVERS 1

class TRandom2;
class TEfficiency;
class TH1D;
class TH2D;
class BmnTofGeoUtils;

class BmnTofHitProducer : public BmnTofHitProducerIdeal 
{
    Double_t	fMCTime[TOF2_MAX_CHAMBERS];
    Double_t	fMCTimeSigma[TOF2_MAX_CHAMBERS];

    Double_t tofcalc[TOF2_MAX_CHAMBERS] = {0.f};
    Double_t tofcals[TOF2_MAX_CHAMBERS][32] = {{0.f}};

    Double_t 	fTimeSigma;     // Uncertainties of time, gaus sigma [ns],  default: 100 ps
    Double_t	fErrX, fErrY; 	// Uncertainties of coordinates, gaus sigma [cm], dX= 10./sqrt(12.) mm, default: dY= 5 mm.
    TRandom2 	*pRandom;       //!

    Bool_t		fDoINL;
    Bool_t		fDoSlewing;

    // QA test histos
    TEfficiency     *effTestEfficiencySingleHit, *effTestEfficiencyDoubleHit; //!
    TH1D            *htR, *h1TestDistance, *h1TestMass, *h1TestMassLong, *h1TestOccupancyTimeShift, *h1TestOccupancyPositionShift;  //!
    TH2D            *h2TestNeighborPair, *h2TestXYSmeared, *h2TestXYSmeared2,*h2TestXYSmearedDouble, *h2TestXYSmearedDouble2, *h2TestEtaPhi, *h2TestStrips, *h2TestRZ, *h2TdetIdStripId;    //!

    Bool_t          HitExist(Double_t val);
    Bool_t          DoubleHitExist(Double_t val);

    BmnTofGeoUtils  *pGeoUtils; //!

    const char *fgeomFile;      //!

    const char *fMCTimeFile;    //!

    const char *fProtonTimeCorrectionFile;  //!

    const double fSignalVelosity; // [ns/cm]

    // input- strip edge position & signal times; output- strip crosspoint; return false, if crosspoint outside strip
    Int_t                   MergeHitsOnStripNew(void);
    bool                    GetCrossPoint(const TVector3& p1, double time1, const TVector3& p2, double time2, TVector3& crossPoint);
    bool                    GetCrossPoint(const LStrip *pStrip, double dtime, TVector3& crossPoint);
    Double_t                CalculateToF (BmnTof2Digit *d1, BmnTof2Digit *d2, BmnTrigDigit *t0);
    BmnTof2Raw2DigitNew     *fTOF2;     //!
    Int_t			fMainStripSelection;
    Int_t			fSelectXYCalibration;
    Double_t		fTimeMin;
    Double_t		fTimeMax;
    Double_t		fDiffTimeMaxSmall;
    Double_t		fDiffTimeMaxBig;

  public:
    BmnTofHitProducer(const char *name = "TOF HitProducer", const char *geomFile = "", Bool_t useMCdata = true, Int_t verbose = 1, Bool_t DoTest = false);
    virtual ~BmnTofHitProducer();

    virtual InitStatus 	Init();
    virtual void 		Exec(Option_t* opt);
    virtual void 		Finish();

    void			SetTimeResolution(Double_t sigma){ fTimeSigma = sigma; };
    void			SetAlongStripResolution(Double_t Xerr){ fErrX = Xerr;};
    void 			SetSeed(UInt_t seed = 0);

    void 			SetDoINL(bool val){ fDoINL = val;}
    void 			SetDoSlewing(bool val){ fDoSlewing = val;}
    void			SetMainStripSelection(Int_t s = 0) { fMainStripSelection = s; };
    void			SetSelectXYCalibration(Int_t s = 0) { fSelectXYCalibration = s; };
    void			SetTimeMin(Double_t mi = -2.) { fTimeMin = mi; };
    void			SetTimeMax(Double_t ma = +15.) { fTimeMax = ma; };
    void			SetDiffTimeMaxSmall(Double_t ds = 0.) { fDiffTimeMaxSmall = ds; };
    void			SetDiffTimeMaxBig(Double_t db = 0.) { fDiffTimeMaxBig = db; };

    TString			GetParameters()
    {
        char s[32];
        TString buf = "\n Run parameters: fTimeSigma="; sprintf(s, "%.5g", fTimeSigma); buf+=(s);
        buf+=" ns, fErrX="; sprintf(s, "%.4g", fErrX); buf+=(s);
        buf+=" cm, fErrY="; sprintf(s, "%.4g", fErrY); buf+=(s);
        buf+=" cm, fDoTest="; buf+=fDoTest; buf+=", fDoMergeHits="; buf+=fDoMergeHits;
        buf+=", fDoINL="; buf+=fDoINL; buf+=", fDoSlewing="; buf+=fDoSlewing;
        return buf;
    }

    int             readMCTimeFile(const char *);
    void            SetMCTimeFile(const char * file) {fMCTimeFile = file;}
    void            SetProtonTimeCorrectionFile(const char * file) {fProtonTimeCorrectionFile = file;}

  ClassDef(BmnTofHitProducer, 2);
};

#endif
