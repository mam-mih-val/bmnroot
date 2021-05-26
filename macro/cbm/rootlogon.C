/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id: rootlogon.C,v 1.4 2007/05/04 11:18:51 ivana Exp $ */
R__ADD_INCLUDE_PATH($VMCWORKDIR)
//R__ADD_INCLUDE_PATH($ROOT_INCLUDE_DIR)
R__ADD_INCLUDE_PATH($VMCWORKDIR/base)
R__ADD_INCLUDE_PATH($VMCWORKDIR/base/field)
R__ADD_INCLUDE_PATH($VMCWORKDIR/steer)
R__ADD_INCLUDE_PATH($VMCWORKDIR/event)
R__ADD_INCLUDE_PATH($VMCWORKDIR/fairtools)
R__ADD_INCLUDE_PATH($VMCWORKDIR/geobase)
R__ADD_INCLUDE_PATH($VMCWORKDIR/parbase)
R__ADD_INCLUDE_PATH($VMCWORKDIR/bmnbase)
R__ADD_INCLUDE_PATH($VMCWORKDIR/bmndata)
R__ADD_INCLUDE_PATH($VMCWORKDIR/bmndata/cbm)
R__ADD_INCLUDE_PATH($VMCWORKDIR/KF)
R__ADD_INCLUDE_PATH($VMCWORKDIR/KF/Interface)
R__ADD_INCLUDE_PATH($VMCWORKDIR/KF/KFQA)
R__ADD_INCLUDE_PATH($VMCWORKDIR/base/sim)
R__ADD_INCLUDE_PATH($VMCWORKDIR/cat/OffLineInterface)
R__ADD_INCLUDE_PATH($VMCWORKDIR/sts)
R__ADD_INCLUDE_PATH($VMCWORKDIR/base/source)
R__ADD_INCLUDE_PATH($VMCWORKDIR/bmnfield)
R__ADD_INCLUDE_PATH($VMCWORKDIR/cat)
R__ADD_INCLUDE_PATH($VMCWORKDIR/database/unicom)
R__ADD_INCLUDE_PATH($VMCWORKDIR/database/uni_db)


//R__ADD_LIBRARY_PATH($VMCWORKDIR/build/lib)
//R__LOAD_LIBRARY($VMCWORKDIR/build/lib/libBmnData.so)
//R__LOAD_LIBRARY($VMCWORKDIR/build/lib/libSts.so)

R__ADD_INCLUDE_PATH($FAIRROOTPATH)
R__ADD_INCLUDE_PATH($FAIRROOTPATH/include)
//R__LOAD_LIBRARY($VMCWORKDIR/build/lib/libBmnData.so)
//R__LOAD_LIBRARY($VMCWORKDIR/build/lib/libKF.so)
//R__LOAD_LIBRARY($VMCWORKDIR/build/lib/libSts.so)
/*
"-I/cvmfs/hybrilit.jinr.ru/sw/slc7_x86-64/FAIR/fairsoft/install/include/root
 -I"/cvmfs/hybrilit.jinr.ru/sw/slc7_x86-64/FAIR/fairroot/install/include" 
 -I"/cvmfs/hybrilit.jinr.ru/sw/slc7_x86-64/FAIR/fairsoft/install/share/root/etc" 
 -I"/cvmfs/hybrilit.jinr.ru/sw/slc7_x86-64/FAIR/fairsoft/install/share/root/etc/cling" 
 -I"/cvmfs/hybrilit.jinr.ru/sw/slc7_x86-64/FAIR/fairsoft/install/include/root"
  -I"/usr/lib/gcc/x86_64-redhat-linux/4.8.5/../../../../include/c++/4.8.5" 
  -I"/usr/lib/gcc/x86_64-redhat-linux/4.8.5/../../../../include/c++/4.8.5/x86_64-redhat-linux"
   -I"/usr/lib/gcc/x86_64-redhat-linux/4.8.5/../../../../include/c++/4.8.5/backward"
    -I"/cvmfs/hybrilit.jinr.ru/sw/slc7_x86-64/FAIR/fairsoft/install/include""
*/


//#include "macro/run/bmnloadlibs.C"

