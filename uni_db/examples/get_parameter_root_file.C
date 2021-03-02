// macro for getting parameter storing as ROOT file (e.g. detector alignment, lorentz shift) from the database
void get_parameter_root_file(char* root_file_path, int period_number, int run_number)
{
    int res_code = UniDbDetectorParameter::ReadFile("BM@N", "alignment", period_number, run_number, root_file_path);
    if (res_code != 0)
    {
        cout << "\nMacro finished with errors" << endl;
        exit(-1);
    }

    cout << "\nMacro finished successfully" << endl;
}
