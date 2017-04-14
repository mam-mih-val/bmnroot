// macro for downloading ROOT file with detector alignment from database
void get_root_alignment(char* root_file_path, int period_number, int run_number)
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    int res_code = UniDbDetectorParameter::ReadBmnAlignment(period_number, run_number, root_file_path);
    if (res_code != 0)
    {
        cout << "\nMacro finished with errors" << endl;
        exit(-1);
    }

    cout << "\nMacro finished successfully" << endl;
}
