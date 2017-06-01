void GetMapToF400(Int_t Periud=6, Int_t Run=1985) {
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries

     BmnTof1Raw2Digit* tof = new BmnTof1Raw2Digit(Periud, Run);
     tof->saveMapToFile("TOF400_PlaseMap_period6_fromDB.txt","TOF400_StripMap_period6_fromDB.txt");
     tof->~BmnTof1Raw2Digit();

}

