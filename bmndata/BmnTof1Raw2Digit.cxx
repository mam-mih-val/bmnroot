#include "BmnTof1Raw2Digit.h"

//	Written by JINR summer programme 2016 student Kurganov Alexander

//	Supervisors:
//	Vadim Babkin
//	Mikhail Rumyantsev
//	Vyatcheslav Golovatjuk

typedef std::map<std::pair<UInt_t, UChar_t>, UInt_t>::iterator Tof1PlMapIter;
typedef std::map<UInt_t, BmnTof1TDCParameters>::iterator Tof1TDCMapIter;
typedef std::pair<UInt_t, UChar_t> Tof1PlMapKey;
typedef std::pair<UInt_t, BmnTof1TDCParameters> Tof1TDCMapElem;

using namespace std;

int Tof1GlobalNameCounter = 0;

//Simple constructors for the BmnTof1Map2 element class
BmnTof1Map2::BmnTof1Map2(Short_t p, Short_t s, Bool_t sd) {
	plane = p;
	strip = s;
	side = sd;
}
BmnTof1Map2::BmnTof1Map2() {
	plane = 0;
	strip = 0;
	side = false;
}

//Main constructor for the BmnTof1TDCParameters()
BmnTof1TDCParameters::BmnTof1TDCParameters() {
	for(int i = 0; i<TOF1_CHANNEL_NUMBER; i++) {
		for(int j = 0; j<TOF1_BIN_NUMBER; j++) {
			INL[i][j] = 0;
		}
		ChannelMap[i] = BmnTof1Map2(0, 0, false);
		t[i] = -1;
	}
}

//A small functional class, which is used to compare two BmnTDCDigits
//It is used to sort by time the TDCDigits in the BmnTof1Raw2Digit::FillEvent()
bool _Tof1TDCCompare::operator()(const BmnTDCDigit& a, const BmnTDCDigit& b) {
	return a.GetValue()<b.GetValue();
}

//A small function, which inserts in the placement map a key-value pair
//Just to make everything more clear
void BmnTof1Raw2Digit::plmap_insert(UInt_t Serial, UChar_t Slot, UInt_t TDCSerial) {
	PlacementMap.insert(std::pair<Tof1PlMapKey, UInt_t>(Tof1PlMapKey(Serial, Slot), TDCSerial));
}



//BmnTof1Raw2Digit main constructor
BmnTof1Raw2Digit::BmnTof1Raw2Digit() {
	init();
}

//BmnTof1Raw2Digit constructor, which also loads mapping from the DB
BmnTof1Raw2Digit::BmnTof1Raw2Digit(int nPeriod, int nRun) {
	init();
	setRun(nPeriod, nRun);
}

//BmnTof1Raw2Digit destructor
BmnTof1Raw2Digit::~BmnTof1Raw2Digit() {
}

//BmnTof1Raw2Digit init function (called in BmnTof1Raw2Digit constructors)
void BmnTof1Raw2Digit::init() {
	//Nothing here
}

//A simple function to convert Hptdcid (0 -- 9) and Hptdc's channel (0 -- 8) to "global" tdc channel (0 -- 72)
UShort_t BmnTof1Raw2Digit::ToGlobalChannel(UChar_t HptdcId, UChar_t channel) {
	return HptdcId*8+channel;
}

