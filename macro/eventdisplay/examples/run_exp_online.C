//run eventdisplay in offline mode for raw root file
void run_exp_online()
{
    //gROOT->ProcessLine(".x ../eventdisplay.C(\"$VMCWORKDIR/macro/run/evetest_run3.root\", \"$VMCWORKDIR/macro/run/bmn_run0607_digit.root\", 1, true)");
    gROOT->ProcessLine(".x ../eventdisplay.C(\"$VMCWORKDIR/macro/run/evetest_run3.root\", \"$VMCWORKDIR/macro/run/bmndst_run688.root\", 1, true)");
}
