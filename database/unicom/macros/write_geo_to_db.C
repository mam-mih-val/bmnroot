// Macro for writing detector geometry from .root file to DataBase
void write_geo_to_db(TString geo_file = "$VMCWORKDIR/macro/mpd/geofile_full.root")
{
    TStopwatch timer;
    timer.Start();
    gDebug = 0;

    int res = UniGeoConverter::WriteRootGeoToDB(geo_file);

    if (res == 0)
        cout<<"Writing the detector geometry to the database has completed successfully"<<endl;
    else
        cout<<"Writing the detector geometry to the database has completed with code: "<<res<<endl;

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);

    //cout << "Macro finished succesfully." << endl;
}
