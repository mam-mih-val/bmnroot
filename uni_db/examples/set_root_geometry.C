#include <stdio.h>
// macro for writing ROOT file with detector geometry to the database
// e.g. root 'set_root_geometry.C("$VMCWORKDIR/macro/run/geometry_run/geometry_run5.root", 5, 1, 5, 10000)'
void set_root_geometry(char* root_file_path, int start_period, int start_run, int end_period, int end_run)
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    // write ROOT file with detector geometry for run range
    int res_code = UniDbRun::WriteGeometryFile(start_period, start_run, end_period, end_run, root_file_path); //(int start_period, int start_run, int end_period, int end_run, char* geo_file_path)
    if (res_code != 0)
    {
        cout << "\nMacro finished with errors" << endl;
        exit(-1);
    }

    cout << "\nMacro finished successfully" << endl;
}
