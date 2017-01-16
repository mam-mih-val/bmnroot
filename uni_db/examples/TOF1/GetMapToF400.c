void GetMapToF400(Int_t Periud=5, Int_t Run=508) {
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries

     BmnTof1Raw2Digit* tof = new BmnTof1Raw2Digit(Periud, Run);
     tof->saveMapToFile("TOF400_PlaseMap_period5_fromDB.txt","TOF400_StripMap_period5_fromDB.txt");
     tof->~BmnTof1Raw2Digit();

}

