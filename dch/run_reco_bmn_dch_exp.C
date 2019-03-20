/* Macro running DCH calibration and alignment. */
#include <TSpectrum.h>
#include <algorithm>
#include <Rtypes.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"
//void run_reco_bmn_dch_exp(UShort_t itermax)
void run_reco_bmn_dch_exp()
{
 TStopwatch timer;
 timer.Start();
 UShort_t runPeriodMax=7;

 //Cling::Interpreter().Eval("BmnDchHitProducer_exp* dchHP");
 //gCling->Eval("BmnDchHitProducer_exp* dchHP");
 //gCling->EvaluateT<void>("BmnDchHitProducer_exp* dchHP");
 //gCling->Evaluate("BmnDchHitProducer_exp* dchHP");
 //EvaluateT<void>("BmnDchHitProducer_exp* dchHP");
  //BmnDchHitProducer_exp* dchHP;
  //const Int_t nreadevents=1;
  //const Int_t nreadevents=2;
  //const Int_t nreadevents=8;
  //const Int_t nreadevents=10;
  //const Int_t nreadevents=20;
  //const Int_t nreadevents=100;
  //const Int_t nreadevents=500;
  //const Int_t nreadevents=1000;
  //const Int_t nreadevents=5000;
  //const Int_t nreadevents=10000;
  const Int_t nreadevents=100000;
  //const Int_t nreadevents=1000000;
  //const Int_t nreadevents=10000000;

  //const UShort_t itermax=20;
  // const UShort_t itermax=15;
  // const UShort_t itermax=5;
   //UShort_t itermax=2;//default
   //UShort_t itermax=0;
   //UShort_t itermax=1;
   UShort_t itermax=3;
  //const UShort_t itermax=0;
  /* Load basic libraries */
  //gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
  bmnloadlibs(); // load bmn libraries

//choose run period //1,2,3,4,5,6,7  

 //BmnDchHitProducer_exp* dchHP;
 //UShort_t runPeriod=2;
 UShort_t runPeriod=6;
 Int_t runNumber=0;
 UShort_t iter=0;
 UShort_t fileNumber=0; //count of read files
 //BmnDchHitProducer_exp* dchHP = new BmnDchHitProducer_exp(false,true,runPeriod,runNumber,iter,fileNumber);//checkDch, fDoCheck,...

 if(runPeriod>runPeriodMax||runPeriod==0){
    cout<<"Unknown run period!"<<endl;
    gApplication->Terminate();
 }

// input files     

  TString inDirectory="/home2/bmn_expdata/run"+TString::Itoa(runPeriod,10)+"/digit/";
  char *dirname = &inDirectory[0u];
  //const char *dirname="/home2/bmn_expdata/run2/digit/";
  const char *ext=".root";


  TSystemDirectory dir(dirname, dirname);
  TList *files = dir.GetListOfFiles();
  if (files) {
      TSystemFile *file;
      TString fname;
      TIter next(files);
      while ((file=(TSystemFile*)next())) {
         fname = file->GetName();
         //cout<<fname<<endl;
         if (!file->IsDirectory() && fname.EndsWith(ext)) {
            //cout << fname.Data() << endl;
            TString s=fname.Data();
            //cout << s << endl;
            //Int_t strSize=fname.Sizeof();
            //Int_t strSize=s.Sizeof();
            //cout << strSize << endl;
            //cout << fname.Sizeof() << endl;
            //Int_t nullpos=fname.First("0");
            //Ssiz_t npos=fname.Last('n');
            Ssiz_t upos=fname.First('u');
            //cout << fname.First('0') << endl;
            //cout << fname.First('u') << endl;
            //cout << fname.Last('n') << endl;
            //cout << nullpos << endl;
            //cout << upos << endl;
            //TString runNumberStr=s[nullpos+1];
            TString runNumberStr=s[upos+2];
            //TString runNumber=s(nullpos+1);
            //cout<<runNumberStr<<endl;
            runNumberStr+=s[upos+3];
            runNumberStr+=s[upos+4];
            runNumberStr+=s[upos+5];
            runNumber = runNumberStr.Atoi();
            //cout<<"runNumber = "<<runNumber<<endl;
            //if(!((runNumber>=246&&runNumber<=258)||(runNumber==360||runNumber==362||runNumber==376)||(runNumber>=380&&runNumber<=386)))continue;//full run list
            //if(!(runNumber==386||runNumber==381||runNumber==362||runNumber==250||runNumber==384||runNumber==254||runNumber==251||runNumber==255))continue;
            //if(!(runNumber==384||runNumber==250||runNumber==254||runNumber==251||runNumber==255))continue;
            //if(!(runNumber==244))continue;
            //if(!(runNumber==246))continue;
            //if(!(runNumber==247))continue;
            //if(!(runNumber==248))continue;
            //if(!(runNumber==249))continue;
            //if(!(runNumber==250))continue;
            //if(!(runNumber==251))continue;
            //if(!(runNumber==252))continue;
            //if(!(runNumber==253))continue;
            //if(!(runNumber==254))continue;
            //if(!(runNumber==255))continue;
            //if(!(runNumber==256))continue;
            //if(!(runNumber==257))continue;
            //if(!(runNumber==258))continue;
            //if(!(runNumber==275))continue;//d+Cu
            //if(!(runNumber==357))continue;//not running
            //if(!(runNumber==360))continue;
            //if(!(runNumber==362))continue;//not running
            //if(!(runNumber==376))continue;
            //if(!(runNumber==380))continue;
            //if(!(runNumber==381))continue;
            //if(!(runNumber==381||runNumber==384))continue;
            //if(!(runNumber==382))continue;
            //if(!(runNumber==383))continue;
            //if(!(runNumber==384))continue;
            //if(!(runNumber==244||runNumber==384))continue;
            //if(!(runNumber==386))continue;//not running
            //if(!(runNumber==387))continue;//
            //if(!(runNumber==388))continue;
            //if(!(runNumber==389))continue;
            //if(!(runNumber==390))continue;//empty runs
            //if(!(runNumber==391))continue;
            //if(!(runNumber==392))continue;
            //if(!(runNumber==393))continue;
            //if(!(runNumber==394))continue;
            //if(!(runNumber==395))continue;//
            //if(!(runNumber==386||runNumber==381||runNumber==362||runNumber==250||runNumber==384||runNumber==254||runNumber==251||runNumber==255||runNumber==383||runNumber==360||runNumber==382||runNumber==376||runNumber==385||runNumber==380))continue;
            //if(!(runNumber==1405))continue;//nt,nf
            //if(!(runNumber==1426))continue;//nt,nf
            //if(!(runNumber==1431))continue;//nt,nf
            //if(!(runNumber==1587))continue;
            //if(!(runNumber==1624))continue;//nt ?
            //if(!(runNumber==1677))continue;//nt ?
            //if(!(runNumber==1770))continue;//no target, field on
            //if(!(runNumber==1800))continue;
            //if(!(runNumber==1812))continue;
            //if(!(runNumber==1814))continue;
            if(!(runNumber==1884))continue;//no target, no field
            //if(!(runNumber==1886))continue;//nt
            //if(!(runNumber==1888))continue;//nt
            //if(!(runNumber==1889))continue;//nt
            //if(!(runNumber==1890))continue;//nt
            //if(!(runNumber==1891))continue;//nt
            cout << "-------------------------------------------------------" << endl;
            cout <<"run period = "<<runPeriod<<", "<< "run number = " << runNumber << endl;
            cout << "-------------------------------------------------------" << endl;

  //gObjectTable->Print(); 
  //TString inFile = "bmn_run0246_digit.root"; // momentum verif.
  TString inFile = s;
  //TString inFile = "bmn_run0161_digit.root";
  //TString inFile = "bmn_run0166_digit.root";
  //TString inFile = "bmn_run0222_digit.root"; // momentum verif. (weak field)
  //TString inFile = "bmn_run0244_digit.root"; // momentum verif. (weak field)
  //TString inFile = "bmn_run0246_digit.root"; // momentum verif.
  //TString inFile = "bmn_run0247_digit.root"; // momentum verif.
  //TString inFile = "bmn_run0248_digit.root"; // momentum verif.
  //TString inFile = "bmn_run0249_digit.root";
  //TString inFile = "bmn_run0250_digit.root"; // momentum verif.
  //TString inFile = "bmn_run0251_digit.root"; // momentum verif.
  //TString inFile = "bmn_run0256_digit.root"; // momentum verif.
  //TString inDirectory,runPeriod;


// parameters of input file
  /*
  Char_t* runname=inFile;
  //Char_t* runname="bmn_run0166_digit.root";
  //Char_t* runname="bmn_run0607_digit.root";
  Char_t runnumber[5]; 
  Char_t* firstdigit=strchr(runname,'0'); 
  strlcpy(runnumber,firstdigit,5);
  UShort_t runNumber=UShort_t(atoi(runnumber));
  */
 
 //fast check of the required run availability
 Bool_t runExists=false;
 switch(runPeriod){
  case 1:
   if(runNumber>=12&&runNumber<=188)runExists=true;
   break;
  case 2:
   if(runNumber>=220&&runNumber<=395)runExists=true;
   break;
  case 3:
   if(runNumber>=403&&runNumber<=688)runExists=true;
   break;
  case 5:
   if(runNumber>=430&&runNumber<=1014)runExists=true;
   break;
  case 6:
   if(runNumber>=1170&&runNumber<=1992)runExists=true;
   break;
  default:
   cout<<"Unknown run number "<<runNumber<<" from run period "<<runPeriod<<"!"<<endl; 
   gApplication->Terminate();
 }

  TString inPut=inDirectory+inFile;
  //TFile fileDST(inFile);
  TFile fileDST(inPut);
  cout<<inPut<<endl;

// output file        
  
  //TString outFile = "bmndst_test.root";

  //TFile *fout = new TFile(outFile,"recreate");
   cout<<"runPeriod = "<<runPeriod<<endl; 
  TString strchain;
  // Activate branches
  if(runPeriod<=3){
   //TChain *bmnTree = new TChain("BMN_DIGIT");
   strchain="BMN_DIGIT"; 
  }else{
   //TChain *bmnTree = new TChain("cbmsim");
   strchain="cbmsim";
  }
  TChain *bmnTree = new TChain(strchain);  
 
cout<<strchain<<endl; 
  //bmnTree->Add(runname);
  bmnTree->Add(inPut);

  BmnDchDigit* digit = NULL;
  TClonesArray *dchDigits;
  if(runPeriod<=3){
   bmnTree->SetBranchAddress("bmn_dch_digit", &dchDigits);
  }else{
   bmnTree->SetBranchAddress("DCH", &dchDigits);
  }

  //bmnTree->SetAutoDelete(kTRUE);

  Int_t nAvailableEvents=bmnTree->GetEntries(); 
  cout<<"number of available events = "<<nAvailableEvents<<endl;
  Int_t nevents=min(nreadevents,nAvailableEvents); 

  Option_t* Opt="";
  UInt_t eventNum1track;
  Double_t sumDeltaPhi;
  Bool_t minFound=false; 
  iter = 0;
  Bool_t noError;


  //Opt="reconstruction";
 
 //BmnDchHitProducer_exp* dchHP;

 //if(fileNumber==0)BmnDchHitProducer_exp* dchHP;
 //BmnDchHitProducer_exp* dchHP = new BmnDchHitProducer_exp(false,true,runPeriod,runNumber,iter,fileNumber);//checkDch, fDoCheck,...
 BmnDchHitProducer_exp* dchHP =(fileNumber==0)? new BmnDchHitProducer_exp(false,true,runPeriod,runNumber,iter,fileNumber):dchHP;//checkDch, fDoCheck,...
 //BmnDchHitProducer_exp* dchHP;
  //if(fileNumber==0)BmnDchHitProducer_exp* dchHP = new BmnDchHitProducer_exp(false,true,runPeriod,runNumber,iter,fileNumber);//checkDch, fDoCheck,...
  //if(fileNumber==0)dchHP = new BmnDchHitProducer_exp(false,true,runPeriod,runNumber,iter,fileNumber);//checkDch, fDoCheck,...
  //dchHP = new BmnDchHitProducer_exp(false,true,runPeriod,runNumber,iter,fileNumber);//checkDch, fDoCheck,...

 if(strcmp(Opt,"reconstruction")==0){
    
     itermax=0;
     dchHP->SetMaxIterationNumber(itermax);
     cout<<"DCH reconstruction cycle "<<endl;
     cout<<"Opt = "<<Opt<<endl;
     dchHP->SetRunPeriod(runPeriod);
     dchHP->SetRunNumber(runNumber);
     dchHP->SetIterationNumber(iter);
     dchHP->SetFileNumber(fileNumber);
     dchHP->SetOpt(Opt);


     dchHP->Init(bmnTree,dchDigits);
     for (Int_t m = 0; m < nevents; m++){ //alignment 
     //for (Int_t m = 9; m < 10; m++){ //alignment 
      bmnTree->GetEntry(m);
      //cout<<"event number = "<<m<<endl;
      dchHP->Exec(m);
     }
     noError=true; //true -> everything's all right
     dchHP->Finish(noError);


 }else{


  //for (UShort_t iter = 0; iter < itermax; iter++){
   do{
 
   //gObjectTable->Print(); 
   //if(iter==0&&fileNumber==0)BmnDchHitProducer_exp* dchHP = new BmnDchHitProducer_exp(false,false,runPeriod,runNumber,iter,fileNumber);//checkDch, fDoCheck,...
   //gObjectTable->Print(); 
   //if(iter>0)dchHP->SetIterationNumber(iter);
   if(!(iter==0&&fileNumber==0)) {
     //dchHP->SetfDoCheck(true);
     dchHP->SetRunPeriod(runPeriod);
     dchHP->SetRunNumber(runNumber);
     dchHP->SetIterationNumber(iter);
     dchHP->SetFileNumber(fileNumber);
   }
   if(iter==0)dchHP->SetMaxIterationNumber(itermax);
   Opt="alignment";
   noError=true; //true -> everything's all right
   dchHP->SetOpt(Opt);
   cout<<"Opt = "<<Opt<<endl;
   if(iter>1)dchHP->SetMeanDeltaPhi(sumDeltaPhi/Double_t(eventNum1track));
   if(iter>1)cout<<"sumDeltaPhi = "<<sumDeltaPhi<<", eventNum1track = "<<eventNum1track<<endl;
   cout<<"DCH Initialization"<<endl;
   dchHP->Init(bmnTree,dchDigits);
   cout<<"DCH initialization done."<<endl;
   cout<<"iteration number = "<<iter<<endl;
  //if(iter>0){
   cout<<"DCH alignment cycle "<<endl;
   //gObjectTable->Print(); 
   for (Int_t m = 0; m < nevents; m++){ //alignment 
   //gObjectTable->Print(); 
     bmnTree->GetEntry(m);
   //gObjectTable->Print(); 
     //if(m%1000==0)cout<<"event number - alignment = "<<m<<endl;
     //if(m%1000==0)cout<<"event number - alignment = "<<endl;
     dchHP->Exec(m);
     if(m%10000==0){
       eventNum1track=dchHP->GetNumberOfAlignmentTracks();
       if(eventNum1track>12000)break;
     }
     if((m%1000)==0)cout<<"event number - alignment = "<<m<<endl;
   } 
   //gObjectTable->Print(); 
   eventNum1track=dchHP->GetNumberOfAlignmentTracks();//alignment tracks
   if(iter>0)sumDeltaPhi=dchHP->GetSumOfDeltaPhi();//delta phi of alignment tracks
   if(iter>0)cout<<"eventNum1track = "<<eventNum1track<<", sumDeltaPhi = "<<sumDeltaPhi<<endl;
   cout<<"eventNum1track = "<<dchHP->GetNumberOfAlignmentTracks()<<endl;
   //if(eventNum1track<=1){
   if(eventNum1track<=20){
    cout<<"Not enough tracks for alignment! Try to increase number of events! Run number = "<<runNumber<<endl;
    noError=false;
    dchHP->FinishAlignment(noError);
    goto finish;
    //break;
    //gApplication->Terminate();
    //dchHP->Finish(hResidRMSvsIter,hResidRMSvsIter0,hResidRMSvsIter1);
    //continue;
   }  
   if(iter>0)cout<<"mean delta phi = "<<sumDeltaPhi/Double_t(eventNum1track)<<endl;
   //gObjectTable->Print(); 
   //dchHP->GlobalAlignmentMatrix();
   //noError=false;
   dchHP->GlobalAlignmentParameters(noError);
   if(!noError){
    dchHP->FinishAlignment(noError);
    cout<<"Not enough tracks for alignment(2)! Try to increase number of events! Run number = "<<runNumber<<endl;
    goto finish;
    //break; 
   }
   //gObjectTable->Print(); 
   dchHP->AlignmentMatrix();
   dchHP->FinishAlignment(noError);
   //gObjectTable->Print(); 
   //}// if Iter>0, no alignment in first cycle
   cout<<"DCH autocalibration cycle "<<endl;
   Opt="autocalibration";
   dchHP->SetOpt(Opt);
   //for (Int_t m = 0; m < nevents*10; m++){ //autocalibration 
   for (Int_t m = 0; m < nevents; m++){ //autocalibration 
     bmnTree->GetEntry(m);
     if(m%10000==0)cout<<"event number - autocalibration = "<<m<<endl;
     //cout<<"event number = "<<m<<endl;
     dchHP->Exec(m);
   } 
   //gObjectTable->Print(); 
   finish:dchHP->Finish(noError);
   //delete dchHP;
   //if(!noError)break;

   //delete dchHP;
   
   //if(iter==itermax)minFound=true;
   //if(iter==itermax){
   // noError=false;
   // cout<<"Maximum number of iterations reached at "<<iter<<"."<<endl;
   //}
   iter++;
 
  //}while(minFound==false); //iterations
   //gObjectTable->Print(); 
  }while(noError); //iterations

 }//if reconstruction or alignment+calibration

  //delete dchHP;
  delete bmnTree;
 
  //gObjectTable->Print(); 
  timer.Print();
  //if(noError)cout << " DCH autocalibration and alignment done!" << endl;
  //exit(0);
   delete dchHP;
  
            fileNumber++; 
         } // file selection
      } // file selection
   //delete dchHP;
   } // file selection
  //remove("DCHpar");
}










