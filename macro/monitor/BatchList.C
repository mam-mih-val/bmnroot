void BatchList(TString dirName = "/bmn/run/current/") {
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();
    //    struct dirent **namelist;
    TString flist[1024];

    //    Int_t n = scandir(dirname, &namelist, 0, versionsort);
    //    if (n < 0) {
    //        perror("scandir");
    //        return;
    //    } else {
    //        for (Int_t i = 0; i < n; ++i) {
    for (Int_t i = 543; i < 1015; i++) {
        //            if (TString(namelist[i]->d_name, re).EndsWith("data")) {
        //                flist.pushback(namelist[i]->d_name);
        TString name = Form("mpd_run_Glob_%d.data", i);
        flist[0] = dirName + name;
        BmnMonitor *bm = new BmnMonitor();
        bm->BatchList(flist, 1);
        delete bm;
    }
    //            free(namelist[i]);
    //        }
    //        free(namelist);
    //    }
}
