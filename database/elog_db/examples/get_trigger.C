TString get_trigger(int period_number = 7, int run_number = 4608)
{
    TString strTrigger = "";
    TObjArray* pRecords = ElogDbRecord::GetRecords(period_number, run_number);

    TIter next(pRecords);
    ElogDbRecord* curRecord;
    while (curRecord == (ElogDbRecord*)next())
    {
        if (curRecord->GetTriggerId() != nullptr)
            strTrigger = ElogDbTrigger::GetTrigger(*(curRecord->GetTriggerId()))->GetTriggerInfo();
    }

    if (strTrigger == "") cout<<"Trigger information was not found"<<endl;
    else cout<<"Trigger for run "<<period_number<<":"<<run_number<<" = "<<strTrigger<<endl;

    return strTrigger;
}
