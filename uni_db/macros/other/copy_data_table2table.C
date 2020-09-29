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
    UniDbSearchCondition* searchCondition = new UniDbSearchCondition(columnStartPeriod, conditionGreaterOrEqual, 0);
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
                for (int i = 0; i < element_count; i++)
                {
                    pNewParameter->value1 = ii_value[i]->int_1;
                    pNewParameter->value2 = ii_value[i]->int_2;
                    parameter_value->push_back(pNewParameter);
                }
                break;
            }
            case IntArrayType:
            {
                IntValue* pNewParameter = new IntValue;
                int* i_value = NULL; int element_count;
                pParameter->GetIntArray(i_value, element_count);
                for (int i = 0; i < element_count; i++)
                {
                    pNewParameter->value = i_value[i];
                    parameter_value->push_back(pNewParameter);
                }
                break;
            }
            case DoubleArrayType:
            {
                DoubleValue* pNewParameter = new DoubleValue;
                double* d_value = NULL; int element_count;
                pParameter->GetDoubleArray(d_value, element_count);
                for (int i = 0; i < element_count; i++)
                {
                    pNewParameter->value = d_value[i];
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
                for (int i = 0; i < element_count; i++)
                {
                    pNewParameter->value = ui_value[i];
                    parameter_value->push_back(pNewParameter);
                }
                break;
            }
            case DchMapArrayType:
            {
                DchMapValue* pNewParameter = new DchMapValue;
                DchMapStructure* dch_value = NULL; int element_count;
                pParameter->GetDchMapArray(dch_value, element_count);
                for (int i = 0; i < element_count; i++)
                {
                    pNewParameter->plane = dch_value[i]->plane;
                    pNewParameter->group = dch_value[i]->group;
                    pNewParameter->crate = dch_value[i]->crate;
                    pNewParameter->slot  = dch_value[i]->slot;
                    pNewParameter->channel_low = dch_value[i]->channel_low;
                    pNewParameter->channel_high = dch_value[i]->channel_high;
                    parameter_value->push_back(pNewParameter);
                }
                break;
            }
            case GemMapArrayType:
            {
                GemMapValue* pNewParameter = new GemMapValue;
                GemMapStructure* gem_value = NULL; int element_count;
                pParameter->GetGemMapArray(gem_value, element_count);
                for (int i = 0; i < element_count; i++)
                {
                    pNewParameter->serial = gem_value[i]->serial;
                    pNewParameter->id = gem_value[i]->id;
                    pNewParameter->station = gem_value[i]->station;
                    pNewParameter->channel_low = gem_value[i]->channel_low;
                    pNewParameter->channel_high = gem_value[i]->channel_high;
                    pNewParameter->hotZone  = gem_value[i]->hotZone;
                    parameter_value->push_back(pNewParameter);
                }
                break;
            }
            case GemPedestalArrayType:
            {
                GemPedestalValue* pNewParameter = new GemPedestalValue;
                GemPedestalStructure* gem_ped_value = NULL; int element_count;
                pParameter->GetGemPedestalArray(gem_ped_value, element_count);
                for (int i = 0; i < element_count; i++)
                {
                    pNewParameter->serial = gem_ped_value[i]->serial;
                    pNewParameter->channel = gem_ped_value[i]->channel;
                    pNewParameter->pedestal = gem_ped_value[i]->pedestal;
                    pNewParameter->noise = gem_ped_value[i]->noise;
                    parameter_value->push_back(pNewParameter);
                }
                break;
            }
            case TriggerMapArrayType:
            {
                TriggerMapValue* pNewParameter = new TriggerMapValue;
                TriggerMapStructure* trigger_value = NULL; int element_count;
                pParameter->GetTriggerMapArray(trigger_value, element_count);
                for (int i = 0; i < element_count; i++)
                {
                    pNewParameter->serial = trigger_value[i]->serial;
                    pNewParameter->slot = trigger_value[i]->slot;
                    pNewParameter->channel = trigger_value[i]->channel;
                    parameter_value->push_back(pNewParameter);
                }
                break;
            }
            case LorentzShiftArrayType:
            {
                LorentzShiftValue* pNewParameter = new LorentzShiftValue;
                LorentzShiftStructure* lorentz_value = NULL; int element_count;
                pParameter->GetLorentzShiftArray(lorentz_value, element_count);
                for (int i = 0; i < element_count; i++)
                {
                    pNewParameter->number = lorentz_value[i]->number;
                    pNewParameter->ls[0] = lorentz_value[i]->ls[0];
                    pNewParameter->ls[1] = lorentz_value[i]->ls[1];
                    pNewParameter->ls[2] = lorentz_value[i]->ls[2];
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
                    pParameter->GetEndPeriod(), pParameter->GetEndRun(), parameter_value, int value_key = 0);
    }

    // clean memory after work
    delete pParameterArray;
    delete PgSQLServer;

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);

    cout<<"Macro was successfull"<<endl;

    return 0;
}