//Loads mapping from the DB
void BmnTof1Raw2Digit::setRun(int nPeriod, int nRun) {
	cout << "Loading the TOF1 Map from DB: Period " << nPeriod << ", Run " << nRun << "..." << endl;
	PeriodIndex = nPeriod;
	RunIndex = nRun;
	
	
	//Load the placement map from the DB
	cout << "Loading the placement map..." << std::flush;
	UniDbDetectorParameter* pLoadedPlacement = UniDbDetectorParameter::GetDetectorParameter("TOF1", "placementmap", nPeriod, nRun);
	if(pLoadedPlacement == 0) {
		cout << "TOF1 Crucial error: failed to load the placement map" << endl;
	} else {
		//Get the placement map
		int elem_count = 0;
		unsigned int* elem_array;
		pLoadedPlacement->GetUIntArray(elem_array, elem_count);
		
		for(int i = 0; i < elem_count; i+=3) {
			plmap_insert(elem_array[i], elem_array[i+1], elem_array[i+2]);
			TDCMap.insert(Tof1TDCMapElem(elem_array[i+2], BmnTof1TDCParameters()));
		}
		
		//Print the result
		cout << '\r' << std::setw(40) << ' ' << '\r' << "Loaded " << (elem_count/3) << " TDCs" << endl;
		//Free the memory
		delete[] elem_array; delete pLoadedPlacement;
	}
	
	//Load the mapping from the DB for all TDCs. Loop over all the TDCs in the placement map
	cout << "Loading main map..." << endl;
	
	Tof1PlMapIter it;
	it = PlacementMap.begin();
	while(it!=PlacementMap.end()) {
		//Firstly, find the TDC parameters
		Tof1TDCMapIter TDCPair = TDCMap.find(it->second);
		
		//Conditions
		//StartPeriod <= nPeriod <= EndPeriod, StartRun <= nRun <= EndRun, Detector name == "TOF1" and TDC Serial == it->second

		TObjArray conditions;
		conditions.Add((TObject* ) new UniDbSearchCondition(columnStartPeriod, conditionLessOrEqual, PeriodIndex));
		conditions.Add((TObject* ) new UniDbSearchCondition(columnStartRun, conditionLessOrEqual, RunIndex));
		conditions.Add((TObject* ) new UniDbSearchCondition(columnEndPeriod, conditionGreaterOrEqual, PeriodIndex));
		conditions.Add((TObject* ) new UniDbSearchCondition(columnEndRun, conditionGreaterOrEqual, RunIndex));
		conditions.Add((TObject* ) new UniDbSearchCondition(columnDetectorName, conditionEqual, TString("TOF1")));
		conditions.Add((TObject* ) new UniDbSearchCondition(columnDCSerial, conditionEqual, it->second));
	
		//The search itself
		cout << '\r' << std::setw(40) << ' ' << "\rSearching ";
		cout << std::hex << std::setfill('0') << std::setw(8) << (it->second) << std::dec << std::setfill(' ') << "..." << std::flush;
		TObjArray* elemArray = UniDbDetectorParameter::Search(conditions);
		cout << '\r' << std::setw(40) << ' ' << "\rEntries: " << elemArray->GetEntries() << ", parsing..." << endl;
		
		//Loop over all of the found rows (TOF1_CHANNEL_NUMBER * 4)
		//This loop is quite slow, because, actually
		//UniDbDetectorParameter::GetInt() calls UniDbDetectorParameter::GetUNC()
		//Which every time makes a request to the DB to check the parameter type
		
		UniDbDetectorParameter* curRow;
		
		TIter res_it = elemArray->begin(); int nRow = 0;
		
		while(res_it!=elemArray->end()) {
			cout << '\r' << std::setw(40) << ' ' << "\rParsing row " << nRow << "..." << std::flush;
			
			curRow = (UniDbDetectorParameter*)(*res_it);
			
			//Channel could be null. Why does UniDbDetectorParameter::GetChannel() return a pointer?
			//Shouldn't it return a "-1", for example, in case of error?
			int* chan; chan = curRow->GetChannel();
			if(chan==NULL) {cout << "\rError: channel is null." << endl; break;}
			if(*chan < 0 || *chan >= TOF1_CHANNEL_NUMBER) {cout << "Wrong channel: " << *chan << endl; break;}
			
			BmnTof1Map2* elem = &((TDCPair->second).ChannelMap[*chan]);
			double* INLelem = (TDCPair->second).INL[*chan];
			
			switch(curRow->GetParameterId()) {
				case 6: 					//INL parameter's index is 6
					//Load the INL double array from the row
					double * inl; int inl_elem_count;
					curRow->GetDoubleArray(inl, inl_elem_count);
				
					//If the array is too big...
					if(inl_elem_count > TOF1_BIN_NUMBER) {
						cerr << "Number of elements in the INL array is larger than the TDC72VHL bin number." << endl;
						break;
					}
				
					//If the array is too small, we'll just assume there is no INL (== 0) on the "unfilled" bins
					//Memcpy is much faster than a loop over all TDC bins (1024)
					memcpy(INLelem, inl, inl_elem_count * sizeof(double));
					
					delete[] inl;
				break;
				
				case 7: elem->plane = curRow->GetInt(); break; //Plane parameter's index is 7
				case 8: elem->strip = curRow->GetInt(); break; //Strip parameter's index is 8
				case 9: elem->side = curRow->GetBool(); break; //Side  parameter's index is 9
				default: break;
			}
			
			//GetChannel() actually creates a new int! So this "delete" is needed to prevent memory leaks
			delete chan;
			
			++res_it; ++nRow;
		}
		cout << endl;
		
		//Again, UniDbDetectorParameter::Search actually creates a new TObjArray
		elemArray->Delete(); //Delete this row as soon as returned TObjArray will be set as owner in UniDbDetectorParameter::Search()
		delete elemArray;
		++it;
	}
	cout << "Loading Tof1 mapping from the DB complete." << endl;
}

