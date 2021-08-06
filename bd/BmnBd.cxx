#include "BmnBd.h"
#include "BmnBdGeo.h"
#include "BmnBdGeoPar.h"
#include "CbmStack.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairVolume.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TParticlePDG.h"
#include "TVector3.h" 
#include "TObjArray.h"

#include <iostream>

// -----   Default constructor   -------------------------------------------
BmnBd::BmnBd() {
    fBdCollection = new TClonesArray("BmnBdPoint");
    volDetector = 0;
    fPosIndex   = 0;
    //fpreflag = 0;
    //fpostflag = 0;
    //fEventID = -1;
    fVerboseLevel = 1;
}

// -----   Standard constructor   ------------------------------------------
BmnBd::BmnBd(const char* name, Bool_t active)
  : FairDetector(name, active) {
    fBdCollection = new TClonesArray("BmnBdPoint");
    fPosIndex   = 0;
    volDetector = 0;
    //fpreflag = 0;  
    //fpostflag = 0;
    //fEventID=-1;
    fVerboseLevel = 1;
}

// -----   Destructor   ----------------------------------------------------
BmnBd::~BmnBd() {
    if (fBdCollection) {
        fBdCollection->Delete();
        delete fBdCollection;
    }
}

// -----   Public method Initialize   ---------------------------------------
void BmnBd::Initialize() {
    FairDetector::Initialize();
    FairRun* sim = FairRun::Instance();
    FairRuntimeDb* rtdb = sim->GetRuntimeDb();
}

