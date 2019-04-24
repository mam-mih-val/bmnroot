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


#include "../gem/BmnGemStripDigit.h"
#include "../silicon/BmnSiliconDigit.h"
#include "../bmnfield/BmnNewFieldMap.h" //GP

#include <iostream>

#include <iomanip>
#include <map>
#include <vector>
using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::pair;
using std::setw;
using std::left;
using std::right;
using std::fixed;
using std::setprecision;
using std::set;
using std::map;
using std::ios_base;
using std::vector;



#include "TKey.h"

//const Int_t ip_r7[6][2]={0,1, 2,3, 4,5, 6,7, 8,9, 10,11};
 Int_t Pedests[6][4][2][1200];

// -----   Default constructor   ------------------------------------------
CbmBmnStsDigitize::CbmBmnStsDigitize()
  : FairTask("STS Digitizer", 1),
  fGeoPar(NULL),
  fDigiPar(NULL),
  fPoints(NULL),
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
  fPointMap(),
  fBMNEventHeader(0)
{
  fDigiScheme  = new CbmStsDigiScheme();
  Reset();

}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmBmnStsDigitize::CbmBmnStsDigitize(Int_t iVerbose)
  : FairTask("STSDigitize", iVerbose),
  fGeoPar(NULL),
  fDigiPar(NULL),
  fPoints(NULL),
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
  fPointMap(),
  fBMNEventHeader(0)
{
  fDigiScheme  = new CbmStsDigiScheme();
  Reset();
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
CbmBmnStsDigitize::CbmBmnStsDigitize(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose),
  fGeoPar(NULL),
  fDigiPar(NULL),
  fPoints(NULL),
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
  fPointMap(),
  fBMNEventHeader(0)
{
  fGeoPar      = NULL;
  fDigiPar     = NULL;
  fPoints      = NULL;
  fDigis       = NULL;
  fDigisCSC = NULL;
  fDigiMatches = NULL;
  fRealistic   = kFALSE;
  fDigiScheme  = new CbmStsDigiScheme();
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
  if(fBMNEventHeader) {
    fBMNEventHeader->Delete();
    delete fBMNEventHeader;
  }
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
  if ( fDigiScheme ) delete fDigiScheme;
  Reset();
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void CbmBmnStsDigitize::Exec(Option_t* opt) {


  fch++;
  // Reset all eventwise counters
  fTimer.Start();
  Reset(); //AZ
  Int_t stationNr=0;
  Int_t sectorNr=0;
  Int_t side=-1;
  Int_t stripNr=0;
  cout<<"rec ev: "<<fch<<endl;
  FairRunAna *run = FairRunAna::Instance();
  run->GetEventHeader()->SetMCEntryNumber(fch);
 // run->GetEventHeader()->SetFieldScale(((BmnNewFieldMap*) run->GetField())->GetScale());
 
  BmnEventHeader* bevh;
//if(!MC){

Int_t Si_broken=0;


bevh= (BmnEventHeader*)fEventHeader->At(0);
BmnEventHeader* behnew= new ((*fBMNEventHeader)[0]) BmnEventHeader(bevh->GetRunId(), bevh->GetEventId(), bevh->GetEventTime(),  bevh->GetType(),
 bevh->GetTripWord(), bevh->GetTrigInfo(), /*fPointsT0->GetEntries()*/0,fPointsBC1->GetEntries(),fPointsBC2->GetEntries(),fPointsVeto->GetEntries(),fPointsBD->GetEntries(),fPointsSi->GetEntries());

for (Int_t i = 0; i < fPointsSi->GetEntries(); ++i) {
BmnTrigDigit* sitrig =(BmnTrigDigit*)fPointsSi->UncheckedAt(i);
behnew->SetModSi(sitrig->GetMod());
behnew->SetAmpSi(sitrig->GetAmp());
behnew->SetTimeSi(sitrig->GetTime());
//cout<<bdtrig->GetMod()<<endl;

//if(sitrig->GetMod()>=9 && sitrig->GetMod()<=54)
//Si_broken++;

}
for (Int_t i = 0; i < fPointsBD->GetEntries(); ++i) {
BmnTrigDigit* bdtrig =(BmnTrigDigit*)fPointsBD->UncheckedAt(i);
behnew->SetModBD(bdtrig->GetMod());
behnew->SetAmpBD(bdtrig->GetAmp());
behnew->SetTimeBD(bdtrig->GetTime());
//cout<<bdtrig->GetMod()<<endl;
}
for (Int_t i = 0; i < fPointsBC1->GetEntries(); ++i) {
BmnTrigDigit* bc1trig =(BmnTrigDigit*)fPointsBC1->UncheckedAt(i);
behnew->SetModBC1(bc1trig->GetMod());
behnew->SetAmpBC1(bc1trig->GetAmp());
behnew->SetTimeBC1(bc1trig->GetTime());
//cout<<bdtrig->GetMod()<<endl;
}
for (Int_t i = 0; i < fPointsBC2->GetEntries(); ++i) {
BmnTrigDigit* bc2trig =(BmnTrigDigit*)fPointsBC2->UncheckedAt(i);
behnew->SetModBC2(bc2trig->GetMod());
behnew->SetAmpBC2(bc2trig->GetAmp());
behnew->SetTimeBC2(bc2trig->GetTime());
//cout<<bdtrig->GetMod()<<endl;
}
for (Int_t i = 0; i < fPointsVeto->GetEntries(); ++i) {
BmnTrigDigit* vetotrig =(BmnTrigDigit*)fPointsVeto->UncheckedAt(i);
behnew->SetModVETO(vetotrig->GetMod());
behnew->SetAmpVETO(vetotrig->GetAmp());
behnew->SetTimeVETO(vetotrig->GetTime());
//cout<<bdtrig->GetMod()<<endl;
}
 /*
 //for (Int_t i = 0; i < nTracks; ++i) {
 //        CbmStsTrack *tr = (CbmStsTrack *)stsTrack->UncheckedAt(i);

 ////cout<<"ddd"<<endl;
} */
 //for(Int_t i=0; i<fEventHeader->GetEntriesFast(); i++){
// BmnEventHeader* bevh2=(BmnEventHeader*)fEventHeader->At(i);
 //cout<<" time: "<< (bevh->GetEventTime()).AsSQLString()<<endl;
// }


 //cout<<" fRunID: "<<bevh->GetEventId()<<endl;

 // cout<<"NUM OF POINTS: "<<fPoints->GetEntries()<<endl;
  Int_t st1=0,st2=0,st3=0,st4=0,st5=0,st6=0,st7=0;
  /*for (Int_t iPoint=0; iPoint<fPoints->GetEntriesFast(); iPoint++)
    {
    BmnGemStripDigit* stdig=  (BmnGemStripDigit*)fPoints->At(iPoint) ;

    if(stdig->GetStation()==0) st1++;
    if(stdig->GetStation()==1) st2++;
    if(stdig->GetStation()==2) st3++;
    if(stdig->GetStation()==3) st4++;
    if(stdig->GetStation()==4) st5++;
    if(stdig->GetStation()==5) st6++;
    if(stdig->GetStation()==6) st7++;

    }
  */
  /*
Int_t GEM[8]={0,0,0,0,0,0,0,0}; 
  for (Int_t iPoint=0; iPoint<fPoints->GetEntriesFast(); iPoint++) {
  
    BmnGemStripDigit* stdig=  (BmnGemStripDigit*)fPoints->At(iPoint) ;
   // cout<<" Event: "<<fch<<" ST: "<<stdig->GetStation()<<" Mod: "<<stdig->GetModule()<<" Layer: "<<stdig->GetStripLayer()<<" strip Num: "<<stdig->GetStripNumber()<<
    //  " signal: "<<stdig->GetStripSignal()<<endl;
GEM[stdig->GetStation()-1]+=1;

  } */




  if( true /* fPointsT0->GetEntriesFast()==1 && fPointsBC2->GetEntriesFast()==1 && fPointsVeto->GetEntriesFast()==0*/ /*Si_broken>=2*/ 
  /*GEM[0] > 10 && GEM[1] > 10 && GEM[3] > 10 && GEM[4] > 10 && GEM[5] > 10 && GEM[6] > 10*/ )
{
//cout<<" !!!!!!!!!!!!!!!!!!!! signal from ST: "<<fPoints->GetEntriesFast()<<endl;
Int_t nDigis = 0, nDigisCSC=0;
Int_t stripDXt=0,stripDUt=0, stripDXb=0,stripDUb=0;

fchtrig++;

//if(fchtrig >60000) exit(0);


//if(false)
  
//if(!MC)
  for (Int_t iPointSI=0; iPointSI<fPointsSI->GetEntriesFast(); iPointSI++) {
  //  if(fchtrig<120000 || fchtrig>130000) continue;
//if(fchtrig<50000 || fchtrig>150000) continue;

  BmnSiliconDigit* sidig=  (BmnSiliconDigit*)fPointsSI->At(iPointSI);
  /*stripDXt=0;
  stripDUt=0;
  stripDXb=0;
  stripDUb=0;*/
    // cout<<" Event: "<<fch<<" SI: "<<sidig->GetStation()<<" Mod: "<<sidig->GetModule()<<" Layer: "<<sidig->GetStripLayer()<<" strip Num: "<<sidig->GetStripNumber()<< " signal: "<<sidig->GetStripSignal()<<endl;
Double_t sisig=sidig->GetStripSignal();
////if(sisig>4095) sisig=4095; //continue;




Int_t stripNrSI=sidig->GetStripNumber()-1; // GP kapishin MYSILICON start from num. 1 
Int_t stationNrSI=sidig->GetStation();
Int_t sectorNrSI=-100; // start from num. 1
Int_t sideSI=-1;

if(sidig->GetStripLayer()==1) sideSI=1;
else if(sidig->GetStripLayer()==0) sideSI=0;

//if(sidig->GetStripLayer()==1) sectorNrSI=sidig->GetModule()*2-1;
//else if(sidig->GetStripLayer()==0) sectorNrSI=sidig->GetModule()*2;
Int_t simod=-100;
sectorNrSI=sidig->GetModule()+1;

// cout<<" Event: "<<fch<<" SI: "<<stationNrSI<<" Side: "<<sideSI<<" Sector: "<<sectorNrSI<<" strip Num: "<<stripNrSI<<" signal: "<<sisig<<endl;

/*
if(simod==0)sectorNrSI=1; //noDATA
 //cout<<"sec: "<<sectorNrSI<<endl;
if(simod==7)sectorNrSI=2;
if(simod==4)sectorNrSI=3;
if(simod==1)sectorNrSI=4;

if(simod==3)sectorNrSI=5; //noDATA

if(simod==5)sectorNrSI=6;
if(simod==6)sectorNrSI=7;
if(simod==2)sectorNrSI=8;

if(bevh->GetRunId()<=1500) {
  stripDXt=2;
  stripDUt=2;
  stripDXb=2;
  stripDUb=2;
} else if(bevh->GetRunId()>=1502 && bevh->GetRunId()<=1505 ){
  stripDXt=4;
  stripDUt=4;
  stripDXb=2;
  stripDUb=2;
}else if(bevh->GetRunId()>=1714 && bevh->GetRunId()<=1803 ){
  stripDXt=2;
  stripDUt=2;
  stripDXb=0;
  stripDUb=0;
}

if(sectorNrSI<5 ){
if(sideSI==0) stripNrSI=stripNrSI-stripDXt; 
if(sideSI==1) stripNrSI=stripNrSI-stripDUt;

} else if(sectorNrSI>4){
if(sideSI==0) stripNrSI=stripNrSI-stripDXb; 
if(sideSI==1) stripNrSI=stripNrSI-stripDUb;
} */

//stripNrSI=1;
//if(sideSI==1) stripNrSI=614; //debug

 new ((*fDigis)[nDigis++]) CbmStsDigi(stationNrSI, sectorNrSI, sideSI, stripNrSI, sisig, 0); //AZ

  }

 
  for (Int_t iPoint=0; iPoint<fPoints->GetEntriesFast(); iPoint++) {
   // if(fchtrig<50000 || fchtrig>150000) continue;
   //  if(fchtrig<120000 || fchtrig>130000) continue;
    //if(fPoints->GetEntriesFast()>600) continue;
    //if(fch==13016) continue;
    //if(fPoints->GetEntriesFast()/10 > 10) continue;
    //if(st2>10 || st3>10 || st4>10 || st5>10 || st6>10 || st7>10) continue;

    BmnGemStripDigit* stdig=  (BmnGemStripDigit*)fPoints->At(iPoint) ;
   // cout<<" Event: "<<fch<<" ST: "<<stdig->GetStation()<<" Mod: "<<stdig->GetModule()<<" Layer: "<<stdig->GetStripLayer()<<" strip Num: "<<stdig->GetStripNumber()<<
    //  " signal: "<<stdig->GetStripSignal()<<endl;
 Double_t stsig= stdig->GetStripSignal();
//if(!MC && stsig>4095) stsig=4095; // GP FIX bug with "zero digits"
//else if(MC) stsig=stsig/1000;
//if(MC) stsig=stsig/100;
// if(stsig<1 )  continue;
    //  stationNr=stdig->GetStation()+1;
    //Double_t zPos[8]={ 50., 635.6, 1055.5, 840.7, 420.1, 1259.9, 1468., 1468.}; // Station=0,4,1,3,2,5,6
    /*  if(stdig->GetStation()==0) stationNr=1;
	if(stdig->GetStation()==1) stationNr=3;
	if(stdig->GetStation()==2) stationNr=5;
	if(stdig->GetStation()==3) stationNr=4;
	if(stdig->GetStation()==4) stationNr=2;
	if(stdig->GetStation()==5) stationNr=6;
	if(stdig->GetStation()==6) stationNr=7;

	stripNr=stdig->GetStripNumber();

	if(stationNr!=1)
	{ //for standard and big station
        if( (stdig->GetStripLayer()==0 || stdig->GetStripLayer()==1) && stdig->GetModule()==0) sectorNr=1;
        if( (stdig->GetStripLayer()==2 || stdig->GetStripLayer()==3) && stdig->GetModule()==0) sectorNr=2;

        if( (stdig->GetStripLayer()==0 || stdig->GetStripLayer()==1) && stdig->GetModule()==1) sectorNr=3;
        if( (stdig->GetStripLayer()==2 || stdig->GetStripLayer()==3) && stdig->GetModule()==1) sectorNr=4;

        }   else
	{
	if( (stdig->GetStripLayer()==2 || stdig->GetStripLayer()==3) && stdig->GetModule()==0) sectorNr=1; //for small station
	}

        if( (stdig->GetStripLayer()==0 || stdig->GetStripLayer()==2) ) side=0; //true for small station
        if( (stdig->GetStripLayer()==1 || stdig->GetStripLayer()==3) ) side=1; //true for small station

	//    if( (stdig->GetStripLayer()==0 || stdig->GetStripLayer()==2) && (stationNr==3 || stationNr==5 )) side=1;  //for inversion station
	//    if( (stdig->GetStripLayer()==1 || stdig->GetStripLayer()==3) && (stationNr==3 || stationNr==5 )) side=0; //for inversion station
	*/




  stripNr=stdig->GetStripNumber()-1; // GP kapishin STRIPGEM start from num. 1

 
  if(stdig->GetStation()==5 && stdig->GetModule()==1 && stdig->GetStripLayer()==2 && stdig->GetStripNumber()>128 && stdig->GetStripNumber()<257)
  stripNr=stripNr-2;
 
 
    //if(stdig->GetStation()>7) continue;

    //if (stdig->GetStation() == 2) continue; //AZ

    stationNr=stdig->GetStation();

    if(stationNr==3) continue;
    else if(stationNr>3) stationNr=stationNr-1;
  //  cout<<" stN: "<<stationNr<<endl;

    //AZ if (stationNr!=1) { //for standard and big station
   // if (stationNr>0) { //for standard and big station

Int_t modNum=stdig->GetModule();
/*
if(stdig->GetStation()%2 != 0){
  if(stdig->GetModule()==0) modNum=1;
  else if(stdig->GetModule()==1) modNum=0;
} */



      if( (stdig->GetStripLayer()==0 || stdig->GetStripLayer()==1) && modNum==0) sectorNr=1;
      if( (stdig->GetStripLayer()==2 || stdig->GetStripLayer()==3) && modNum==0) sectorNr=2;


      if( (stdig->GetStripLayer()==0 || stdig->GetStripLayer()==1) && modNum==1) sectorNr=3;
      if( (stdig->GetStripLayer()==2 || stdig->GetStripLayer()==3) && modNum==1) sectorNr=4;

      //if(sectorNr==2 || sectorNr==4) continue;

if(stationNr==7) sectorNr=stdig->GetModule()*2+stdig->GetStripLayer()/2;

/*
if(sectorNr==2 && stdig->GetStation()==2){
stripNr=1019-stdig->GetStripNumber();
} */ 

/*      if (stationNr==6 ) { //for standard and big station
        if( (stdig->GetStripLayer()==0 || stdig->GetStripLayer()==1) && stdig->GetModule()==1) sectorNr=1;
        if( (stdig->GetStripLayer()==2 || stdig->GetStripLayer()==3) && stdig->GetModule()==1) sectorNr=2;


        if( (stdig->GetStripLayer()==0 || stdig->GetStripLayer()==1) && stdig->GetModule()==0) sectorNr=3;
        if( (stdig->GetStripLayer()==2 || stdig->GetStripLayer()==3) && stdig->GetModule()==0) sectorNr=4;
      }
*/


   // } 

    if( (stdig->GetStripLayer()==0 || stdig->GetStripLayer()==2) ) side=0; //true for small station
    if( (stdig->GetStripLayer()==1 || stdig->GetStripLayer()==3) ) side=1; //true for small station
    
    /*if(Pedests[stationNr-2][sectorNr-1][side][stripNr]<0) continue;
    Double_t pSubStrSignal=stsig-Pedests[stationNr-2][sectorNr-1][side][stripNr];
    if(pSubStrSignal<=20) continue;
    
    stsig=pSubStrSignal;

*/
//stripNr=0; //debug
//stripNr=1; //debug
//---------------------------------end AZ adaptive threshold adjustment --------------------------
//if(sectorNr%2==0) continue;
  /* if(writeDigi) */   if(stationNr<7) new ((*fDigis)[nDigis++]) CbmStsDigi(stationNr+3, sectorNr, side, stripNr, stsig, 0); //AZ
                          else if(stationNr==7)  new ((*fDigisCSC)[nDigisCSC++]) CbmStsDigi(stationNr+3, sectorNr, side, stripNr, stsig, 0); //AZ

  } // for (Int_t iPoint=0;
  }
//exit(0);
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

  fPointsSI = (TClonesArray*) ioman->GetObject("MYSILICON");//StsPoint

  fEventHeader = (TClonesArray*) ioman->GetObject("EventHeader");//StsPoint


//fPointsT0=(TClonesArray*) ioman->GetObject("T0");     /** Input array of CbmStsDigi from T0 **/
  fPointsBC1=(TClonesArray*) ioman->GetObject("BC1"); 
  fPointsBC2=(TClonesArray*) ioman->GetObject("BC2");     /** Input array of CbmStsDigi from BC2 **/
  fPointsVeto=(TClonesArray*) ioman->GetObject("VC");      /** Input array of CbmStsDigi from Veto **/
  fPointsBD=(TClonesArray*) ioman->GetObject("BD");      /** Input array of CbmStsDigi from Veto **/
    fPointsSi=(TClonesArray*) ioman->GetObject("Si");      /** Input array of CbmStsDigi from Veto **/



 // fPointsT0=(TClonesArray*) ioman->GetObject("T0");     /** Input array of CbmStsDigi from T0 **/
//  fPointsBC2=(TClonesArray*) ioman->GetObject("BC2");     /** Input array of CbmStsDigi from BC2 **/
//  fPointsVeto=(TClonesArray*) ioman->GetObject("VETO");      /** Input array of CbmStsDigi from Veto **/
 // fPointsBD=(TClonesArray*) ioman->GetObject("BD");      /** Input array of CbmStsDigi from Veto **/
 //   fPointsSi=(TClonesArray*) ioman->GetObject("Si");      /** Input array of CbmStsDigi from Veto **/

    // Register output array BMNEventHeader
  fBMNEventHeader = new TClonesArray("BmnEventHeader",1000);
  ioman->Register("EventHeaderBmn", "BMN raw event header", fBMNEventHeader, kTRUE);

  fDigiMatches = new TClonesArray("CbmStsDigiMatch",1000);
  ioman->Register("StsDigiMatch", "Digi Match in STS", fDigiMatches, kTRUE);

  // Register output array StsDigi
  fDigis = new TClonesArray("CbmStsDigi",1000);
  ioman->Register("StsDigi", "Digital response in STS", fDigis, kTRUE);

   // Register output array StsDigi
  fDigisCSC = new TClonesArray("CbmStsDigi",1000);
  ioman->Register("StsDigiCSC", "Digital response in STS", fDigisCSC, kTRUE);

  GetBmnGeom(fGeoFile.Data()); //GP
  ApplyAlignment(); //AZ
  gGeoManager->CheckOverlaps(); //AZ
  gGeoManager->PrintOverlaps(); //AZ

  //exit(0);

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
  if( fBMNEventHeader ) fBMNEventHeader->Delete();
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


Bool_t CbmBmnStsDigitize::GetBmnGeom(const char* fileName)
{
  // ---> Get TGeoManager and top node (TOP)

  TFile* geoFile = new TFile(fileName, "READ");
  if ( ! geoFile->IsOpen() ) {
    cout << "-E- Could not open file!" << endl;
    return kFALSE;
  }
  geoFile->FindObjectAny("SIGEMS");//"GEMS_geom SIGEMS"); //AZ
  return kTRUE; //AZ

  // ---> Get TOP node from file
  TList* keyList = geoFile->GetListOfKeys();
  TIter next(keyList);
  TKey* key = NULL;
  TGeoVolume* top = NULL;
  while ( (key = (TKey*)next() ) ) {
    TString className(key->GetClassName());
    if ( className.BeginsWith("TGeoVolume") ) {
      top = dynamic_cast<TGeoVolume*> (key->ReadObj() );
      std::cout << "Found volume " << top->GetName() << endl;
      break;
    }
  }
  if ( ! top ) {
    cout << "-E- Could not find volume object in file" << endl;
    return kFALSE;
  }
  //cout << gGeoManager << endl;
  gGeoManager->GetListOfVolumes()->ls();
  gGeoManager->SetTopVolume(top);
   
  return kTRUE;
}

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
 /*
 Double_t xAlign[26] = 
 {3.32829,-2.67635,2.83055,-3.16945, //I Si
  2.84975, -3.11505, //II Si
  8.91718,2.91888,-3.09552,-9.09292,  8.91618,2.91618,-3.08382,-9.08382, //III Si
  -40.716, 40.914, 41.085, -40.598, 42.278, -39.429, -38.45, 43.25,  44.076, -37.596,  -36.8092, 44.8207};//GEM
 //{-40.8258, 40.8258, 40.845, -40.845, 40.8608, -40.8608, -40.8578, 40.8578, 40.845, -40.845, -40.8298, 40.8298}; //for 4 GeV

  Double_t yAlign[26] =
  {4.46297,5.07597, -1.55503,-1.05503, //I Si
   1.8115,1.42, //II Si
   7.84856,7.80306,7.92256,7.85506,-4.25594,-4.25594, -4.25594,-4.25594, // III Si  
   22.5211, 22.2759, 22.574, 22.6124,  22.8827,  22.516, 22.5385, 22.9377, 22.5828, 22.3481, 22.7875, 23.007};//GEM
  //{22.2753, 22.4706, 22.4851, 22.6689, 22.7851, 22.5916,  22.3308, 23.1519, 22.4786, 22.4484, 22.5971, 23.2184}; //for 4 GeV

   Double_t zAlign3[9] =
   {14.365-0.025, 17.69-0.05, 26.0775, //3 planes of Si
    42.2,67.77,115.569,137.8325,163.54,186.103}; //6 planes of GEM

  Double_t zAlign23[26] = 
  {-0.315,0.315,0.365,-0.365, // I Si
   -0.33,0.33, //II Si
    0.8325,-0.7675, 0.7525,  -0.8175,    -0.4275,  1.1825, -0.2875, 1.3225, //III Si                 
    0,0,  0,0,  -0.085, 0.085,  -0.0175,0.0175,  0.01,-0.01, 0,0}; //GEMS sensors
  //{ 0.14595, -0.14595, -0.1258, 0.1258, -0.2005, 0.2005, 0.0945, -0.0945, -0.1125, 0.1125, 0.0865, -0.0865 }; //for 4 GeV
*/
 Double_t xAlign[ 26 ]=
{3.33029,-2.67835,2.82585 ,-3.14615, //I Si
  2.85175, -3.11705, //II Si
  8.91918 ,2.92088, -3.09752,-9.09492,  8.91198 ,2.94528 ,-2.98182,-8.98402, //III Si
  -40.718 , 40.916, 41.087, -40.6, 42.28, -39.431, -38.452, 43.252,  44.078, -37.598,  -36.8112, 44.8227};//GEM

 Double_t yAlign[ 26 ]=
  {4.46297,5.07597, -1.48803,-0.910531, //I Si
   1.8115,1.42, //II Si
   7.84856,7.80306,7.92256,7.85506,-4.57154,-4.61864, -4.43934,-4.51954, // III Si  
  // 22.5211-0.17, 22.2759-0.17, 22.574-0.15, 22.6124-0.15,  22.8827-0.1,  22.516-0.1, 22.5385-0.05, 22.9377-0.05, 22.5828+0.025, 22.3481+0.025, 22.7875+0.07, 23.007+0.07};//GEM
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
  //for (Int_t iNode = 1; iNode < sts->GetNdaughters(); iNode++) {
  for (Int_t iNode = 0; iNode < nStat0; iNode++) {
    // Go to station node
    gGeoManager->CdDown(iNode);
    TGeoNode* stationNode = gGeoManager->GetCurrentNode();
    TString statName = stationNode->GetName();
    if ( ! statName.Contains("station") ) {
      gGeoManager->CdUp();
      continue;
    }
    //AZ Int_t statNr = stationNode->GetNumber();
    ++stn; //AZ
/*
    if(stn<=3){
gGeoManager->CdUp();
 continue;}

 if(stn<=3) continue;
*/
 ++ statNr;

    TGeoHMatrix *matr = gGeoManager->GetCurrentMatrix();
    Double_t* statTrans = matr->GetTranslation();
    //Double_t* statRot = matr->GetRotationMatrix();
    TGeoRotation *r2;
  //if(statNr-1 >1)  statTrans[2] = zAlign3[statNr-1]+driftcorr;//zAlign[statNr];
  cout<<"statNr : "<<statNr <<", "<<statTrans[2]  <<" zal: "<<zAlign3[statNr-1]<<endl;
   statTrans[2] = zAlign3[statNr-1];//zAlign[statNr];

   TGeoTranslation *t2 = new TGeoTranslation(statTrans[0],statTrans[1],statTrans[2]);
  //if( statNr==2 || statNr==3 || statNr==5)r2 = new TGeoRotation("rot",0,180,180);
   // if(statNr==1 || statNr==4 || statNr==6) r2 = new TGeoRotation("rot",0,180,180);
     r2 = new TGeoRotation("rot",0,0,0);

      TGeoCombiTrans *cc1 = new TGeoCombiTrans(*t2,*r2);
    //  statTrans[0] = xAlign[statNr];
    //  statTrans[1] = yAlign[statNr];

    //  cout<<"ST TRANS Z: "<<statTrans[2]<<endl;

   //if(statNr-1 >1)  statTrans[2]=statTrans[2]+driftcorr;
    //cout<<"ST TRANS Z: "<<statTrans[2]<<endl;
    //if(iNode>0) statTrans[2]=statTrans[2]-2.47;
    //matr->SetTranslation(statTrans);
    //matr->SetMatrix(statRot);

    TGeoHMatrix *matr0 = new TGeoHMatrix(*cc1);
    matr0->RegisterYourself();
    
    //  Int_t nModules = stationNode->GetNdaughters();
    Int_t nModules = GetNofModules(stationNode);
    //  cout<<"nModules: "<<nModules<<endl;
    
    //sts->GetVolume()->ReplaceNode(stationNode,0,gGeoManager->GetCurrentMatrix()); //AZ
    //sts->GetVolume()->RemoveNode(stationNode); //AZ
    sts->GetVolume()->AddNode((TGeoVolumeAssembly*)stationNode->GetVolume(),0,matr0); //AZ

    //AZ- hard-coded st_node=sts->GetVolume()->GetNode(iNode+6);
    st_node = (TGeoNode*) sts->GetVolume()->GetNodes()->Last(); //AZ

   // Double_t  statZ = statTrans[2];
   //cout <<"sta: " << statNr << " " << gGeoManager->GetCurrentMatrix()->GetTranslation()[2] << " " << sts->GetNdaughters() << endl;

    //gGeoManager->CdUp();               // to sts

    //-----------------------module translate-------------
    Int_t moduleNr = 0, copy_no = 0;
    //cout<<"nMODULES: "<< nModules<<endl;
    /*if(iNode<2){
    //  nMod0sts=nModules;
      nModules=nModules*2;
      
      } */
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
	 // fprintf(parFile, "%4d %4d\n", moduleNr, 1);
	  // ---> Sensor number
   // geoMan->CdUp(); // back to module sensor
	 // SaveSensor(geoMan, parFile, phiStat, sensor);
	TGeoHMatrix *matrMod = gGeoManager->GetCurrentMatrix();
	Double_t* modTrans = matrMod->GetTranslation();
  //cout<<"trans: "<<modTrans[2]<<endl;
  modTrans[0] = xAlign[nOfModSi];
  modTrans[1] = yAlign[nOfModSi];
  modTrans[2] = zAlign23[nOfModSi];//zAlign2[ipNr];
	matrMod->SetTranslation(modTrans);
	TGeoHMatrix *matr0Mod = new TGeoHMatrix(*matrMod);
	matr0Mod->RegisterYourself();

	//sts->GetVolume()->ReplaceNode(stationNode,0,gGeoManager->GetCurrentMatrix()); //AZ
	//sts->GetVolume()->RemoveNode(stationNode); //AZ
	//  stationNode->GetVolume()->AddNode((TGeoVolumeAssembly*)module->GetVolume(),0,matr0Mod); //AZ
	
	//cout<<" 1 st name add: "<< stationNode->GetName()<<" mod name add: "<<module->GetName()<< " "<< module<< " i: "<<iStatD<<" cols: "<<st_node->GetVolume()->GetNdaughters()<<endl;
	//Double_t* sensTrans = matrMod->GetTranslation();
	//cout<<"trans: "<<sensTrans[0]<<" "<<sensTrans[1]<<" "<<sensTrans[2]<< " Nr mod:  "<<moduleNr<<endl;
	//stationNode->GetVolume()->AddNode((TGeoVolumeAssembly*)module->GetVolume(),copy_no,matr0Mod);
	st_node->GetVolume()->AddNode((TGeoVolumeAssembly*)module->GetVolume(),copy_no,matr0Mod);
	//cout<<" 1 st name add: "<< stationNode->GetName()<<" mod name add: "<<module->GetName() << " "<< module<< " i: "<<iStatD<<" cols: "<<st_node->GetVolume()->GetNdaughters()<<endl;
	//Double_t  modZ = modTrans[2];
   Double_t* modTrans1 = gGeoManager->GetCurrentMatrix()->GetTranslation();
//	cout <<"mod: " << nOfModSi << " VEC: " << modTrans1[0] << " "<<modTrans1[1] << " "<<modTrans1[2] << endl;
	copy_no++;
	//delete matr0Mod;
  gGeoManager->CdUp();  // back to module

  SensCh++;

}
//-------------------------add I II III Si----------------------------

      
 if(iNode>2){
      if (TString(module->GetName()).Contains("Senso")) {
	//if(iNode>0 && iNode<=3) moduleNr=0;
	//else{
	  if(iStatD==0) moduleNr=0;
	  if(iStatD>0) moduleNr=1;
//	}
	//moduleNr=iStatD;
	//fprintf(parFile, "%4d %4d\n", moduleNr, 1);
	//SaveSensor(geoMan, parFile, phiStat, module);

	Int_t ipNr = 0;
  //cout<<" iNode: "<< iNode<<endl;
	//if (iNode>0 && iNode<=3) ipNr = iNode;
	//else ipNr = iNode*2 - 4 + moduleNr;
//Int_t order=-1;
//if( (statNr==1 || statNr==4 || statNr==6) && moduleNr==0) order=1; 
  

	TGeoHMatrix *matrMod = gGeoManager->GetCurrentMatrix();
	Double_t* modTrans = matrMod->GetTranslation();
  //cout<<" modtransX: "<<modTrans[0]<<" xali: "<<xAlign[ip_r7[statNr-1][moduleNr]]<< "sub: "<<xAlign[ip_r7[statNr-1][moduleNr]]-modTrans[0]  <<endl;
	 cout<<" aaaa ^^^^^^^^^^^^^^^^^^^ : "<<statNr-1;
  cout<<"\n"<<moduleNr;
  cout<<"\n"<<ip_r7[ (statNr-1)-3 ][moduleNr]<<endl;

   modTrans[0] = xAlign[ip_r7[(statNr-1)-3][moduleNr]+14];
	 modTrans[1] = yAlign[ip_r7[(statNr-1)-3][moduleNr]+14];// - modTrans[1]; //not need anymore (- mod...)
   modTrans[2] = zAlign23[ip_r7[(statNr-1)-3][moduleNr]+14];//zAlign2[ipNr];
  //cout<<"ST TRANS Z: "<<zAlign23[ipNr]<<endl;
	matrMod->SetTranslation(modTrans);
	TGeoHMatrix *matr0Mod = new TGeoHMatrix(*matrMod);
	matr0Mod->RegisterYourself();
  ipNr++;
//cout<<" ip N: "<< ipNr<<endl;
	//sts->GetVolume()->ReplaceNode(stationNode,0,gGeoManager->GetCurrentMatrix()); //AZ
	//sts->GetVolume()->RemoveNode(stationNode); //AZ
	//  stationNode->GetVolume()->AddNode((TGeoVolumeAssembly*)module->GetVolume(),0,matr0Mod); //AZ
	
	//cout<<" 1 st name add: "<< stationNode->GetName()<<" mod name add: "<<module->GetName()<< " "<< module<< " i: "<<iStatD<<" cols: "<<st_node->GetVolume()->GetNdaughters()<<endl;
	//Double_t* sensTrans = matrMod->GetTranslation();
	//cout<<"trans: "<<sensTrans[0]<<" "<<sensTrans[1]<<" "<<sensTrans[2]<< " Nr mod:  "<<moduleNr<<endl;
	//stationNode->GetVolume()->AddNode((TGeoVolumeAssembly*)module->GetVolume(),copy_no,matr0Mod);
	st_node->GetVolume()->AddNode((TGeoVolumeAssembly*)module->GetVolume(),copy_no,matr0Mod);
	//cout<<" 1 st name add: "<< stationNode->GetName()<<" mod name add: "<<module->GetName() << " "<< module<< " i: "<<iStatD<<" cols: "<<st_node->GetVolume()->GetNdaughters()<<endl;
	//Double_t  modZ = modTrans[2];
	//cout <<"mod: " << ipNr << " VEC: " << modTrans[0] << " "<<modTrans[1] << " "<<modTrans[2] << endl;
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
      Double_t* sensTrans = gGeoManager->GetCurrentMatrix()->GetTranslation();
        cout<<"trans hot: "<<sensTrans[0]<<" "<<sensTrans[1]<<" "<<sensTrans[2]<< endl;

 gGeoManager->CdUp();
        }
      //  stationNode->GetVolume()->RemoveNode(moduleNode); //AZ
      gGeoManager->CdUp();
    }
    
    gGeoManager->CdUp();               // to sts
  }
 
  
//exit(0);
//gGeoManager->SetVisLevel(500);
//gGeoManager->GetTopVolume()->SetTransparency(0);
//gGeoManager->GetTopVolume()->Draw("ogl");
// gGeoManager->Export("SIGEMS_r7_eventDisplay.root","SIGEMS"); //with geomanager

//Int_t c;
//cin>>c;

}

ClassImp(CbmBmnStsDigitize)
