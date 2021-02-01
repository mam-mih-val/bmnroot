// macro for run selection by specified conditions
void selecting_parameters()
{
    TObjArray conditions;
    conditions.Add((TObject*) (new UniDbSearchCondition(columnStartPeriod, conditionLessOrEqual, 4)));
    conditions.Add((TObject*) (new UniDbSearchCondition(columnStartRun, conditionLessOrEqual, 83)));
    conditions.Add((TObject*) (new UniDbSearchCondition(columnEndPeriod, conditionGreaterOrEqual, 4)));
    conditions.Add((TObject*) (new UniDbSearchCondition(columnEndRun, conditionGreaterOrEqual, 83)));
    conditions.Add((TObject*) (new UniDbSearchCondition(columnDetectorName, conditionEqual, TString("TOF1"))));

    TObjArray* pValuesArray = UniDbDetectorParameter::Search(conditions);

    // clean memory for conditions after search
    conditions.Delete();

    cout<<"The number of parameter values is "<<pValuesArray->GetEntries()<<endl;

    TIter res_it(pValuesArray);
    UniDbDetectorParameter* curRow;
    while ((curRow = (UniDbDetectorParameter*)res_it()) != NULL)
        curRow->Print();

    // clean memory after work - deleting the array of parameter values
    delete pValuesArray;

    cout << "\nMacro finished successfully" << endl;
}
