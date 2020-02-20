//run eventdisplay in online mode for simulation data
void run_mc_online()
{
    gROOT->ProcessLine(".x ../eventdisplay.C(\"$VMCWORKDIR/macro/run/bmnsim.root\", \"$VMCWORKDIR/macro/run/bmndst.root\", 0, true)");
}
