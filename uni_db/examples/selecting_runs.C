// macro for run selection by specified conditions
#include "../db_structures.h"

void selecting_runs()
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    TObjArray arrayConditions;
    UniDbSearchCondition* searchCondition = new UniDbSearchCondition(columnBeamParticle, conditionEqual, TString("d"));
    arrayConditions.Add((TObject*)searchCondition);
    //searchCondition = new UniDbSearchCondition(columnTargetParticle, conditionNull);
    searchCondition = new UniDbSearchCondition(columnTargetParticle, conditionEqual, TString("Cu"));
    arrayConditions.Add((TObject*)searchCondition);

    TObjArray* pRunArray = UniDbRun::Search(arrayConditions);

    // clean memory for conditions after search
    for (int i = 0; i < arrayConditions.GetEntriesFast(); i++)
        delete (UniDbSearchCondition*) arrayConditions[i];
    arrayConditions.Clear();

    // print run numbers and file path with raw data
    for (int i = 0; i < pRunArray->GetEntriesFast(); i++)
    {
        UniDbRun* pRun = (UniDbRun*) pRunArray->At(i);
        cout<<"Run (d-Cu): number - "<<pRun->GetRunNumber()<<", file path - "<<pRun->GetFilePath()<<endl;
    }

    // clean memory after work
    for (int i = 0; i < pRunArray->GetEntriesFast(); i++)
        delete (UniDbRun*)pRunArray->At(i);
    delete pRunArray;

    cout << "\nMacro finished successfully" << endl;
}
