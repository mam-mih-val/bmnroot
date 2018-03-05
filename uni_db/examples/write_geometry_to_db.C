#include <stdio.h>
#include "../../gconfig/basiclibs.C"

// macro for writing ROOT file with full detector geometry to the Unified Database
// e.g. root 'write_geometry_to_db.C("$VMCWORKDIR/macro/run/geometry_run/geometry_src_run7.root", 7, 1, 7, 10000)'
void write_geometry_to_db(const char* root_file_path, int start_period, int start_run, int end_period, int end_run)
{
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
