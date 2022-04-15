//--------------------------------------------------------------------------------------------------------------------------------------
#ifndef __BmnLANDHitProducer_H
#define __BmnLANDHitProducer_H 1

#include <TClonesArray.h>
#include <FairTask.h>
#include <TVector3.h>
#include <map>
#include <math.h>
#include "BmnLANDHit.h"
#include "BmnTrigDigit.h"
#include "BmnLANDDigit.h"

#define LAND_PLANE_N 6
#define LAND_BAR_N 20

class TRandom2;
class TEfficiency;
class TH1D;
class TH2D;
//class BmnTof1GeoUtils;
//--------------------------------------------------------------------------------------------------------------------------------------
class BmnLANDHitProducer : public FairTask 
{
protected:
	TClonesArray		*aExpDigits;   // Exp input
	TClonesArray		*aExpDigitsT0;
	TClonesArray		*aLandHits;     //! output
	
	Bool_t			fOnlyPrimary;
	Bool_t			fUseMCData;
public:
	BmnLANDHitProducer(const char *name = "LAND HitProducer", Bool_t useMCdata = true, Int_t verbose = 1, Bool_t DoTest = false);
	virtual ~BmnLANDHitProducer();

	virtual InitStatus 	Init();
	virtual void 		Exec(Option_t* opt);
	virtual void 		Finish();

	bool SetVelMap(TString filename);

private:
	struct VScint {
	  VScint(): vscint(1) {}
	  Float_t vscint;
	};
	VScint m_vscint[LAND_PLANE_N][LAND_BAR_N];

ClassDef(BmnLANDHitProducer, 2);
};

#endif
//--------------------------------------------------------------------------------------------------------------------------------------

