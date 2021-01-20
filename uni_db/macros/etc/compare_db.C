#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60
void printProgress(double percentage)
{
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush(stdout);
}

int compare_db()
{
    TStopwatch timer;
    timer.Start();

    // find all parameters in the old table of the old database
    cout<<"Forming conditions to compare all parameters..."<<endl;
    TObjArray arrayConditions;
    //UniDbSearchCondition* searchConditionPeriod = new UniDbSearchCondition(columnStartPeriod, conditionGreater, 0);
    //arrayConditions.Add((TObject*)searchConditionPeriod);
    UniDbSearchCondition* searchConditionSerial = new UniDbSearchCondition(columnDCSerial, conditionNotNull);
    arrayConditions.Add((TObject*)searchConditionSerial);
    //UniDbSearchCondition* searchConditionName = new UniDbSearchCondition(columnParameterName, conditionEqual, (TString)"inl");
    //arrayConditions.Add((TObject*)searchConditionName);

    cout<<"Getting original array for parameters..."<<endl;
    TObjArray* pParameterArray = UniDbDetectorParameter::Search(arrayConditions);
    // clean memory for the conditions after the search
    arrayConditions.Delete();

    // read all parameters and find in the new table of the new database
    cout<<"Reading and searching for corresponding parameters: "<<pParameterArray->GetEntriesFast()<<endl;
    for (int i = 0; i < pParameterArray->GetEntriesFast(); i++)
    {
        UniDbDetectorParameter* pParameter = (UniDbDetectorParameter*) pParameterArray->At(i);

        //printProgress(((double)i)/(pParameterArray->GetEntriesFast()-1));
        cout<<"pParameter->GetDetectorName() = "<<pParameter->GetDetectorName()<<". pParameter->GetParameterName() = "<<pParameter->GetParameterName()<<
              ". pParameter->GetStartPeriod() = "<<pParameter->GetStartPeriod()<<". pParameter->GetStartRun() = "<<pParameter->GetStartRun()<<endl;
        if ((pParameter->GetStartPeriod() == 4) && (pParameter->GetStartRun() == 61) && (pParameter->GetParameterName() == "GEM_pedestal"))
            continue;

        UniDbDetectorParameterNew* pNewParameter = UniDbDetectorParameterNew::GetDetectorParameter(pParameter->GetDetectorName(), pParameter->GetParameterName(),
                                                                                                   pParameter->GetStartPeriod(), pParameter->GetStartRun());
        if (pNewParameter == NULL)
        {
            cout<<"ERROR: returning new parameter was failed, where"<<endl<<"pParameter->GetDetectorName() = "<<pParameter->GetDetectorName()<<
                ". pParameter->GetParameterName() = "<<pParameter->GetParameterName()<<". pParameter->GetStartPeriod() = "<<pParameter->GetStartPeriod()<<
                ". pParameter->GetStartRun() = "<<pParameter->GetStartRun()<<endl;
            return -3;
        }

        vector<UniValue*> parameter_value;
        if (pNewParameter->GetParameterValue(parameter_value) != 0)
        {
            cout<<"ERROR: returning new parameter value was failed for "<<pNewParameter->GetParameterType()<<endl;
            return -4;
        }

        switch (pParameter->GetParameterType())
        {
            case BoolType:
            {
                if (pParameter->GetDcSerial() == NULL)
                {
                    BoolValue* pValue = (BoolValue*) parameter_value[0];
                    if (parameter_value.size() != 1)
                    {
                        cout<<"ERROR: new parameter must have 1 value, but it has "<<parameter_value.size()<<" (old BoolType)"<<endl;
                        return -5;
                    }
                    if (pValue->value != pParameter->GetBool())
                    {
                        cout<<"ERROR: new parameter must be equal to old value "<<pParameter->GetBool()<<" (BoolType)"<<endl;
                        return -6;
                    }
                }
                else
                {
                    bool is_found = false;
                    for (int j = 0; j < parameter_value.size(); j++)
                    {
                        MapBoolValue* pValue = (MapBoolValue*) parameter_value[j];
                        if ((pValue->serial == *(pParameter->GetDcSerial())) && (pValue->channel == *(pParameter->GetChannel())) &&
                           (pValue->value == pParameter->GetBool()))
                        {
                            is_found = true;
                            break;
                        }
                    }
                    if (!is_found)
                    {
                        cout<<"ERROR: new parameter corresponding to the old value was not found:"<<endl<<"pParameter->GetDcSerial() = "<<
                              *(pParameter->GetDcSerial())<<". pParameter->GetChannel() = "<<*(pParameter->GetChannel())<<". pParameter->GetBool() = "<<
                              pParameter->GetBool()<<endl;
                        return -7;
                    }
                }

                break;
            }
            case IntType:
            {
                if (pParameter->GetDcSerial() == NULL)
                {
                    IntValue* pValue = (IntValue*) parameter_value[0];
                    if (parameter_value.size() != 1)
                    {
                        cout<<"ERROR: new parameter must have 1 value, but it has "<<parameter_value.size()<<" (old IntType)"<<endl;
                        return -8;
                    }
                    if (pValue->value != pParameter->GetInt())
                    {
                        cout<<"ERROR: new parameter must be equal to old value "<<pParameter->GetInt()<<" (IntType)"<<endl;
                        return -9;
                    }
                }
                else
                {
                    bool is_found = false;
                    for (int j = 0; j < parameter_value.size(); j++)
                    {
                        MapIntValue* pValue = (MapIntValue*) parameter_value[j];
                        if ((pValue->serial == *(pParameter->GetDcSerial())) && (pValue->channel == *(pParameter->GetChannel())) &&
                           (pValue->value == pParameter->GetInt()))
                        {
                            is_found = true;
                            break;
                        }
                    }
                    if (!is_found)
                    {
                        cout<<"ERROR: new parameter corresponding to the old value was not found:"<<endl<<"pParameter->GetDcSerial() = "<<
                              *(pParameter->GetDcSerial())<<". pParameter->GetChannel() = "<<*(pParameter->GetChannel())<<". pParameter->GetInt() = "<<
                              pParameter->GetInt()<<endl;
                        return -10;
                    }
                }

                break;
            }
            case DoubleType:
            {
                if (pParameter->GetDcSerial() != NULL)
                {
                    cout<<"ERROR: old parameter has dc_serial, but it belongs to DoubleType"<<endl;
                    return -11;
                }

                DoubleValue* pValue = (DoubleValue*) parameter_value[0];
                if (parameter_value.size() != 1)
                {
                    cout<<"ERROR: new parameter must have 1 value, but it has "<<parameter_value.size()<<" (old DoubleType)"<<endl;
                    return -12;
                }
                if (pValue->value != pParameter->GetDouble())
                {
                    cout<<"ERROR: new parameter must be equal to old value "<<pParameter->GetDouble()<<" (DoubleType)"<<endl;
                    return -13;
                }

                break;
            }
            case StringType:
            {
                if (pParameter->GetDcSerial() != NULL)
                {
                    cout<<"ERROR: old parameter has dc_serial, but it belongs to StringType"<<endl;
                    return -14;
                }

                StringValue* pValue = (StringValue*) parameter_value[0];
                if (parameter_value.size() != 1)
                {
                    cout<<"ERROR: new parameter must have 1 value, but it has "<<parameter_value.size()<<" (old StringType)"<<endl;
                    return -15;
                }
                string s(pParameter->GetString().Data());
                if (pValue->value != s)
                {
                    cout<<"ERROR: new parameter must be equal to old value "<<pParameter->GetString()<<" (StringType)"<<endl;
                    return -16;
                }

                break;
            }
            case IIArrayType:
            {
                if (pParameter->GetDcSerial() != NULL)
                {
                    cout<<"ERROR: old parameter has dc_serial, but it belongs to IIArrayType"<<endl;
                    return -17;
                }

                IIStructure* ii_value = NULL; int element_count;
                pParameter->GetIIArray(ii_value, element_count);
                if (parameter_value.size() != element_count)
                {
                    cout<<"ERROR: new parameter must have "<<element_count<<" values, but it has "<<parameter_value.size()<<" (old IIArrayType)"<<endl;
                    return -18;
                }
                for (int j = 0; j < element_count; j++)
                {
                    IIValue* pValue = (IIValue*) parameter_value[j];
                    if ((pValue->value1 != ii_value[j].int_1) || (pValue->value2 != ii_value[j].int_2))
                    {
                        cout<<"ERROR: new parameter must be equal to old value "<<ii_value[j].int_1<<" (IIArrayType)"<<endl;
                        return -19;
                    }
                }

                break;
            }
            case IntArrayType:
            {
                if (pParameter->GetDcSerial() != NULL)
                {
                    cout<<"ERROR: old parameter has dc_serial, but it belongs to IntArrayType"<<endl;
                    return -20;
                }

                int* i_value = NULL; int element_count;
                pParameter->GetIntArray(i_value, element_count);
                if (parameter_value.size() != element_count)
                {
                    cout<<"ERROR: new parameter must have "<<element_count<<" values, but it has "<<parameter_value.size()<<" (old IntArrayType)"<<endl;
                    return -21;
                }
                for (int j = 0; j < element_count; j++)
                {
                    IntValue* pValue = (IntValue*) parameter_value[j];
                    if (pValue->value != i_value[j])
                    {
                        cout<<"ERROR: new parameter must be equal to old value "<<i_value[j]<<" (IntArrayType)"<<endl;
                        return -22;
                    }
                }

                break;
            }
            case DoubleArrayType:
            {
                if (pParameter->GetDcSerial() == NULL)
                {
                    double* d_value = NULL; int element_count;
                    pParameter->GetDoubleArray(d_value, element_count);
                    if (parameter_value.size() != element_count)
                    {
                        cout<<"ERROR: new parameter must have "<<element_count<<" values, but it has "<<parameter_value.size()<<" (old DoubleArrayType)"<<endl;
                        return -23;
                    }
                    for (int j = 0; j < element_count; j++)
                    {
                        DoubleValue* pValue = (DoubleValue*) parameter_value[j];
                        if (pValue->value != d_value[j])
                        {
                            cout<<"ERROR: new parameter must be equal to old value "<<d_value[j]<<" (DoubleArrayType)"<<endl;
                            return -22;
                        }
                    }
                }
                else
                {
                    double* d_value = NULL; int element_count;
                    pParameter->GetDoubleArray(d_value, element_count);

                    bool is_global_found = false;
                    for (int j = 0; j < parameter_value.size(); j++)
                    {
                        MapDVectorValue* pMapValue = (MapDVectorValue*) parameter_value[j];
                        //cout<<"old array size = "<<element_count<<"; new array size = "<<pMapValue->value.size()<<endl;
                        if (pMapValue->value.size() != element_count)
                            continue;

                        //cout<<"old serial = "<<*(pParameter->GetDcSerial())<<"; new serial = "<<pMapValue->serial<<". old channel = "<<*(pParameter->GetChannel())<<"; new channel = "<<pMapValue->channel<<endl;
                        if ((pMapValue->serial != *(pParameter->GetDcSerial())) || (pMapValue->channel != *(pParameter->GetChannel())))
                            continue;

                        bool is_found = true;
                        for (int k = 0; k < element_count; k++)
                        {
                            //cout<<"current old value = "<<d_value[k]<<"; current new value = "<<(pMapValue->value)[k]<<endl;
                            //if (fabs((pMapValue->value)[k] - d_value[k]) > FLT_EPSILON)
                            if ((pMapValue->value)[k] != d_value[k])
                            {
                                is_found = false;
                                break;
                            }
                        }
                        if (is_found)
                        {
                            is_global_found = true;
                            break;
                        }
                    }
                    if (!is_global_found)
                    {
                        cout<<"ERROR: new parameter corresponding to the old value was not found:"<<endl<<"dc_serial = "<<*(pParameter->GetDcSerial())
                            <<". channel = "<<*(pParameter->GetChannel())<<". element_count = "<<element_count<<endl;
                        return -23;
                    }
                }

                break;
            }
            case BinaryArrayType:
            {
                if (pParameter->GetDcSerial() != NULL)
                {
                    cout<<"ERROR: old parameter has dc_serial, but it belongs to BinaryArrayType"<<endl;
                    return -24;
                }

                BinaryValue* pValue = (BinaryValue*) parameter_value[0];
                if (parameter_value.size() != 1)
                {
                    cout<<"ERROR: new parameter must have 1 value, but it has "<<parameter_value.size()<<" (old BinaryArrayType)"<<endl;
                    return -25;
                }

                unsigned char* p_old_bytes = NULL; size_t byte_count;
                if (pParameter->GetBinaryArray(p_old_bytes, byte_count) != 0)
                {
                    cout<<"ERROR: returning binary array was failed for BinaryArrayType"<<endl;
                    return -26;
                }

                if (pValue->size != byte_count)
                {
                    cout<<"ERROR: new parameter must have "<<byte_count<<" values, but it has "<<pValue->size<<" (old BinaryArrayType)"<<endl;
                    return -27;
                }
                for (int j = 0; j < byte_count; j++)
                {
                    if (pValue->value[j] != p_old_bytes[j])
                    {
                        cout<<"ERROR: new parameter must be equal to old value "<<p_old_bytes[j]<<" (BinaryArrayType)"<<endl;
                        return -28;
                    }
                }

                break;
            }
            case UIntArrayType:
            {
                if (pParameter->GetDcSerial() != NULL)
                {
                    cout<<"ERROR: old parameter has dc_serial, but it belongs to UIntArrayType"<<endl;
                    return -29;
                }

                unsigned int* ui_value = NULL; int element_count;
                pParameter->GetUIntArray(ui_value, element_count);
                if (parameter_value.size() != element_count)
                {
                    cout<<"ERROR: new parameter must have "<<element_count<<" values, but it has "<<parameter_value.size()<<" (old UIntArrayType)"<<endl;
                    return -30;
                }
                for (int j = 0; j < element_count; j++)
                {
                    UIntValue* pValue = (UIntValue*) parameter_value[j];
                    if (pValue->value != ui_value[j])
                    {
                        cout<<"ERROR: new parameter must be equal to old value "<<ui_value[j]<<" (UIntArrayType)"<<endl;
                        return -31;
                    }
                }

                break;
            }
            case DchMapArrayType:
            {
                if (pParameter->GetDcSerial() != NULL)
                {
                    cout<<"ERROR: old parameter has dc_serial, but it belongs to DchMapArrayType"<<endl;
                    return -32;
                }

                DchMapStructure* dch_value = NULL; int element_count;
                pParameter->GetDchMapArray(dch_value, element_count);
                if (parameter_value.size() != element_count)
                {
                    cout<<"ERROR: new parameter must have "<<element_count<<" values, but it has "<<parameter_value.size()<<" (old DchMapArrayType)"<<endl;
                    return -33;
                }
                for (int j = 0; j < element_count; j++)
                {
                    DchMapValue* pValue = (DchMapValue*) parameter_value[j];
                    if ((pValue->plane != dch_value[j].plane) || (pValue->group != dch_value[j].group) || (pValue->crate != dch_value[j].crate) ||
                       (pValue->slot != dch_value[j].slot) || (pValue->channel_low != dch_value[j].channel_low) || (pValue->channel_high != dch_value[j].channel_high))
                    {
                        cout<<"ERROR: new parameter must be equal to old value "<<dch_value[j].plane<<" (DchMapArrayType)"<<endl;
                        return -34;
                    }
                }

                break;
            }
            case GemMapArrayType:
            {
                if (pParameter->GetDcSerial() != NULL)
                {
                    cout<<"ERROR: old parameter has dc_serial, but it belongs to GemMapArrayType"<<endl;
                    return -35;
                }

                GemMapStructure* gem_value = NULL; int element_count;
                pParameter->GetGemMapArray(gem_value, element_count);
                if (parameter_value.size() != element_count)
                {
                    cout<<"ERROR: new parameter must have "<<element_count<<" values, but it has "<<parameter_value.size()<<" (old GemMapArrayType)"<<endl;
                    return -36;
                }
                for (int j = 0; j < element_count; j++)
                {
                    GemMapValue* pValue = (GemMapValue*) parameter_value[j];
                    if ((pValue->serial != gem_value[j].serial) || (pValue->id != gem_value[j].id) || (pValue->station != gem_value[j].station) ||
                       (pValue->channel_low != gem_value[j].channel_low) || (pValue->channel_high != gem_value[j].channel_high) || (pValue->hotZone != gem_value[j].hotZone))
                    {
                        cout<<"ERROR: new parameter must be equal to old value "<<gem_value[j].serial<<" (GemMapArrayType)"<<endl;
                        return -37;
                    }
                }

                break;
            }
            case GemPedestalArrayType:
            {
                if (pParameter->GetDcSerial() != NULL)
                {
                    cout<<"ERROR: old parameter has dc_serial, but it belongs to GemPedestalArrayType"<<endl;
                    return -38;
                }

                GemPedestalStructure* gem_ped_value = NULL; int element_count;
                pParameter->GetGemPedestalArray(gem_ped_value, element_count);
                if (parameter_value.size() != element_count)
                {
                    cout<<"ERROR: new parameter must have "<<element_count<<" values, but it has "<<parameter_value.size()<<" (old GemPedestalArrayType)"<<endl;
                    return -39;
                }
                for (int j = 0; j < element_count; j++)
                {
                    GemPedestalValue* pValue = (GemPedestalValue*) parameter_value[j];
                    if ((pValue->serial != gem_ped_value[j].serial) || (pValue->channel != gem_ped_value[j].channel) || (pValue->pedestal != gem_ped_value[j].pedestal) ||
                       (pValue->noise != gem_ped_value[j].noise))
                    {
                        cout<<"ERROR: new parameter must be equal to old value "<<gem_ped_value[j].serial<<" (GemPedestalArrayType)"<<endl;
                        return -40;
                    }
                }

                break;
            }
            case TriggerMapArrayType:
            {
                if (pParameter->GetDcSerial() != NULL)
                {
                    cout<<"ERROR: old parameter has dc_serial, but it belongs to TriggerMapArrayType"<<endl;
                    return -41;
                }

                TriggerMapStructure* trigger_value = NULL; int element_count;
                pParameter->GetTriggerMapArray(trigger_value, element_count);
                if (parameter_value.size() != element_count)
                {
                    cout<<"ERROR: new parameter must have "<<element_count<<" values, but it has "<<parameter_value.size()<<" (old TriggerMapArrayType)"<<endl;
                    return -42;
                }
                for (int j = 0; j < element_count; j++)
                {
                    TriggerMapValue* pValue = (TriggerMapValue*) parameter_value[j];
                    if ((pValue->serial != trigger_value[j].serial) || (pValue->slot != trigger_value[j].slot) || (pValue->channel != trigger_value[j].channel))
                    {
                        cout<<"ERROR: new parameter must be equal to old TriggerMapArrayType value: iter = "<<j<<endl<<"pValue->serial = "<<pValue->serial
                            <<"; trigger_value.serial = "<<trigger_value[j].serial<<". pValue->slot = "<<pValue->slot<<"; trigger_value.slot = "<<trigger_value[j].slot
                            <<". pValue->channel = "<<pValue->channel<<"; trigger_value.channel = "<<trigger_value[j].channel<<endl;
                        return -43;
                    }
                }

                break;
            }
            case LorentzShiftArrayType:
            {
                if (pParameter->GetDcSerial() != NULL)
                {
                    cout<<"ERROR: old parameter has dc_serial, but it belongs to LorentzShiftArrayType"<<endl;
                    return -44;
                }

                LorentzShiftStructure* lorentz_value = NULL; int element_count;
                pParameter->GetLorentzShiftArray(lorentz_value, element_count);
                if (parameter_value.size() != element_count)
                {
                    cout<<"ERROR: new parameter must have "<<element_count<<" values, but it has "<<parameter_value.size()<<" (old LorentzShiftArrayType)"<<endl;
                    return -45;
                }
                for (int j = 0; j < element_count; j++)
                {
                    LorentzShiftValue* pValue = (LorentzShiftValue*) parameter_value[j];
                    if ((pValue->number != lorentz_value[j].number) || (pValue->ls[0] != lorentz_value[j].ls[0]) || (pValue->ls[1] != lorentz_value[j].ls[1]) ||
                       (pValue->ls[2] != lorentz_value[j].ls[2]))
                    {
                        cout<<"ERROR: new parameter must be equal to old value "<<lorentz_value[j].number<<" (LorentzShiftArrayType)"<<endl;
                        return -46;
                    }
                }

                break;
            }
            case ErrorType:
            {
                cout<<"ERROR: ErrorType was unexpectedly found"<<endl;
                return -47;
            }
            default:
            {
                cout<<"ERROR: the parameter type was not defined"<<endl;
                return -48;
            }
        }

        //cout<<"DetectorName = "<<pParameter->GetDetectorName()<<". ParameterName = "<<pParameter->GetParameterName()<<". StartPeriod = "<<pParameter->GetStartPeriod()<<". StartRun = "<<pParameter->GetStartRun()<<". EndPeriod = "<<pParameter->GetEndPeriod()<<". GetEndRun() = "<<pParameter->GetEndRun()<<". parameter_value size = "<<parameter_value.size()<<endl;
        // clean memory for destination array
        for (int j = 0; j < parameter_value.size(); j++)
            delete parameter_value[j];
        parameter_value.clear();
        delete pNewParameter;
    }
    cout<<endl;

    // clean memory after work
    delete pParameterArray;

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);

    cout<<"Macro was successfull"<<endl;

    return 0;
}
