//------------------------------------------------------------------------------------------------------------------------
#ifndef __BMNTOF1TDC_H
#define	__BMNTOF1TDC_H 1

#include <assert.h>
#include <map>
#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TVector3.h"
#include "TList.h"

#include "BmnTDCDigit.h"
#include "BmnTof1Digit.h"
class BmnTof1TDC;
class TH2D;
//------------------------------------------------------------------------------------------------------------------------
class TDCChannel
{
	std::vector<double> 	inlData;
	bool			inlReady;
	const BmnTof1TDC*	fTDC;

public:
	TDCChannel(BmnTof1TDC*);

	void LoadInl(double* array, size_t size);	
	bool IsInlReady() const { return inlReady; }	
	
	double Convert(int iTime) const; // return [ns]
};
//------------------------------------------------------------------------------------------------------------------------
class BmnTof1TDC : public TNamed
{
typedef std::vector<TDCChannel>		TChannal;
	TChannal 		*fChannels; //!
	const size_t		fTDCSerial, fTDCtype;
	
	static size_t		fVerbose;

	bool			fUseINL;
	const double		fINLWidth;
	const size_t		fINLBins;
			
public:
typedef std::map<int, BmnTof1TDC*> 	TmTdcList;

	friend class TDCChannel;

private:
static 	TmTdcList			gTDC;	

public:

	BmnTof1TDC( size_t serial, size_t tdcType, size_t nChannels, double inlWidth, size_t inlBins);
	~BmnTof1TDC();

	size_t			GetSerial()const { return fTDCSerial;}
	void			LoadChannelInl(int channel, double* arrayD, size_t size);
	void 			Status(ostream& out) const;	
	void 			SetUseINL(bool val) { fUseINL = val; }
	const 	TDCChannel*	GetTDCChannel(size_t tdcChannel)const;	
	
	static	bool			CheckTdcType(int tdcSerial, size_t tdcType);
	static	BmnTof1TDC* 		find(int serial);
	static	BmnTof1TDC* 		find(const char* tdcName);
		
	static 	bool 			LoadInl(int* tdcSerial, int* tdcChannel, double* arrayD, size_t size);
	static	const TDCChannel*	GetTDCChannel(int tdcSerial, size_t tdcChannel);
	
	static 	void			dump(const char* comment = nullptr, ostream& out = std::cout);
	static	void			setNameTitle(int serial, const char* name, const char* title = nullptr); 
	static	void			SetVerboselevel(size_t level){	fVerbose = level;}
	
	static size_t			GetChannel(UChar_t iChannel, UChar_t iHptdcId); // return  tdc channel [0,71]
	
ClassDef(BmnTof1TDC, 2);
};
//------------------------------------------------------------------------------------------------------------------------
class BmnTof1Configure
{

public:
	enum  kDetSlote { kDetSlote_1 = 0, kDetSlote_2, kDetSlote_3, kDetSlote_4 };
	enum  kTdcSlote { kTdcSlote_1 = 0, kTdcSlote_2, kTdcSlote_3};
private:

	const size_t fNDets, fNdetSlots;

typedef std::map<long, size_t> 	Tlink;			// pairs<Hash(CRATEserial, CRATEslote), TDCserial>
Tlink					mTDC;

typedef std::pair<size_t, size_t> 	Tpair;		// pairs<TDCserial, TDCslote>
Tpair**					fDetOutputs;	// fDetOutputs[fNDets][fNdetSlots]

	int 				isReady; // -1 == unknown state, 0 == Not Ready, 1 == Ready

	inline const Tpair* GetOutput(size_t detId, size_t detSlote) const // getter
	{
	assert(detId < fNDets);
	assert(detSlote < fNdetSlots);
	
	return *(fDetOutputs + detId * fNdetSlots + detSlote);
	}

	inline void SetOutput(size_t detId, size_t detSlote, Tpair* pair)  // setter
	{
	assert(detId < fNDets);
	assert(detSlote < fNdetSlots);
	
	 *(fDetOutputs + detId * fNdetSlots + detSlote) = pair;
	}

	inline void ResetOutput(size_t detId, size_t detSlote)  // resetter
	{
	assert(detId < fNDets);
	assert(detSlote < fNdetSlots);
	
	delete (*(fDetOutputs + detId * fNDets + detSlote));
	*(fDetOutputs + detId * fNdetSlots + detSlote) = nullptr;
	}