//Load mapping from the file
void BmnTof1Raw2Digit::setMapFromFile(string placementMapFile, string mapFile) {
	UInt_t fcrate, fserial, fslot, fchan, fplane, fstrip;
	char side_c;
	
	//1. Placement map
	fstream ff;
	ff.open(placementMapFile.c_str(), std::fstream::in);
	if(ff.fail()) {cerr << "Cannot open the file " << placementMapFile << endl; return;}
        else cout << "placementMapFile " << placementMapFile << endl;
	while(!ff.eof()) {
		ff >> std::hex >> fcrate >> std::dec >> fslot >> std::hex >> fserial >> std::dec;
		if(ff.eof()) {break;}
		plmap_insert(fcrate, fslot, fserial);
		TDCMap.insert(Tof1TDCMapElem(fserial, BmnTof1TDCParameters()));
	}
	ff.close();	
	
	//2. Main map
	ff.open(mapFile.c_str(), std::fstream::in);
	if(ff.fail()) {cerr << "Cannot open the file " << mapFile << endl; return;}
        else cout << "StripmapFile " << mapFile << endl;
	while(!ff.eof()) {
		ff >> std::hex >> fserial >> std::dec >> fchan >> fplane >> fstrip >> side_c;
		if(ff.eof()) {break;}
		Tof1TDCMapIter TDCPair = TDCMap.find(fserial);
		if(TDCPair == TDCMap.end()) {cerr << "Error: TDC #" << std::hex << fserial << std::dec << " not found" << endl;}
		BmnTof1Map2* elem = &((TDCPair->second).ChannelMap[fchan]);
		elem->plane = fplane;
		elem->strip = fstrip;
		if(side_c == 'L') {elem -> side = TOF1_LEFT;}
		else if (side_c == 'R') {elem->side = TOF1_RIGHT;}
		else {cerr << "ToF1 Error: side is not L or R" << endl; continue;}
	}
	ff.close();
}

