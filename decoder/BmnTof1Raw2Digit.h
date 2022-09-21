#ifndef BMNTOF1RAW2DIGIT_H
#define	BMNTOF1RAW2DIGIT_H 

#include "TString.h"
#include "TClonesArray.h"
#include "BmnTDCDigit.h"
#include "BmnTof1Digit.h"
#include "BmnMath.h"
#include <iostream>
#include "Riostream.h"
#include <cstdlib>
#include <UniDetectorParameter.h>
#include <string>
#include <map>
#include <set>
#include <utility>
#include <fstream>
#include <sstream>

//Side of the strip is stored as a bool variable
#define TOF1_LEFT true
#define TOF1_RIGHT false

#define TOF1_CHANNEL_NUMBER 72
#define TOF1_BIN_NUMBER 1024
#define TOF1_MAX_TIME (24.) //In ns
#define TOF1_TDC_TYPE (0x12) //TDC72VHL

//A simple class to compare the TDCDigits. See .cxx code
struct _Tof1TDCCompare {
    bool operator()(const BmnTDCDigit& a, const BmnTDCDigit& b) const;
};

//Map element
struct BmnTof1Map2 {
	Short_t plane;
	Short_t strip;
	Bool_t side;
	BmnTof1Map2(Short_t, Short_t, Bool_t);
	BmnTof1Map2();
};

//TDC parameters
struct BmnTof1TDCParameters {
	double INL[TOF1_CHANNEL_NUMBER][TOF1_BIN_NUMBER]; //INL
	BmnTof1Map2 ChannelMap[TOF1_CHANNEL_NUMBER]; //A BmnTof1Map2 for every channel
	double t[TOF1_CHANNEL_NUMBER]; //To store the value temporarily. See .cxx code
	BmnTof1TDCParameters(); //Simple constructor
};

class BmnTof1Raw2Digit {
	public:		
		BmnTof1Raw2Digit(); //BmnTof1Raw2Digit main constructor
		BmnTof1Raw2Digit(Int_t nPeriod, Int_t nRun, Int_t verbose); //Calls setRun(...)
		~BmnTof1Raw2Digit(); //Destructor
		
		Bool_t setRun(Int_t nPerion, Int_t nRun); //Load mapping and INL from the DB for run #nRun in period #nPeriod
		void setMapFromFile(TString placementMapFile, TString mapFile); //Load mapping from two files
		void saveMapToFile(std::string placementMapFile, std::string mapFile); //Save the mapping to two files
		
		void setINLFromFile(std::string INLFile);	//Load INL from an INI file
		void saveINLToFile(std::string INLFile, unsigned int TDCSerial); //Save INL for TDCSerial to an INI file
		
		void print(){} //Prints some info
                void setVerbose(Int_t verbose) {fVerbose = verbose;}
		
		void FillEvent(TClonesArray *data, map<UInt_t, Long64_t> *mapTS,TClonesArray *tof1digit); //
		
		static UShort_t ToGlobalChannel(UChar_t HptdcId, UChar_t channel);
                
	private:
		ClassDef(BmnTof1Raw2Digit, 1);
		void init(); //BmnTof1Raw2Digit init function (called in BmnTof1Raw2Digit constructors)
		Int_t RunIndex, PeriodIndex, fVerbose; //To store the RunIndex and PeriodIndex and print level
		std::map<std::pair<UInt_t, UChar_t>, UInt_t> PlacementMap;	//Stores the placement map
		std::map<UInt_t, BmnTof1TDCParameters> TDCMap;			//Stores the TDC mapping (TDC's channel -> plane/strip/side) and INL
		void plmap_insert(UInt_t Serial, UChar_t Slot, UInt_t TDCSerial); //See .cxx code
		
		//std::map provides a way to find TDC by Serial and Slot really fast (O(logN))
		//BmnTof1Parameters could also be found so fast (O(logN))
		//All other operations (extracting the INL, for example) are already fast because of the structure - O(1)
};
#endif	/* BMNTOF1RAW2DIGIT_H */


