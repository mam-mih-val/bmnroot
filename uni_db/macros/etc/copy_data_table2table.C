#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60
void MoveParameter(TString parameter_name, enumParameterType parameter_type, enumValueType parameter_type_new);
void printProgress(double percentage)
{
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush(stdout);
}

int copy_data_table2table()
{
    TStopwatch timer;
    timer.Start();

    cout<<"Forming conditions..."<<endl;
    // find all parameters in the source table, except having dc_serial
    TObjArray arrayConditions;
    UniDbSearchCondition* searchCondition = new UniDbSearchCondition(columnDCSerial, conditionNull);
    arrayConditions.Add((TObject*)searchCondition);

    cout<<"Getting original array for general parameters..."<<endl;
    TObjArray* pParameterArray = UniDbDetectorParameter::Search(arrayConditions);
    // clean memory for the conditions after the search
    arrayConditions.Delete();

    // read all parameters and write to the new table
    cout<<"Reading/writing general parameters: "<<pParameterArray->GetEntriesFast()<<endl;
    for (int i = 0; i < pParameterArray->GetEntriesFast(); i++)
    {
        UniDbDetectorParameter* pParameter = (UniDbDetectorParameter*) pParameterArray->At(i);
        vector<UniValue*> parameter_value;

        printProgress(((double)i)/(pParameterArray->GetEntriesFast()-1));
        //cout<<"Current parameter: "<<pParameter->GetParameterName()<<endl;

        switch (pParameter->GetParameterType())
        {
            case BoolType:
            {
                BoolValue* pNewParameter = new BoolValue;
                pNewParameter->value = pParameter->GetBool();
                parameter_value.push_back(pNewParameter);
                break;
            }
            case IntType:
            {
                IntValue* pNewParameter = new IntValue;
                pNewParameter->value = pParameter->GetInt();
                parameter_value.push_back(pNewParameter);
                break;
            }
            case DoubleType:
            {
                DoubleValue* pNewParameter = new DoubleValue;
                pNewParameter->value = pParameter->GetDouble();
                parameter_value.push_back(pNewParameter);
                break;
            }
            case StringType:
            {
                StringValue* pNewParameter = new StringValue;
                string s(pParameter->GetString().Data());
                pNewParameter->value = s;
                parameter_value.push_back(pNewParameter);
                break;
            }
            case IIArrayType:
            {
                IIStructure* ii_value = NULL; int element_count;
                pParameter->GetIIArray(ii_value, element_count);
                for (int j = 0; j < element_count; j++)
                {
                    IIValue* pNewParameter = new IIValue;
                    pNewParameter->value1 = ii_value[j].int_1;
                    pNewParameter->value2 = ii_value[j].int_2;
                    parameter_value.push_back(pNewParameter);
                }
                break;
            }
            case IntArrayType:
            {
                int* i_value = NULL; int element_count;
                pParameter->GetIntArray(i_value, element_count);
                for (int j = 0; j < element_count; j++)
                {
                    IntValue* pNewParameter = new IntValue;
                    pNewParameter->value = i_value[j];
                    parameter_value.push_back(pNewParameter);
                }
                break;
            }
            case DoubleArrayType:
            {
                double* d_value = NULL; int element_count;
                pParameter->GetDoubleArray(d_value, element_count);
                for (int j = 0; j < element_count; j++)
                {
                    DoubleValue* pNewParameter = new DoubleValue;
                    pNewParameter->value = d_value[j];
                    parameter_value.push_back(pNewParameter);
                }
                break;
            }
            case BinaryArrayType:
            {
                BinaryValue* pNewParameter = new BinaryValue;
                size_t byte_count;
                if (pParameter->GetBinaryArray(pNewParameter->value, byte_count) != 0)
                {
                    cout<<"ERROR: returning binary array was failed"<<endl;
                    return -4;
                }
                pNewParameter->size = byte_count;

                parameter_value.push_back(pNewParameter);
                break;
            }
            case UIntArrayType:
            {
                unsigned int* ui_value = NULL; int element_count;
                pParameter->GetUIntArray(ui_value, element_count);
                for (int j = 0; j < element_count; j++)
                {
                    UIntValue* pNewParameter = new UIntValue;
                    pNewParameter->value = ui_value[j];
                    parameter_value.push_back(pNewParameter);
                }
                break;
            }
            case DchMapArrayType:
            {
                DchMapStructure* dch_value = NULL; int element_count;
                pParameter->GetDchMapArray(dch_value, element_count);
                for (int j = 0; j < element_count; j++)
                {
                    DchMapValue* pNewParameter = new DchMapValue;
                    pNewParameter->plane = dch_value[j].plane;
                    pNewParameter->group = dch_value[j].group;
                    pNewParameter->crate = dch_value[j].crate;
                    pNewParameter->slot  = dch_value[j].slot;
                    pNewParameter->channel_low = dch_value[j].channel_low;
                    pNewParameter->channel_high = dch_value[j].channel_high;
                    parameter_value.push_back(pNewParameter);
                }
                break;
            }
            case GemMapArrayType:
            {
                GemMapStructure* gem_value = NULL; int element_count;
                pParameter->GetGemMapArray(gem_value, element_count);
                for (int j = 0; j < element_count; j++)
                {
                    GemMapValue* pNewParameter = new GemMapValue;
                    pNewParameter->serial = gem_value[j].serial;
                    pNewParameter->id = gem_value[j].id;
                    pNewParameter->station = gem_value[j].station;
                    pNewParameter->channel_low = gem_value[j].channel_low;
                    pNewParameter->channel_high = gem_value[j].channel_high;
                    pNewParameter->hotZone  = gem_value[j].hotZone;
                    parameter_value.push_back(pNewParameter);
                }
                break;
            }
            case GemPedestalArrayType:
            {
                GemPedestalStructure* gem_ped_value = NULL; int element_count;
                pParameter->GetGemPedestalArray(gem_ped_value, element_count);
                for (int j = 0; j < element_count; j++)
                {
                    GemPedestalValue* pNewParameter = new GemPedestalValue;
                    pNewParameter->serial = gem_ped_value[j].serial;
                    pNewParameter->channel = gem_ped_value[j].channel;
                    pNewParameter->pedestal = gem_ped_value[j].pedestal;
                    pNewParameter->noise = gem_ped_value[j].noise;
                    parameter_value.push_back(pNewParameter);
                }
                break;
            }
            case TriggerMapArrayType:
            {
                TriggerMapStructure* trigger_value = NULL; int element_count;
                pParameter->GetTriggerMapArray(trigger_value, element_count);
                for (int j = 0; j < element_count; j++)
                {
                    TriggerMapValue* pNewParameter = new TriggerMapValue;
                    pNewParameter->serial = trigger_value[j].serial;
                    pNewParameter->slot = trigger_value[j].slot;
                    pNewParameter->channel = trigger_value[j].channel;
                    parameter_value.push_back(pNewParameter);
                }
                break;
            }
            case LorentzShiftArrayType:
            {
                LorentzShiftStructure* lorentz_value = NULL; int element_count;
                pParameter->GetLorentzShiftArray(lorentz_value, element_count);
                for (int j = 0; j < element_count; j++)
                {
                    LorentzShiftValue* pNewParameter = new LorentzShiftValue;
                    pNewParameter->number = lorentz_value[j].number;
                    pNewParameter->ls[0] = lorentz_value[j].ls[0];
                    pNewParameter->ls[1] = lorentz_value[j].ls[1];
                    pNewParameter->ls[2] = lorentz_value[j].ls[2];
                    parameter_value.push_back(pNewParameter);
                }
                break;
            }
            case ErrorType:
            {
                cout<<"ERROR: ErrorType was unexpectedly found"<<endl;
                return -2;
            }
            default:
            {
                cout<<"ERROR: the parameter type was not defined"<<endl;
                return -3;
            }
        }

        //cout<<"DetectorName = "<<pParameter->GetDetectorName()<<". ParameterName = "<<pParameter->GetParameterName()<<". StartPeriod = "<<pParameter->GetStartPeriod()<<". StartRun = "<<pParameter->GetStartRun()<<". EndPeriod = "<<pParameter->GetEndPeriod()<<". GetEndRun() = "<<pParameter->GetEndRun()<<". parameter_value size = "<<parameter_value.size()<<endl;
        UniDbDetectorParameterNew* pParameterNew = UniDbDetectorParameterNew::CreateDetectorParameter(
                    pParameter->GetDetectorName(), pParameter->GetParameterName(), pParameter->GetStartPeriod(), pParameter->GetStartRun(),
                    pParameter->GetEndPeriod(), pParameter->GetEndRun(), parameter_value);
        delete pParameterNew;
        parameter_value.clear();
    }
    cout<<endl;

    // clean memory after work
    delete pParameterArray;

    // SPECIAL CASES: INL = double* (serial + channel) -> MapDVectorValue
    MoveParameter(TString("inl"), DoubleArrayType, MapDVectorTypeNew);
    // plane = int (serial + channel) -> MapIntValue
    MoveParameter(TString("plane"), IntType, MapIntTypeNew);
    // strip = int (serial + channel) -> MapIntValue
    MoveParameter(TString("strip"), IntType, MapIntTypeNew);
    // side = bool (serial + channel) -> MapBoolValue
    MoveParameter(TString("side"), BoolType, MapBoolTypeNew);

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);

    cout<<"Macro was successfull"<<endl;

    return 0;
}

