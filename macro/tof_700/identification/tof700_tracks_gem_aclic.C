void tof700_tracks_gem_aclic(int runnum = 1882)
{
  gROOT->ProcessLine(".x $VMCWORKDIR/macro/run/bmnloadlibs.C");
  cout << "Setting include path ..." << endl;
  TString includePath = " ";
  includePath        += "-I${SIMPATH}/include ";
  includePath        += "-I${SIMPATH}/include/root ";
  includePath        += "-I${VMCWORKDIR}/base ";
  includePath        += "-I${VMCWORKDIR}/base/field ";
  includePath        += "-I${VMCWORKDIR}/base/steer ";
  includePath        += "-I${VMCWORKDIR}/base/event ";
  includePath        += "-I${VMCWORKDIR}/fairtools ";
  includePath        += "-I${VMCWORKDIR}/geobase ";
  includePath        += "-I${VMCWORKDIR}/parbase ";
  includePath        += "-I${VMCWORKDIR}/bmndata ";
  includePath        += "-I${VMCWORKDIR}/base/source ";
  includePath        += "-I${VMCWORKDIR}/uni_db/db_classes ";
  includePath        += "-I${VMCWORKDIR}/uni_db ";
  includePath        += "-I${VMCWORKDIR}/bmnfield ";
  includePath        += "-I${VMCWORKDIR}/globaltracking ";
  includePath        += "-I${VMCWORKDIR}/sts ";
  includePath        += "-I${VMCWORKDIR}/tof ";
  includePath        += "-I${VMCWORKDIR}/KF ";
  includePath        += "-I${VMCWORKDIR}/KF/Interface ";
  includePath        += "-Wno-overloaded-virtual -Wno-unused-variable -Wno-deprecated -Wno-unused-but-set-variable -Wno-shadow -Wno-unused-parameter";
  gSystem->SetIncludePath(includePath.Data());
  gROOT->ProcessLine(".L addMomentumToRoot.C++");
  addMomentumToRoot(runnum);
}
