#include <stdio.h>

// macro for writing bytes of root geometry file to database
void set_root_geometry(char* root_file_path, int start_run, int end_run)
{
    gROOT->LoadMacro("$VMCWORKDIR/macro/mpd/mpdloadlibs.C");
    mpdloadlibs(1, 1); // load main libraries

    FILE* root_file = fopen(root_file_path, "rb");
    if (root_file == NULL)
    {
        cout<<"Error: opening root file: "<<root_file_path<<endl;
        exit(-1);
    }

    fseek(root_file, 0, SEEK_END);
    long file_size = ftell(root_file);
    rewind(root_file);
    if (file_size <= 0)
    {
        cout<<"Error: getting file size: "<<root_file_path<<endl;
        fclose(root_file);
        exit(-2);
    }

    unsigned char* buffer = new unsigned char[file_size];
    if (buffer == NULL)
    {
        cout<<"Error: getting memory from heap"<<endl;
        fclose(root_file);
        exit(-3);
    }

    size_t bytes_read = fread(buffer, 1, file_size, root_file);
    if (bytes_read != file_size)
    {
        cout<<"Error: reading file: "<<root_file_path<<", got "<<bytes_read<<" bytes of "<<file_size<<endl;
        delete [] buffer;
        fclose(root_file);
        exit(-4);
    }

    fclose(root_file);

    // set root geometry file's bytes for run range
    int res_code = MpdDbRun::SetRootGeometry(start_run, end_run, buffer, file_size); //(int start_run_number, int end_run_number, unsigned char* root_geometry, Long_t size_root_geometry)

    if (res_code != 0)
    {
        cout << "\nMacro finished with errors" << endl;
        delete [] buffer;
        exit(-5);
    }

    cout << "\nMacro finished successfully" << endl;
    delete [] buffer;
}
