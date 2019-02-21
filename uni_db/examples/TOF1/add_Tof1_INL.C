//A simple macros to upload INL from an .ini file

//#include "../db_structures.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#define CHANNEL_NUMBER 72
#define BIN_NUMBER 1024

using namespace std;
//add_Tof1_INL(the .ini filename, start period, start run, end period, end run)

void add_Tof1_INL(TString ListOfINLFiles = "ListINLFiles.txt", int sP = 7, int sR = 1, int eP = 7, int eR = 10000)
{
    fstream fList(ListOfINLFiles, std::fstream::in);
    if (fList.fail())
    {
        cout << "Failed to open " << ListOfINLFiles << endl;
        return;
    }

    TString FileINL[100];
    unsigned int TDCSerial[100];
    Int_t counter = 0;
    TString dir = Form("%s%s", getenv("VMCWORKDIR"), "/input/");
    Double_t INL[100][CHANNEL_NUMBER][BIN_NUMBER];
    cout << "Location for INL file is " << dir.Data() << endl << endl;

    while (!fList.eof())
    {
        fList >> FileINL[counter];
        cout << "INL file " << FileINL[counter].Data() << endl;
        fstream fINL(dir + FileINL[counter], std::fstream::in);
        if (fINL.fail())
        {
            cout << "Failed to open " << FileINL[counter].Data() << endl;
            return;
        } else cout << "Open File " << FileINL[counter].Data() << endl;
        //The format of the header seems to be [TDC-THESERIAL-inl_corr]
        fINL.ignore(10, '-');
        fINL >> std::hex >> TDCSerial[counter] >> std::dec;
        //cout << std::hex << TDCSerial[counter] << std::dec << endl;
        fINL.ignore(100, '\n');

        //Fill the INL with 0s
        for (int i = 0; i < CHANNEL_NUMBER; i++)
            for (int j = 0; j < BIN_NUMBER; j++)
            {
                INL[counter][i][j] = 0;
            }
        int lines_num = 0;
        while (!fINL.eof())
        {
            int channel_id;
            string line, number;

            //Read the line
            std::getline(fINL, line, '\n');
            if (fINL.eof())
            {
                break;
            }
            istringstream ss(line);
            istringstream ns;

            //Read the channel id
            char dummy;
            ss >> channel_id >> dummy; //"dummy" is needed to read the "=" character
            //cout << "Loading channel " << channel_id << "..." << std::flush;

            //Read until the end of sstream (line)
            int i_bin = 0;
            while (ss.tellg() != -1)
            {
                if (i_bin >= BIN_NUMBER)
                {
                    cerr << "Too many bins in the INL file for channel " << channel_id << endl;
                    break;
                }

                //Read until the ',' character or the end of line
                std::getline(ss, number, ',');
                ns.str(number);
                ns.clear();
                ns >> INL[counter][channel_id][i_bin];
                i_bin++;
            }
            //cout << " loaded " << i_bin << " bins" << endl;
            lines_num++;
        }
        fINL.close();
        if (lines_num != CHANNEL_NUMBER) cerr << "Wrong number of lines in the file (" << lines_num << ")" << endl;
        cout << endl;
        counter++;
    }
    fList.close();
    // For convert file
    {
        fstream fINL_out("TRIG_INL_076D-16A8.txt", std::fstream::out);
        for (Int_t ii = 0; ii < 72; ii++)
        {
            for (Int_t jj = 0; jj < 72; jj++)
                fINL_out << INL[0][ii][jj] << ' ';
            fINL_out << endl;
        }
        fINL_out.close();
    }


    char anwser;
    cout << "Will be uploaded " << counter << " INL's file" << endl;
    cout << "Okay to upload it? (y/N) ";
    cin >> anwser;
    if (anwser == 'Y' || anwser == 'y')
    {
        for (Int_t i = 0; i < counter; i++)
        {
            cout << endl << "Uploading the INL..." << std::hex << TDCSerial[i] << std::dec << endl;
            for (int chan = 0; chan < CHANNEL_NUMBER; chan++)
            {
                cout << '\r' << std::setw(40) << ' ' << "\rUploading channel " << chan << std::flush;
                //Upload it
                UniDbDetectorParameter* t = UniDbDetectorParameter::CreateDetectorParameter("TOF1", "inl", sP, sR, eP, eR, TDCSerial[i], chan, INL[i][chan], BIN_NUMBER);
                if (t == NULL)
                {
                    cerr << "Failed to upload the INL" << endl;
                    break;
                } else delete t;

            }
        }
    }
    cout << endl;

}
