//run eventdisplay in offline mode for experimental root data files
void run_exp_offline()
{    
    //gROOT->ProcessLine(".x ../eventdisplay.C(\"$VMCWORKDIR/macro/run/evetest_run3.root\", \"$VMCWORKDIR/macro/run/bmn_run0607_digit.root\", 1, false)");
    //gROOT->ProcessLine(".x ../eventdisplay.C(\"$VMCWORKDIR/macro/run/evetest_run3.root\", \"$VMCWORKDIR/macro/run/bmndst_run688.root\", 1, false)");
    gROOT->ProcessLine(".x ../eventdisplay.C(\"$VMCWORKDIR/macro/run/evetest_run3.root\", \"$VMCWORKDIR/macro/run/bmndst_run688_gl.root\", 1, false)");
}