//Save mapping to file
void BmnTof1Raw2Digit::saveMapToFile(string placementMapFile, string mapFile) {
	fstream ff;
	
	//1.Placement map
	ff.open(placementMapFile.c_str(), std::fstream::out);
	if(ff.fail()) {cerr << "Cannot open the file " << placementMapFile << endl; return;}
	
	Tof1PlMapIter plit = PlacementMap.begin();
	while(plit != PlacementMap.end()) {
		ff << std::hex << std::setfill('0') << std::setw(8) << plit->first.first << '\t';
		ff << std::dec << int(plit->first.second) << '\t';
		ff << std::hex << std::setfill('0') << std::setw(8) << plit->second << endl;
		++plit;
	}
	ff.close();
	
	//2. Main map
	ff.open(mapFile.c_str(), std::fstream::out);
	if(ff.fail()) {cerr << "Cannot open the file " << mapFile << endl; return;}
	
	Tof1TDCMapIter mapit = TDCMap.begin();
	while(mapit!=TDCMap.end()) {
		for(int i = 0; i < TOF1_CHANNEL_NUMBER; i++) {
			ff << std::hex << std::setfill('0') << std::setw(8) << mapit->first << std::setfill(' ') << '\t';
			ff << std::dec << i << '\t';
			ff << mapit->second.ChannelMap[i].plane << '\t';
			ff << mapit->second.ChannelMap[i].strip << '\t';
			bool side = mapit->second.ChannelMap[i].side;
			if(side == TOF1_LEFT) {ff << 'L';}
			else if (side == TOF1_RIGHT) {ff << 'R';}
			else {cerr << "Side is not bool." << endl;} //I don't think this can even occur
			ff << endl;
		}
		++mapit;
	}
}

//Save INL to file
void BmnTof1Raw2Digit::setINLFromFile(string INLFile) {
	fstream ff(INLFile.c_str(), std::fstream::in);
	if(ff.fail()) {cerr << "Failed to open " << INLFile << endl; return;}
	unsigned int TDCSerial;
	
	//Read the header from the file
	//The format of the header seems to be [TDC-THESERIAL-inl_corr]
	ff.ignore(10, '-');
	ff >> std::hex >> TDCSerial >> std::dec;
	ff.ignore(1000, '\n');
	
	//Find the TDC
	Tof1TDCMapIter TDCPair = TDCMap.find(TDCSerial);
	if(TDCPair == TDCMap.end()) {
		cerr << "Tof400: TDC " << std::setfill('0') << std::setw(8) << std::hex << TDCSerial << std::setfill(' ') << std::dec;
		cerr << " isn't in the placement map." << endl;
		ff.close(); return;
	}
	
	unsigned int chan_id = 0;
	unsigned int lines_num = 0;
	
	while(!ff.eof()) {
		string line; char dummy;
		
		std::getline(ff, line, '\n');
		if(ff.eof()) {break;}
		if(line == "") {continue;}
		istringstream ss(line);
		
		ss >> chan_id >> dummy;
		if(dummy != '=') {cerr << "Tof400: Wrong INL file format." << endl; ff.close(); return;}
		if(chan_id > TOF1_CHANNEL_NUMBER) {cerr << "Tof400: Wrong channel in in the INL file" << endl; ff.close(); return;}
		
		double* INLelem = (TDCPair->second).INL[chan_id];
		
		unsigned int i_bin = 0;
		while(ss.tellg() != -1) {
			if(i_bin > TOF1_BIN_NUMBER) {
				cerr << "Tof400: INL File contains too many bins in channel." << endl;
				ff.close(); return;
			}
			if(ss.peek()==',') {ss.ignore();}
			ss >> INLelem[i_bin]; i_bin++;
		}
		if(i_bin != TOF1_BIN_NUMBER) {
			cout << "Warning: wrong number of bins in the INL file for channel " << chan_id << " (" << i_bin << ")" << endl;
		}
		lines_num++;
	}
	
	if(lines_num != TOF1_CHANNEL_NUMBER) {
		cout << "Warning: wrong number of lines in the INL file (" << lines_num << endl;
	}
	cout << "Tof400: INL for TDC " << std::setfill('0') << std::setw(8) << std::hex << TDCSerial << std::setfill(' ') << std::dec << " loaded succesfully from INL file." << endl;
}

