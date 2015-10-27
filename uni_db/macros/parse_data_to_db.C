void parse_data_to_db()
{
    TStopwatch timer;
    timer.Start();
    gDebug = 0;

    gSystem->Load("/usr/lib/x86_64-linux-gnu/libpugixml");

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load main libraries

    UniDbParser pars;
    //(TString fileName, TString schemaPath, bool isUpdate = false)
    pars.ParseTxt2Db("parse_schemes/0168fdca_PRPC20g_A.dat", "parse_schemes/tdc_inl_txt.xslt");
    //pars.ParseTxt2Db("parse_schemes/zdc_map.txt", "parse_schemes/mapping_zdc_txt.xslt");
    //pars.ParseXml2Db("parse_schemes/run_info.xml", "parse_schemes/run_info_xml.xslt");
    //pars.ParseCsv2Db("parse_schemes/run3_log.csv", "parse_schemes/run_info_csv.xslt", true);

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("\nRealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
}
