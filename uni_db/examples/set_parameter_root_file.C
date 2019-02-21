// macro for writing ROOT file with detector parameter (e.g BM@N alignment, lorentz shift) to the database
// e.g. root 'set_parameter_root_file.C("$VMCWORKDIR/input/align.root", 6, 1170, 6, 1177)'
void set_parameter_root_file(char* root_file_path, int start_period, int start_run, int end_period, int end_run)
{
    // write ROOT file with detector parameter for run range
    // int res_code = UniDbDetectorParameter::WriteRootFile(start_period, start_run, end_period, end_run, "BM@N", "lorentz_shift", root_file_path);
    int res_code = UniDbDetectorParameter::WriteRootFile(start_period, start_run, end_period, end_run, "BM@N", "alignment", root_file_path);
    if (res_code != 0)
    {
        cout << "\nMacro finished with errors" << endl;
        exit(-1);
    }

    cout << "\nMacro finished successfully" << endl;
}
