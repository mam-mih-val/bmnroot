
//-------------------------------------------------------------------------------------
#if !defined(__CINT__) || defined(__MAKECINT__)
// ROOT includes
#include "TROOT.h"
#include "TString.h"
#include "TStopwatch.h"
#include "TSystem.h"
#include "TChain.h"
#include "TNamed.h"
#include "TVector3.h"
#include "Riosfwd.h"                 
#include "TGeoManager.h"            
#include "TList.h"
#include "TObject.h"
#include "TFile.h"
#include "TBrowser.h"
#include "TGeoNavigator.h"
#include "TGeoVolume.h"
#include "TGeoBBox.h"
#include "TClonesArray.h"

// Fair includes
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoInterface.h"          
#include "FairGeoRootBuilder.h"       



// BMN includes
#include "UniSearchCondition.h"
#include "UniDbDetectorParameter.h"
#include "UniDbRun.h"

#include "BmnTOF1Point.h"
#include "BmnTDCDigit.h"
#include "BmnTof1Digit.h"

#include "BmnTOF1TDC.h"
#include "BmnTof1GeoUtils.h"

#include "TClonesArray.h"

#include <assert.h>
#include <iomanip>
#include <iostream>
#include <string.h>
//////#include <tuple>

using namespace std;
#endif



