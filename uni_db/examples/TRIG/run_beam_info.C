//#define ScaleBorderInfo

// print summary information about beam spill for a given run in a period (or for a whole period)
// if 'run' parameter is zero or absent then all runs in the period will be shown
void run_beam_info(int period = 7, int run = 0, TString target = "",
                   TString txtfile_path= "$VMCWORKDIR/uni_db/macros/parse_schemes/spill_run7/summaryMK.txt",
                   TString scheme_path = "$VMCWORKDIR/uni_db/macros/parse_schemes/spill_run7/spill_run7.xslt")
{
    // get spill info
    UniDbParser parser;
    vector<structParseValue*> parse_values;
    vector<structParseSchema> vecElements;
    int res_code = parser.ParseTxt2Struct(txtfile_path, scheme_path, parse_values, vecElements);
    if (res_code != 0)
    {
        cout<<endl<<"Macro finished with errors: parser error ("<<res_code<<")"<<endl;
        exit(-1);
    }

    if (parse_values.size() < 1)
    {
        cout<<endl<<"Macro finished with errors: there are no lines to parse"<<endl;
        exit(-2);
    }

    UniqueRunNumber* run_numbers = NULL;
    TObjArray* pRunArray = NULL;
    int run_count = 1;
    // get all runs for the given period if run_number = 0
    if (run == 0)
    {
        if (target == "")
        {
            run_count = UniDbRunPeriod::GetRunNumbers(period, run_numbers);
            if (run_count <= 0)
            {
                cout<<endl<<"Macro finished with errors: no runs exists in the given period #"<<period<<endl;
                exit(-3);
            }
        }
        else
        {
            // select only with a given condition, e.g. target particle
            TObjArray arrayConditions;
            arrayConditions.SetOwner(kTRUE);

            UniDbSearchCondition* searchCondition = new UniDbSearchCondition(columnPeriodNumber, conditionEqual, period);
            arrayConditions.Add((TObject*) searchCondition);
            searchCondition = new UniDbSearchCondition(columnTargetParticle, conditionEqual, target);   // one can choose any condition: beam, energy...
            arrayConditions.Add((TObject*) searchCondition);

            pRunArray = UniDbRun::Search(arrayConditions);
            run_count = pRunArray->GetEntriesFast();
            if (run_count <= 0)
            {
                cout<<endl<<"Macro finished with errors: no runs exists for the given target: "<<target<<endl;
                exit(-4);
            }
            arrayConditions.Delete();
        }
    }

    int sum_size = parse_values[0]->arrValues.size();
    vector<Long64_t> total_columns;
    for (int i_total = 0; i_total < sum_size; i_total++)
        total_columns.push_back(0);
    // cycle for all runs to sum the spill data
    for (int i = 0; i < run_count; i++)
    {
        if (run_numbers != NULL) run = run_numbers[i].run_number;
        if (pRunArray != NULL) run = ((UniDbRun*) pRunArray->At(i))->GetRunNumber();

        // get run time
        UniDbRun* pRun = UniDbRun::GetRun(period, run);
        if (pRun == NULL)
        {
            cout<<endl<<"Macro finished with errors: no experimental run was found "<<period<<" : "<<run<<" / "<<((UniDbRun*) pRunArray->At(i))->GetRunNumber()<<endl;
            if (run_numbers != NULL)
                delete [] run_numbers;
            if (pRunArray != NULL)
                delete pRunArray;
            exit(-5);
        }

        TDatime dtStart = pRun->GetStartDatetime();
        TDatime* dateEnd = pRun->GetEndDatetime();
        if (dateEnd == NULL)
        {
            cout<<"Macro finished with errors: no end datetime in the database for this run"<<endl;
            delete pRun;
            if (run_numbers != NULL)
                delete [] run_numbers;
            if (pRunArray != NULL)
                delete pRunArray;
            exit(-6);
        }
        TDatime dtEnd = *dateEnd;
        delete pRun;

        vector<Long64_t> sum_columns;
        for (int i_sum = 0; i_sum < sum_size; i_sum++)
            sum_columns.push_back(0);
        structParseValue* stPrevious;
        bool isFound = false;
        // cycle for all spills to sum for the given run
        for (int ind = 0; ind < parse_values.size(); ind++)
        {
            structParseValue* st = parse_values.at(ind);
            //cout<<"dtStart = "<<dtStart.AsSQLString()<<" : dtEnd = "<<dtEnd.AsSQLString()<<". st->dtSpillEnd = "<<st->dtSpillEnd.AsSQLString()<<endl;
            if (st->dtSpillEnd >= dtEnd)
            {
                if (isFound)
                {
                    stPrevious = parse_values.at(ind - 1);

                    #ifdef ScaleBorderInfo
                    // scale values
                    int sec1 = st->dtSpillEnd.Convert() - stPrevious->dtSpillEnd.Convert();
                    int sec2 = dtEnd.Convert() - stPrevious->dtSpillEnd.Convert();
                    double sec_ratio = sec2 / sec1;
                    for (int i_sum = 0; i_sum < sum_size; i_sum++)
                        sum_columns[i_sum] += TMath::Nint(boost::any_cast<int>(st->arrValues[i_sum]) * sec_ratio); // fix (Int_t)
                    #else
                    for (int i_sum = 0; i_sum < sum_size; i_sum++)
                        sum_columns[i_sum] += boost::any_cast<int>(st->arrValues[i_sum]); // fix (Int_t)
                    #endif
                }
                break;
            }

            if (st->dtSpillEnd > dtStart)
            {
                if (ind > 0)
                {
                    stPrevious = parse_values.at(ind - 1);

                    if (!isFound)
                    {
                        isFound = true;

                        #ifdef ScaleBorderInfo
                        // scale values
                        int sec1 = st->dtSpillEnd.Convert() - stPrevious->dtSpillEnd.Convert();
                        int sec2 = st->dtSpillEnd.Convert() - dtStart.Convert();
                        double sec_ratio = sec2 / sec1;
                        for (int i_sum = 0; i_sum < sum_size; i_sum++)
                            sum_columns[i_sum] += TMath::Nint(boost::any_cast<int>(st->arrValues[i_sum]) * sec_ratio); // fix (Int_t)
                        #endif
                    }
                }

                for (int i_sum = 0; i_sum < sum_size; i_sum++)
                    sum_columns[i_sum] += boost::any_cast<int>(st->arrValues[i_sum]); // fix (Int_t)
            }//if (st->dtSpillEnd > dtStart)
        }//for (int ind = 0; ind < parse_values.size(); ind++)

        for (int i_sum = 0; i_sum < sum_size; i_sum++)
            total_columns[i_sum] += sum_columns[i_sum];


        cout<<endl<<"Run #"<<run<<" ("<<dtStart.AsSQLString()<<" - "<<dtEnd.AsSQLString()<<")"<<endl;
        if (isFound)
        {
            int count_column = 0;
            for (vector<structParseSchema>::iterator it = vecElements.begin(); it != vecElements.end(); ++it)
            {
                structParseSchema schema = *it;
                if (schema.isSkip)
                    continue;

                if (schema.vecRows[0].strStatementType == "int")
                {
                    if (count_column > 0) cout<<", ";
                    cout<<"sum"<<schema.vecRows[0].strColumnName<<": "<<sum_columns[count_column];
                    count_column++;
                }
                else {
                    if (schema.vecRows[0].strStatementType == "double")
                    {
                        cout<<"ERROR: type of the column is not supported: "<<schema.vecRows[0].strStatementType<<endl;
                        exit(-7);
                    }
                }
            }
            cout<<endl;
        }
        else
            cout<<"No spill data!"<<endl;
    }

    if (run_count > 1)
    {
        cout<<endl<<"Total count for all runs:"<<endl;
        int count_column = 0;
        for (vector<structParseSchema>::iterator it = vecElements.begin(); it != vecElements.end(); ++it)
        {
            structParseSchema schema = *it;
            if (schema.isSkip)
                continue;

            if (schema.vecRows[0].strStatementType == "int")
            {
                if (count_column > 0) cout<<", ";
                cout<<"total"<<schema.vecRows[0].strColumnName<<": "<<total_columns[count_column];
                count_column++;
            }
            else {
                if (schema.vecRows[0].strStatementType == "double")
                {
                    cout<<"ERROR: type of the column is not supported: "<<schema.vecRows[0].strStatementType<<endl;
                    exit(-8);
                }
            }
        }
        cout<<endl<<endl;
    }

    // cleaning memory after work
    if (run_numbers != NULL)
        delete [] run_numbers;
    if (pRunArray != NULL)
        delete pRunArray;

    cout<<"Macro finished successfully"<<endl;
}
