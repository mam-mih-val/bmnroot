/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   add_Tof1_INL_new.C
 * Author: mikhailr
 *
 * Created on August 31, 2021, 1:46 PM
 */

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define CHANNEL_NUMBER 72
#define BIN_NUMBER 1024

using namespace std;
//add_Tof1_INL(the .ini filename, start period, start run, end period, end run)

void add_Tof1_INL_new(TString ListOfINLFiles = "ListINLFiles_Run7.txt", int sP = 7, int sR = 2041, int eP = 7, int eR = 5184) {
    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    fstream fList(ListOfINLFiles, std::fstream::in);
    if (fList.fail()) {
        cout << "Failed to open " << ListOfINLFiles << endl;
        return;
    }

    TString FileINL[100];
    UInt_t TDCSerial;
    Int_t counter = 0;
    Double_t Inltemp, InlSum;
    TString dir = Form("%s%s", getenv("VMCWORKDIR"), "/input/");
    cout << "Location for INL file is " << dir.Data() << endl << endl;

    vector<UniValue*> InlData;

    while (!fList.eof()) {

        TdcInlValue* tempInl = new TdcInlValue;

        fList >> FileINL[counter];
        cout << "INL file " << FileINL[counter].Data() << endl;
        fstream fINL(dir + FileINL[counter], std::fstream::in);
        if (fINL.fail()) {
            cout << "Failed to open " << FileINL[counter].Data() << endl;
            return;
        } else cout << "Open File " << FileINL[counter].Data() << endl;
        //The format of the header seems to be [TDC-THESERIAL-inl_corr]
        fINL.ignore(10, '-');
        fINL >> std::hex >> TDCSerial >> std::dec;
        tempInl->serial = TDCSerial;
        cout << std::hex << TDCSerial << std::dec << endl;
        fINL.ignore(100, '\n');


        //Fill the INL with 0s
        for (int i = 0; i < CHANNEL_NUMBER; i++)
            for (int j = 0; j < BIN_NUMBER; j++) {
                tempInl->inl[i][j] = 0;
            }
        int lines_num = 0;
        while (!fINL.eof()) {
            int channel_id;
            string line, number;

            //Read the line
            std::getline(fINL, line, '\n');
            if (fINL.eof()) {
                break;
            }
            istringstream ss(line);
            istringstream ns;

            //Read the channel id
            char dummy;
            ss >> channel_id >> dummy; //"dummy" is needed to read the "=" character
            //            cout << "Loading channel " << channel_id << "..." << std::flush;

            //Read until the end of sstream (line)
            int i_bin = 0;
            InlSum = 0;
            while (ss.tellg() != -1) {
                if (i_bin >= BIN_NUMBER) {
                    cerr << "Too many bins in the INL file for channel " << channel_id << endl;
                    break;
                }

                //Read until the ',' character or the end of line
                std::getline(ss, number, ',');
                ns.str(number);
                ns.clear();
                Inltemp = 0;
                ns >> Inltemp;
                tempInl->inl[channel_id][i_bin] = Inltemp;
                InlSum += Inltemp;
                i_bin++;
            }
            //            cout << " loaded " << i_bin << " bins" << ". InlSum = " << InlSum << endl;
            lines_num++;
        }
        fINL.close();
        InlData.push_back(tempInl);
        if (lines_num != CHANNEL_NUMBER) cerr << "Wrong number of lines in the file (" << lines_num << ")" << endl;
        cout << endl;
        counter++;
    }
    fList.close();


    //read INL data from vector
    //
    //    for (Int_t i = 0; i < InlData.size(); i++) {
    //
    //        TdcInlValue* tempInlread = (TdcInlValue*) InlData.at(i);
    //        cout << "Inl for TDC " << std::hex << tempInlread->serial << std::dec << endl;
    //        for (Int_t ch = 0; ch < 72; ch++) {
    //            InlSum = 0;
    //            for (Int_t j = 0; j < 1024; j++) {
    //                InlSum+= tempInlread->inl[ch][j];
    //            }
    //            cout << " read " << ch << " channel" << ". InlSum = " << InlSum << endl;
    //        }
    //        cout << endl;
    //
    //    }

    //create parameter in DB
    //        UniDbParameter* pParameter2 = UniDbParameter::CreateParameter("TOF1_inl", TdcInlType, 0); //(TString parameter_name, int parameter_type)
    //        delete pParameter2;

    char anwser;
    cout << "Will be uploaded " << counter << " INL's file" << endl;
    printf("\tStart: period%d - run%d\n\tStop: period%d - run%d\n", sP, sR, eP, eR);
    cout << "Okay to upload it? (y/N) ";
    cin >> anwser;
    if (anwser == 'Y' || anwser == 'y') {
        //Upload it
        UniDbDetectorParameter* t = UniDbDetectorParameter::CreateDetectorParameter("TOF1", "TOF1_inl", sP, sR, eP, eR, InlData);
        if (t == NULL) {
            cerr << "Failed to upload the INL" << endl;
        } else {
            delete t;
            cout << "Uupload finished successfully " << endl;
        }
        cout << endl;

    }
}

