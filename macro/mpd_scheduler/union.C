#if !defined(__CINT__) || defined(__MAKECINT__)

// ROOT includes
#include "TSystem.h"
#include "TFile.h"
#include "TObjArray.h"
#include "TIter.h"
#include "TChain.h"
#include "TChainElement.h"

#include <iostream>
using namespace std;
#endif

//mode: 0 - save TChain, 1 - merge and delete files
void union(char* pcFileList, int mode = 0)
{
    // Load libraries
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // all libs

    //addition result files for "cbmsim" Tree
    TChain chainUnion("cbmsim");
    TString sResultFile;
    int i=0, beg=0, countFiles=0;

    while (pcFileList[i] != '\0'){
        if ((pcFileList[i] == ' ') && (beg != i)){
            pcFileList[i] = '\0';

            if (countFiles == 0)
                sResultFile = &pcFileList[beg];
            else
                chainUnion.Add(&pcFileList[beg]);

            pcFileList[i] = ' ';
            beg = i+1;
            countFiles++;
        }

        i++;
    }
    if (beg != i){
        if (countFiles == 0)
            sResultFile = &pcFileList[beg];
        else
            chainUnion.Add(&pcFileList[beg]);

        countFiles++;
    }

    //merge|write result files
    if (countFiles > 1){
        if (mode == 1) chainUnion.Merge(sResultFile);
        else{
            TFile fChain(sResultFile, "RECREATE");
            chainUnion.Write();
            fChain.Close();
        }

        Int_t events = chainUnion.GetEntries();
        if (mode == 1) cout<<"The Chain witn "<<events<<" event(s) was merged to file \""<<sResultFile<<"\" from following files:"<<endl;
        else cout<<"The Chain witn "<<events<<" event(s) was written to file \""<<sResultFile<<"\" to point following files:"<<endl;

        TObjArray *fileElements = chainUnion.GetListOfFiles();
        TIter next(fileElements);
        TChainElement *chEl=0;
        while (( chEl=(TChainElement*)next() )) {
            char* pc = chEl->GetTitle();
            cout<<pc<<endl;
        }
    }

    //test reading result file
    TChain chainRead("cbmsim");
    chainRead.Add(sResultFile);

    Int_t events = chainRead.GetEntries();
    cout<<"The count of events in test reading is equal "<<events<<endl;
}
