// macro to collapse all TOF400 histograms to one so that
// we can then analyze them in my TOF400 scrip
#include <iostream>
#include <cstdlib>
#include <cmath>

#include "TFile.h"

using namespace std;

int main(int argc, char** argv){
	
	if (argc < 3){
		cerr << "\nWrong number of arguments! Instead use:\n"
			<< "\tcombine_hists   /path/to/output    /path/to/files\n";
		exit(-1);
	}

	TFile * outFile = new TFile(argv[1],"RECREATE");
	
	int numFiles = argc - 2;
	cout << "Number of files to combine: " << numFiles << "\n";

	for( int i = 0; i < numFiles ; i++){
		cout << "\tWorking on file: " << argv[i+2] << "\n";
		//TFile * oneRun = new TFile(argv[i+2]);
		
	}

	outFile->Close();

}
