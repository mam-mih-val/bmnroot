#include <utility>
#include <map>
#include <vector>
#include <TDatime.h>
#include <TClonesArray.h>
#include <TH1D.h>
#include <TCanvas.h>

using namespace std;
/** flux ~= sum [BT * (DAQ_Busy -peds)/ (DAQ_TRigger - peds)]*/

void Draw2Can(TCanvas *c, TH1* h1, TH1* h2 = nullptr) {
    c->cd();
    h1->Draw();
    if (h2) {
        h2->Draw("same");
        Double_t k = max(h1->GetMaximum(), h2->GetMaximum());
        h1->SetMaximum(1.05 * k);
        h1->SetMinimum(0);
    }
}

void Spill(int period = 7, TString beam = "C", TString target = "",
        //        TString digi_path = "/ncx/eos/nica/bmn/exp/digi/run7/20.02.0/3590-4707_BMN_Argon/",
        TString digi_path = "/ncx/scratch1/ilnur/mini_release/",
                TString txtfile_path = "$VMCWORKDIR/database/uni_db/macros/parse_schemes/spill_run7/SRC_Data.txt",
                TString scheme_path = "$VMCWORKDIR/database/uni_db/macros/parse_schemes/spill_run7/spill_run7_src_full.xslt") {
//        TString txtfile_path = "$VMCWORKDIR/database/uni_db/macros/parse_schemes/spill_run7/summary_corr_v2.txt",
//        TString scheme_path = "$VMCWORKDIR/database/uni_db/macros/parse_schemes/spill_run7/spill_run7_bmn_full.xslt") {

    Int_t nPedestals = 100;
    Int_t run = 0;
    Int_t HW = 1920;
    Int_t HH = 1080;
    Int_t log_shift = 6;
    Int_t log_shift_src = -3;
    map<TDatime, vector < Int_t>> spill_map;
    map<Int_t, vector < Int_t>> run_map;
    // get spill info
    UniParser parser;
    vector<structParseValue*> parse_values;
    vector<structParseSchema> vecElements;
    int res_code = parser.ParseTxt2Struct(txtfile_path, scheme_path, parse_values, vecElements);
    if (res_code != 0) {
        cout << endl << "Macro finished with errors: parser error (" << res_code << ")" << endl;
        exit(-1);
    }

    if (parse_values.size() < 1) {
        cout << endl << "Macro finished with errors: there are no lines to parse" << endl;
        exit(-2);
    }

    UniqueRunNumber* run_numbers = NULL;
    TObjArray* pRunArray = NULL;
    int run_count = 1;
    // select only with a given condition, e.g. target particle
    TObjArray arrayConditions;
    arrayConditions.SetOwner(kTRUE);

    UniSearchCondition* searchCondition = new UniSearchCondition(columnPeriodNumber, conditionEqual, period);
    arrayConditions.Add((TObject*) searchCondition);
    if (target != "") {
        searchCondition = new UniSearchCondition(columnTargetParticle, conditionEqual, target); // one can choose any condition: beam, energy...
        arrayConditions.Add((TObject*) searchCondition);
    }
    if (beam != "") {
        searchCondition = new UniSearchCondition(columnBeamParticle, conditionEqual, beam);
        arrayConditions.Add((TObject*) searchCondition);
    }
    pRunArray = UniRun::Search(arrayConditions);
    run_count = pRunArray->GetEntriesFast();
    if (run_count <= 0) {
        cout << endl << "Macro finished with errors: no runs exists for the given target: " << target << endl;
        exit(-4);
    }
    arrayConditions.Delete();

    // Create histograms
    printf("len %d\n", pRunArray->GetEntries());
    Int_t first_run_id =(reinterpret_cast<UniRun*> (pRunArray->First()))->GetRunNumber(); //4635;
    Int_t last_run_id = (reinterpret_cast<UniRun*> (pRunArray->Last()))->GetRunNumber(); //4680;
    printf("range %d - %d\n", first_run_id, last_run_id);
    Int_t nbins = (last_run_id + 1 - first_run_id);
    TCanvas * can = new TCanvas("runs_btnb", "Run vs BT\\& not Busy", HW, HH);
    TH1* hBT = new TH1I("hBT", "BT", nbins, first_run_id, last_run_id + 1);
    TH1* hBTnB = new TH1I("hBTnB", "BT & notBusy", nbins, first_run_id, last_run_id + 1);
    TH1* hBTAc = new TH1D("hBTAc", "BT Accepted", nbins, first_run_id, last_run_id + 1);
    //    TH1* hAcc = new TH1I("hAcc", "Accepted", nbins, first_run_id, last_run_id + 1);
    hBT->SetLineColor(kRed);
    hBTnB->SetLineColor(kRed);
    hBTAc->SetLineColor(kRed);
    //    hAcc->SetLineColor(kRed);
    vector <TH1*> hists;
    hists.push_back(hBT);
    hists.push_back(hBTnB);
    hists.push_back(hBTAc);

    TH1I* hBT_log = new TH1I("hBT_log", "BT", nbins, first_run_id, last_run_id + 1);
    TH1* hBTnB_log = new TH1I("hBTnB_log", "BT & notBusy", nbins, first_run_id, last_run_id + 1);
    TH1D* hBTAc_log = new TH1D("hBTAc_log", "BT Accepted", nbins, first_run_id, last_run_id + 1);
    //    TH1* hAcc_log = new TH1I("hAcc_log", "Accepted", nbins, first_run_id, last_run_id + 1);
    TH1* hBC1H_log = new TH1I("hBC1H_log", "BC1 High", nbins, first_run_id, last_run_id + 1);
    TH1* hBC1BP_log = new TH1I("hBC1BP_log", "BC1 Before Protection", nbins, first_run_id, last_run_id + 1);
    TH1* hBC1xBC2_log = new TH1I("hBC1xBC2_log", "BC1 * BC2", nbins, first_run_id, last_run_id + 1);
    TH1* hBC1nBusy_log = new TH1I("hBC1nBusy_log", "BC1 * !Busy", nbins, first_run_id, last_run_id + 1);
    TH1* hIT_log = new TH1I("hIT_log", "Interaction Trigger", nbins, first_run_id, last_run_id + 1);
    TH1* hSRC_log = new TH1I("hSRC_log", "SRC Trigger", nbins, first_run_id, last_run_id + 1);
    TH1* hTrignBusy_log = new TH1I("hTrignBusy_log", "Trigger * !Busy", nbins, first_run_id, last_run_id + 1);
    vector <TH1*> hists_log;
    //    hists_log.push_back(hBT_log);
    //    hists_log.push_back(hBTnB_log);
    //    hists_log.push_back(hBTAc_log);
    hists_log.push_back(hBC1H_log);
    //    hists_log.push_back(hBC1BP_log);
    //    hists_log.push_back(hBC1xBC2_log);
    //    hists_log.push_back(hBC1nBusy_log);
    hists_log.push_back(hIT_log);
    hists_log.push_back(hSRC_log);
        hists_log.push_back(hTrignBusy_log);


    int sum_size = parse_values[0]->arrValues.size();

    for (int ind = 0; ind < parse_values.size(); ind++) {
        structParseValue* st = parse_values.at(ind);
        vector<Int_t> vals;
        for (Int_t iValue = 0; iValue < sum_size; iValue++)
            vals.push_back(boost::any_cast<Int_t>(st->arrValues[iValue]));
        st->dtSpillEnd.Set(st->dtSpillEnd.Convert() - log_shift_src);
        spill_map.insert(pair<TDatime, vector < Int_t >> (st->dtSpillEnd, vals));
    }

    vector<Long64_t> total_columns;
    for (int i_total = 0; i_total < sum_size; i_total++)
        total_columns.push_back(0);

    // cycle for all runs to sum the spill data
    for (int i = 0; i < run_count; i++) {
        if (run_numbers != NULL) run = run_numbers[i].run_number;
        if (pRunArray != NULL) run = ((UniRun*) pRunArray->At(i))->GetRunNumber();

        // get run time
        UniRun* pRun = UniRun::GetRun(period, run);
        if (pRun == NULL) {
            cout << endl << "Macro finished with errors: no experimental run was found " << period << " : " << run << " / " << ((UniRun*) pRunArray->At(i))->GetRunNumber() << endl;
            if (run_numbers != NULL)
                delete [] run_numbers;
            if (pRunArray != NULL)
                delete pRunArray;
            exit(-5);
        }

        TDatime dtStart = pRun->GetStartDatetime();
        TDatime* dateEnd = pRun->GetEndDatetime();
        if (dateEnd == NULL) {
            cout << "Macro finished with errors: no end datetime in the database for this run" << endl;
            delete pRun;
            if (run_numbers != NULL)
                delete [] run_numbers;
            if (pRunArray != NULL)
                delete pRunArray;
            exit(-6);
        }
        TDatime dtEnd = *dateEnd;
        delete pRun;

        vector<Long64_t> sum_columns;
        for (int i_sum = 0; i_sum < sum_size; i_sum++)
            sum_columns.push_back(0);

        if (!((run >= first_run_id) && (run <= last_run_id)))
            continue;
        // check for presence in ELOG
        TObjArray* recs = ElogRecord::GetRecords(period, run);
        if (recs == NULL) {
            fprintf(stderr, "ELOG Error!\n");
            continue;
        } else
            if (recs->GetEntries() == 0) {
            fprintf(stderr, "Run %d not found in ELOG!\n", run);
            //        return NULL;
            continue;
        }
        printf("Run %d  %s", run, dtStart.AsSQLString());
        printf(" - %s\n", dtEnd.AsSQLString());
        Double_t bt_acc = 0;
        vector<Int_t> preVals;
        Bool_t isRepeat;
        preVals.resize(20, 0);
        for (auto it = spill_map.lower_bound(dtStart); it != spill_map.upper_bound(dtEnd); it++) {

            printf(" spill  %s\n", it->first.AsSQLString());
            vector<Int_t> v = it->second;
            isRepeat = kTRUE;
            for (Int_t i = 0; i < v.size() - 1; i++)
                if ((preVals[i] != v[i]))
                    isRepeat = kFALSE;
            if (isRepeat)
                printf("\n\tALARM!\n\n");
            preVals = v;
            Long64_t acc = v[13];
            Long64_t bp = v[17];
            Long64_t ap = v[18];
            Long64_t bt = v[6];
            Long64_t bt_notBusy = v[16];
            if (acc)
                bt_acc += bt_notBusy * (acc - nPedestals) / ((Double_t) (acc - nPedestals + bp + ap));
            //            printf("\t %s   bt= %05lld bt_notBusy= %05lld acc = %05lld bt_acc = %05.3f\n",
            //                    it->first.AsSQLString(), bt, bt_notBusy, acc, bt_acc);

            int count_column = 0;
            for (vector<structParseSchema>::iterator it = vecElements.begin(); it != vecElements.end(); ++it) {
                if (count_column >= sum_size) break;

                structParseSchema schema = *it;
                if (schema.isSkip)
                    continue;

                if (schema.vecRows[0].strStatementType == "int") {
                    if (count_column > 0) cout << ", ";
                    cout << "\tsum(" << schema.vecRows[0].strColumnName << "): " << v[count_column];
                    count_column++;
                } else {
                    if (schema.vecRows[0].strStatementType == "double") {
                        cout << "ERROR: type of the column is not supported: " << schema.vecRows[0].strStatementType << endl;
                        exit(-7);
                    }
                }
            }
            cout << endl << endl;
            for (int i_sum = 0; i_sum < sum_size; i_sum++)
                sum_columns[i_sum] += v[i_sum];
        }
        //        printf("\tsum  BT = %lld\n", sum_columns[6]);
        Long64_t bt_sum = sum_columns[6];
        Long64_t bt_notBusy_sum = sum_columns[16];
        Long64_t acc_sum = sum_columns[13];
        printf("\tSum bt= %05lld bt_notBusy= %05lld acc = %05lld bt_acc = %05.3f\n\n",
                bt_sum, bt_notBusy_sum, acc_sum, bt_acc);
        Int_t iBin = hBT_log->GetXaxis()->FindBin(run);
        Double_t bc = hBT_log->GetBinCenter(iBin);
        //        printf("bin %d  center %f bt_sum %lld bt_notBusy_sum %lld  bt_acc %f\n",
        //                iBin, bc, bt_sum, bt_notBusy_sum, bt_acc);
        hBT_log->SetBinContent(iBin, bt_sum);
        hBTnB_log->SetBinContent(iBin, bt_notBusy_sum);
        hBTAc_log->SetBinContent(iBin, bt_acc);
        //        hAcc_log->SetBinContent(iBin, acc_sum);
        hBC1H_log->SetBinContent(iBin, sum_columns[3]);
        //        hBC1BP_log->SetBinContent(iBin, sum_columns[]);
        //        hBC1xBC2_log->SetBinContent(iBin, sum_columns[]);
        //        hBC1nBusy_log->SetBinContent(iBin, sum_columns[]);
        hIT_log->SetBinContent(iBin, sum_columns[18]);
        hSRC_log->SetBinContent(iBin, sum_columns[19]);
        //        hTrignBusy_log->SetBinContent(iBin, sum_columns[7]);

        //        TString fileName = Form("%s/bmn_run%d_digi.root", digi_path.Data(), run);
        //        TFile *file = new TFile(fileName, "READ");
        //        if (
        //                (file->IsOpen() == kTRUE) &&
        //                (file->IsZombie() == kFALSE) &&
        //                (file->TestBit(TFile::kRecovered) == kFALSE)) {
        //            printf("%s is ok\n", fileName.Data());
        //            DigiRunHeader * rh = static_cast<DigiRunHeader*> (file->Get("DigiRunHeader"));
        //            Int_t iBin = hBT_log->GetXaxis()->FindBin(run);
        //            hBT->SetBinContent(iBin, rh->GetBT());
        //            hBTnB->SetBinContent(iBin, rh->GetBTnBusy());
        //            hBTAc->SetBinContent(iBin, rh->GetBTAccepted());
        //            printf("\tbt= %u bt_notBusy= %u bt_acc = %05u\n", rh->GetBT(), rh->GetBTnBusy(), rh->GetBTAccepted());
        //            //        hAcc->SetBinContent(iBin, rh->Get());
        //        }
    }



    //    for (Int_t run = first_run_id; run <= last_run_id; run++) {
    //        TString fileName = Form("%s/bmn_run%d_digi.root", digi_path.Data(), run);
    //        TFile *file = new TFile(fileName, "READ");
    //        if (
    //                (file->IsOpen() == kTRUE) &&
    //                (file->IsZombie() == kFALSE) &&
    //                (file->TestBit(TFile::kRecovered) == kFALSE)) {
    //            printf("%s is ok\n", fileName.Data());
    //            DigiRunHeader * rh = static_cast<DigiRunHeader*> (file->Get("DigiRunHeader"));
    //            Int_t iBin = hBT_log->GetXaxis()->FindBin(run);
    //            hBT->SetBinContent(iBin, rh->GetBT());
    //            hBTnB->SetBinContent(iBin, rh->GetBTnBusy());
    //            hBTAc->SetBinContent(iBin, rh->GetBTAccepted());
    ////            printf("\tbt_notBusy= %u bt_acc = %05u\n", rh->GetBTnBusy(), rh->GetBTAccepted());
    //            //        hAcc->SetBinContent(iBin, rh->Get());
    //
    //
    //
    //        }
    //
    //    }

    TString plotName = "spill.pdf";
    for (Int_t i = 0; i < hists_log.size(); i++) {
        Draw2Can(can, hists_log[i], (i < hists.size()) ? hists[i] : nullptr);
        can->SaveAs(Form("%s.png", hists_log[i]->GetName()));
    }

    //    Draw2Can(can, hBT_log, hBT);
    //    can->SaveAs("counter_bt.png");
    //    //can->Print(plotName + "(", "pdf");
    //
    //    Draw2Can(can, hBTnB_log, hBTnB);
    //    can->SaveAs("counter_bt_nb.png");
    //    //can->Print(plotName, "pdf");
    //
    //    Draw2Can(can, hBTAc_log, hBTAc);
    //    can->SaveAs("counter_bt_acc.png");
    //    //can->Print(plotName + ")", "pdf");

    // cleaning memory after work
    if (run_numbers != NULL)
        delete [] run_numbers;
    if (pRunArray != NULL)
        delete pRunArray;
    for (auto &h : hists)
        delete h;
    for (auto &h : hists_log)
        delete h;
    //    delete hBT;
    //    delete hBTnB;
    //    delete hBTAc;
    ////    delete hAcc;
    //    delete hBT_log;
    //    delete hBTnB_log;
    //    delete hBTAc_log;
    ////    delete hAcc_log;


    cout << "Macro finished successfully" << endl;
}