//#include "../db_structures.h"
//------------------------------------------------------------------------------------------------------------------------
bool 	ParseGeoFile(BmnTof1GeoUtils*, const char* geoFlnm);
bool 	LoadGeoFile(int runId, const char* geoFlnm);
void	LoadTofDBParameters();
//------------------------------------------------------------------------------------------------------------------------
// before run:
// $ export TOF1inFlnm=bmn_run0014.root
//------------------------------------------------------------------------------------------------------------------------
void digitConvertor()
{
	char* inFlnm = getenv("TOF1inFlnm");
	char* outFlnm = getenv("TOF1outFlnm");	
assert(	inFlnm != nullptr);
	
	TString outFile, inFile(inFlnm);
	if(nullptr != outFlnm) 	outFile = outFlnm;
	else 			{outFile = "tof1digit_"; outFile += inFile;}
	
	// Load DB geometry
	TString geoFlnm("geo.root");
	bool geoLoadOK = LoadGeoFile(689, geoFlnm.Data());
assert(geoLoadOK);	
	
	// Parsing geo file
	BmnTof1GeoUtils *pGeoUtils = new BmnTof1GeoUtils;
	bool geoParseOK = ParseGeoFile(pGeoUtils, geoFlnm.Data());
assert(geoParseOK);
	
	
	// Create TDCs
	//BmnTof1TDC(int serial, size_t tdcType, size_t nChannels, double inlWidth, size_t inlBins)
//	BmnTof1TDC tdc1(0xFFE9DA91, 0x12, 72, 25., 1024);
//	BmnTof1TDC tdc2(0x076CCF62, 0x12, 72, 25., 1024); 
//	BmnTof1TDC tdc3(0x076CAD53, 0x12, 72, 25., 1024);	
//	BmnTof1TDC tdc4(0xFFE9F5CB, 0x12, 72, 25., 1024);
	
	BmnTof1TDC tdc1(0xFFE9DA91, 0x12, 72, 25., 1024);
	BmnTof1TDC tdc2(0x076CCF62, 0x12, 72, 25., 1024); 
	BmnTof1TDC tdc3(0x076CAD53, 0x12, 72, 25., 1024);	
	BmnTof1TDC tdc4(0xFFE9F5CB, 0x12, 72, 25., 1024);	
	
	
	// Set unique TDC name id
	tdc2.SetNameTitle("TDC_N2", "blablabla2222");
	tdc1.SetNameTitle("TDC_N1", "blablabla2222");
	tdc3.SetNameTitle("TDC_N3", "blablabla2222");
	tdc4.SetNameTitle("TDC_N4", "blablabla2222");						
//	BmnTof1TDC::setNameTitle(0x168fdca, "tdcN1", "blablabla");		
	
	// Load DB parameters
	LoadTofDBParameters();		
	
	BmnTof1TDC::dump();	
	
	// Setup configure
	BmnTof1Configure config(3); // 0x168fdca, 0x1692190, 0x46f3326, 0x46f3872	
	config.Reset();

// +Y
// LEFT			RIGHT 
// UP
//	kDetSlote_1	kDetSlote_2
//    	-----------------
//	|		|
//	|		|
//	-----------------
//	kDetSlote_3	kDetSlote_4 
//      -X	 0	+X -->
// DOWN
// -Y
	
	// Connect detectors with TDC (detID, detSlote, tdcSerial, tdcSlote) 
	// detector# 1
	config.AddConnection(0, slote_D1, "TDC_N1", slote_T3);
	config.AddConnection(0, slote_D2, "TDC_N1", slote_T2);
	config.AddConnection(0, slote_D3, "TDC_N1", slote_T1);	
	config.AddConnection(0, slote_D4, "TDC_N2", slote_T3);
	
	// detector# 2
	config.AddConnection(1, slote_D2, "TDC_N2", slote_T2);
	config.AddConnection(1, slote_D1, "TDC_N2", slote_T1);
	config.AddConnection(1, slote_D4, "TDC_N3", slote_T3);	
	config.AddConnection(1, slote_D3, "TDC_N3", slote_T2);

	// detector# 3
	config.AddConnection(2, slote_D1, "TDC_N3", slote_T1);
	config.AddConnection(2, slote_D2, "TDC_N4", slote_T3);
	config.AddConnection(2, slote_D3, "TDC_N4", slote_T2);	
	config.AddConnection(2, slote_D4, "TDC_N4", slote_T1);
	
	// Install TDC to CRATE (TDC_serial, CRATE_serial, CRATE_slote)
//	config.InstallTDC(0xFFE9DA91, 0x1690454, 4);
//	config.InstallTDC(0x076CCF62, 0x1690454, 5);
//	config.InstallTDC(0x076CAD53, 0x1690454, 6);
//	config.InstallTDC(0xFFE9F5CB, 0x1690454, 7);	

	config.InstallTDC(0xFFE9DA91, 0x1690454, 4);
	config.InstallTDC(0x076CCF62, 0x1690454, 5);
	config.InstallTDC(0x076CAD53, 0x1690454, 6);
	config.InstallTDC(0xFFE9F5CB, 0x1690454, 7);


	config.Status();
		
	//config.RemoveConnection(0x46f3872, slote_T2);	
	//config.AddConnection(2, slote_D3, 0x46f3872, slote_T2);		
	//config.RemoveConnection(1, slote_D3);	
	//config.Status();

	BmnTof1TDCMapping tofMapping(&config);

	// ------------------------------------------------------------------------ processing  BmnTDCDigit ---> BmnTof1Digit
	// Open output file
	TFile file(outFile.Data(),"RECREATE");
	if(file.IsZombie()) 
	{
       		cerr<<"\n\t --->ERROR: opening output file:\""<<outFile.Data()<<"\""<<endl;
       		exit(-1);
	}	
	
	TClonesArray *aTofDigits = new TClonesArray("BmnTof1Digit");	
	TTree *treeOut =  new TTree("cbmsim", "only Tof1Digits now");
	treeOut->Branch("bmn_tof400", &aTofDigits);		
	
	// Open input file
	TChain *treeIn = new TChain("BMN_RAW");
	treeIn->Add(inFile.Data());
	
	Int_t nEntries = treeIn->GetEntries();
	cout<<"\n input file  entries: "<<nEntries;

	// Activate branches  	
	TClonesArray	*aTofTDCdigit = NULL; treeIn->SetBranchAddress("TDC", &aTofTDCdigit); //"bmn_tof400"
assert(nullptr != aTofTDCdigit);
	
	BmnTDCDigit 		*pTDCdigit;
	const BmnTof1Digit	*pTofDigit;
	BmnTof1DigitConvertor 	convertor;
	
	size_t tdcSerial, NtofDigits = 0;
	int detID, stripID, stripSideID;
	int crateSerial, crateSlote, tdcChannel, tdcType, tdcLeading, tdcValue;
	UChar_t tChannel, tHpId;
	
//	nEntries = 100;
	for(Int_t event = 0; event < nEntries; event++) 		
  	{		
  		treeIn->GetEntry(event);
  		
  		convertor.Reset();
  		
  		int Ndigits = aTofTDCdigit->GetEntries();
  		for(int digit =0; digit < Ndigits; digit++) // cycle by TDCdigits
   		{
   			pTDCdigit = (BmnTDCDigit*) aTofTDCdigit->UncheckedAt(digit);
   			
   			crateSerial = pTDCdigit->GetSerial();
   			crateSlote = pTDCdigit->GetSlot();
   			tChannel = pTDCdigit->GetChannel();
   			tHpId = pTDCdigit->GetHptdcId();
			tdcType = pTDCdigit->GetType();
			tdcLeading = pTDCdigit->GetLeading();
			tdcValue = pTDCdigit->GetValue();
   			
   			//cout<<"\nTDCdigit  serial:"<<hex<<showbase<<crateSerial<<dec<<", slote:"<<crateSlote<<"  "<<tdcType;
   			  			
   			if(config.GetTDCserial(crateSerial, crateSlote, tdcSerial)) // TDCserial found == TOF1 tdc
   			{  			
      				tdcChannel = BmnTof1TDC::GetChannel(tChannel, tHpId);
   			
   				//cout<<"\nTDCdigit  serial:"<<hex<<showbase<<crateSerial<<dec<<", slote:"<<crateSlote<<", channel:"<<tdcChannel
   				//	<<", type:"<<tdcType<<", leading:"<<tdcLeading<<", value:"<<tdcValue;			
   			
   				convertor.AddSample(tdcSerial, tdcChannel);		
   				
   				if(BmnTof1TDC::CheckTdcType(tdcSerial, tdcType)) // FIXME: maybe naxren nenado
   				if(tofMapping.Convert(tdcSerial, tdcChannel, &pTofDigit)) // Tof strip found 
   				{
   					convertor.AddDigit(pTDCdigit,  BmnTof1TDC::GetTDCChannel(tdcSerial, tdcChannel), pTofDigit);
   				} 			
   			}
 
		} // cycle by TDCdigits
		
		convertor.CalcCorrelationMatrix();
				
		size_t entries = convertor.Convert(aTofDigits); NtofDigits += entries;
		cout<<"\n event# "<<event<<", TDC digits: "<<Ndigits<<" Tof1 digits = "<<entries;
		
		// save BmnTof1Digits to file
		treeOut->Fill();
	}

	file.cd();
	treeOut->Write();
	file.Close();
		
	cout<<"\n$TOF1inFlnm = "<<inFile.Data()<<", $TOF1outFlnm = "<<outFile.Data()<<", NtofDigits = "<<NtofDigits<<", nEntries="<<nEntries<<endl;	
	cout<<"\nTask finished OK."<<endl;
}
//------------------------------------------------------------------------------------- <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
void	LoadTofDBParameters()
{
	TObjArray arrayConditions;
        UniSearchCondition* searchCondition = new UniSearchCondition(columnDetectorName, conditionEqual, TString("TOF1"));
        arrayConditions.Add((TObject*)searchCondition);

    	TObjArray* pParameterValueArray = UniDbDetectorParameter::Search(arrayConditions);

    	// clean memory for conditions after search
        for (int i = 0; i < arrayConditions.GetEntriesFast(); i++) delete (UniSearchCondition*) arrayConditions[i];
	arrayConditions.Clear();

	size_t Nloaded = 0;
	TString parName, inlParamName("inl");
	int* DcSerial;
	int* DcChannel;
		
    	for (int i = 0; i < pParameterValueArray->GetEntriesFast(); i++)
    	{
        	UniDbDetectorParameter* pParameterValue = (UniDbDetectorParameter*) pParameterValueArray->At(i);
        	
        	parName = pParameterValue->GetParameterName();
        	DcSerial = pParameterValue->GetDcSerial();
        	DcChannel = pParameterValue->GetChannel();

		// inl TDC loading             
                if(parName == inlParamName)                 
                {
	               	double* doublePtr;
 			int 	counter;
                   	pParameterValue->GetDoubleArray(doublePtr, counter);
                
                  	Nloaded += BmnTof1TDC::LoadInl(DcSerial, DcChannel, doublePtr, counter);
		}                    
    	}
    	
    	cout<<" Parameters loaded = "<<Nloaded<<endl;   	

    	// clean memory after work
    	for (int i = 0; i < pParameterValueArray->GetEntriesFast(); i++)	delete (UniDbRun*)pParameterValueArray->At(i);   	
    	delete pParameterValueArray;
}
//-------------------------------------------------------------------------------------
bool LoadGeoFile(int runId, const char* geoFlnm)
{
    int res_code = UniDbRun::ReadGeometryFile(runId, const_cast<char*>(geoFlnm)); 
    if (res_code != 0)
    {
        cerr<<"\n ---> [LoadGeoFile] Error: Can't load geo file from DB."<<endl;
        return false;
    }
    
return true;    
}
//-------------------------------------------------------------------------------------
bool 	ParseGeoFile(BmnTof1GeoUtils *parser, const char* geoFlnm)
{
	TFile file(geoFlnm, "READ");
	TGeoManager *geoMan = (TGeoManager*) file.Get("FAIRGeom");
		
	if(geoMan == nullptr)
	{
		cerr<<"\n ---> [ParseGeoFile] Error: Can't load TGeoManager from file."<<endl;
		return false;
	}
	
	parser->ParseTGeoManager(false, nullptr, true);

return true;
}
//-------------------------------------------------------------------------------------





