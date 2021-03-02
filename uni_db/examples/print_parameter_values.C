// macro for printing parameter values for selected detector
void print_parameter_values(const char* pcDetectorName="")
{
    if (pcDetectorName[0] == '\0')
    {
        UniDbDetectorParameter::PrintAll();
        return;
    }

    TObjArray arrayConditions;
    UniDbSearchCondition* searchCondition = new UniDbSearchCondition(columnDetectorName, conditionEqual, TString(pcDetectorName));
    arrayConditions.Add((TObject*)searchCondition);

    TObjArray* pParameterValueArray = UniDbDetectorParameter::Search(arrayConditions);

    // clean memory for conditions after search
    arrayConditions.Delete();

    // print run numbers and file path with raw data
    for (int i = 0; i < pParameterValueArray->GetEntriesFast(); i++)
    {
        UniDbDetectorParameter* pParameterValue = (UniDbDetectorParameter*) pParameterValueArray->At(i);

        cout<<"Parameter: name - "<<pParameterValue->GetParameterName()<<", parameter value - "<<pParameterValue->GetParameterValue()
            <<" (run "<<pParameterValue->GetStartRun()<<"-"<<pParameterValue->GetEndRun()<<")"<<endl;
    }

    // clean memory after work
    for (int i = 0; i < pParameterValueArray->GetEntriesFast(); i++)
        delete (UniDbRun*)pParameterValueArray->At(i);
    delete pParameterValueArray;

    cout << "\nMacro finished successfully" << endl;
}