//Load INL from file
void BmnTof1Raw2Digit::saveINLToFile(string INLFile, unsigned int TDCSerial) {
	//Find the TDC
	Tof1TDCMapIter TDCPair = TDCMap.find(TDCSerial);
	if(TDCPair == TDCMap.end()) {
		cerr << "Tof400: TDC " << std::setfill('0') << std::setw(8) << std::hex << TDCSerial << std::setfill(' ') << std::dec;
		cerr << " isn't in the placement map." << endl;
		return;
	}
	
	fstream ff(INLFile.c_str(), std::fstream::out);
	ff << "[TDC-" << std::setfill('0') << std::setw(8) << std::hex << TDCSerial << std::dec << std::setfill(' ') << "-inl_corr]" << endl;
	
	
	for(int chan = 0; chan<TOF1_CHANNEL_NUMBER; chan++) {
		ff << chan << "=";
		double* INLelem = (TDCPair->second).INL[chan];
		for(int bin = 0; bin<TOF1_BIN_NUMBER; bin++) {
			ff << INLelem[bin];
			if(bin!=TOF1_BIN_NUMBER-1) {
				ff << ", ";
			}
		}
		if(chan!=TOF1_CHANNEL_NUMBER - 1) {ff << endl;}
	}
	
	ff.close();
}

