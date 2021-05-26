// macro for ELOG record selection by specified conditions
void selecting_records()
{
    TObjArray arrayConditions;
    UniSearchCondition* searchCondition = new UniSearchCondition(columnPeriodNumber, conditionEqual, 6);
    arrayConditions.Add((TObject*)searchCondition);
    searchCondition = new UniSearchCondition(columnTargetParticle, conditionEqual, TString("Cu"));
    arrayConditions.Add((TObject*)searchCondition);
    searchCondition = new UniSearchCondition(columnSp41, conditionGreaterOrEqual, 1200);
    arrayConditions.Add((TObject*)searchCondition);
    searchCondition = new UniSearchCondition(columnSp41, conditionLessOrEqual, 1800);
    arrayConditions.Add((TObject*)searchCondition);

    TObjArray* pRecordArray = ElogDbRecord::Search(arrayConditions);

    // clean memory for conditions after search
    arrayConditions.Delete();

    // print run numbers and file path with raw data
    for (int i = 0; i < pRecordArray->GetEntriesFast(); i++)
    {
        ElogDbRecord* pRecord = (ElogDbRecord*) pRecordArray->At(i);
        cout<<"Record: time - "<<pRecord->GetRecordDate().AsSQLString()<<", period - "<<(pRecord->GetPeriodNumber() == NULL ? -1 : *pRecord->GetPeriodNumber())
            <<", run - "<<(pRecord->GetRunNumber() == NULL ? -1 : *pRecord->GetRunNumber())<<", SP-41 field - "<<(pRecord->GetSp41() == NULL ? -1 : *pRecord->GetSp41())<<"A"<<endl;
    }

    // clean memory after work - deleting the array of runs
    delete pRecordArray;

    cout << "\nMacro finished successfully" << endl;
}
