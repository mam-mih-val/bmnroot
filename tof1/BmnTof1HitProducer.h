//--------------------------------------------------------------------------------------------------------------------------------------
#ifndef __BmnTof1HitProducer_H
#define __BmnTof1HitProducer_H 1

#include <TClonesArray.h>
#include <FairTask.h>

#include <map>
#include <math.h>

struct LStrip
{
enum Side_t { kUp=0, kRight=1, kDown=2, kLeft=3, kInvalid= -1 };

 	TVector3 	center, point[4]; 
 	Int_t 		detectorUID, regionID, moduleID, stripID; 
 	Int_t 		neighboring[4]; // same as  k_side enum
 	
 	LStrip() : detectorUID(kInvalid), regionID(kInvalid), moduleID(kInvalid), stripID(kInvalid) { neighboring[kUp] = kInvalid;  neighboring[kRight] = kInvalid; neighboring[kDown] = kInvalid; neighboring[kLeft] = kInvalid; }
 	
 	
	void 		SetIDs(Int_t uid, Int_t region, Int_t  module, Int_t  strip){ detectorUID = uid; regionID = region; moduleID = module; stripID = strip;}	
	
	inline bool	IsSameModule(const LStrip& strip){ return ( regionID == strip.regionID && moduleID == strip.moduleID );}
	inline bool 	operator==(const LStrip& rhs){ return ( regionID == rhs.regionID && moduleID == rhs.moduleID && stripID == rhs.stripID);}
	inline bool 	operator!=(const LStrip& rhs){ return !((*this) == rhs);}
	
	void		Dump(const char* comment = nullptr, ostream& out = std::cout) const 
	{ 	
		if(comment) out<<comment; 
		out<<"  ids: "<<regionID<<", "<<moduleID<<", "<<stripID<<", center: "<< center.X()<<","<<center.Y()<<","<<center.Z();
	}
	
	Double_t 	Distance(Side_t side, const LStrip& strip)
	{
		Double_t min1 = 1.e+10, min2 = 1.e+10; // big value
		
		if((*this) == strip) 		return min1+min2; // same strip
		if(!IsSameModule(strip)) 	return min1+min2; // different modules
		
		TVector3 *p1, *p2;
		switch(side)
		{
		case kUp: 	p1 = &point[0]; p2 = &point[1]; break;
		case kRight: 	p1 = &point[1]; p2 = &point[2]; break;	
		case kDown: 	p1 = &point[2]; p2 = &point[3]; break;
		case kLeft: 	p1 = &point[3]; p2 = &point[0]; break;				
		};
		
		for(Int_t iSide = kUp; iSide <= kLeft; iSide++)
		{
			double value 	= fabs((*p1 - strip.point[iSide]).Mag());	if(value < min1) min1 = value;	 
			value 		= fabs((*p2 - strip.point[iSide]).Mag());	if(value < min2) min2 = value;
		}
		
	return min1 + min2;	
	}
	
};

class TRandom2;
class TEfficiency;
class TH1D;
class TH2D;
//--------------------------------------------------------------------------------------------------------------------------------------
class BmnTof1HitProducer : public FairTask 
{
        Double_t 	fTimeSigma;     // Uncertainties of time, gaus sigma [ns],  default: 100 ps
   	Double_t	fErrX, fErrY; 	// Uncertainties of coordinates, gaus sigma [cm], dX= 10./sqrt(12.) mm, default: dY= 5 mm.
   	
	Bool_t		fDoTest;	//! 
	TRandom2 	*pRandom;	//! 

	TClonesArray	*aTofPoints; 
	TClonesArray	*aMCTracks; 	 
	TClonesArray 	*aTofHits; 		

	Bool_t 		fOnlyPrimary;
	
	// QA test histos
	TString		fTestFlnm;
	TList		fList;//!
	TEfficiency	*effTestEfficiencySingleHit, *effTestEfficiencyDoubleHit; //!
	TH1D		*h1TestDistance;  //! 
	TH2D		*h2TestMergedTimes, *h2TestChainPID, *h2TestXYSmeared, *h2TestXYSmearedDouble, *h2TestNeighborPair; //!

	
typedef std::map<Int_t, LStrip> 	MStripType; // pair<detectorUID, Strip parameters>
typedef MStripType::const_iterator	MStripCIT;
typedef MStripType::iterator		MStripIT;

	MStripType		mStrips; //! 

	void			ParseTGeoManager();
	void			FindNeighborStrips();
	void			AddHit(Int_t UID, TVector3 XYZ_smeared, TVector3 XYZ_err, Int_t pointIndex, Double_t time);	
	Int_t 			MergeHitsOnStrip(); // leave only the fastest hit in the strip 	
	Int_t 			CompressHits();
	
	void			Dump(TClonesArray *array, const char* title = NULL, ostream& out = std::cout)const;	
	Bool_t 			HitExist(Double_t distance)const;
	Bool_t 			DoubleHitExist(Double_t distance)const;
	const LStrip*		FindStrip(Int_t UID)const;
	void			MinDistanceToEdge(const TVector3* pos,const LStrip *strip, Double_t& distance, LStrip::Side_t& side)const;
	Double_t		DistanceFromPointToLineSegment(const TVector3* pos, const TVector3& A,const TVector3& B)const;


public:
	BmnTof1HitProducer(const char *name = "TOF1 HitProducer", Int_t verbose = 1, Bool_t DoTest = false);
	virtual ~BmnTof1HitProducer();

	virtual InitStatus 	Init();
	virtual void 		Exec(Option_t* opt);
	virtual void 		Finish();

	void 			SetOnlyPrimary(Bool_t opt = kFALSE) { fOnlyPrimary = opt; }
	void			SetTimeSigma(Double_t sigma){ fTimeSigma = sigma; };
	void			SetYSigma(Double_t Yerr){ fErrY = Yerr;};
	void 			SetSeed(UInt_t seed = 0);
			
	void			Dump(const char* comment = nullptr, ostream& out = std::cout) const { Dump(aTofHits, comment, out); };

	
ClassDef(BmnTof1HitProducer, 1);
};

#endif
//--------------------------------------------------------------------------------------------------------------------------------------

