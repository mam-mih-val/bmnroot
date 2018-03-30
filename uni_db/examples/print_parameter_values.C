#include "../../gconfig/basiclibs.C"

// macro for printing parameter values for selected detector
void print_parameter_values(char* pcDetectorName="")
{
    basiclibs();
    gSystem->Load("libUniDb");

    if (pcDetectorName == "")
    {
        UniDbDetectorParameter::PrintAll();
        return;
    }

    TObjArray arrayConditions;
    UniDbSearchCondition* searchCondition = new UniDbSearchCondition(columnDetectorName, conditionEqual, TString(pcDetectorName));
    arrayConditions.Add((TObject*)searchCondition);

    TObjArray* pParameterValueArray = UniDbDetectorParameter::Search(arrayConditions);

    // clean memory for conditions after search
    for (int i = 0; i < arrayConditions.GetEntriesFast(); i++)
        delete (UniDbSearchCondition*) arrayConditions[i];
    arrayConditions.Clear();

    // print run numbers and file path with raw data
    for (int i = 0; i < pParameterValueArray->GetEntriesFast(); i++)
    {
        UniDbDetectorParameter* pParameterValue = (UniDbDetectorParameter*) pParameterValueArray->At(i);

        if (pParameterValue->GetDcSerial() == 0x00)
            cout<<"Parameter: name - "<<pParameterValue->GetParameterName()<<", parameter value - "<<pParameterValue->GetParameterValue()
               <<" (run "<<pParameterValue->GetStartRun()<<"-"<<pParameterValue->GetEndRun()<<")"<<endl;
        else
            cout<<"Parameter: name - "<<pParameterValue->GetParameterName()<<", parameter value - "<<pParameterValue->GetParameterValue()
               <<", TDC:Channel - "<<*pParameterValue->GetDcSerial()<<":"<<*pParameterValue->GetChannel()
               <<" (run "<<pParameterValue->GetStartRun()<<"-"<<pParameterValue->GetEndRun()<<")"<<endl;
    }

    // clean memory after work
    for (int i = 0; i < pParameterValueArray->GetEntriesFast(); i++)
        delete (UniDbRun*)pParameterValueArray->At(i);
    delete pParameterValueArray;

    cout << "\nMacro finished successfully" << endl;
}
