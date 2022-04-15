/* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmBmnStsDigitize source file             -----
// -----                  Created 08/07/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------




// Includes from ROOT
#include "TClonesArray.h"
#include "TGeoBBox.h"
#include "TObjArray.h"
#include "TMath.h"
#include "TF1.h"
#include "TRandom3.h"

#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"

// Includes from base
#include "FairEventHeader.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// Includes from STS
#include "CbmGeoStsPar.h"
#include "CbmStsDigi.h"
#include "CbmStsDigiMatch.h"
#include "CbmStsDigiPar.h"


#include "CbmBmnStsDigitize.h"
#include "CbmStsPoint.h"
#include "CbmStsSensor.h"
#include "CbmStsSector.h"
#include "CbmStsStation.h"
#include "BmnTrigDigit.h"

#include "BmnGemStripDigit.h"
#include "BmnSiliconDigit.h"
#include "BmnStripDigit.h"

#include <iostream>

#include <iomanip>
#include <map>
#include <vector>

using namespace std;

Int_t GetNofModules(TGeoNode* station);
// -----   Default constructor   ------------------------------------------
CbmBmnStsDigitize::CbmBmnStsDigitize()
  : FairTask("STS Digitizer", 1),
  fGeoPar(NULL),
  fDigiPar(NULL),
  fPoints(NULL),
  fPointsSI(NULL),
  fDigis(NULL),
  fDigiMatches(NULL),
  fRealistic(kFALSE),
  fDigiScheme(NULL),
  fNDigis(0),
   fDigisCSC(0),
  fNMulti(0),
  fNEvents(0),
  fNPoints(0),
  fNDigisFront(0),
  fNDigisBack(0),
  fStep(0.001),
  fEnergyLossToSignal(0.),
  fFThreshold(4.0),
  fBThreshold(4.0),
  fFNoiseWidth(0.1),
  fBNoiseWidth(0.1),
  fStripDeadTime(10),
  fFNofBits(12),
  fBNofBits(12),
  fFNofElPerAdc(10.),
  fBNofElPerAdc(10.),
  fFNofSteps(0),
  fBNofSteps(0),
  fStripSignalF(NULL),
  fStripSignalB(NULL),
  fTime(0.),
  fTimer(),
  fFChannelPointsMap(),
  fBChannelPointsMap(),
  fPointMap()
{
  //AZ fDigiScheme  = new CbmStsDigiScheme();
  fDigiScheme  = CbmStsDigiScheme::Instance();
  Reset();

}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmBmnStsDigitize::CbmBmnStsDigitize(Int_t iVerbose)
  : FairTask("STSDigitize", iVerbose),
  fGeoPar(NULL),
  fDigiPar(NULL),
  fPoints(NULL),
  fPointsSI(NULL),
  fDigis(NULL),
  fDigiMatches(NULL),
  fRealistic(kFALSE),
  fDigiScheme(NULL),
  fNDigis(0),
  fDigisCSC(0),
  fNMulti(0),
  fNEvents(0),
  fNPoints(0),
  fNDigisFront(0),
  fNDigisBack(0),
  fStep(0.001),
  fEnergyLossToSignal(0.),
  fFThreshold(4.0),
  fBThreshold(4.0),
  fFNoiseWidth(0.1),
  fBNoiseWidth(0.1),
  fStripDeadTime(10),
  fFNofBits(12),
  fBNofBits(12),
  fFNofElPerAdc(10.),
  fBNofElPerAdc(10.),
  fFNofSteps(0),
  fBNofSteps(0),
  fStripSignalF(NULL),
  fStripSignalB(NULL),
  fTime(0.),
  fTimer(),
  fFChannelPointsMap(),
  fBChannelPointsMap(),
  fPointMap()
{
  //AZ fDigiScheme  = new CbmStsDigiScheme();
  fDigiScheme  = CbmStsDigiScheme::Instance();
  Reset();
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
CbmBmnStsDigitize::CbmBmnStsDigitize(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose),
  fGeoPar(NULL),
  fDigiPar(NULL),
  fPoints(NULL),
  fPointsSI(NULL),
  fDigis(NULL),
  fDigiMatches(NULL),
  fRealistic(kFALSE),
  fDigiScheme(NULL),
  fNDigis(0),
  fDigisCSC(0),
  fNMulti(0),
  fNEvents(0),
  fNPoints(0),
  fNDigisFront(0),
  fNDigisBack(0),
  fStep(0.001),
  fEnergyLossToSignal(0.),
  fFThreshold(4.0),
  fBThreshold(4.0),
  fFNoiseWidth(0.1),
  fBNoiseWidth(0.1),
  fStripDeadTime(10),
  fFNofBits(12),
  fBNofBits(12),
  fFNofElPerAdc(10.),
  fBNofElPerAdc(10.),
  fFNofSteps(0),
  fBNofSteps(0),
  fStripSignalF(NULL),
  fStripSignalB(NULL),
  fTime(0.),
  fTimer(),
  fFChannelPointsMap(),
  fBChannelPointsMap(),
  fPointMap()
  {
  
  fGeoPar      = NULL;
  fDigiPar     = NULL;
  fPoints      = NULL;
  fDigis       = NULL;
  fDigisCSC = NULL;
  fDigiMatches = NULL;
  fRealistic   = kFALSE;
  //AZ fDigiScheme  = new CbmStsDigiScheme();
  fDigiScheme  = CbmStsDigiScheme::Instance();
  Reset();

  fStep = 0.001;

  fFThreshold  = 4.0;
  fBThreshold  = 4.0;
  fFNoiseWidth = 0.1;
  fBNoiseWidth = 0.1;

  fFNofBits    = 12;
  fBNofBits    = 12;
  fFNofElPerAdc    = 10.;
  fBNofElPerAdc    = 10.;
  fStripDeadTime = 10;
  fNEvents = 0.;
  
  
  
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmBmnStsDigitize::~CbmBmnStsDigitize() {
  

  if ( fGeoPar)    delete fGeoPar;
  if ( fDigiPar)   delete fDigiPar;
  if ( fDigis ) {
    fDigis->Delete();
    delete fDigis;
  }
    if ( fDigisCSC ) {
    fDigisCSC->Delete();
    delete fDigisCSC;
  }
  if ( fDigiMatches ) {
    fDigiMatches->Delete();
    delete fDigiMatches;
  }
  //AZ if ( fDigiScheme ) delete fDigiScheme;
  Reset();
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void CbmBmnStsDigitize::Exec(Option_t* opt) {

  fTimer.Start();
  Reset(); //AZ
  Int_t stationNr=0;
  Int_t sectorNr=0;
  Int_t side=-1;
  Int_t stripNr=0;

  FairRunAna *run = FairRunAna::Instance();
 

Int_t nDigis = 0, nDigisCSC=0;

if(fPointsSI->GetEntries()+fPoints->GetEntries() > 3000 ) return;

  for (Int_t iPointSI=0; iPointSI<fPointsSI->GetEntries(); iPointSI++) {
  
  BmnSiliconDigit* sidig=(BmnSiliconDigit*)fPointsSI->At(iPointSI);
  
//     cout<<" Event: "<<fch<<" SI: "<<sidig->GetStation()<<" Mod: "<<sidig->GetModule()<<" Layer: "<<sidig->GetStripLayer()<<" strip Num: "<<sidig->GetStripNumber()<<
//     " signal: "<<sidig->GetStripSignal()<<endl;
     
Double_t sisig=sidig->GetStripSignal();
Int_t stripNrSI=sidig->GetStripNumber()-1; // GP kapishin MYSILICON start from num. 1 
Int_t stationNrSI=sidig->GetStation();
Int_t sectorNrSI=-100; // start from num. 1
Int_t sideSI=-1;

if(sidig->GetStripLayer()==1) sideSI=1; 
else if(sidig->GetStripLayer()==0) sideSI=0;

Int_t simod=-100;
sectorNrSI=sidig->GetModule()+1;

 new ((*fDigis)[nDigis++]) CbmStsDigi(stationNrSI, sectorNrSI, sideSI, stripNrSI, sisig, 0); //AZ
 
  }
   
  for (Int_t iPoint=0; iPoint<fPoints->GetEntries(); iPoint++) {

    BmnGemStripDigit* stdig=  (BmnGemStripDigit*)fPoints->At(iPoint) ;
//    cout<<" Event: "<<fch<<" ST: "<<stdig->GetStation()<<" Mod: "<<stdig->GetModule()<<" Layer: "<<stdig->GetStripLayer()<<" strip Num: "<<stdig->GetStripNumber()<<
//      " signal: "<<stdig->GetStripSignal()<<endl;
 Double_t stsig= stdig->GetStripSignal();

  stripNr=stdig->GetStripNumber()-1; // GP kapishin STRIPGEM start from num. 1
 
  if(stdig->GetStation()==5 && stdig->GetModule()==1 && stdig->GetStripLayer()==2 && stdig->GetStripNumber()>128 && stdig->GetStripNumber()<257)
  stripNr=stripNr-2;
 
 
    // if(TMath::Abs(stripNr-120)<=64) continue; //remove strips for MC && DATA compare
 
    stationNr=stdig->GetStation();

    if(stationNr==3) continue;
    else if(stationNr>3) stationNr=stationNr-1;
  
    Int_t modNum=stdig->GetModule();


      if( (stdig->GetStripLayer()==0 || stdig->GetStripLayer()==1) && modNum==0) sectorNr=1;
      if( (stdig->GetStripLayer()==2 || stdig->GetStripLayer()==3) && modNum==0) sectorNr=2;


      if( (stdig->GetStripLayer()==0 || stdig->GetStripLayer()==1) && modNum==1) sectorNr=3;
      if( (stdig->GetStripLayer()==2 || stdig->GetStripLayer()==3) && modNum==1) sectorNr=4;

    //if(stationNr==7) sectorNr=stdig->GetModule()*2+stdig->GetStripLayer()/2;

    if( (stdig->GetStripLayer()==0 || stdig->GetStripLayer()==2) ) side=0; //true for small station
    if( (stdig->GetStripLayer()==1 || stdig->GetStripLayer()==3) ) side=1; //true for small station
    
    if(stationNr<7) new ((*fDigis)[nDigis++]) CbmStsDigi(stationNr+3, sectorNr, side, stripNr, stsig, 0); //AZ
    //else if(stationNr==7)  new ((*fDigisCSC)[nDigisCSC++]) CbmStsDigi(stationNr+3, sectorNr, side, stripNr, stsig, 0); //AZ

  } 
  


  fTimer.Stop();

  fTime        += fTimer.RealTime();

}
// -----------------------end Exec--------------------------------------------------

// -----   Private method CrossSpacer   ------------------------------------
Bool_t CbmBmnStsDigitize::CrossSpacer(const TGeoNode *node, const CbmStsPoint *point)
{

  return kFALSE;
}
// -------------------------------------------------------------------------

// -----   Private method ProduceHitResponse   --------------------------------
void CbmBmnStsDigitize::ProduceHitResponse(CbmStsSensor* sensor) {


}

// -----   Private method SetParContainers   -------------------------------
void CbmBmnStsDigitize::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");
  // Get STS digitisation parameter container
  fDigiPar = (CbmStsDigiPar*) db->getContainer("CbmStsDigiPar");

}
// -------------------------------------------------------------------------



// -----   Private method Init   -------------------------------------------
InitStatus CbmBmnStsDigitize::Init() {

  // Get input array
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fPoints = (TClonesArray*) ioman->GetObject("STRIPGEM");//StsPoint

  fPointsSI = (TClonesArray* ) ioman->GetObject("MYSILICON");//StsPoint

  fEventHeader = (TClonesArray*) ioman->GetObject("EventHeader");//StsPoint

/*
  fPointsT0=(TClonesArray*) ioman->GetObject("T0");     
  fPointsBC1=(TClonesArray*) ioman->GetObject("BC1"); 
  fPointsBC2=(TClonesArray*) ioman->GetObject("BC2");     
  fPointsVeto=(TClonesArray*) ioman->GetObject("VC");      
  fPointsBD=(TClonesArray*) ioman->GetObject("BD");    
  fPointsSi=(TClonesArray*) ioman->GetObject("Si");    
*/
  //fDigiMatches = new TClonesArray("CbmStsDigiMatch",1000);
  //ioman->Register("StsDigiMatch", "Digi Match in STS", fDigiMatches, kTRUE);

  // Register output array StsDigi
  fDigis = new TClonesArray("CbmStsDigi",1000);
  ioman->Register("StsDigi", "Digital response in STS", fDigis, kTRUE);

   // Register output array StsDigi
  //fDigisCSC = new TClonesArray("CbmStsDigi",1000);
  //ioman->Register("StsDigiCSC", "Digital response in STS", fDigisCSC, kTRUE);

  ApplyAlignment(); //AZ
  gGeoManager->CheckOverlaps(); //AZ
  gGeoManager->PrintOverlaps(); //AZ

  // Build digitisation scheme
  if ( fDigiScheme->Init(NULL, fDigiPar) ) {
  //  MakeSets();

    if      (fVerbose == 1 || fVerbose == 2) fDigiScheme->Print(kFALSE);
      else if (fVerbose >  2) fDigiScheme->Print(kTRUE);
      cout << "-I- " << fName << "::Init: "
  	 << "STS digitisation scheme succesfully initialised" << endl;
      if ( fDigiScheme->IsNewGeometry() ) cout << "-I- Using new geometry" << endl;
      cout << "    Stations: " << fDigiScheme->GetNStations()
  	 << ", Sectors: " << fDigiScheme->GetNSectors() << ", Channels: "
  	 << fDigiScheme->GetNChannels() << endl;


    return kSUCCESS;
  }

 

  return kSUCCESS;//kERROR;

}
// -------------------------------------------------------------------------



