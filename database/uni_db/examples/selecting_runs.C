// macro for run selection by specified conditions
void selecting_runs()
{
    TObjArray arrayConditions;
    UniSearchCondition* searchCondition = new UniSearchCondition(columnBeamParticle, conditionEqual, TString("d"));
    arrayConditions.Add((TObject*)searchCondition);
    //searchCondition = new UniSearchCondition(columnTargetParticle, conditionNull);
    searchCondition = new UniSearchCondition(columnTargetParticle, conditionEqual, TString("Cu"));
    arrayConditions.Add((TObject*)searchCondition);

    TObjArray* pRunArray = UniDbRun::Search(arrayConditions);

    // clean memory for conditions after search
    arrayConditions.Delete();

    // print run numbers and file path with raw data
    for (int i = 0; i < pRunArray->GetEntriesFast(); i++)
    {
        UniDbRun* pRun = (UniDbRun*) pRunArray->At(i);
        cout<<"Run (d-Cu): period - "<<pRun->GetPeriodNumber()<<", run - "<<pRun->GetRunNumber()<<", file path - "<<pRun->GetFilePath()<<endl;
    }

    // clean memory after work - deleting the array of runs
    delete pRunArray;

    cout << "\nMacro finished successfully" << endl;
}
