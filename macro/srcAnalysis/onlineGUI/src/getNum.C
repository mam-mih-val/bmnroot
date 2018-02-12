void getNum(TString file_curr, TString file_ref){
	TChain* rootTree_curr = new TChain("cbmsim");
	rootTree_curr->Add(file_curr);
	Int_t event_count_curr = rootTree_curr->GetEntries();

	TChain* rootTree_ref = new TChain("cbmsim");
	rootTree_ref->Add(file_ref);
	Int_t event_count_ref = rootTree_ref->GetEntries();

	cout << "\tNumber of events in reference data set: " << event_count_ref << "\n\tNumber of events in current data set: " << event_count_curr << "\n";
}