void MoveParameter(TString parameter_name, enumParameterType parameter_type, enumValueType parameter_type_new)
{
    cout<<endl<<"Forming conditions for a special parameter: "<<parameter_name<<"..."<<endl;
    TObjArray arrayConditions;
    UniDbSearchCondition* searchCondition = new UniDbSearchCondition(columnParameterName, conditionEqual, parameter_name);
    arrayConditions.Add((TObject*)searchCondition);

    cout<<"Getting original array for parameter: "<<parameter_name<<"..."<<endl;
    TObjArray* pParameterArray = UniDbDetectorParameter::Search(arrayConditions);
    arrayConditions.Delete();

    int start_period = -1, start_run = -1, end_period = -1, end_run = -1;
    UniDbDetectorParameter* pParameter;
    vector<UniValue*> parameter_value;
    cout<<"Reading/writing '"<<parameter_name<<"' parameter: "<<pParameterArray->GetEntriesFast()<<endl;
    for (int i = 0; i < pParameterArray->GetEntriesFast(); i++)
    {
        pParameter = (UniDbDetectorParameter*) pParameterArray->At(i);

        printProgress(((double)i)/(pParameterArray->GetEntriesFast()-1));
        //cout<<"Current parameter: "<<pParameter->GetParameterName()<<". Parameter type = "<<pParameter->GetParameterType()<<
        //      ". Start period = "<<pParameter->GetStartPeriod()<<". Start run = "<<pParameter->GetStartRun()<<endl;

        if ((start_period == -1) && (start_run == -1))
        {
            start_period = pParameter->GetStartPeriod(); start_run = pParameter->GetStartRun();
            end_period = pParameter->GetEndPeriod(); end_run = pParameter->GetEndRun();
        }

        if ((start_period != pParameter->GetStartPeriod()) || (start_run != pParameter->GetStartRun()))
        {
            //cout<<"Write new parameter: "<<pParameter->GetParameterName()<<", start_period = "<<start_period<<", start_run = "<<start_run<<", value size = "<<parameter_value.size()<<endl;
            UniDbDetectorParameterNew* pParameterNew = UniDbDetectorParameterNew::CreateDetectorParameter(
                        pParameter->GetDetectorName(), pParameter->GetParameterName(), start_period, start_run,
                        end_period, end_run, parameter_value);
            delete pParameterNew;
            start_period = pParameter->GetStartPeriod(); start_run = pParameter->GetStartRun();
            end_period = pParameter->GetEndPeriod(); end_run = pParameter->GetEndRun();
            parameter_value.clear();    
        }

        switch (parameter_type)
        {
            case BoolType:
            {
                bool b_value = pParameter->GetBool();

                switch (parameter_type_new)
                {
                    case MapBoolTypeNew:
                    {
                        MapBoolValue* pNewParameter = new MapBoolValue;
                        pNewParameter->serial = *(pParameter->GetDcSerial());
                        pNewParameter->channel = *(pParameter->GetChannel());
                        pNewParameter->value = b_value;
                        parameter_value.push_back(pNewParameter);

                        break;
                    }
                    default:
                    {
                        cout<<"ERROR: the new parameter type was not defined for BoolType"<<endl;
                    }
                }//switch (parameter_type_new)

                break;
            }
            case IntType:
            {
                int i_value = pParameter->GetInt();

                switch (parameter_type_new)
                {
                    case MapIntTypeNew:
                    {
                        MapIntValue* pNewParameter = new MapIntValue;
                        pNewParameter->serial = *(pParameter->GetDcSerial());
                        pNewParameter->channel = *(pParameter->GetChannel());
                        pNewParameter->value = i_value;
                        parameter_value.push_back(pNewParameter);

                        break;
                    }
                    default:
                    {
                        cout<<"ERROR: the new parameter type was not defined for IntType"<<endl;
                    }
                }//switch (parameter_type_new)

                break;
            }
            case DoubleArrayType:
            {
                double* d_value; int element_count;
                pParameter->GetDoubleArray(d_value, element_count);

                switch (parameter_type_new)
                {
                    case MapDVectorTypeNew:
                    {
                        MapDVectorValue* pNewParameter = new MapDVectorValue;
                        //cout<<"serial = "<<*(pParameter->GetDcSerial())<<", channel = "<<*(pParameter->GetChannel())<<endl;
                        pNewParameter->serial = *(pParameter->GetDcSerial());
                        pNewParameter->channel = *(pParameter->GetChannel());
                        for (int j = 0; j < element_count; j++)
                            pNewParameter->value.push_back(d_value[j]);
                        parameter_value.push_back(pNewParameter);

                        break;
                    }
                    default: cout<<"ERROR: the new parameter type was not defined for DoubleArrayType"<<endl;
                }//switch (parameter_type_new)

                break;
            }
            default: cout<<"ERROR: the old parameter type was not defined for the special case"<<endl;
        }//switch (parameter_type)
    }//for (int i = 0; i < pParameterArray->GetEntriesFast(); i++)
    cout<<endl;

    if ((start_period != -1) && (start_run != -1))
    {
        //cout<<"Final writing new parameter: "<<pParameter->GetParameterName()<<", start_period = "<<start_period<<", start_run = "<<start_run<<", value size = "<<parameter_value.size()<<endl;
        UniDbDetectorParameterNew* pParameterNew = UniDbDetectorParameterNew::CreateDetectorParameter(
                    pParameter->GetDetectorName(), pParameter->GetParameterName(), start_period, start_run,
                    end_period, end_run, parameter_value);
        delete pParameterNew;
    }
}
