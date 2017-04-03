// macro for writing ROOT file with BM@N alignment to the database
// e.g. root 'set_root_alignment.C("$VMCWORKDIR/input/align.root", 6, 1170, 6, 1177)'
void set_root_alignment(char* root_file_path, int start_period, int start_run, int end_period, int end_run)
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    // write ROOT file with detector alignment for run range
    int res_code = UniDbDetectorParameter::WriteBmnAlignment(start_period, start_run, end_period, end_run, root_file_path);
    if (res_code != 0)
    {
        cout << "\nMacro finished with errors" << endl;
        exit(-1);
    }

    cout << "\nMacro finished successfully" << endl;
}
