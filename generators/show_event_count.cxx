//============================================================================
// Name        : show_event_count.cxx
// Author      : Konstantin Gertsenberger
// Copyright   : JINR
// Description : Get event count for the generator file
//============================================================================

// own headers
#include "MpdGetNumEvents.h"

// ROOT includes
#include "TString.h"
#include "TSystem.h"

// C++ includes
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

bool ends_with(string const &fullString, string const &ending)
{
    if (fullString.length() >= ending.length())
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    else
        return false;
}

int main(int argc, char** argv)
{
    // help information
    if (argc > 1)
    {
        string first_par = argv[1];
        if ((first_par == "/?") || (first_par == "--help") || (first_par == "-h"))
        {
            cout<<"The utility shows event count for a given generator file or just ROOT file"<<endl
               <<"The first parameter: case-insensitive generator name (urqmd, hsd (phsd), qgsm (laqgsm,dcm-gsqm) or 'root' value (w/o quotes)"<<endl
               <<"The second parameter: generator file path"<<endl<<"Example: show_event_count dcmqgsm /eos/nica/bmn/sim/DQGSM/"<<endl
               <<"Example: show_event_count root example.root"<<endl;

            return 0;
        }
    }

    if (argc != 3)
    {
        cout<<"The ShowEventCount utility takes 2 parameters: "<<endl<<"1st - generator name or 'root' value (w/o quotes); 2nd - generator or ROOT file path"<<endl;
        return -1;
    }

    string file_path = argv[2];
    TString strFilePath(file_path);
    gSystem->ExpandPathName(strFilePath);
    if (gSystem->AccessPathName(strFilePath.Data()) == true)
    {
        cout<<endl<<"No file was found: "<<strFilePath<<endl;
        return -2;
    }

    string generator_name = argv[1];
    transform(generator_name.begin(), generator_name.end(), generator_name.begin(),
        [](unsigned char c){ return tolower(c); });

    if (generator_name == "urqmd")
    {
        cout<<MpdGetNumEvents::GetNumURQMDEvents(strFilePath.Data(), 1);
        return 0;
    }
    else if (ends_with(generator_name, "hsd"))
    {
        cout<<MpdGetNumEvents::GetNumPHSDEvents(strFilePath.Data(), 1);
        return 0;
    }
    else if (ends_with(generator_name, "qgsm")) {
        cout<<MpdGetNumEvents::GetNumQGSMEvents(strFilePath.Data(), 1);
        return 0;
    }
    else if (ends_with(generator_name, "smm"))
    {
        cout<<MpdGetNumEvents::GetNumDCMSMMEvents(strFilePath.Data(), 1);
        return 0;
    }
    else if (ends_with(generator_name, "root"))
    {
        cout<<MpdGetNumEvents::GetNumROOTEvents(strFilePath.Data(), "", 1);
        return 0;
    }
    else
    {
        cout<<"The generator name (or 'root' value) was not defined. See possible values: show_event_count /?";
        return -3;
    }

    return 0;
}
