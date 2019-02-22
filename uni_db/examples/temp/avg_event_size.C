// macro for calculating average event size for small runs (raw files) stored in Dhe database for one session
class UniqueRunNumber;
void avg_event_size(int period)
{
    UniqueRunNumber* run_numbers;
    int run_count = UniDbRun::GetRunNumbers(period, 1, period, 100000, run_numbers);
    if (run_count <= 0)
        return;

    // red color if terminal
    //bool isTerminal = isatty(fileno(stdout));
    const string red("\033[0;31m");
    const string reset("\033[0m");

    int right_count = 0;
    double sum_size = 0;
    for (int i = 0; i < run_count; i++)
    {
        // get run
        UniDbRun* pRun = UniDbRun::GetRun(run_numbers[i].period_number, run_numbers[i].run_number);
        if (pRun == NULL)
        {
            cout<<"The function encountered with errors: no experimental run was found ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            continue;
        }

        // file size in KB
        double* p_file_size = pRun->GetFileSize();
        int* p_event_count = pRun->GetEventCount();
        delete pRun;

        if (p_file_size == NULL)
        {
            cout<<"The function encountered with errors: file size was not defined for "<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<endl;
            continue;
        }
        if (p_event_count == NULL)
        {
            cout<<"The function encountered with errors: event count was not defined for "<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<endl;
            continue;
        }

        double file_size = (*p_file_size) * 1024;
        int event_count = *p_event_count;
        double event_size = file_size / event_count;

        // don't take into account files with event size < 50 KB or > 500 KB
        if ((event_size < 50) || (event_size > 500)) cout<<red;
        else
        {
            sum_size += event_size;
            right_count++;
        }
        cout<<"Event size for "<<run_numbers[i].period_number<<"-"<<run_numbers[i].run_number<<" ("<<file_size/1024<<"MB/"<<event_count<<"): "<<event_size<<" KB";
        if ((event_size < 50) || (event_size > 500)) cout<<reset;
        cout<<endl;
    }
    double average_event_size = sum_size / right_count;
    cout<<endl<<"Average event size for period "<<period<<": "<<average_event_size<<" KB"<<endl;

    delete run_numbers;
    cout<<"Macro finished successfully"<<endl;
}
