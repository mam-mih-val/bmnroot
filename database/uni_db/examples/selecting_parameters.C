// macro for run selection by specified conditions
void selecting_parameters()
{
    TObjArray conditions;
    conditions.Add((TObject*) (new UniSearchCondition(columnStartPeriod, conditionLessOrEqual, 4)));
    conditions.Add((TObject*) (new UniSearchCondition(columnStartRun, conditionLessOrEqual, 83)));
    conditions.Add((TObject*) (new UniSearchCondition(columnEndPeriod, conditionGreaterOrEqual, 4)));
    conditions.Add((TObject*) (new UniSearchCondition(columnEndRun, conditionGreaterOrEqual, 83)));
    conditions.Add((TObject*) (new UniSearchCondition(columnDetectorName, conditionEqual, TString("TOF1"))));

    TObjArray* pValuesArray = UniDbDetectorParameter::Search(conditions);

    // clean memory for conditions after search
    conditions.Delete();

    cout<<"The number of parameter values is "<<pValuesArray->GetEntries()<<endl;

    TIter res_it(pValuesArray);
    UniDbDetectorParameter* curRow;
    while ((curRow = (UniDbDetectorParameter*)res_it()) != nullptr)
        curRow->Print();

    // clean memory after work - deleting the array of parameter values
    delete pValuesArray;

    cout << "\nMacro finished successfully" << endl;
}