// -----   Private method ReInit   -----------------------------------------
InitStatus CbmBmnStsDigitize::ReInit() {
/*
  // Clear digitisation scheme
  fDigiScheme->Clear();

  // Build new digitisation scheme
  if ( fDigiScheme->Init(fGeoPar, fDigiPar) ) {
    MakeSets();
    return kSUCCESS;
  }
*/
  return kSUCCESS;//kERROR;

}
// -------------------------------------------------------------------------


// -----   Private method MakeSets   ---------------------------------------
void CbmBmnStsDigitize::MakeSets() {


}
// -------------------------------------------------------------------------
void CbmBmnStsDigitize::MakeSets1() { //with occupancy file - default not used


}
// -------------------------------------------------------------------------

// -----   Private method Reset   ------------------------------------------
void CbmBmnStsDigitize::Reset() {
  fNDigis = fNMulti = 0;
  fFChannelPointsMap.clear();
  fBChannelPointsMap.clear();
  //  if ( fDigis ) fDigis->Clear();
  //  if ( fDigiMatches ) fDigiMatches->Clear();
  if ( fDigis ) fDigis->Delete();
  if ( fDigisCSC ) fDigisCSC->Delete();
  if ( fDigiMatches ) fDigiMatches->Delete();
}
// -------------------------------------------------------------------------


