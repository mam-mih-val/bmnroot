/*************************************************************************************
 *
 *            BmnNdetDigitizer 
 *    Class to create digital data taken from BmnNdet detector 
 *         
 *  Author:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  18-11-2015
 *  Modified by M.Golubeva July 2022
*
 ************************************************************************************/


#include <iostream>
#include "TClonesArray.h"
#include "TFile.h"

#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "BmnNdetDigitizer.h"
#include "BmnNdetDigit.h"
#include "BmnNdetPoint.h"

#include "TROOT.h"
#include "TVectorT.h"


// -----   Default constructor   -------------------------------------------
BmnNdetDigitizer::BmnNdetDigitizer(const char* name) :
  FairTask(name) {
  fPointArray=0;
  fDigiArray=0;
  fGeoPar=0;
  //fHistNdetEn=0;
  //fELossNdetValue = NULL;
  //fELossNdetHisto = NULL;
   fPix2Mip   = 15;    // 15 MPPC pixels per MIP
   fMIPEnergy = 0.005; // 5 MeV
   fMIPNoise  = 0.3;   // 0.2 MIP noise level
   fMIP2GeV   = 0.005;

   fMappingFile_sections = "nofile";
   fMappingFile_modules = "nofile";
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
BmnNdetDigitizer::~BmnNdetDigitizer() { }
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void BmnNdetDigitizer::SetParContainers() 
{
   cout << "-I- BmnNdetDigitizer: SetParContainers started..." << endl;

   //   Get run and runtime database
   FairRunAna* run = FairRunAna::Instance();
   if ( ! run ) Fatal("FairMuchDigitize::SetParContainers", "No analysis run");

   FairRuntimeDb* rtdb = run->GetRuntimeDb();
   if ( ! rtdb ) Fatal("FairMuchDigitize::SetParContainers", "No runtime database");
  
   cout << "-I- BmnNdetDigitizer: SetParContainers continued..." << endl;

   rtdb->activateParIo(rtdb->getFirstInput());
   //   fGeoPar=( BmnNdetGeoPar*) rtdb->getContainer("BmnNdetGeoPar");
   fGeoPar=( BmnNdetGeoPar*) gROOT->FindObject("BmnNdetGeoPar");
   fGeoPar->print();

   cout << "-I- BmnNdetDigitizer: SetParContainers finished." << endl;
}

// -------------------------------------------------------------------------
// -----   Public method Init   --------------------------------------------
InitStatus BmnNdetDigitizer::Init() {
 

  cout << "-I- BmnNdetDigitizer: Init started..." << endl;
   fRandom3 = new TRandom3();

   Int_t nbMods=225;
   Int_t nbSect=15;

   // Get nDet module mapping (if mapping file is set)
   for (int i = 0; i < 1000; i++) { fModuleX[i] = -10000; fModuleY[i] = -10000; }

   if (!(fMappingFile_modules == "nofile")) {

     TString dummy;
     ifstream in;

     TString dir = getenv("VMCWORKDIR");
     TString path = dir + "/input/";
     in.open((path + fMappingFile_modules).Data());
     if (!in.is_open())
     {
         printf("Loading nDet modules Map from file: %s - file open error!\n", fMappingFile_modules.Data());
         return kFATAL;
     }
     printf("Loading nDet modules Map from file: %s\n", fMappingFile_modules.Data());
     in >> dummy >> dummy >> dummy;
     while (!in.eof()) {
         int id;
         float x,y;
         in >> id >> x >> y;
         if (!in.good()) break;
         printf("%d %f %f\n",id,x,y);
         fModuleX[id] = x;
         fModuleY[id] = y;
     }

     in.close();

   }// if (!(fMappingFile == "nofile"))

   // Get nDet sectio mapping (if mapping file is set)
   for (int i = 0; i < 70; i++) { fSectionZ[i] = -10000; }

   if (!(fMappingFile_sections == "nofile")) {

     TString dummy;
     ifstream in;

     TString dir = getenv("VMCWORKDIR");
     TString path = dir + "/input/";
     in.open((path + fMappingFile_sections).Data());
     if (!in.is_open())
     {
         printf("Loading nDet sections Map from file: %s - file open error!\n", fMappingFile_sections.Data());
         return kFATAL;
     }
     printf("Loading nDet sections_Map from file: %s\n", fMappingFile_sections.Data());
     in >> dummy >> dummy;
     while (!in.eof()) {
         int id;
         float z;
         in >> id >> z;
         if (!in.good()) break;
         printf("%d %f\n",id,z);
         fSectionZ[id] = z;
     }

     in.close();

   }// if (!(fMappingFile == "nofile"))


  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- BmnNderDigiProducer::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  
  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject("NdetPoint");
  if ( ! fPointArray ) {
    cout << "-W- BmnNdetDigitizer::Init: "
	 << "No NdetPoint array!" << endl;
    return kERROR;
  }
  
  // Create and register output array
  fDigiArray = new TClonesArray("BmnNdetDigit");  
  ioman->Register("NdetDigi","Ndet",fDigiArray,kTRUE);
 
  BmnNdetDigitScheme *fDigiScheme  = BmnNdetDigitScheme::Instance();
  fDigiScheme->Init(fGeoPar,0,2);

  cout << "-I- BmnNdetDigitizer: Intialization successfull" << endl;
  
  return kSUCCESS;

}

// -------------------------------------------------------------------------
void BmnNdetDigitizer::CreateHistograms ( BmnNdetDigitId_t *pDigiID)
{
  Int_t nx,ny, nz;
  Double_t dx, dy, dz;

  BmnNdetDigitScheme *fDigiScheme  = BmnNdetDigitScheme::Instance();
  // fDigiScheme->GetNdetDimensions (nx,ny,nz);
  // fDigiScheme->GetVolDxDyDz (pDigiID,dx, dy, dz);
  nx = 10 ; ny = 10 ; dx = 75; dy = 75; dz=1010;  // mm
  
  Int_t Nx=nx+2;
  Double_t Dx=dx*Nx/2; 
  Int_t Ny=ny+2;
  Double_t Dy=dy*Ny/2; 
  /*
  fHistNdetEn = new TH2F ("HistNdetEn","HistNdetEnergy",Nx,-Dx,Dx,Ny,-Dy,Dy);

  if (!fHistNdetEn) 
    cout << "-E- BmnNdetDigitizer: HistNdetEn Histogram not created !!" << endl;
  else {
    FairRootManager* ioman = FairRootManager::Instance();
    fHistNdetEn->SetDirectory((TFile*)ioman->GetOutFile());
    fHistNdetEn->Write();
}
  */
}

// -----   Public method Exec   --------------------------------------------
void BmnNdetDigitizer::Exec(Option_t* opt) {
 

  //#define EDEBUG
#ifdef EDEBUG
  static Int_t lEDEBUGcounter=0;
  cout << "EDEBUG-- BmnNdetDigitizer::Exec() started... " << endl;;
#endif

  if ( ! fDigiArray ) Fatal("Exec", "No DigiArray");
  
  fDigiArray->Clear();

  BmnNdetDigitScheme *pDigiScheme  = BmnNdetDigitScheme::Instance();

  if (!pDigiScheme) 
    Fatal("BmnNdetDigitizer::Exec", "No DigiScheme");

  //Int_t module_groupID, modID, chanID;
  Int_t modID, chanID;
  Double_t time;
  BmnNdetDigitId_t digiID;
   // marina
   Int_t nbMods=225;
   Int_t nbSect=15;
   Double_t dEdepSectEv[nbMods][nbSect];
   Double_t dTimeEv[nbMods][nbSect];

   for (Int_t i = 0; i < nbMods; i++) {       // mod
      for (Int_t ii = 0; ii < nbSect; ii++) { // section
         dEdepSectEv[i][ii] = 0.;
         dTimeEv[i][ii] = -100.;
      }
   }
   // end marina
  
  BmnNdetPoint* point  = NULL;

  map<BmnNdetDigitId_t, Float_t> fDigiIdEnergy;
  fDigiIdEnergy.clear();
  map<BmnNdetDigitId_t, Float_t>::const_iterator p;
  
  Int_t nPoints = fPointArray->GetEntriesFast();
  Double_t e1=0;

  for (Int_t iPoint=0; iPoint<nPoints; iPoint++) {

      // marina
    point  = (BmnNdetPoint*) fPointArray->At(iPoint);
    modID = point->GetCopyMother(); // modules 1-9
    chanID = point->GetCopy(); // sections 1-15

      //    cout <<"marina 1 " <<detID <<" " <<modID <<" " <<point->GetCopy() <<' ' <<chanID <<endl;
    dTimeEv[modID - 1][chanID-1] = point->GetTime();
    dEdepSectEv[modID - 1][chanID-1] = point->GetEnergyLoss();
      // end marina

    Int_t pMMcopy=1; 
    digiID = pDigiScheme->GetDigiIdFromVolumeData  (point->GetDetectorID(), point->GetCopyMother());

    if ((digiID[0]!=-1)&&(digiID[1]!=-1)) {

      if (fDigiIdEnergy.find(digiID)==fDigiIdEnergy.end())
	fDigiIdEnergy[digiID] = point->GetEnergyLoss();
      else
	fDigiIdEnergy[digiID] += point->GetEnergyLoss();

      e1 += point->GetEnergyLoss();

#ifdef EDEBUG
      if (lEDEBUGcounter<20) {
	cout << "EDEBUG-- BmnNdetDigitizer::Exec:  point : "; point->Print("");
	lEDEBUGcounter++;
      }
#endif

    }//if ((digiID[0]!=-1)&&(digiID[1]!=-1))
#ifdef EDEBUG
    else {
      if (lEDEBUGcounter<100) {
	cout << "EDEBUG-- BmnNdetDigitizer::Exec:  Boundary point? : "; point->Print("");
	lEDEBUGcounter++;
      }
    }
#endif
  }// for (Int_t iPoint=0; iPoint<nPoints; iPoint++)
   e1 = 0;

   //cout <<"marina 3 " <<endl;
   for (Int_t i = 0; i < nbMods; i++) {       // mod
      for (Int_t ii = 0; ii < nbSect; ii++) { // section
         // cout <<"dEdepSectEv " <<i <<" " <<ii <<" " <<dEdepSectEv[i][ii] <<endl;
         if (dEdepSectEv[i][ii] > 0) {
            Double_t recEnergy = RecoEnergy(dEdepSectEv[i][ii]);

	    //module_groupID=1;
	    //BmnNdetDigit *digi = AddHit(module_groupID, i + 1, ii + 1, dEdepSectEv[i][ii]);
               BmnNdetDigit *digi = AddHit(dTimeEv[i][ii], i + 1, ii + 1, dEdepSectEv[i][ii]);
               digi->ConvertSim();
               digi->SetELossReco(recEnergy);
               e1 += recEnergy;
               digi->SetModuleX(fModuleX[i+1]);
               digi->SetModuleY(fModuleY[i+1]);
               digi->SetSectionZ(fSectionZ[ii+1]);
         }//if (dEdepSectEv[i][ii] > 0)
      }//sect
   } // mods

  /*
  Float_t eloss=0;
  
  for(p=fDigiIdEnergy.begin(); p!=fDigiIdEnergy.end(); ++p) {

    pDigiScheme->SplitDigiID((*p).first, module_groupID, modID, chanID);


    if ((module_groupID!=-1)&&(chanID!=-1)) {
      eloss = (*p).second;
      BmnNdetDigit* digi = AddHit(module_groupID, modID, chanID, eloss); 
#ifdef EDEBUG
      if (lEDEBUGcounter<50) {
	cout << "EDEBUG-- BmnNdetDigitizer::Exec: "<< module_groupID<< " " << chanID << "   " << 
	  (*p).second << "     " << lEDEBUGcounter << endl;
	lEDEBUGcounter++;
      }
#endif
    }
  }
  */
#undef EDEBUG
}
// -------------------------------------------------------------------------


// -----   Private method AddDigi   --------------------------------------------
//BmnNdetDigit* BmnNdetDigitizer::AddHit(Int_t module_groupID, Int_t modID, Int_t chanID,Float_t energy)
BmnNdetDigit* BmnNdetDigitizer::AddHit(Double_t time, Int_t modID, Int_t chanID,Float_t energy)
{
  TClonesArray& clref = *fDigiArray;
  Int_t size = clref.GetEntriesFast();
  //BmnNdetDigit* result = new(clref[size]) BmnNdetDigit(module_groupID,modID,chanID,(Double_t)energy);
  BmnNdetDigit* result = new(clref[size]) BmnNdetDigit(time,modID,chanID,(Double_t)energy);
  //  result->Print();
  return result;
}
// ----
Double_t BmnNdetDigitizer::RecoEnergy(Double_t pfELoss)
{
   Double_t energyMIP        = pfELoss / fMIPEnergy;
   Double_t energyPix        = fRandom3->Poisson(energyMIP * fPix2Mip);
   Double_t energyMIPSmeared = energyPix / fPix2Mip;
   Double_t noise            = fRandom3->Gaus(0, fMIPNoise);
   energyMIPSmeared += noise;
   //cout <<"marina 5 " <<endl;
   return energyMIPSmeared * fMIP2GeV;
}


ClassImp(BmnNdetDigitizer)
