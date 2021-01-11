TString get_trigger(int period_number, int run_number)
{
    TString strTrigger = "";
    TObjArray* pRecords = ElogDbRecord::GetRecords(period_number, run_number);

    TIter next(pRecords);
    ElogDbRecord* curRecord;
    while (curRecord == (ElogDbRecord*)next())
    {
        cout<<"HERE!"<<endl;
        if (curRecord->GetTriggerId() != NULL)
            strTrigger = ElogDbTrigger::GetTrigger(*(curRecord->GetTriggerId()))->GetTriggerInfo();
    }

    if (strTrigger == "") cout<<"Trigger information was not found"<<endl;
    else cout<<"Trigger for run "<<period_number<<":"<<run_number<<" = "<<strTrigger<<endl;

    return strTrigger;
}