// -----   Virtual method Finish   -----------------------------------------
void CbmBmnStsDigitize::Finish() {

//  u_gem.close();
/*  cout << endl;
  cout << "============================================================"
       << endl;
  cout << "===== " << fName << ": Run summary " << endl;
  cout << "===== " << endl;
  cout << "===== Events processed          : " << setw(8) << fNEvents << endl;
  cout.setf(ios_base::fixed, ios_base::floatfield);
  cout << "===== Real time per event       : "
       << setw(8) << setprecision(4)
       << fTime / fNEvents << " s" << endl;
  cout << "===== StsPoints per event       : "
       << setw(8) << setprecision(2)
       << fNPoints / fNEvents << endl;
  cout << "===== StsDigis per event        : "
       << setw(8) << setprecision(2)
       << (fNDigisFront+fNDigisBack) / fNEvents << endl;
  cout << "===== Front digis per point     : "
       << setw(8) << setprecision(2)
       << fNDigisFront / (fNPoints) << endl;
  cout << "===== Back digis per point      : "
       << setw(8) << setprecision(2)
       << fNDigisBack / (fNPoints) << endl;
  cout << "============================================================"
       << endl;
*/
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------

Int_t GetNofModules(TGeoNode* station) {

  Int_t nModules = 0; //station->GetNdaughters();

  // --- Modules

  //*
  for (Int_t iModule = 0; iModule < station->GetNdaughters(); iModule++) {
    TGeoNode* module = station->GetDaughter(iModule);
    if ( TString(module->GetName()).Contains("module") ) nModules++;
  }
  //*/

  return nModules;
}

// -------------------------------------------------------------------------

void CbmBmnStsDigitize::ApplyAlignment()
{

    // Apply alignment

    const int ip_r7[6][2]={0,1, 2,3, 4,5, 6,7, 8,9, 10,11};

    TGeoNode* st_node = NULL;
    // Get STS node
    TGeoNode* sts = NULL;
    gGeoManager->CdTop();
    TGeoNode* cave = gGeoManager->GetCurrentNode();
    for (Int_t iNode = 0; iNode < cave->GetNdaughters(); iNode++) {
        TGeoNode* node = cave->GetDaughter(iNode);
        TString name = node->GetName();
        if ( name.Contains("GEMS_0") ) {
            sts = node;
            gGeoManager->CdDown(iNode);
            break;
        }
    }
    if ( ! sts ) {
        cout << "-E- CbmBmnStsDigitize::ApplyAlignment: Cannot find top GEM node"
	 << endl;
        //return kFALSE;
    }

    // Loop over stations in STS
    Int_t statNr = 0;
    Int_t nStat0 = sts->GetNdaughters();
    Int_t nMod0sts=0;

    cout<<"STATIONS : "<<nStat0<<endl;

    Double_t xAlign[ 26 ]=
    {3.33029,-2.67835,2.82585 ,-3.14615, //I Si
     2.85175, -3.11705, //II Si
     8.91918 ,2.92088, -3.09752,-9.09492,  8.91198 ,2.94528 ,-2.98182,-8.98402, //III Si
     -40.718 , 40.916, 41.087, -40.6, 42.28, -39.431, -38.452, 43.252,  44.078, -37.598,  -36.8112, 44.8227};//GEM

    Double_t yAlign[ 26 ]=
    {4.46297,5.07597, -1.48803,-0.910531, //I Si
     1.8115,1.42, //II Si
     7.84856,7.80306,7.92256,7.85506,-4.57154,-4.61864, -4.43934,-4.51954, // III Si
     22.5211, 22.2759, 22.574, 22.6124,  22.8827,  22.516, 22.5385, 22.9377, 22.5828, 22.3481, 22.7875, 23.007};//GEM

    Double_t zAlign3[9] =
    {14.352475, 17.68975, 26.2729875, //3 planes of Si
     42.19995,67.76995,115.569,137.8325,163.54,186.103}; //6 planes of GEM

    Double_t zAlign23[26] =
    {-0.406375,0.430925,0.274025,-0.298575, // I Si
     -0.43165,0.43165, //II Si
     0.540813, -1.06009, 0.653113, -0.915988, -0.719887, 0.761012, -0.386287, 1.12281, //III Si
     0.08685,-0.08685,  -0.07145,0.07145,  -0.128, 0.128,  0.0115,-0.0115,  -0.004,0.004, 0,0}; //GEMS sensors

    Double_t driftcorr= 0;//-0.2;
    Int_t stn=0,SensCh=0;

    for (Int_t iNode = 0; iNode < nStat0; iNode++) {
        // Go to station node
        gGeoManager->CdDown(iNode);
        TGeoNode* stationNode = gGeoManager->GetCurrentNode();
        TString statName = stationNode->GetName();
        if ( ! statName.Contains("station") ) {
            gGeoManager->CdUp();
            continue;
        }
        ++stn; //AZ
        ++ statNr;

        TGeoHMatrix *matr = gGeoManager->GetCurrentMatrix();
        Double_t* statTrans = matr->GetTranslation();
        TGeoRotation *r2;
        cout<<"statNr : "<<statNr <<", "<<statTrans[2]  <<" zal: "<<zAlign3[statNr-1]<<endl;
        statTrans[2] = zAlign3[statNr-1];//zAlign[statNr];

        TGeoTranslation *t2 = new TGeoTranslation(statTrans[0],statTrans[1],statTrans[2]);

        r2 = new TGeoRotation("rot",0,0,0);
        TGeoCombiTrans *cc1 = new TGeoCombiTrans(*t2,*r2);

        TGeoHMatrix *matr0 = new TGeoHMatrix(*cc1);
        matr0->RegisterYourself();

        //  Int_t nModules = stationNode->GetNdaughters();
        Int_t nModules = GetNofModules(stationNode);
        //  cout<<"nModules: "<<nModules<<endl;

        sts->GetVolume()->AddNode((TGeoVolumeAssembly*)stationNode->GetVolume(),0,matr0); //AZ

        //AZ- hard-coded st_node=sts->GetVolume()->GetNode(iNode+6);
        st_node = (TGeoNode*) sts->GetVolume()->GetNodes()->Last(); //AZ

        //-----------------------module translate-------------
        Int_t moduleNr = 0, copy_no = 0;

        // ---> Large sensors
        for (Int_t iStatD = 0; iStatD < nModules; iStatD++) {
            gGeoManager->CdDown(iStatD);

            TGeoNode* module = gGeoManager->GetCurrentNode();
            if ( ! TString(module->GetName()).Contains("module") ) {
                gGeoManager->CdUp();
                continue;
            }
            cout << iStatD << " " << module->GetName() << " stn: "<<statNr << endl;

            //-------------------------add I II III Si----------------------------
            if(iNode<=2){

                Int_t nOfModSi=SensCh;//10+(moduleNr++);
                cout<<"modSI: "<<nOfModSi<<" "<<zAlign23[nOfModSi]<<endl;

                TGeoHMatrix *matrMod = gGeoManager->GetCurrentMatrix();
                Double_t* modTrans = matrMod->GetTranslation();
                //cout<<"trans: "<<modTrans[2]<<endl;
                modTrans[0] = xAlign[nOfModSi];
                modTrans[1] = yAlign[nOfModSi];
                modTrans[2] = zAlign23[nOfModSi];//zAlign2[ipNr];
                matrMod->SetTranslation(modTrans);
                TGeoHMatrix *matr0Mod = new TGeoHMatrix(*matrMod);
                matr0Mod->RegisterYourself();

                st_node->GetVolume()->AddNode((TGeoVolumeAssembly*)module->GetVolume(),copy_no,matr0Mod);

                Double_t* modTrans1 = gGeoManager->GetCurrentMatrix()->GetTranslation();
                copy_no++;
                //delete matr0Mod;
                gGeoManager->CdUp();  // back to module

                SensCh++;

            }
            //-------------------------add I II III Si----------------------------


            if(iNode>2){
                if (TString(module->GetName()).Contains("Senso")) {

                    if(iStatD==0) moduleNr=0;
                    if(iStatD>0) moduleNr=1;

                    Int_t ipNr = 0;

                    TGeoHMatrix *matrMod = gGeoManager->GetCurrentMatrix();
                    Double_t* modTrans = matrMod->GetTranslation();
                    cout<<" aaaa ^^^^^^^^^^^^^^^^^^^ : "<<statNr-1;
                    cout<<"\n"<<moduleNr;
                    cout<<"\n"<<ip_r7[ (statNr-1)-3 ][moduleNr]<<endl;

                    modTrans[0] = xAlign[ip_r7[(statNr-1)-3][moduleNr]+14];
                    modTrans[1] = yAlign[ip_r7[(statNr-1)-3][moduleNr]+14];// - modTrans[1]; //not need anymore (- mod...)
                    modTrans[2] = zAlign23[ip_r7[(statNr-1)-3][moduleNr]+14];//zAlign2[ipNr];

                    matrMod->SetTranslation(modTrans);
                    TGeoHMatrix *matr0Mod = new TGeoHMatrix(*matrMod);
                    matr0Mod->RegisterYourself();
                    ipNr++;

                    st_node->GetVolume()->AddNode((TGeoVolumeAssembly*)module->GetVolume(),copy_no,matr0Mod);

                    copy_no++;
                    //delete matr0Mod;

                }

                gGeoManager->CdUp();  // back to module

            }} // for (Int_t iStatD = 0; iStatD < nModules;
        //----------------------end module translate----------
        //delete matr0;
        gGeoManager->CdUp();               // to sts
    }                                    // station loop

    Int_t snr=1;
    vector<TGeoNode*> removNodes;
    // Remove extra nodes
    for (Int_t iNode = 0; iNode < nStat0; iNode++) {
        // Go to station node
        //gGeoManager->CdDown(1);
        gGeoManager->CdDown(iNode);
        TGeoNode* stationNode = gGeoManager->GetCurrentNode();
        cout<<" st name del: "<< stationNode->GetName()<< " iNode: "<<iNode<<endl;
        /*if(iNode>2)*/ removNodes.push_back(stationNode);//sts->GetVolume()->RemoveNode(stationNode); //AZ
        gGeoManager->CdUp();               // to sts
    }

    for(Int_t o=0; o<removNodes.size(); o++)
        sts->GetVolume()->RemoveNode(removNodes[o]);
    removNodes.clear();


    for (Int_t iNode = 0; iNode <nStat0; iNode++) {
        // Go to station node
        //gGeoManager->CdDown(iNode);
        gGeoManager->CdDown(iNode);
        TGeoNode* stationNode = gGeoManager->GetCurrentNode();

        TString statName = stationNode->GetName();
        if ( ! statName.Contains("station") ) {
            gGeoManager->CdUp();
            continue;
        }
        if(iNode>2){
            Int_t nMod = 2;
            for (Int_t iStatD = 0; iStatD < nMod; iStatD++) {
                gGeoManager->CdDown(0);//stationNode->GetNdaughters()-1);

                TGeoNode* module = gGeoManager->GetCurrentNode();
                //  cout<<" 2 st name del: "<< stationNode->GetName()<<" mod name del: "<<module->GetName() << " i: "<<iStatD<<endl;
                stationNode->GetVolume()->RemoveNode(module); //AZ

                gGeoManager->CdUp();               // to sts
            } } else{  //cout<<" nModules: "<<GetNofModules(stationNode)<<endl;
            Int_t currNodes=GetNofModules(stationNode)/2;
            for (Int_t iStatD = 0; iStatD <currNodes; iStatD++) {
                gGeoManager->CdDown(0);//stationNode->GetNdaughters()-1);
                TString mName = gGeoManager->GetCurrentNode()->GetName();
                if(!mName.Contains("module")) {
                    gGeoManager->CdUp();
                    continue;
                }
                TGeoNode* moduleS = gGeoManager->GetCurrentNode();
                //   cout<<" DELETE VALUME: "<< stationNode->GetName()<<" mod name del: "<<moduleS->GetName() << " i: "<<iStatD<<endl;
                stationNode->GetVolume()->RemoveNode(moduleS); //AZ

                gGeoManager->CdUp();               // to sts
            }

        }

        gGeoManager->CdUp();
    }

    for (Int_t iNode = 0; iNode < sts->GetNdaughters(); iNode++) {

        // Go to station node
        //gGeoManager->CdDown(iNode);
        gGeoManager->CdDown(iNode);
        TGeoNode* stationNode = gGeoManager->GetCurrentNode();
        cout<<" Check-in STATION: "<< stationNode->GetName()<<" zpos : "<<gGeoManager->GetCurrentMatrix()->GetTranslation()[2]<<endl;
        for (Int_t iModule = 0; iModule < stationNode->GetNdaughters() ; iModule++) {
            gGeoManager->CdDown(iModule);
            TGeoNode* moduleNode = gGeoManager->GetCurrentNode();


            TString gName = moduleNode->GetName();
            if(gName.Contains("frame")) {
                stationNode->GetVolume()->RemoveNode(moduleNode); // remove frames
                iModule--;
                gGeoManager->CdUp();
                continue;
            }


            cout<<" Check-in st name : "<< stationNode->GetName()<<" mod name : "<<moduleNode->GetName()<<endl;
            Double_t* sensTrans = gGeoManager->GetCurrentMatrix()->GetTranslation();
            cout<<"trans mod: "<<sensTrans[0]<<" "<<sensTrans[1]<<" "<<sensTrans[2]<< endl;
            for (Int_t iModule2 = 0; iModule2 < moduleNode->GetNdaughters() ; iModule2++) {
                gGeoManager->CdDown(iModule2);
                TGeoNode* moduleNode2 = gGeoManager->GetCurrentNode();

                TString hName = moduleNode2->GetName();
                if(hName.Contains("frame")) {
                    moduleNode->GetVolume()->RemoveNode(moduleNode2); // remove frames
                    iModule2--;
                    gGeoManager->CdUp();
                    continue;
                }

                cout<<" Check-in st name : "<< stationNode->GetName()<<" hot name : "<<moduleNode2->GetName()<<endl;
                sensTrans = gGeoManager->GetCurrentMatrix()->GetTranslation();
                cout<<"trans hot: "<<sensTrans[0]<<" "<<sensTrans[1]<<" "<<sensTrans[2]<< endl;

                gGeoManager->CdUp();
            }
            //  stationNode->GetVolume()->RemoveNode(moduleNode); //AZ
            gGeoManager->CdUp();
        }

        gGeoManager->CdUp();               // to sts
    }

    //exit(0);
    //gGeoManager->GetTopVolume()->Draw("ogl");
    // gGeoManager->Export("SIGEMS_r7_eventDisplay.root","SIGEMS"); //with geomanager

}

ClassImp(CbmBmnStsDigitize)
