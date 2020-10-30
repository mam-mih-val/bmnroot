#include "../db_settings.h"

int copy_data_table2table()
{
    TStopwatch timer;
    timer.Start();
    gDebug = 0;

    // connection to PostgreSQL Database
    TSQLServer* PgSQLServer = TSQLServer::Connect("pgsql://" + UNI_DB_HOST + "/" + UNI_DB_NAME, UNI_DB_USERNAME, UNI_DB_PASSWORD);
    if (PgSQLServer == 0x00)
    {
        cout<<"ERROR: connection to PgSQLServer was not established"<<endl;
        return -1;
    }

    // find all parameters in the source table
    TObjArray arrayConditions;
    UniDbSearchCondition* searchCondition = new UniDbSearchCondition(columnDCSerial, conditionNull);
    arrayConditions.Add((TObject*)searchCondition);

    TObjArray* pParameterArray = UniDbDetectorParameter::Search(arrayConditions);

    // clean memory for the conditions after the search
    arrayConditions.Delete();

    // read all parameters and write to the new table
    for (int i = 0; i < pParameterArray->GetEntriesFast(); i++)
    {
        UniDbDetectorParameter* pParameter = (UniDbDetectorParameter*) pParameterArray->At(i);
        vector<UniDbParameterValue*> parameter_value;

        switch (pParameter->GetParameterType())
        {
            case BoolType:
            {
                BoolValue* pNewParameter = new BoolValue;
                pNewParameter->value = pParameter->GetBool();
                parameter_value->push_back(pNewParameter);
                break;
            }
            case IntType:
            {
                IntValue* pNewParameter = new IntValue;
                pNewParameter->value = pParameter->GetInt();
                parameter_value->push_back(pNewParameter);
                break;
            }
            case DoubleType:
            {
                DoubleValue* pNewParameter = new DoubleValue;
                pNewParameter->value = pParameter->GetDouble();
                parameter_value->push_back(pNewParameter);
                break;
            }
            case StringType:
            {
                StringValue* pNewParameter = new StringValue;
                string s(pParameter->GetString.Data());
                pNewParameter->value = s;
                parameter_value->push_back(pNewParameter);
                break;
            }
            case IIArrayType:
            {
                IIValue* pNewParameter = new IIValue;
                IIStructure* ii_value = NULL; int element_count;
                pParameter->GetIIArray(ii_value, int& element_count);
                for (int j = 0; j < element_count; j++)
                {
                    pNewParameter->value1 = ii_value[j]->int_1;
                    pNewParameter->value2 = ii_value[j]->int_2;
                    parameter_value->push_back(pNewParameter);
                }
                break;
            }
            case IntArrayType:
            {
                IntValue* pNewParameter = new IntValue;
                int* i_value = NULL; int element_count;
                pParameter->GetIntArray(i_value, element_count);
                for (int j = 0; j < element_count; j++)
                {
                    pNewParameter->value = i_value[j];
                    parameter_value->push_back(pNewParameter);
                }
                break;
            }
            case DoubleArrayType:
            {
                DoubleValue* pNewParameter = new DoubleValue;
                double* d_value = NULL; int element_count;
                pParameter->GetDoubleArray(d_value, element_count);
                for (int j = 0; j < element_count; j++)
                {
                    pNewParameter->value = d_value[j];
                    parameter_value->push_back(pNewParameter);
                }
                break;
            }
            case BinaryArrayType:
            {
                BinaryValue* pNewParameter = new BinaryValue;
                size_t byte_count;
                pParameter->GetBinaryArray(pNewParameter->value, byte_count);
                pNewParameter->size = byte_count;
                parameter_value->push_back(pNewParameter);
                break;
            }
            case UIntArrayType:
            {
                UIntValue* pNewParameter = new UIntValue;
                unsigned int* ui_value = NULL; int element_count;
                pParameter->GetUIntArray(ui_value, element_count);
                for (int j = 0; j < element_count; j++)
                {
                    pNewParameter->value = ui_value[j];
                    parameter_value->push_back(pNewParameter);
                }
                break;
            }
            case DchMapArrayType:
            {
                DchMapValue* pNewParameter = new DchMapValue;
                DchMapStructure* dch_value = NULL; int element_count;
                pParameter->GetDchMapArray(dch_value, element_count);
                for (int j = 0; j < element_count; j++)
                {
                    pNewParameter->plane = dch_value[j]->plane;
                    pNewParameter->group = dch_value[j]->group;
                    pNewParameter->crate = dch_value[j]->crate;
                    pNewParameter->slot  = dch_value[j]->slot;
                    pNewParameter->channel_low = dch_value[j]->channel_low;
                    pNewParameter->channel_high = dch_value[j]->channel_high;
                    parameter_value->push_back(pNewParameter);
                }
                break;
            }
            case GemMapArrayType:
            {
                GemMapValue* pNewParameter = new GemMapValue;
                GemMapStructure* gem_value = NULL; int element_count;
                pParameter->GetGemMapArray(gem_value, element_count);
                for (int j = 0; j < element_count; j++)
                {
                    pNewParameter->serial = gem_value[j]->serial;
                    pNewParameter->id = gem_value[j]->id;
                    pNewParameter->station = gem_value[j]->station;
                    pNewParameter->channel_low = gem_value[j]->channel_low;
                    pNewParameter->channel_high = gem_value[j]->channel_high;
                    pNewParameter->hotZone  = gem_value[j]->hotZone;
                    parameter_value->push_back(pNewParameter);
                }
                break;
            }
            case GemPedestalArrayType:
            {
                GemPedestalValue* pNewParameter = new GemPedestalValue;
                GemPedestalStructure* gem_ped_value = NULL; int element_count;
                pParameter->GetGemPedestalArray(gem_ped_value, element_count);
                for (int j = 0; j < element_count; j++)
                {
                    pNewParameter->serial = gem_ped_value[j]->serial;
                    pNewParameter->channel = gem_ped_value[j]->channel;
                    pNewParameter->pedestal = gem_ped_value[j]->pedestal;
                    pNewParameter->noise = gem_ped_value[j]->noise;
                    parameter_value->push_back(pNewParameter);
                }
                break;
            }
            case TriggerMapArrayType:
            {
                TriggerMapValue* pNewParameter = new TriggerMapValue;
                TriggerMapStructure* trigger_value = NULL; int element_count;
                pParameter->GetTriggerMapArray(trigger_value, element_count);
                for (int j = 0; j < element_count; j++)
                {
                    pNewParameter->serial = trigger_value[j]->serial;
                    pNewParameter->slot = trigger_value[j]->slot;
                    pNewParameter->channel = trigger_value[j]->channel;
                    parameter_value->push_back(pNewParameter);
                }
                break;
            }
            case LorentzShiftArrayType:
            {
                LorentzShiftValue* pNewParameter = new LorentzShiftValue;
                LorentzShiftStructure* lorentz_value = NULL; int element_count;
                pParameter->GetLorentzShiftArray(lorentz_value, element_count);
                for (int j = 0; j < element_count; j++)
                {
                    pNewParameter->number = lorentz_value[j]->number;
                    pNewParameter->ls[0] = lorentz_value[j]->ls[0];
                    pNewParameter->ls[1] = lorentz_value[j]->ls[1];
                    pNewParameter->ls[2] = lorentz_value[j]->ls[2];
                    parameter_value->push_back(pNewParameter);
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

        UniDbDetectorParameterNew* pParameterNew = UniDbDetectorParameterNew::CreateDetectorParameter(
                    pParameter->GetDetectorName(), pParameter->GetParameterName(), pParameter->GetStartPeriod(), pParameter->GetStartRun(),
                    pParameter->GetEndPeriod(), pParameter->GetEndRun(), parameter_value);
    }

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

    delete PgSQLServer;

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);

    cout<<"Macro was successfull"<<endl;

    return 0;
}

void MoveParameter(TString parameter_name, enumParameterType parameter_type, enumParameterTypeNew parameter_type_new)
{
    TObjArray arrayConditions;
    UniDbSearchCondition* searchCondition = new UniDbSearchCondition(columnParameterName, conditionEqual, parameter_name);
    arrayConditions.Add((TObject*)searchCondition);

    pParameterArray = UniDbDetectorParameter::Search(arrayConditions);
    arrayConditions.Delete();

    int start_period = -1, start_run = -1;
    UniDbDetectorParameter* pParameter;
    vector<UniDbParameterValue*> parameter_value;
    for (int i = 0; i < pParameterArray->GetEntriesFast(); i++)
    {
        pParameter = (UniDbDetectorParameter*) pParameterArray->At(i);

        if ((start_period == -1) && (start_run == -1))
        {
            start_period = pParameter->GetStartPeriod(); start_run = pParameter->GetStartRun();
        }

        if ((start_period != pParameter->GetStartPeriod()) || (start_run != pParameter->GetStartRun()))
        {
            UniDbDetectorParameterNew* pParameterNew = UniDbDetectorParameterNew::CreateDetectorParameter(
                        pParameter->GetDetectorName(), pParameter->GetParameterName(), pParameter->GetStartPeriod(), pParameter->GetStartRun(),
                        pParameter->GetEndPeriod(), pParameter->GetEndRun(), parameter_value);
            start_period = pParameter->GetStartPeriod(); start_run = pParameter->GetStartRun();
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
                        parameter_value->push_back(pNewParameter);

                        break;
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
                        parameter_value->push_back(pNewParameter);

                        break;
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
                        pNewParameter->serial = *(pParameter->GetDcSerial());
                        pNewParameter->channel = *(pParameter->GetChannel());
                        for (int j = 0; j < element_count; j++)
                            pNewParameter->value.push_back(d_value[j]);
                        parameter_value->push_back(pNewParameter);

                        break;
                    }
                }//switch (parameter_type_new)

                break;
            }
        }//switch (parameter_type)
    }//for (int i = 0; i < pParameterArray->GetEntriesFast(); i++)

    if ((start_period != -1) || (start_run != -1))
    {
        UniDbDetectorParameterNew* pParameterNew = UniDbDetectorParameterNew::CreateDetectorParameter(
                    pParameter->GetDetectorName(), pParameter->GetParameterName(), pParameter->GetStartPeriod(), pParameter->GetStartRun(),
                    pParameter->GetEndPeriod(), pParameter->GetEndRun(), parameter_value);
    }
}