//Main function. "Converts" the TObjArray *data of BmnTDCDigit to the TObjArray *output of BmnTof1Digit
void BmnTof1Raw2Digit::FillEvent(TClonesArray *data, TClonesArray *output) {
	//0. Initialize: clear all the tempory times in the BmnTof1TDCParameters
	Tof1TDCMapIter tdci = TDCMap.begin();
	while(tdci!=TDCMap.end()) {
		for(int i = 0; i<TOF1_CHANNEL_NUMBER; i++) {
			(tdci->second).t[i] = -1;
		}
		tdci++;
	}


	//1. Filter and sort the TDC Data
	//TODO: Well.. maybe it's better to do it NOT here?
	//Maybe both of these should be done in the BmnRawDataDecoder?

	int nOut = output->GetEntries();
	//nOut variable is needed. GetEntries is quite a slow function,
	//so storing the number of entries before filling the *output TObjArray works much faster
	
	multiset<BmnTDCDigit, _Tof1TDCCompare> temp; //Create a multiset
	//Multiset automaticly sorts the elements in it.
	//_Tof1TDCCompare functional class is used to compare two BmnTDCDigit (see above)
	
	//Fill the multiset from the input array (it sorts the input array)
	TIter it = data->begin();
	while(it!=data->end()) {
		BmnTDCDigit* obj = static_cast<BmnTDCDigit*>(*it);
		if(obj->GetType()!=TOF1_TDC_TYPE) {++it; continue;} //Just skip it in case if TDC is not TDC72VHL (see TOF1_TDC_TYPE define in the .h file)
		temp.insert(*obj);
		//cout << std::hex << obj->GetSerial() << " "  << std::dec << int(obj->GetSlot()) << " " << int(obj->GetHptdcId()) << " " << int(obj->GetChannel()) << " " << (obj->GetLeading()?"L":"T") << obj->GetValue() << endl;
		++it;
	}
	
	//Now a loop over the temp multiset using iterator si
	multiset<BmnTDCDigit, _Tof1TDCCompare>::iterator si = temp.begin();
	
	while(si != temp.end()) {
		//Look for the crate serial and slot in the placement map
		Tof1PlMapIter parPair = PlacementMap.find(std::make_pair<UInt_t, UChar_t>(si->GetSerial(), si->GetSlot()));
		
		//In case if nothing was found
		if(parPair == PlacementMap.end()) {
			//Just skip it
			si++;
			continue;
		}
		
		//Get the TDC Serial for this crate serial and slot
		UInt_t TDC_Serial = parPair -> second;
		//And look for this TDC Serial in the TDCMap to get the BmnTof1TDCParameters for this TDC
		Tof1TDCMapIter TDCPair = TDCMap.find(TDC_Serial);
		//In case if TDC Serial isn't in the TDCMap
		if(TDCPair == TDCMap.end()) {
			//Display a warning and skip. This is quite a serious error, actually
			cerr << "Warning: TDC with ID " << std::hex << TDC_Serial << std::dec <<": no data" << endl;
			si++;
			continue;
		}
		
		//Finally, get the BmnTof1TDCParameters instance for this TDC Serial
		BmnTof1TDCParameters* par = &(TDCPair->second);
		//And get it's channel using BmnTof1Raw2Digit::ToGlobalChannel (see above)
		UShort_t rchan = ToGlobalChannel(si->GetHptdcId(), si->GetChannel());
		
		//Get the time from the TDC value
		//t = (val + INL[channel][val % 1024]) * (24ns / 1024)
		//See TOF1_BIN_NUMBER and TOF1_MAX_TIME defines in the .h file
		/*
		if(par->INL[rchan][(si->GetValue()) % TOF1_BIN_NUMBER] == 0) {
			cout << std::hex << TDC_Serial << std::dec << rchan << ":" << ((si->GetValue()) % TOF1_BIN_NUMBER) << " - " << par->INL[rchan][(si->GetValue()) % TOF1_BIN_NUMBER] << endl;
		}
		*/
		double t = (si->GetValue() + par->INL[rchan][(si->GetValue()) % TOF1_BIN_NUMBER])* TOF1_MAX_TIME / double(TOF1_BIN_NUMBER);
		
		if(si->GetLeading()) {
			//If this is a leading TDC digit, just fill the temporary time in the BmnTof1TDCParameters.
			par->t[rchan] = t;
		} else {
			//If this is NOT a leading TDC Digit...
			if(par->t[rchan] != -1) {
				//BmbTof1TDCParameters' temporary time (par->t[rchan]) has been already set
				//This temporary time should store the leading time
				if( t < (par->t[rchan]) ) {
					//This is impossible:
					//Leading value is larger than the trailing time
					//The BmnTDCDigit array has already been sorted, so this shouldn't happen
					//But let's just make sure, this could be a serious error
					cout << "Error: negative dt" << endl;
					
					//Print all the TDC Digits we have loaded in the sorted order
					multiset<BmnTDCDigit, _Tof1TDCCompare>::iterator si2 = temp.begin();
					while(si2 != temp.end()) {
						cout << std::hex << si2->GetSerial() << " " << int(si2->GetSlot()) << " " << std::dec << int(si2->GetHptdcId()) << " " << int(si2->GetChannel()) << " " << (si2->GetLeading()?"L":"T") << si2->GetValue() << endl;
						si2++;
					}
					//Print where exactly the error has occured
					cout << "Error at: " << endl;
					cout << std::hex << si->GetSerial() << " " << int(si->GetSlot()) << " " << std::dec << int(si->GetHptdcId()) << " " << int(si->GetChannel()) << " " << (si->GetLeading()?"L":"T") << si->GetValue() << endl;					
					cout << std::hex << TDC_Serial << std::dec << " " << rchan << " " << (par->t[rchan]) << "--" << t << endl;
				} else {
					//So we've got the Leading and Trail times and everything seems to be okay
					//Find the BmnTof1Map2 mapping element, which stores the plane, strip and side.
					BmnTof1Map2* elem = &(par->ChannelMap[rchan]);
					
					//Create a new BmnTof1Digit:
					//Plane, strip, and side are loaded from the found BmnTof1Map2
					//Leading time is loaded from the temporary storage in the BmnTof1TDCParameters class
					//Trailing time is just the time in current TDC Digit
					//(See above: this piece of code is executed only if the current BmnTDCDigit is NOT leading)
					
					new((*output)[nOut]) BmnTof1Digit(elem->plane,elem->strip,elem->side,t,t - (par->t[rchan]));
					nOut++;
				}
			}
			//We don't need to do anything in case if no leading time has occured yet (if par->t[rchan] == -1)
			
			//Finally, reset the temporary storage to prevent errors
			par->t[rchan] = -1;
		}
		si++;
	}	
}

ClassImp(BmnTof1Raw2Digit)
