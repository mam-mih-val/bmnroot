//--------------------------------------------------------------------------------------------------------------------------------------
#ifndef __BmnTof1HitProducer_H
#define __BmnTof1HitProducer_H 1

#include <TClonesArray.h>
#include <FairTask.h>

#include <map>
#include <math.h>

#include "BmnTof1HitProducerIdeal.h"
#include "BmnTof1GeoUtils.h"
#include "BmnTof1Digit.h"
#include "BmnTrigDigit.h"
#include "BmnTOF1Detector.h"

class TRandom2;
class TEfficiency;
class TH1D;
class TH2D;
class BmnTof1GeoUtils;
//--------------------------------------------------------------------------------------------------------------------------------------
class BmnTof1HitProducer : public BmnTof1HitProducerIdeal 
{
        Double_t 		fTimeSigma;     // Uncertainties of time, gaus sigma [ns],  default: 100 ps
   	Double_t		fErrX, fErrY; 	// Uncertainties of coordinates, gaus sigma [cm], dX= 10./sqrt(12.) mm, default: dY= 5 mm.
        TRandom2 		*pRandom;    
   
   	Bool_t			fDoINL;
   	Bool_t			fDoSlewing;   	
   
	// QA test histos
	TEfficiency			*effTestEfficiencySingleHit, *effTestEfficiencyDoubleHit; //!		
        TH1D                		*htR, *h1TestDistance;
        TH2D   				*h2TestNeighborPair, *h2TestXYSmeared, *h2TestXYSmeared2,*h2TestXYSmearedDouble, *h2TestXYSmearedDouble2, *h2TestEtaPhi, *h2TestStrips, *h2TestRZ, *h2TdetIdStripId;
						
	Bool_t 				HitExist(Double_t val); 
	Bool_t 				DoubleHitExist(Double_t val);

	BmnTof1GeoUtils			*pGeoUtils;
        
        BmnTOF1Detector                 **pDetector;
        Int_t                           fNDetectors;
	
	const 	double		fSignalVelosity; // [ns/cm]

	// input- strip edge position & signal times; output- strip crosspoint; return false, if crosspoint outside strip 
	bool			GetCrossPoint(const TVector3& p1, double time1, const TVector3& p2, double time2, TVector3& crossPoint);
	bool                    GetCrossPoint(const LStrip1 *pStrip, double time1, double time2, TVector3& crossPoint);
        Double_t                CalculateToF (BmnTof1Digit *d1, BmnTof1Digit *d2, BmnTrigDigit *t0);
public:
	BmnTof1HitProducer(const char *name = "TOF1 HitProducer", Bool_t useMCdata = true, Int_t verbose = 1, Bool_t DoTest = false);
	virtual ~BmnTof1HitProducer();

	virtual InitStatus 	Init();
	virtual void 		Exec(Option_t* opt);
	virtual void 		Finish();

	void			SetTimeResolution(Double_t sigma){ fTimeSigma = sigma; };
	void			SetAlongStripResolution(Double_t Xerr){ fErrX = Xerr;};	
	void 			SetSeed(UInt_t seed = 0);	
	
	void 			SetDoINL(bool val){ fDoINL = val;}
	void 			SetDoSlewing(bool val){ fDoSlewing = val;}		
	
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
	
ClassDef(BmnTof1HitProducer, 2);
};

#endif
//--------------------------------------------------------------------------------------------------------------------------------------