	inline kTdcSlote GetTDCSlote(size_t tdcChannel, size_t& tslotChannel) const    // tdcChannel [0,71] - tcd channel,  tslotChannel [0, 23] - tdc slote channel
	{ 
		if(tdcChannel < 24){ 	tslotChannel = tdcChannel; 	return kTdcSlote_1;} 
		if(tdcChannel < 48){ 	tslotChannel = tdcChannel - 24; return kTdcSlote_2;} 
					tslotChannel = tdcChannel - 48; return kTdcSlote_3;
	}
		
	inline long Hash(long crateSerial, long crateSlote) // MUSTBE: crateSerial < 0xFFFFFFFF (32 bits)
	{
		return (crateSlote << 32) | crateSerial; 
	}
	
	void CheckReady();

	bool 	_addConnection(size_t detectorID, kDetSlote dSlote, size_t TDCserial, kTdcSlote tSlote);

public:
	BmnTof1Configure(size_t detNmb);
	
	void 	Reset();
	bool 	IsReady(){ CheckReady(); return (isReady == 1);}
	bool 	InstallTDC(size_t tdcSerial, int crateSerial, int crateSlote); // return true, if new unique TDC position inserted
	bool 	GetTDCserial(int crateSerial, int crateSlote, size_t& tdcSerial);
	bool 	Find(size_t tdcSerial, size_t tdcChannel, int& detID, kDetSlote& dSlote, size_t& normChannel);
	bool 	LoadFromDB();
	bool 	CheckIntegrity();
	void 	Status(bool doPrint = true);
	
	
	bool 	AddConnection(size_t detID, kDetSlote dSlote, size_t TDCserial, kTdcSlote tSlote);
	bool 	AddConnection(size_t detectorID, kDetSlote dSlote, const char* tdcName, kTdcSlote tSlote);	
	bool 	RemoveConnection(size_t detID, kDetSlote detSlote);
	bool 	RemoveConnection(size_t tdcSerial, kTdcSlote tdcSlote);	
};

#define slote_D1 BmnTof1Configure::kDetSlote_1
#define slote_D2 BmnTof1Configure::kDetSlote_2
#define slote_D3 BmnTof1Configure::kDetSlote_3
#define slote_D4 BmnTof1Configure::kDetSlote_4

#define slote_T1 BmnTof1Configure::kTdcSlote_1
#define slote_T2 BmnTof1Configure::kTdcSlote_2
#define slote_T3 BmnTof1Configure::kTdcSlote_3
//------------------------------------------------------------------------------------------------------------------------
class BmnTof1TDCMapping
{
	BmnTof1Configure* 	fConfig;
	size_t			fVerbose;
	
// LEFT			RIGHT 
// UP
//	kDetSlote_1	kDetSlote_2
//    	-----------------
//	|		|
//	|		|
//	-----------------
//	kDetSlote_3	kDetSlote_4 
// DOWN

typedef	std::map<long, BmnTof1Digit> 	TmMapping;
	TmMapping			fMapping;

public:
	BmnTof1TDCMapping(BmnTof1Configure* config, size_t verbose = 1);
	bool Convert(UInt_t TDCserial, size_t tdcChannel, const BmnTof1Digit**);  // dUpDown:  <0, Up> <1, Down>

};
//------------------------------------------------------------------------------------------------------------------------
class  BmnTof1DigitConvertor // looking for pair leading-trailing TDC digit and convert to Tof digit 
{

struct digitData
{
	const BmnTDCDigit	*pTdcDigit;
	const TDCChannel	*pTdcChannel;
	const BmnTof1Digit	*pTofDigit;	

	digitData(BmnTDCDigit* pTdc, const TDCChannel* pChan, const BmnTof1Digit* pTof)
	: pTdcDigit(pTdc), pTdcChannel(pChan), pTofDigit(pTof) {}
};

typedef std::multimap<Int_t, digitData> 	MMpairType; // pair< Hash(detID, stripID, stripSideID), digitData >
MMpairType					mmDigits; //!

	size_t				fVerbose;

typedef std::multimap<size_t, size_t> 	TmmCorr;	
	TmmCorr				mmCorrelations; //! pair<tdcSerial, tdcChannel>
typedef std::map<size_t, TH2D*>		mHistos;	
	mHistos				fHistos;
	
public:
 	BmnTof1DigitConvertor(size_t verbose = 1) : fVerbose(verbose){}

	void	Reset(){ mmDigits.clear(); }
	void	AddDigit(BmnTDCDigit *pTdcDigit, const TDCChannel*, const BmnTof1Digit*);
	size_t	Convert(TClonesArray *aTofDigits); 

	void	AddSample(size_t tdcSerial, size_t tdcChannel);
	void	CalcCorrelationMatrix();
	void	Write(const char* flnm);
};

//------------------------------------------------------------------------------------------------------------------------
#endif
