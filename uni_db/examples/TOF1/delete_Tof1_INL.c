/* 
 * File:   delete_Tof1_INL.c
 * Author: mikhail
 *
 * Created on January 16, 2017, 2:48 PM
 */
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#define CHANNEL_NUMBER 72
#define BIN_NUMBER 1024

using namespace std;

void delete_Tof1_INL(TString ListOfINLFiles, int sP = 5, int sR = 419, int eP = 5, int eR = 1014) {
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    fstream fList(ListOfINLFiles, std::fstream::in);
    if (fList.fail()) {
        cout << "Failed to open " << filename << endl;
        return;
    }

    TString FileINL[100];
    unsigned int TDCSerial[100];
    Int_t counter = 0;
    TString dir = Form("%s%s", getenv("VMCWORKDIR"), "/input/");

    while (!fList.eof()) {
        fList >> FileINL[counter];
        cout << "INL file " << FileINL[counter].Data() << endl;

        fstream fINL(dir + FileINL[counter], std::fstream::in);
        if (fINL.fail()) {
            cout << "Failed to open " << FileINL[counter].Data() << endl;
            return;
        }
        //The format of the header seems to be [TDC-THESERIAL-inl_corr]
        fINL.ignore(10, '-');
        fINL >> std::hex >> TDCSerial[counter] >> std::dec;
        //cout << std::hex << TDCSerial << std::dec << endl;
        fINL.ignore(100, '\n');
        fINL.close();
        counter++;
    }
    fList.close();
    cout << "Will be delete " << counter + 1 << " Inl's " << endl << endl;
    char anwser;
    cout << "Okay to delete it? (y/N) ";
    cin >> anwser;
    Int_t t = 1;
    if (anwser == 'Y' || anwser == 'y') {
        for (Int_t i = 0; i < counter; i++) {
            cout << "deleting the INL " << std::hex << TDCSerial[i] << std::dec << endl;
            for (int chan = 0; chan < CHANNEL_NUMBER; chan++) {
                cout << '\r' << std::setw(40) << ' ' << "\rdeleting channel " << chan << std::flush;
                t = UniDbDetectorParameter::DeleteDetectorParameter("TOF1", "inl", sP, sR, eP, eR, TDCSerial[i], chan);
                if (t == 0) cout << "INL TDC " << std::hex << TDCSerial[i] << std::dec << "  ch " << chan << "are deleted " << endl;
                else cout << "INL TDC " << std::hex << TDCSerial[i] << std::dec << "  ch " << chan << " are NOT deleted " << endl;
            }
        }
    }
    cout << endl;
}