// ------  Begin of the event         ----------------------------------------
void BmnBd::BeginEvent()
{
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t BmnBd::ProcessHits(FairVolume* vol) {
  // if (TMath::Abs(gMC->TrackCharge()) <= 0) return kFALSE;

    static const Double_t dP = 1.032 ;
    static const Double_t BirkC1 =  0.013/dP;
    static const Double_t BirkC2 =  9.6e-6/(dP * dP);

    static Double_t lightYield;
    static Double_t timeIn;
    static Double_t timeOut;
    static Double_t lengthtrack;
    static Double_t de_dx;
	
    Int_t ivol = vol->getMCid();
    //TLorentzVector tPos1, tMom1;
    //TLorentzVector tPos, tMom;

    //#define EDEBUG
#ifdef EDEBUG
    static Int_t lEDEBUGcounter = 0;
    if (lEDEBUGcounter < 1)
        std::cout << "EDEBUG-- BmnBd::ProcessHits: entered" << gMC->CurrentVolPath() << endl;
#endif

    if(gMC->IsTrackEntering())
    {
        ResetParameters();

    	fELoss = 0.;
        fIsPrimary = 0;
        fCharge = -1;
        fPdgId = 0;
	    lightYield = 0.;
	    lengthtrack = 0.;
	   		
        TLorentzVector PosIn;
        gMC->TrackPosition(PosIn);
        fPosIn.SetXYZ(PosIn.X(), PosIn.Y(), PosIn.Z());

        TLorentzVector MomIn;
        gMC->TrackMomentum(MomIn);
        fMomIn.SetXYZ(MomIn.Px(), MomIn.Py(), MomIn.Pz());
	    	
	    timeIn = gMC->TrackTime() * 1.0e09;

#ifdef EDEBUG
      //gMC->TrackPosition(tPos1);
      //gMC->TrackMomentum(tMom1);
#endif

        TParticle* part = 0;
        part = gMC->GetStack()->GetCurrentTrack();
        
        if (part) {
            fIsPrimary = (Int_t)part->IsPrimary();
            fCharge = (Int_t)part->GetPDG()->Charge() / 3.;
            fPdgId = (Int_t)part->GetPdgCode();
       	}
    }

    if (gMC->TrackCharge() != 0)
    {
        Double_t BirkC1Mod = 0;
         			
        if (TMath::Abs(gMC->TrackCharge()) >= 2)
            BirkC1Mod = BirkC1 * 7.2/12.6;
        else
            BirkC1Mod = BirkC1;

        if (gMC->TrackStep() > 0)
        {
            Double_t dedxcm = gMC -> Edep() * 1000./gMC -> TrackStep(); //[MeV/cm]
            Double_t curLightYield = gMC -> Edep() * 1000./(1. + BirkC1Mod * dedxcm + BirkC2 * dedxcm * dedxcm);
            curLightYield /= 1000.; //[GeV]
           lightYield += curLightYield;
        }
    }

    Double_t eLoss = gMC->Edep();
    if (eLoss > 0) 
        fELoss += eLoss;
    	
    Double_t slengthtrack = gMC-> TrackStep();
    lengthtrack += slengthtrack;
		
    if ( gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared() && fELoss > 0 )
    {
        TLorentzVector PosOut;
        gMC->TrackPosition(PosOut);
        fPosOut.SetXYZ(PosOut.X(), PosOut.Y(), PosOut.Z());

        TLorentzVector MomOut;
        gMC->TrackMomentum(MomOut);
        fMomOut.SetXYZ(MomOut.Px(), MomOut.Py(), MomOut.Pz());
        
        timeOut  = gMC->TrackTime() * 1.0e09;
/*
        TArrayI processesID;

        ofstream fout("process.txt", ios::app);

        //if (gMC->TrackPid() == 11 && gMC->Etot()  <= 0.00006){

        gMC->StepProcesses(processesID);
        fout << gMC->TrackPid() << " " << gMC->Edep() << " " ;
        for (Int_t i = 0; i<processesID.GetSize(); i++){

        fout << TMCProcessName[processesID[i]]  << " ";
        }
        fout << endl;

        //fout<<" "<<gMC->TrackPid() <<" " <<gMC->Edep()<<" "<< TMCProcessName[processesID[i]]<<endl;
        fout.close();
*/		

#ifndef EDEBUG
        if (fELoss == 0. ) return kFALSE;
#endif
	
        fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
			   	
        Double_t time    = gMC->TrackTime() * 1.0e09;
        Double_t length  = gMC->TrackLength();
     
        Int_t copyNo;
        Int_t ivol1 = gMC->CurrentVolID(copyNo);
      
        if (copyNo < 41)
        {
            Int_t iCell;
            gMC->CurrentVolOffID(1, iCell);
	
#ifdef EDEBUG
            if (lEDEBUGcounter < 100)
            {
                std::cout << "EDEBUG-- BmnBd::ProcessHits: TrackID:" << fTrackID <<
                //           " ELoss: " << fELoss <<
                //           "   particle: " << (part->GetName()) <<
                             "   " << gMC->CurrentVolPath() << " " << tPos.Z() <<
                //           "   " << (gMC->GetStack()->GetCurrentTrack()->GetMother(1)) <<
                             "   "  << ivol << "=="<< gMC->CurrentVolID(copyNo) << ","<< copyNo <<
                             "   "  << gMC->CurrentVolOffID(1,iCell) << " " << iCell <<
                             " " <<  gMC->CurrentVolOffName(1) << " " << gMC->CurrentVolOffName(0) <<
                //           "   " << vol->getRealName() << "  " << gMC->CurrentVolPath() <<
                //           "   ivol,iCell,copyNo= " << ivol << ","<< iCell << ","<< copyNo <<
                //           "   " << vol->getRealName() << "  "<< gMC->CurrentVolName() << "  "<< gMC->CurrentVolPath() <<
                //           "   "  << ivol << ","<< vol->getVolumeId() << " : "<< gMC->CurrentVolID(copyNo) << ","<< copyNo <<
                //           "  "<< gMC->CurrentVolOffName(2) << "  "<< gMC->CurrentVolOffName(3) <<
                std::endl;
                lEDEBUGcounter++;
            }

            AddHit(fTrackID, ivol, copyNo,  fPosIn, fPosOut, fMomIn, fMomOut,
                   time, length, fELoss, fIsPrimary, fCharge, fPdgId, lightYield, timeIn, timeOut, lengthtrack);
#else
            AddHit(fTrackID, ivol, copyNo,  fPosIn, fPosOut, fMomIn, fMomOut,
                   time, length, fELoss, fIsPrimary, fCharge, fPdgId, lightYield, timeIn, timeOut, lengthtrack);
#endif

////      Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
//       Int_t nBdPoints = (points & (1<<30)) >> 30;
//       nBdPoints ++;
//       if (nBdPoints > 1) nBdPoints = 1;
//      points = ( points & ( ~ (1<<30) ) ) | (nBdPoints << 30);
////       points = ( points & ( ~ (1<<30) ) ) | (1 << 30);
////       gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);

            ((CbmStack*)gMC->GetStack())->AddPoint(kBD);
        }
	}
  
    return kTRUE;
  
	#undef EDEBUG
}

// -----   Public method EndOfEvent   -----------------------------------------
void BmnBd::EndOfEvent() {
    if (fVerboseLevel)  Print();
    Reset();
}

// -----   Public method Register   -------------------------------------------
void BmnBd::Register() {
    FairRootManager::Instance()->Register("BdPoint","BD", fBdCollection, kTRUE);
}

// -----   Public method GetCollection   --------------------------------------
TClonesArray* BmnBd::GetCollection(Int_t iColl) const {
    if (iColl == 0) return fBdCollection;

    return NULL;
}

// -----   Public method Print   ----------------------------------------------
void BmnBd::Print() const {
    Int_t nHits = fBdCollection->GetEntriesFast();
    cout << "-I- BmnBd: " << nHits << " points registered in this event." << endl;

    if (fVerboseLevel > 1)
        for (Int_t i=0; i < nHits; i++) (*fBdCollection)[i]->Print();
}

// -----   Public method Reset   ----------------------------------------------
void BmnBd::Reset() {
   fBdCollection->Delete();
   fPosIndex = 0;
}

// guarda in FairRootManager::CopyClones
// -----   Public method CopyClones   -----------------------------------------
void BmnBd::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset ) {
    Int_t nEntries = cl1->GetEntriesFast();
    TClonesArray& clref = *cl2;
    BmnBdPoint* oldpoint = NULL;
    for (Int_t i = 0; i < nEntries; i++)
    {
        oldpoint = (BmnBdPoint*) cl1->At(i);
        Int_t index = oldpoint->GetTrackID() + offset;
        oldpoint->SetTrackID(index);
        new (clref[fPosIndex]) BmnBdPoint(*oldpoint);
        fPosIndex++;
    }
    cout << " -I- BmnBd: " << cl2->GetEntriesFast() << " merged entries." << endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnBd::ConstructGeometry()
{
    TString fileName = GetGeometryFileName();
    if (fileName.EndsWith(".root"))
    {
        LOG(info) << "Constructing BD geometry from ROOT file" << fileName.Data();
        ConstructRootGeometry();
    }
    else if ( fileName.EndsWith(".geo") )
    {
        LOG(info) << "Constructing BD geometry from ASCII file" << fileName.Data();
        ConstructAsciiGeometry();
    }
    else	LOG(fatal) << "Geometry format of BD file " << fileName.Data() << " not supported.";
}

void BmnBd::ConstructAsciiGeometry()
{
    FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    BmnBdGeo* BdGeo  = new BmnBdGeo();
    BdGeo->setGeomFile(GetGeometryFileName());
    geoFace->addGeoModule(BdGeo);

    Bool_t rc = geoFace->readSet(BdGeo);
    if (rc) BdGeo->create(geoLoad->getGeoBuilder());
    TList* volList = BdGeo->getListOfVolumes();

    // store geo parameter
    FairRun *fRun = FairRun::Instance();
    FairRuntimeDb *rtdb = FairRun::Instance()->GetRuntimeDb();
    BmnBdGeoPar* par = (BmnBdGeoPar*)(rtdb->getContainer("BmnBdGeoPar"));
    TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
    TObjArray *fPassNodes = par->GetGeoPassiveNodes();

    TListIter iter(volList);
    FairGeoNode *node = nullptr;
    FairGeoVolume *aVol = nullptr;

    while ( (node = (FairGeoNode*)iter.Next()) )
    {
        aVol = dynamic_cast<FairGeoVolume*>(node);
        cout<<aVol->GetName()<<endl;

        if (node->isSensitive()) fSensNodes->AddLast(aVol);
        else fPassNodes->AddLast(aVol);
    }
  
    par->setChanged();
    par->setInputVersion(fRun->GetRunId(),1);
    ProcessNodes( volList );
}

//--------------------------------------------------------------------------------------------------------------------------------------
Bool_t BmnBd::CheckIfSensitive(std::string name)
{
    TString tsname = name;
    if (tsname.Contains("Active")) return kTRUE;
  
    return kFALSE;
}

// -----   Private method AddHit   --------------------------------------------
BmnBdPoint* BmnBd::AddHit(Int_t trackID, Int_t detID, Int_t copyNo, TVector3 posIn, TVector3 posOut,
                          TVector3 momIn, TVector3 momOut, Double_t time, Double_t length, Double_t eLoss, Bool_t isPrimary,
                          Double_t charge, Int_t pdgId, Double_t lightYield, Double_t timeIn, Double_t timeOut, Double_t lengthtrack)
{
    TClonesArray& clref = *fBdCollection;
    Int_t size = clref.GetEntriesFast();
    return new(clref[size]) BmnBdPoint(trackID, detID, copyNo, posIn, posOut, momIn, momOut, time, length, eLoss, isPrimary,
                                       charge, pdgId, lightYield, timeIn, timeOut, lengthtrack);
}

ClassImp(BmnBd)