void rootlogon(){
   // cout << "Loading MPD libraries ..." << endl;
   // gROOT->LoadMacro("${ALICE_ROOT}/MUON/loadlibs.C");
   // gInterpreter->ProcessLine("loadlibs()");

//TString ROOT_DIR = getenv("ROOT_INCLUDE_DIR");
//TString VMCWORKIDR = getenv("VMCWORKDIR");
//TString FAIRROOTPATH = getenv("FAIRROOTPATH");

   cout << "Setting include path ..." << endl;
   
  // bmnloadlibs();
   
  /* TString includePath = "-I"+ROOT_DIR+" "; //dosent work on root 6+, coz path have to -I + ""
   includePath += "-I"+VMCWORKIDR+"/base ";
   includePath += "-I"+VMCWORKIDR+"/base/field ";
   includePath += "-I"+VMCWORKIDR+"/base/steer ";
   includePath += "-I"+VMCWORKIDR+"/base/event ";
   includePath += "-I"+VMCWORKIDR+"/fairtools ";
   includePath += "-I"+VMCWORKIDR+"/geobase ";
   includePath += "-I"+VMCWORKIDR+"/parbase ";
   includePath += "-I"+VMCWORKIDR+"/bmnbase ";
   includePath += "-I"+VMCWORKIDR+"/bmndata ";
   includePath += "-I"+VMCWORKIDR+"/bmndata/cbm ";
   includePath += "-I"+VMCWORKIDR+"/base/source ";
   //includePath += "-I${VMCWORKDIR}/database/uni_db ";
   //includePath += "-I${VMCWORKDIR}/database/unicom ";
   includePath += "-I"+VMCWORKIDR+"/bmnfield ";
   includePath += "-I"+VMCWORKIDR+"/cat ";
   includePath += "-I"+VMCWORKIDR+"/KF ";
   includePath += "-I"+VMCWORKIDR+"/KF/KFQA ";
   includePath += "-I"+VMCWORKIDR+"/base/sim ";
   includePath += "-I"+VMCWORKIDR+"/cat/OffLineInterface ";
   // includePath        += "-I${VMCWORKDIR}/cbmdata/much ";
   // includePath        += "-I${VMCWORKDIR}/cbmdata/sts ";
   // includePath        += "-I${VMCWORKDIR}/cbmdata/global ";
   // includePath        += "-I${VMCWORKDIR}/cbmdata/tof ";
   // includePath        += "-I${VMCWORKDIR}/littrack ";
   // includePath        += "-I${VMCWORKDIR}/littrack/base ";
   // includePath        += "-I${VMCWORKDIR}/much ";
   // includePath        += "-I${VMCWORKDIR}/much/geo ";
   includePath += "-I"+VMCWORKIDR+"/sts ";
   // includePath        += "-I${VMCWORKDIR}/stt ";
   includePath += "-I"+VMCWORKIDR+"/tof ";
   // includePath        += "-I${VMCWORKDIR}/trd ";
   includePath += "-I"+VMCWORKIDR+"/KF ";
   includePath += "-I"+VMCWORKIDR+"/KF/Interface ";
   includePath += "-I"+FAIRROOTPATH+"/include ";
   includePath += "-Wno-unused-variable -Wno-deprecated "
                  "-Wno-shadow -Wno-unused-parameter -Wmissing-field-initializers -Wno-overloaded-virtual ";
   //gSystem->SetIncludePath(includePath.Data());
   gInterpreter->AddIncludePath(includePath.Data());
   // gSystem->AddLinkedLibs("-lgomp");
*/
   //  gROOT->ProcessLine(".x ~/bmon/loadlibs.C");
   //  gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
   //gROOT->ProcessLine(".x $VMCWORKDIR/macro/run/bmnloadlibs.C");

   // Debug option
   //gSystem->SetFlagsDebug("-gdwarf-2");
   gSystem->SetFlagsOpt("-std=c++11");

   gStyle->SetCanvasColor(0);
   gStyle->SetCanvasBorderMode(0);
   gStyle->SetFrameFillColor(0);
   gStyle->SetFrameBorderMode(0);


  // gStyle->SetGridWidth(2);
//gStyle->SetGridStyle(20);
// gStyle->SetLineStyleString(7,"20 20");
//gStyle->SetLineStyleString(2,"4 8");


   gStyle->SetTextFont(62);
   gStyle->SetTitleFont(62, "XYZ");
   gStyle->SetTitleFont(62, "A");
   gStyle->SetLabelFont(62, "XYZ");
   // gStyle->SetLegendFont(62);
   gStyle->SetLegendFont(132); // Times
   gStyle->SetStatFont(62);

   gStyle->SetLabelSize(0.06, "XYZ");
   gStyle->SetTitleSize(0.06, "XYZ");
   // gStyle->SetLabelSize(0.05,"XYZ");
   // gStyle->SetTitleSize(0.05,"XYZ");
   gStyle->SetTitleXOffset(0.94);
   gStyle->SetTitleYOffset(0.96);

   gStyle->SetTitleStyle(1001);
   gStyle->SetTitleBorderSize(2);
   gStyle->SetTitleFillColor(19);
   gStyle->SetTitleAlign(13);
   gStyle->SetTitleX(0.01);

   gStyle->SetStatColor(19);

   gStyle->SetPadBottomMargin(0.14);
   gStyle->SetPadTopMargin(0.04);
   gStyle->SetPadLeftMargin(0.14);
   gStyle->SetPadRightMargin(0.04);
   // gStyle->SetCanvasDefH(467);
   // gStyle->SetCanvasDefW(712);
   gStyle->SetCanvasDefH(500);
   gStyle->SetCanvasDefW(720);

      gStyle->SetPadGridX(kTRUE);
   gStyle->SetPadGridY(kTRUE);
   /*
   Double_t w = 600;
   Double_t h = 600;
   TCanvas * c1 = new TCanvas("c", "c", w, h);
   c->SetWindowSize(w + (w - c->GetWw()), h + (h - c->GetWh()));
   c->SetWindowSize(712 + (712 - c->GetWw()), 467 + (467 - c->GetWh()));
   c->SetWindowSize(720 + (720 - c->GetWw()), 500 + (500 - c->GetWh()));
   */
   // TCanvas a("a");
   // TCanvas *c= &a;
   // c->SetWindowSize(720 + (720 - c->GetWw()), 500 + (500 - c->GetWh()));

   gROOT->ForceStyle();
}
