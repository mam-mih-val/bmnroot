//--------------------------------------------------------------------------------------------------------------------------------------
#ifndef __BmnTof1HitProducer_H
#define __BmnTof1HitProducer_H 1

#include<TClonesArray.h>
#include<FairTask.h>


class TRandom2;
//--------------------------------------------------------------------------------------------------------------------------------------
class BmnTof1HitProducer : public FairTask 
{
        Double_t 	fTimeSigma;             // [sigma]=ns,  default 100 ps
   
	Bool_t		fDoTest;	//!
	TRandom2 	*pRandom;	//!

	TClonesArray	*aTofPoints; 
	TClonesArray	*aMCTracks; 	 
	TClonesArray 	*aTofHits; 		

	Bool_t 		fOnlyPrimary;


public:
	BmnTof1HitProducer(const char *name = "TOF1 HitProducer", Int_t verbose = 1, Bool_t DoTest = false);
	virtual ~BmnTof1HitProducer();

	virtual InitStatus 	Init();
	virtual void 		Exec(Option_t* opt);
	virtual void 		Finish();

	void 			SetOnlyPrimary(Bool_t opt = kFALSE) { fOnlyPrimary = opt; }
	void			SetTimeSigma(Double_t sigma){ fTimeSigma = sigma; };
	void			Dump(const char* title = NULL, ostream& out = std::cout){Dump(aTofHits, title, out);};

private:

	void			Dump(TClonesArray *array, const char* title = NULL, ostream& out = std::cout);

ClassDef(BmnTof1HitProducer, 1);
};

#endif
//--------------------------------------------------------------------------------------------------------------------------------------

