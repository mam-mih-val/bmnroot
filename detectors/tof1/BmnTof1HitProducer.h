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
        
        Int_t                   fPeriod, fRun;
   
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
        
        TString NameFileLRcorrection, NameFileSlewingCorrection, NameFileTimeShiftCorrection;
        Bool_t FlagFileLRcorrection, FlagFileSlewingCorrection, FlagFileTimeShiftCorrection;

        BmnTrigDigit*           FingT0Digit();
        Bool_t                  IsFile(TString NameFile);
        Bool_t                  SetCorrFiles();
        Bool_t                  OutOfRange(Int_t iPlane);
        
public:
	BmnTof1HitProducer(const char *name = "TOF1 HitProducer", Bool_t useMCdata = true, Int_t verbose = 1, Bool_t DoTest = false);
	virtual ~BmnTof1HitProducer();

	virtual InitStatus 	Init();
	virtual void 		Exec(Option_t* opt);
	virtual void 		Finish();

	void			SetTimeResolution(Double_t sigma){ fTimeSigma = sigma; };
	void			SetAlongStripResolution(Double_t Xerr){ fErrX = Xerr;};	
	void 			SetSeed(UInt_t seed = 0);	
	
	TString			GetParameters()
	{ 
		char s[32];
		TString buf = "\n Run parameters: fTimeSigma="; sprintf(s, "%.5g", fTimeSigma); buf+=(s);  
		buf+=" ns, fErrX="; sprintf(s, "%.4g", fErrX); buf+=(s); 
		buf+=" cm, fErrY="; sprintf(s, "%.4g", fErrY); buf+=(s); 
		buf+=" cm, fDoTest="; buf+=fDoTest; buf+=", fDoMergeHits="; buf+=fDoMergeHits;
		return buf;
	}
        
        void                    SetPeriodRun (Int_t p, Int_t r)
        {
                fPeriod = p;
                fRun = r;
        }
        
        
	
ClassDef(BmnTof1HitProducer, 3);
};

#endif
//--------------------------------------------------------------------------------------------------------------------------------------

