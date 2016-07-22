//------------------------------------------------------------------------------------------------------------------------
#include <iomanip>
#include <fstream>
#include <assert.h>

#include "TH2D.h"
#include "TFile.h"
#include "TClonesArray.h"

#include "BmnTOF1TDC.h"

using namespace std;



//------------------------------------------------------------------------------------------------------------------------
//----------------------------------------      TDCChannel        --------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
TDCChannel::TDCChannel(BmnTof1TDC *ptr)
: inlReady(false), fTDC(ptr)
{

}
//------------------------------------------------------------------------------------------------------------------------
void 	TDCChannel::LoadInl(double* array, size_t size)
{ 
	std::copy(&array[0], &array[size], std::inserter(inlData, inlData.begin()));  
	inlReady = (fTDC->fINLBins == inlData.size()); 
} 	
//------------------------------------------------------------------------------------------------------------------------
double 	TDCChannel::Convert(int iTime) const
{
	double slope = fTDC->fINLWidth / fTDC->fINLBins;
	double time = iTime * slope; // [ns]

	if(fTDC->fUseINL)
	{
		if(!inlReady)
		{
///			cout<<"\n ---> [TDCChannel::Convert] Warning:  fUseINL = true, inlReady = false."<<endl;
			return time;
		}
		
		size_t inlBin = iTime % fTDC->fINLBins;		
		time += inlData[inlBin] * slope;
	}
	
return time;	
}
//------------------------------------------------------------------------------------------------------------------------
//----------------------------------------      BmnTof1TDC        --------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
ClassImp(BmnTof1TDC)
BmnTof1TDC::TmTdcList	BmnTof1TDC::gTDC;
size_t 			BmnTof1TDC::fVerbose = 1;
//------------------------------------------------------------------------------------------------------------------------
BmnTof1TDC::BmnTof1TDC(size_t serial, size_t tdcType, size_t nChannels, double inlWidth, size_t inlBins)
: fTDCSerial(serial), fTDCtype(tdcType), fINLWidth(inlWidth), fINLBins(inlBins), fUseINL(true)
{	
	fChannels = new TChannal(nChannels, TDCChannel(this));	

	bool IsUniqueSerial = BmnTof1TDC::gTDC.insert(std::make_pair(serial, this)).second;
	assert(IsUniqueSerial == true);
		
	if(fVerbose > 1) cout<<" created new TDC - "<<hex<<showbase<<serial<<dec<<endl;
}
//------------------------------------------------------------------------------------------------------------------------
BmnTof1TDC::~BmnTof1TDC()
{	
	fChannels->clear();
	delete fChannels;
}
//-------------------------------------------------------------------------------------
bool BmnTof1TDC::LoadInl(int* serial, int* channel, double* arrayD, size_t size)
{
if(serial == nullptr || channel == nullptr) return false;

	BmnTof1TDC *tdc = find(*serial);
	if(tdc == nullptr)
	{ 
		cout<<"\n ---> [ BmnTof1TDC::LoadInl] Warning: unknown TDC serial: "<<hex<<showbase<<(*serial)<<dec<<". Create the TDC first."<<endl;
		return false;
	}
	
	tdc->LoadChannelInl(*channel, arrayD, size);

	if(fVerbose > 1) cout<<" loading: name - inl, TDC:Channel - "<<hex<<showbase<<(*serial)<<":"<<dec<<(*channel)<<endl;
return true;	
}
//-------------------------------------------------------------------------------------
BmnTof1TDC* 		BmnTof1TDC::find(int serial)
{
	TmTdcList::iterator it = gTDC.find(serial);
	if(it != BmnTof1TDC::gTDC.end()) return it->second;

return nullptr;
}
//-------------------------------------------------------------------------------------
BmnTof1TDC* 		BmnTof1TDC::find(const char* tdcName)
{
	for(TmTdcList::iterator it = gTDC.begin(), itEnd = gTDC.end(); it != itEnd;  it++)
	{
	  	if(TString(it->second->GetName()) == tdcName) return it->second;
	}
	
return nullptr;	
}
//------------------------------------------------------------------------------------------------------------------------
const TDCChannel*	BmnTof1TDC::GetTDCChannel(size_t tdcChannel) const
{
	if(tdcChannel >= fChannels->size()) return nullptr;
	
return 	&((*fChannels)[tdcChannel]);
}
//------------------------------------------------------------------------------------------------------------------------
const TDCChannel*	BmnTof1TDC::GetTDCChannel(int tdcSerial, size_t tdcChannel)
{
	BmnTof1TDC* tdc = find(tdcSerial);
	if(nullptr == tdc) return nullptr;
	
return tdc->GetTDCChannel(tdcChannel);
}
//------------------------------------------------------------------------------------------------------------------------
bool	BmnTof1TDC::CheckTdcType(int tdcSerial, size_t tdcType)
{
	BmnTof1TDC *tdc = find(tdcSerial);
	if(nullptr == tdc) return false;
		
return (tdc->fTDCtype == tdcType);	
}
//-------------------------------------------------------------------------------------
void	BmnTof1TDC::dump(const char* comment, ostream& out)
{
	out<<" [BmnTof1TDC::dump]   "; if(comment) out<<comment;  out<<" size = "<<gTDC.size()<<" TDCs."<<endl;

	for(TmTdcList::const_iterator it = gTDC.begin(), itEnd = gTDC.end(); it != itEnd;  it++)
	{
	  	out<<"serial: "<<hex<<showbase<<it->first<<dec<<" name: \""<<it->second->GetName()<<"\" title: \""<<it->second->GetTitle()<<"\"  "; it->second->Status(out); out<<endl;
	}
}
//-------------------------------------------------------------------------------------
void	BmnTof1TDC::setNameTitle(int serial, const char* name, const char* title)
{
	BmnTof1TDC *tdc =  find(serial);
	if(tdc == nullptr)
	{
		cerr<<"\n ---> [BmnTof1TDC::setNameTitle] Warning: unknown TDC serial - "<<hex<<showbase<<serial<<dec<<endl;
		return;
	}
	
	tdc->SetName(name);
	if(title != nullptr) tdc->SetTitle(title);
}
//------------------------------------------------------------------------------------------------------------------------
void	BmnTof1TDC::LoadChannelInl(int channel, double* arrayD, size_t size)
{
	channel--;	// FIXME: shift [1,72] to [0,71]
	assert(0 <= channel && channel < 72);		
	(*fChannels)[channel].LoadInl(arrayD, size);
}
//------------------------------------------------------------------------------------------------------------------------	
void 	BmnTof1TDC::Status(ostream& out) const
{
	// Check inl data
	out<<" [Status] -  inl: "; bool inlOK = true;
	for(size_t i = 0; i < 72; i++)
	{
		if(! (*fChannels)[i].IsInlReady()) // mustbe 1024 inl digits
		{
			if(inlOK == true){ out<<" MIS: "; inlOK = false;}
			out<<" "<<i;
		}
	}
	
	if(inlOK == true) out<<" loaded. ";
}
//------------------------------------------------------------------------------------------------------------------------	
size_t		BmnTof1TDC::GetChannel(UChar_t iChannel, UChar_t iHptdcId)  // return  tdc channel [0,71]
{
assert(iChannel < 8); 
assert(iHptdcId < 9); 
	
return  iHptdcId * 8  + iChannel;  
}
//------------------------------------------------------------------------------------------------------------------------
//----------------------------------------      BmnTof1Configure        --------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
BmnTof1Configure::BmnTof1Configure(size_t detNmb)
: isReady(0), fNDets(detNmb), fNdetSlots(4) // 4 slots per detector
{
	fDetOutputs = new Tpair* [fNDets*fNdetSlots];
	memset(fDetOutputs, 0, fNDets * fNdetSlots * sizeof(Tpair*));
}
//------------------------------------------------------------------------------------------------------------------------	
void 	BmnTof1Configure::CheckReady()
{
	if(isReady == -1) 
	{
		bool ok = true;
		for(size_t i = 0, iEnd = fNDets*fNdetSlots; i < iEnd; i++)
		{
			if( (*(fDetOutputs + i))  == nullptr)
			{
				ok = false;				
				break;
			}
		}		
		isReady	= ok ? 1 : 0;
	}
}
//------------------------------------------------------------------------------------------------------------------------
void 	BmnTof1Configure::Reset()
{
	// delete pairs
	for(size_t i = 0, iEnd = fNDets*fNdetSlots; i < iEnd; i++)
	{
		Tpair*  entry = (*(fDetOutputs + i));
		
		if(entry  != nullptr) delete entry;
	}
		
	// reset pair pointers			
	memset(fDetOutputs, 0, fNDets * fNdetSlots * sizeof(Tpair*));
	isReady = 0;
}
//------------------------------------------------------------------------------------------------------------------------	
bool 	BmnTof1Configure::InstallTDC(size_t tdcSerial, int crateSerial, int crateSlote) // return true, if new unique TDC position inserted
{
return mTDC.insert(Tlink::value_type( Hash(crateSerial, crateSlote), tdcSerial)).second; 		
}
//------------------------------------------------------------------------------------------------------------------------	
bool 	BmnTof1Configure::GetTDCserial(int crateSerial, int crateSlote, size_t& tdcSerial)
{
	Tlink::iterator it = mTDC.find( Hash(crateSerial, crateSlote) );
	if(it != mTDC.end())
	{
		tdcSerial = it->second;
		return true;
	}
	
return false;
}
//------------------------------------------------------------------------------------------------------------------------	
bool 	BmnTof1Configure::Find(size_t TDCserial, size_t tdcChannel, int& detectorID, kDetSlote& dSlote, size_t& tslotChannel)
{
	kTdcSlote tSlote = GetTDCSlote(tdcChannel, tslotChannel);
	
	for(int det=0; det < fNDets; det++)
	for(int slot=0; slot < fNdetSlots; slot++) 
	{
		const Tpair* ptr = GetOutput(det, slot);
assert(nullptr != ptr);

		if(ptr->first == TDCserial && ptr->second == (size_t)tSlote)
		{
			detectorID = det;
			dSlote = (kDetSlote) slot;
			return true;
		}	
	}	

assert(nullptr == "ERROR: undefined pair<TDCserial, kTdcSlote>. ");
return false;
}
//------------------------------------------------------------------------------------------------------------------------	 
bool 	BmnTof1Configure::LoadFromDB()
{
	
return false;
}	
//------------------------------------------------------------------------------------------------------------------------	
bool 	BmnTof1Configure::CheckIntegrity()
{
	
return false;
}
//------------------------------------------------------------------------------------------------------------------------	
void 	BmnTof1Configure::Status(bool doPrint)
{
	CheckReady();
		
	if(1 == isReady) cout<<" [TofConfigure::Status] It's ready."<<endl;
	if(0 == isReady) cout<<" [TofConfigure::Status] It's NOT ready."<<endl;	
		
	if(doPrint)
	{
		cout<<"----------- Connection list ----------------->>>"<<endl;
		for(int det=0; det < fNDets; det++)
		for(int slot=0; slot < fNdetSlots; slot++) 
		{
			const Tpair* ptr = GetOutput(det, slot);
			
			if(ptr != nullptr)	cout<<" detId: "<<det<<" dSlote: "<<slot<<" dcSerial: "<<hex<<showbase<< ptr->first<<dec<<" tSlote: "<<ptr->second<<endl;					
			else			cout<<" detId: "<<det<<" dSlote: "<<slot<<" dcSerial: -------- tSlote: ---"<<endl;
		}	
		cout<<"----------- Connection list -----------------<<<"<<endl;		
	}			
}	
//------------------------------------------------------------------------------------------------------------------------
bool 	BmnTof1Configure::AddConnection(size_t detectorID, kDetSlote dSlote, size_t TDCserial, kTdcSlote tSlote)
{
assert(detectorID < fNDets);
assert(nullptr != BmnTof1TDC::find(TDCserial));

return _addConnection(detectorID, dSlote, TDCserial, tSlote);
}
//------------------------------------------------------------------------------------------------------------------------
bool 	BmnTof1Configure::AddConnection(size_t detectorID, kDetSlote dSlote, const char* tdcName, kTdcSlote tSlote)
{
assert(detectorID < fNDets);

	BmnTof1TDC *tdc = BmnTof1TDC::find(tdcName);
assert(nullptr != tdc);

return _addConnection(detectorID, dSlote, tdc->GetSerial(), tSlote);
}
//------------------------------------------------------------------------------------------------------------------------
bool 	BmnTof1Configure::_addConnection(size_t detectorID, kDetSlote dSlote, size_t TDCserial, kTdcSlote tSlote)
{
	int slote = (int)dSlote;
	if(nullptr != GetOutput(detectorID, slote))
	{
		cerr<<"\n ---> [TDC::AddConnection] Connection already exist."<<endl;
		return false;
	}

	//cout<<"\n addConnection detectorID= "<<detectorID<<" dSlote= "<<slote<<" TDCserial= "<<TDCserial<<" tSlote= "<<tSlote<<endl;
	SetOutput(detectorID, slote, new Tpair(TDCserial, tSlote));	
	isReady = -1;

return true;
}
//------------------------------------------------------------------------------------------------------------------------	
bool 	BmnTof1Configure::RemoveConnection(size_t detectorID, kDetSlote dSlote)
{
assert(detectorID < fNDets);

	int slote = (int)dSlote;
	if(nullptr == GetOutput(detectorID, slote))
	{
		cerr<<"\n ---> [TDC::RemoveConnection] Connection don't exist."<<endl;
		return false;
	}
	
	ResetOutput(detectorID, slote);
	isReady = 0;
		
return true;
}	
//------------------------------------------------------------------------------------------------------------------------	
bool 	BmnTof1Configure::RemoveConnection(size_t tdcSerial, kTdcSlote tdcSlote)
{
	for(size_t i = 0, iEnd = fNDets*fNdetSlots; i < iEnd; i++)
	{
		Tpair*&  entry = (*(fDetOutputs + i));
		
		if(entry != nullptr && entry->first == tdcSerial && entry->second == tdcSlote) 
		{		
			delete entry;
			entry = nullptr;
			
			isReady = 0;			
			return true;			
		}	
	}

	cerr<<"\n ---> [TDC::RemoveConnection] Connection don't exist."<<endl;
return false;			
}	
//------------------------------------------------------------------------------------------------------------------------
//----------------------------------------      BmnTof1TDCMapping       --------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
BmnTof1TDCMapping::BmnTof1TDCMapping(BmnTof1Configure* config, size_t verbose)
: fConfig(config), fVerbose(verbose)
{
assert(fConfig->IsReady() == true);
	
	UInt_t  fplane, fstrip;
	char side_c;	
	long hash, fserial, fchan, nLines = 0;
	
	TString flnm = getenv("VMCWORKDIR"); flnm += "/macro/tof_400/map.txt";
	fstream ff(flnm.Data(), std::fstream::in);
	
	while(!ff.eof()) 
	{
		ff >> std::hex >> fserial >> std::dec >> fchan >> fplane >> fstrip >> side_c;
		if(ff.eof()) {break;}
		if(side_c != 'L' && side_c != 'R'){ cout << "Error: side is not L or R" << endl; continue;}
		
		hash = fserial | (fchan << 48);	
		BmnTof1Digit digit(fplane, fstrip, ((side_c == 'L') ? 0 : 1), -1., -1.);
			
		bool IsUniqueKey = fMapping.insert(make_pair(hash, digit)).second;							
		if(!IsUniqueKey) { cout << "\n ---> WARNING: [BmnTof1TDCMapping] TDC #" << std::hex << fserial << std::dec << " chan.: "<<fchan<<" already loaded."<<endl;}
		
		nLines++;
	}
	
	ff.close();	
	cout << " -I [BmnTof1TDCMapping] Loaded " << nLines << " lines in map.txt" << endl;				
}
//------------------------------------------------------------------------------------------------------------------------
bool 	BmnTof1TDCMapping::Convert(UInt_t TDCserial, size_t tdcChannel, const BmnTof1Digit** digit)  // dUpDown:  <0, Up> <1, Down>
{
	BmnTof1TDC *pTdc = BmnTof1TDC::find(TDCserial);
assert(nullptr != pTdc);
assert(tdcChannel < 72);

	long hash = TDCserial | (tdcChannel << 48);	
	TmMapping::const_iterator it = fMapping.find(hash);

	if(it != fMapping.end())
	{	
		*digit = & it->second;	
			
		//if(fVerbose > 1) 
		cout<<"\n [TofTDCMapping::Convert] serial:"<<hex<<showbase<<TDCserial<<dec<<", channel:"<<tdcChannel
					<<" ---> detID:"<<it->second.GetPlane()<<", strip: "<<it->second.GetStrip()<<", updown:"<<it->second.GetSide();		
		return true;
	}
	
	*digit = nullptr;
	cout<<"\n ---> [TofTDCMapping::Convert] WARNING: unmapped tdcSerial:"<<hex<<showbase<<TDCserial<<dec<<", channel:"<<tdcChannel<<endl;	
return false;	
}
//------------------------------------------------------------------------------------------------------------------------
//----------------------------------------     BmnTof1DigitConvertor    --------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
void	BmnTof1DigitConvertor::AddDigit(BmnTDCDigit *pTdcDigit, const TDCChannel *pChannel, const BmnTof1Digit* pTofDigit)
{ 
	digitData data(pTdcDigit, pChannel, pTofDigit);
	Int_t uid = (pTofDigit->GetPlane() << 16) | (pTofDigit->GetStrip() << 8) | pTofDigit->GetSide();  // Hash 	
	
	mmDigits.insert(MMpairType::value_type(uid, data)); 
}
//------------------------------------------------------------------------------------------------------------------------
size_t	BmnTof1DigitConvertor::Convert(TClonesArray *aTofDigits) 
{
	aTofDigits->Clear();
		
	size_t NtofDigits = 0;
	float time, timeLeading, timeTrailing;
	const BmnTof1Digit *pDigit;
	
	for(MMpairType::const_iterator it = mmDigits.begin(), itEnd = mmDigits.end(); it != itEnd ; )
	{
  		int counter = mmDigits.count(it->first); // one strip one side digits
  		if(counter > 1)
  		{  		  			
  			bool LeadingFound = false,  TrailingFound = false;
			timeLeading = timeTrailing = 1.e+20; // big value

  			for(int i=0; i<counter; i++) // cycle by BmnTDCDigit for same "strip side"
  			{
  				const digitData& data = it->second;
 
 cout<<"\n\n  TDC time  "<<data.pTdcDigit->GetValue()<<endl; 
 data.pTofDigit->print("DIGIT"); 

							 			
  				if(data.pTdcDigit->GetLeading() == true) 
  				{					
  	  				LeadingFound = true;										
  					time = data.pTdcChannel->Convert(data.pTdcDigit->GetValue());	
  					
 cout<<"\n  LeadingFound "<<	data.pTdcDigit->GetValue()<<" "<<time;	
  									
  					if(time < timeLeading) timeLeading = time;					   				
  				}
  				else 
  				{
  		  			TrailingFound = true;
  		  			time = data.pTdcChannel->Convert(data.pTdcDigit->GetValue());

 cout<<"  TrailingFound "<<	data.pTdcDigit->GetValue()<<" "<<time<<endl;	

  		  			if(time < timeTrailing)
  		  			{
  		  				timeTrailing = time;
  		  				pDigit = data.pTofDigit;	
  		  			}
  				}	  				
  				it++;
  			} // cycle by BmnTDCDigit for same "strip side"
  			
  			if(LeadingFound && TrailingFound) // add TofDigit
  			{ 							
  				BmnTof1Digit *ptr = new ((*aTofDigits)[aTofDigits->GetEntriesFast()]) BmnTof1Digit(pDigit, timeLeading, timeTrailing - timeLeading);
				//if(fVerbose > 1)  
				 ptr->print("ADD digit");					
  				NtofDigits++;		
  			} 			
  		}
  		else it++;
	}
		
return 	NtofDigits;
}
//------------------------------------------------------------------------------------------------------------------------
void	BmnTof1DigitConvertor::AddSample(size_t tdcSerial, size_t tdcChannel)
{
	mmCorrelations.insert(make_pair(tdcSerial, tdcChannel));
}
//------------------------------------------------------------------------------------------------------------------------
void	BmnTof1DigitConvertor::CalcCorrelationMatrix()
{
	for(TmmCorr::const_iterator it = mmCorrelations.begin(), itEnd = mmCorrelations.end(); it != itEnd;  it = mmCorrelations.upper_bound(it->first)) // cycle by unique tdcSerial
	{
		size_t tdcSerial = it->first;
		
		mHistos::iterator itHisto = fHistos.find(tdcSerial);
		if(itHisto == fHistos.end()) // create histo for new tdcSerial
		{
			TString name = "corr_"; name += tdcSerial;
			TH2D *h2 = new TH2D(name.Data(), "", 50, -0.5, 49.5, 50, -0.5, 49.5);
			itHisto = fHistos.insert(make_pair(tdcSerial, h2)).first; // update iterator to current  tdcSerial histo
		}
		
		size_t counter = mmCorrelations.count(tdcSerial);
		
		
		
	}



	mmCorrelations.clear();
}
//------------------------------------------------------------------------------------------------------------------------
void	BmnTof1DigitConvertor::Write(const char* flnm)
{
	TFile file(flnm, "RECREATE");	
	for(mHistos::const_iterator it = fHistos.begin(), itEnd = fHistos.end(); it != itEnd; it++)  it->second->Write();
	file.Close();
}
//------------------------------------------------------------------------------------------------------------------------



