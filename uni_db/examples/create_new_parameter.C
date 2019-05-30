// macro for creating new parameter (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)
void create_new_parameter()
{
    // add 'voltage' parameter, for example (double value)
    UniDbParameter* pParameter = UniDbParameter::CreateParameter("BD_global_mapping", TriggerMapArrayType); //(TString parameter_name, int parameter_type)
    if (pParameter == NULL)
    {
        cout << "\nMacro finished with errors" << endl;
        return;
    }

    // clean memory after work
    delete pParameter;

    cout << "\nMacro finished successfully" << endl;
}
