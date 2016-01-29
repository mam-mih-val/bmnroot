/*************************************************************************************
*  Example by: Elena Litvinenko
*  e-mail:     litvin@nf.jinr.ru
*  Version:    29-Jan-2016
************************************************************************************/
  
void extract_zdc_histo_from_reco (const char *filename="reco_zdc.root")
{

  Double_t x,y,ee;  

  TFile *f=new TFile (filename);

  TH2F*  hist_zdc_orig=(TH2F*)f->Get("HistZdcEn");

  TChain chain ("cbmsim");
  chain.Add(filename);
  TChain *t=&chain;  
  Int_t n_events = t->GetEntries();

  TClonesArray *ELossZdcHisto_arr=0;
  t->SetBranchAddress("ELossZdcHisto",&ELossZdcHisto_arr);  

  TString main_title_zdc1=hist_zdc_orig->GetTitle();

  Int_t n_entries=t->GetEntries();
  if (n_entries>8)
    n_entries=4;

  for (int i=0;i<n_entries;i++) {

    t->GetEntry(i); 

    TH2F *hist_zdc = hist_zdc_orig->Clone("hist_zdc");

    TVectorT<float> &v1=*((TVectorT<float>*)ELossZdcHisto_arr->At(0));
    hist_zdc->Set(v1.GetNoElements(),v1.GetMatrixArray());
    hist_zdc->SetEntries(v1.GetNoElements());
    hist_zdc->SetTitle(Form("%s (event %d)",main_title_zdc1.Data(),i));
    cout << hist_zdc->GetTitle() << " " << hist_zdc->GetSum() << endl;
     
    if (i<8)
       new TCanvas(); 
    hist_zdc->Draw("colz");
  }
}
