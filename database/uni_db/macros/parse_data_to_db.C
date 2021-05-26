void parse_data_to_db()
{
    TStopwatch timer;
    timer.Start();
    gDebug = 0;

    //(TString fileName, TString schemaPath, bool isUpdate = false)
    UniParser parser;
    //parser.ParseTxt2Db("parse_schemes/0168fdca_PRPC20g_A.dat", "parse_schemes/tdc_inl_txt.xslt");
    //parser.ParseTxt2Db("parse_schemes/zdc_map.txt", "parse_schemes/mapping_zdc_txt.xslt");
    parser.ParseXml2Db("parse_schemes/run_info.xml", "parse_schemes/run_info_xml.xslt");
    //parser.ParseCsv2Db("parse_schemes/run3_log.csv", "parse_schemes/run_info_csv.xslt", true);
    //parser.ParseTxtNoise2Db("parse_schemes/noise_run1_2.txt", "parse_schemes/noise_txt.xslt");
    //parser.ParseDb2Db();
    //parser.ConvertElogCsv("parse_schemes/elog.csv", ';');

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("\nRealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
}
