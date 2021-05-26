// macro for adding parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)
void delete_parameter_value()
{
    // set 'on' parameter value (boolean value)
    int res_code = UniDbDetectorParameter::DeleteDetectorParameter(TString("BM@N"), TString("alignment"), 6, 1170, 6, 1992);
    if (res_code != 0)
    {
        cout << "\nMacro finished with errors" << endl;
        return;
    }

    cout << "\nMacro finished successfully" << endl;
}
