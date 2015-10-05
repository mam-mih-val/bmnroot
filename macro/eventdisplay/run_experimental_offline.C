//run eventdisplay in offline mode for experimental root data files
void run_experimental_offline()
{
    //eventdisplay.C(char* exp_reco_file = 0, char* geo_sim_file = 0, char* out_file = 0, bool is_online = false, int data_source = 0);
    
    //gROOT->ProcessLine(".x eventdisplay.C(\"$VMCWORKDIR/macro/run/bmn_run0607_digit.root\", \"$VMCWORKDIR/macro/run/geometry_run/evetest_r3.root\", \"tmp.root\", false, 2)");
    gROOT->ProcessLine(".x eventdisplay.C(\"$VMCWORKDIR/macro/run/bmndst_run688.root\", \"$VMCWORKDIR/macro/run/evetest_r3.root\", \"tmp.root\", false, 2)");
}
