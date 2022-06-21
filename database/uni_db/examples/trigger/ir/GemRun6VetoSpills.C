TString datFile("$VMCWORKDIR/database/uni_db/examples/trigger/ir/uRun6TripsSpBounded.dat");

int GetNGemTripsSpill( int RUN=1408, int chamber[]=nullptr, int evFirst[]=nullptr, int evLast[]=nullptr, 
                       int spFirst[]=nullptr, int spLast[]=nullptr) {
  int run, nTngs, nTrips;
  ifstream fs;
  gSystem->ExpandPathName(datFile);
  fs.open(datFile.Data(), ios::in);
  while ( !fs.eof() ) {
    fs >> run >> nTngs >> nTrips;
    for(int i=0; i<nTrips; i++) fs >> chamber[i] >> evFirst[i] >> evLast[i]
                                   >> spFirst[i] >> spLast[i];
    if(run>=RUN) break;
  }
  fs.close();
  if(run!=RUN) nTrips=-1;
  // if(run!=RUN) { nTrips=-1; cout<<"ERROR uRun6Trips.dat: no data for run= "<<RUN<<"!!!"<<endl;}
  return nTrips;
}


int FillSpillVeto(int run=1408, int MSP=3000, bool *spv=nullptr) {

  for(int i=0; i<MSP; i++) spv[i]=false;

  const int MTRIPS=20; int gem[MTRIPS], ev1[MTRIPS], ev2[MTRIPS], spi1[MTRIPS], spi2[MTRIPS];
  int nTrips = GetNGemTripsSpill(run, gem, ev1, ev2, spi1, spi2);
  
  cout<<run<<" nTrips="<<nTrips<<endl;
  if(nTrips<=0) return nTrips;

  int *nSpTrips = new int[MSP];
  bool *boTrip = new bool[MSP*7];

  for(int i=0; i<MSP; i++) { 
    nSpTrips[i]=0; 
    for(int j=0; j<7; j++) boTrip[i*7+j]=false;
  }

  for(int j=0; j<nTrips; j++) {
    int k1=gem[j], k2=gem[j];
    if(gem[j]==9) { k1=0; k2=6;}
    for(int k=k1; k<=k2; k++) {
      for(int i=spi1[j]; i<=spi2[j]; i++) { nSpTrips[i]++; boTrip[i*7+k]=true;}
    }
    cout<<"   chamber:"<<gem[j]<<" events:"<<ev1[j]<<" "<<ev2[j]<<" spills:"<<spi1[j]<<" "<<spi2[j]<<endl;
  }
  for(int i=0; i<MSP; i++) if( nSpTrips[i]>1 || boTrip[i*7+2]) spv[i]=true;

  delete [] nSpTrips;
  delete [] boTrip;
  return nTrips;
}


vector<int> GemRun6VetoSpills(int run=1710) {
  const int MSP=3000;
  bool spillVeto[MSP];
  int nSpVeto= FillSpillVeto(run, MSP, spillVeto);
  vector<int> vecSpillVeto;
  for(int i=0; i<MSP; i++) if(spillVeto[i]) { cout<<i<<" "; vecSpillVeto.push_back(i); }
  cout<<endl;
  return vecSpillVeto;
}
