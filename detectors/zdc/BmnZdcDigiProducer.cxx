/*************************************************************************************
 *
 *            BmnZdcDigiProducer 
 *    Class to create digital data taken from BmnZdc detector 
 *         
 *  Author:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  18-11-2015
 *
 ************************************************************************************/


#include <iostream>
#include "TClonesArray.h"
#include "TFile.h"

#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "BmnZdcDigiProducer.h"
#include "BmnZdcDigi.h"
#include "BmnZdcPoint.h"

#include "TROOT.h"
#include "TVectorT.h"


// -----   Default constructor   -------------------------------------------
BmnZdcDigiProducer::BmnZdcDigiProducer(const char* name) :
  FairTask(name) {
  fPointArray=0;
  fDigiArray=0;
  fGeoPar=0;
  fHistZdcEn=0;
  fELossZdcValue = NULL;
  fELossZdcHisto = NULL;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
BmnZdcDigiProducer::~BmnZdcDigiProducer() { }
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void BmnZdcDigiProducer::SetParContainers() 
{
   cout << "-I- BmnZdcDigiProducer: SetParContainers started..." << endl;

   //   Get run and runtime database
   FairRunAna* run = FairRunAna::Instance();
   if ( ! run ) Fatal("FairMuchDigitize::SetParContainers", "No analysis run");

   FairRuntimeDb* rtdb = run->GetRuntimeDb();
   if ( ! rtdb ) Fatal("FairMuchDigitize::SetParContainers", "No runtime database");
  
   cout << "-I- BmnZdcDigiProducer: SetParContainers continued..." << endl;

   rtdb->activateParIo(rtdb->getFirstInput());
   //   fGeoPar=( BmnZdcGeoPar*) rtdb->getContainer("BmnZdcGeoPar");
   fGeoPar=( BmnZdcGeoPar*) gROOT->FindObject("BmnZdcGeoPar");
   fGeoPar->print();

   cout << "-I- BmnZdcDigiProducer: SetParContainers finished." << endl;
}

// -------------------------------------------------------------------------
// -----   Public method Init   --------------------------------------------
InitStatus BmnZdcDigiProducer::Init() {
 

  cout << "-I- BmnZdcDigiProducer: Init started..." << endl;

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- BmnZdcDigiProducer::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  
  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject("ZdcPoint");
  if ( ! fPointArray ) {
    cout << "-W- BmnZdcDigiProducer::Init: "
	 << "No ZdcPoint array!" << endl;
    return kERROR;
  }
  
  // Create and register output array
  fDigiArray = new TClonesArray("BmnZdcDigi");  
  ioman->Register("ZdcDigi","Zdc",fDigiArray,kTRUE);

  fELossZdcValue = new TClonesArray("TParameter<double>");
  ioman->Register("ELossZdcValue","Zdc",fELossZdcValue,kTRUE);

  fELossZdcHisto = new TClonesArray("TVectorT<float>");
  ioman->Register("ELossZdcHisto","Zdc",fELossZdcHisto,kTRUE);

  BmnZdcDigiScheme *fDigiScheme  = BmnZdcDigiScheme::Instance();
  fDigiScheme->Init(fGeoPar,0,2);

  cout << "-I- BmnZdcDigiProducer: Intialization successfull" << endl;
  
  return kSUCCESS;

}
// -------------------------------------------------------------------------
void BmnZdcDigiProducer::CreateHistograms ( BmnZdcDigiId_t *pDigiID)
{
  Int_t nx,ny, nz;
  Double_t dx, dy, dz;

  BmnZdcDigiScheme *fDigiScheme  = BmnZdcDigiScheme::Instance();
  // fDigiScheme->GetZdcDimensions (nx,ny,nz);
  // fDigiScheme->GetVolDxDyDz (pDigiID,dx, dy, dz);
  nx = 10 ; ny = 10 ; dx = 75; dy = 75; dz=1010;  // mm
  
  Int_t Nx=nx+2;
  Double_t Dx=dx*Nx/2; 
  Int_t Ny=ny+2;
  Double_t Dy=dy*Ny/2; 

  fHistZdcEn = new TH2F ("HistZdcEn","HistZdcEnergy",Nx,-Dx,Dx,Ny,-Dy,Dy);

  if (!fHistZdcEn) 
    cout << "-E- BmnZdcDigiProducer: HistZdcEn Histogram not created !!" << endl;
  else {
    FairRootManager* ioman = FairRootManager::Instance();
    fHistZdcEn->SetDirectory((TFile*)ioman->GetOutFile());
    fHistZdcEn->Write();
}

}

// -----   Public method Exec   --------------------------------------------
void BmnZdcDigiProducer::Exec(Option_t* opt) {
 

  //#define EDEBUG
#ifdef EDEBUG
  static Int_t lEDEBUGcounter=0;
  cout << "EDEBUG-- BmnZdcDigiProducer::Exec() started... " << endl;;
#endif

  if ( ! fDigiArray ) Fatal("Exec", "No DigiArray");
  
  fDigiArray->Clear();

  BmnZdcDigiScheme *pDigiScheme  = BmnZdcDigiScheme::Instance();

  if (!pDigiScheme) 
    Fatal("BmnZdcDigiProducer::Exec", "No DigiScheme");

  Int_t module_groupID, modID, chanID;
  BmnZdcDigiId_t digiID;
  
  BmnZdcPoint* point  = NULL;

  map<BmnZdcDigiId_t, Float_t> fDigiIdEnergy;
  fDigiIdEnergy.clear();
  map<BmnZdcDigiId_t, Float_t>::const_iterator p;
  
  Int_t nPoints = fPointArray->GetEntriesFast();
  Double_t e1=0;

  if (fHistZdcEn) {
    fHistZdcEn->Reset();
  }
  TH2F* hist1=fHistZdcEn;

  Bool_t flag_of_not_created=1;

  for (Int_t iPoint=0; iPoint<nPoints; iPoint++) {

    point  = (BmnZdcPoint*) fPointArray->At(iPoint);

    Int_t pMMcopy=1; 
    digiID = pDigiScheme->GetDigiIdFromVolumeData  (point->GetDetectorID(), point->GetCopyMother());

    if ((digiID[0]!=-1)&&(digiID[1]!=-1)) {


      if (!fHistZdcEn) {

	CreateHistograms(&digiID);

	hist1=fHistZdcEn;
      }

      if (fDigiIdEnergy.find(digiID)==fDigiIdEnergy.end())
	fDigiIdEnergy[digiID] = point->GetEnergyLoss();
      else
	fDigiIdEnergy[digiID] += point->GetEnergyLoss();

      e1 += point->GetEnergyLoss();
      hist1->Fill(point->GetX()*10,point->GetY()*10,point->GetEnergyLoss()); // in mm
#ifdef EDEBUG
      if (lEDEBUGcounter<20) {
	cout << "EDEBUG-- BmnZdcDigiProducer::Exec:  point : "; point->Print("");
	lEDEBUGcounter++;
      }
#endif

    }
#ifdef EDEBUG
    else {
      if (lEDEBUGcounter<100) {
	cout << "EDEBUG-- BmnZdcDigiProducer::Exec:  Boundary point? : "; point->Print("");
	lEDEBUGcounter++;
      }
    }
#endif
  }

  TClonesArray& clref1 = *fELossZdcValue;
  new(clref1[0]) TParameter<double>("ELossZdc",e1);

  if (fHistZdcEn) {
    TClonesArray& clref1e = *fELossZdcHisto;
    new(clref1e[0]) TVectorT<float>(fHistZdcEn->GetSize(),fHistZdcEn->GetArray());
  }

  Float_t eloss=0;
  
  for(p=fDigiIdEnergy.begin(); p!=fDigiIdEnergy.end(); ++p) {

    pDigiScheme->SplitDigiID((*p).first, module_groupID, modID, chanID);


    if ((module_groupID!=-1)&&(chanID!=-1)) {
      eloss = (*p).second;
      BmnZdcDigi* digi = AddHit(module_groupID, modID, chanID, eloss); 
#ifdef EDEBUG
      if (lEDEBUGcounter<50) {
	cout << "EDEBUG-- BmnZdcDigiProducer::Exec: "<< module_groupID<< " " << chanID << "   " << 
	  (*p).second << "     " << lEDEBUGcounter << endl;
	lEDEBUGcounter++;
      }
#endif
    }

  }
 
#undef EDEBUG
}
// -------------------------------------------------------------------------



// -----   Private method AddDigi   --------------------------------------------
BmnZdcDigi* BmnZdcDigiProducer::AddHit(Int_t module_groupID, Int_t modID, Int_t chanID,Float_t energy)
{
  TClonesArray& clref = *fDigiArray;
  Int_t size = clref.GetEntriesFast();
  BmnZdcDigi* result = new(clref[size]) BmnZdcDigi(module_groupID,modID,chanID,(Double_t)energy);
  //  result->Print();
  return result;
}
// ----


ClassImp(BmnZdcDigiProducer)
