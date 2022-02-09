//--------------------------------------------------------------------------------------------------------------------------------------
#ifndef __BmnTofCalHitProducer_H
#define __BmnTofCalHitProducer_H 1

#include <TClonesArray.h>
#include <FairTask.h>
#include <TVector3.h>
#include <map>
#include <math.h>
#include "BmnTofCalHit.h"
#include "BmnTrigDigit.h"
#include "BmnTofCalDigit.h"

#define TOFCAL_ARM_N 2
#define TOFCAL_PLANE_N 4
#define TOFCAL_BAR_N 15

class TRandom2;
class TEfficiency;
class TH1D;
class TH2D;
//class BmnTof1GeoUtils;
//--------------------------------------------------------------------------------------------------------------------------------------
class BmnTofCalHitProducer : public FairTask 
{
protected:
	TClonesArray		*aExpDigits;   // Exp input
	TClonesArray		*aExpDigitsT0;
	TClonesArray		*aTofcalHits;     //! output
	
	Bool_t			fOnlyPrimary;
	Bool_t			fUseMCData;
public:
	BmnTofCalHitProducer(const char *name = "TofCal HitProducer", Bool_t useMCdata = true, Int_t verbose = 1, Bool_t DoTest = false);
	virtual ~BmnTofCalHitProducer();

	virtual InitStatus 	Init();
	virtual void 		Exec(Option_t* opt);
	virtual void 		Finish();

	bool SetVelMap(TString filename);

private:
	struct VScint {
	  VScint(): vscint(1) {}
	  Float_t vscint;
	};
	VScint m_vscint[TOFCAL_ARM_N][TOFCAL_PLANE_N][TOFCAL_BAR_N];

ClassDef(BmnTofCalHitProducer, 2);
};

#endif
//--------------------------------------------------------------------------------------------------------------------------------------

