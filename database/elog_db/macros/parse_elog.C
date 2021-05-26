// macro for parsing ELOG and getting beam particle, target particle and energy
void parse_elog(int period, TString csvName)
{
    UniParser parser;
    int res_code = parser.ParseElogCsv(period, csvName, ';');

    if (res_code >= 0) cout<<"Macro finished. The count of updated records: "<<res_code<<endl;
    else cout<<"Macro finished with errors"<<endl;
}
