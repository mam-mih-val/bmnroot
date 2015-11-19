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
  fHistZdc1En=0;
  fHistZdc2En=0;
  fELossZdc1Value = NULL, fELossZdc2Value = NULL, fELossZdc1Histo = NULL, fELossZdc2Histo = NULL;
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


  fELossZdc1Value = new TClonesArray("TParameter<double>");
  ioman->Register("ELossZdc1Value","Zdc",fELossZdc1Value,kTRUE);

  fELossZdc2Value = new TClonesArray("TParameter<double>");
  ioman->Register("ELossZdc2Value","Zdc",fELossZdc2Value,kTRUE);

  fELossZdc1Histo = new TClonesArray("TVectorT<float>");
  ioman->Register("ELossZdc1Histo","Zdc",fELossZdc1Histo,kTRUE);

  fELossZdc2Histo = new TClonesArray("TVectorT<float>");
  ioman->Register("ELossZdc2Histo","Zdc",fELossZdc2Histo,kTRUE);  

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
  fDigiScheme->GetZdcDimensions (nx,ny,nz);
  fDigiScheme->GetVolDxDyDz (pDigiID,dx, dy, dz);
  
  Int_t Nx=nx+2;
  Double_t Dx=dx*Nx; 
  Int_t Ny=ny+2;
  Double_t Dy=dy*Ny; 

  fHistZdc1En = new TH2F ("HistZdc1En","HistZdc1Energy",Nx,-Dx,Dx,Ny,-Dy,Dy);
  fHistZdc2En = new TH2F ("HistZdc2En","HistZdc2Energy",Nx,-Dx,Dx,Ny,-Dy,Dy);

  if ((!fHistZdc1En)||(!fHistZdc2En)) 
    cout << "-E- BmnZdcDigiProducer: HistZdc1En or HistZdc2En Histograms not created !!" << endl;
  else {
    FairRootManager* ioman = FairRootManager::Instance();
    fHistZdc1En->SetDirectory((TFile*)ioman->GetOutFile());
    fHistZdc2En->SetDirectory((TFile*)ioman->GetOutFile());
    fHistZdc1En->Write();
    fHistZdc2En->Write();
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
  Double_t e1=0, e2=0;

  if (fHistZdc1En) {
    fHistZdc1En->Reset();
    fHistZdc2En->Reset();
  }
  TH2F* hist1=fHistZdc1En;
  TH2F* hist2=fHistZdc2En;

  Bool_t flag_of_not_created=1;

  for (Int_t iPoint=0; iPoint<nPoints; iPoint++) {

    point  = (BmnZdcPoint*) fPointArray->At(iPoint);

    Int_t pMMcopy=1; 
    digiID = pDigiScheme->GetDigiIdFromVolumeData  (point->GetDetectorID(), point->GetCopyMother());

    if ((digiID[0]!=-1)&&(digiID[1]!=-1)) {


      if (!fHistZdc1En) {

	CreateHistograms(&digiID);

	hist1=fHistZdc1En;
	hist2=fHistZdc2En;
      }

      if (fDigiIdEnergy.find(digiID)==fDigiIdEnergy.end())
	fDigiIdEnergy[digiID] = point->GetEnergyLoss();
      else
	fDigiIdEnergy[digiID] += point->GetEnergyLoss();

      if (pMMcopy==1) {
	e1 += point->GetEnergyLoss();
        hist1->Fill(point->GetX(),point->GetY(),point->GetEnergyLoss());
      }
      else {
	e2 += point->GetEnergyLoss();
        hist2->Fill(point->GetX(),point->GetY(),point->GetEnergyLoss());
      }

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

  TClonesArray& clref1 = *fELossZdc1Value;
  new(clref1[0]) TParameter<double>("ELossZdc1",e1);
  TClonesArray& clref2 = *fELossZdc2Value;
  new(clref2[0]) TParameter<double>("ELossZdc2",e2); 

  if (fHistZdc1En) {
    TClonesArray& clref1e = *fELossZdc1Histo;
    new(clref1e[0]) TVectorT<float>(fHistZdc1En->GetSize(),fHistZdc1En->GetArray());
  }

  if (fHistZdc2En) {
    TClonesArray& clref2e = *fELossZdc2Histo;
    new(clref2e[0]) TVectorT<float>(fHistZdc2En->GetSize(),fHistZdc2En->GetArray());
  }
  
  for(p=fDigiIdEnergy.begin(); p!=fDigiIdEnergy.end(); ++p) {

    pDigiScheme->SplitDigiID((*p).first, module_groupID, modID, chanID);


    if ((module_groupID!=-1)&&(chanID!=-1)) {
      BmnZdcDigi* digi = AddHit(module_groupID, modID, chanID, (*p).second); 
#ifdef EDEBUG
      if (lEDEBUGcounter<20) {
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
  BmnZdcDigi* result = new(clref[size]) BmnZdcDigi(module_groupID,modID,chanID,energy);
  return result;
}
// ----


ClassImp(BmnZdcDigiProducer)
