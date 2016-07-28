//A simple macros to upload INL from an .ini file

#include "../db_structures.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#define CHANNEL_NUMBER 72
#define BIN_NUMBER 1024

using namespace std;
//add_Tof1_INL(the .ini filename, start period, start run, end period, end run)
void add_Tof1_INL(string filename, int sP = 4, int sR = 61, int eP = 4, int eR = 84) {
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gSystem->Load("libUniDb");
	
	unsigned int loadedTDCSerial;
	
	fstream ff(filename.c_str(), std::fstream::in);
	if(ff.fail()) {cout << "Failed to open " << filename << endl; return;}
	
	//The format of the header seems to be [TDC-THESERIAL-inl_corr]
	ff.ignore(10, '-');
	ff >> std::hex >> loadedTDCSerial >> std::dec;
	cout << std::hex << loadedTDCSerial << std::dec << endl;
	ff.ignore(100, '\n');
	
	//Fill the INL with 0s
	double INL[CHANNEL_NUMBER][BIN_NUMBER];
	for(int i = 0; i<CHANNEL_NUMBER;i++) {
		for(int j = 0; j<BIN_NUMBER; j++) {
			INL[i][j] = 0;
		}
	}
	int lines_num = 0;
	while(!ff.eof()) {
		int channel_id;
		string line, number;
		
		//Read the line
		std::getline(ff, line, '\n');
		if(ff.eof()) {break;}
		istringstream ss(line);
		istringstream ns;
		
		//Read the channel id
		ss >> channel_id;
		cout << '\r' << std::setw(30) << ' ' << "\rLoading channel " << channel_id << std::flush;
		
		//Read until the end of sstream (line)
		int i_bin = 0;
		while(ss.tellg() != -1) {
			if(i_bin >= BIN_NUMBER) {
				cerr << "Too many bins in the INL file for channel " << channel_id << endl;
				break;
			}
			
			//Read until the ',' character or the end of line
			std::getline(ss, number, ',');
			
			ns.str(number);
			ns >> INL[channel_id][i_bin];
			i_bin++;
		}
		lines_num++;
	}
	
	cout << endl;
	if(lines_num != CHANNEL_NUMBER) {cerr << "Wrong number of lines in the file (" << lines_num << ")" << endl;}
	
	cout << "Loaded the INL for " << std::hex << std::setfill('0') << std::setw(8) << loadedTDCSerial << std::dec << std::setfill(' ') << endl;
	char anwser;
	
	cout << "Okay to upload it? (y/N) ";
	cin >> anwser;
	
	if(anwser == 'Y' || anwser == 'y') {
		cout << "Uploading the INL..." << endl;
		for(int chan = 0; chan < CHANNEL_NUMBER; chan++) {
			cout << '\r' << std::setw(40) << ' ' << "\rUploading channel " << chan << std::flush;
			//Upload it
			UniDbDetectorParameter* t = UniDbDetectorParameter::CreateDetectorParameter("TOF1", "inl", sP, sR, eP, eR, loadedTDCSerial, chan, INL[chan], BIN_NUMBER);
			if(t == NULL) {cerr << "Failed to upload the INL" << endl; break;}
			else {delete t;}
		}
	}
	cout << endl;
	
	ff.close();
}
