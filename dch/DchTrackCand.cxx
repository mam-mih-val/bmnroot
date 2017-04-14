#include "DchTrackCand.h"

#include <TMatrix.h>
#include <ScalarUI.h>
#include "Riostream.h"
#include <iomanip>

//using namespace ROOT::Math;

//
ClassImp(DchTrackCand)


//_________________________________________________________________
//DchTrackCand::DchTrackCand() : TObject() {
DchTrackCand::DchTrackCand() : TNamed() {
//DchTrackCand::DchTrackCand():checkDTC(false) {
//DchTrackCand::DchTrackCand():checkDTC(true) {

  checkDTC=false; 
  fDchTrackCand  = new TClonesArray("TMatrix");
  //fDchTrackCand2  = new TClonesArray("ScalarUI");
  //fDchTrackCand3  = new TClonesArray("ScalarUI");
  //fDchTrackCand4  = new TClonesArray("ScalarUI");

}

//_________________________________________________________________
/*DchTrackCand::DchTrackCand(const char *name, const char *title):FairTask(name) {

  fDchTrackCand = new TClonesArray("TMatrix");

}
*/
//_________________________________________________________________
DchTrackCand::~DchTrackCand() {

  fDchTrackCand->Delete();   
  delete fDchTrackCand;   
  fDchTrackCand=0;   
  /*fDchTrackCand2->Delete();   
  delete fDchTrackCand2;   
  fDchTrackCand2=0;   
  fDchTrackCand3->Delete();   
  delete fDchTrackCand3;   
  fDchTrackCand3=0;   
  fDchTrackCand4->Delete();   
  delete fDchTrackCand4;   
  fDchTrackCand4=0;*/   
  //FairRootManager *fManager =FairRootManager::Instance();
  //fManager->Write();

}

//_______________________________________________________________
//void DchTrackCand::SetDchTrackHits(UInt_t n, Double_t x0, Double_t y0, Double_t z0, Double_t tdc0, Double_t hitX1, Double_t hitY1, Double_t hitX1_2, Double_t hitY1_2, UShort_t ijk0, Double_t x1, Double_t y1, Double_t z1, Double_t tdc1, Double_t hitX2, Double_t hitY2, Double_t hitX2_2, Double_t hitY2_2, UShort_t ijk1, Double_t x2, Double_t y2, Double_t z2, Double_t tdc2, Double_t hitX3, Double_t hitY3, Double_t hitX3_2, Double_t hitY3_2, UShort_t ijk2, Double_t x3, Double_t y3, Double_t z3, Double_t tdc3, Double_t hitX4, Double_t hitY4, Double_t hitX4_2, Double_t hitY4_2, UShort_t ijk3) {
//void DchTrackCand::SetDchTrackHits(UInt_t n, Double_t x0, Double_t y0, Double_t z0, Double_t tdc0, Double_t hitX1, Double_t hitY1, Double_t hitZ1, Double_t hitX1_2, Double_t hitY1_2, Double_t hitZ1_2, UShort_t ijk0, Double_t x1, Double_t y1, Double_t z1, Double_t tdc1, Double_t hitX2, Double_t hitY2, Double_t hitZ2, Double_t hitX2_2, Double_t hitY2_2, Double_t hitZ2_2, UShort_t ijk1, Double_t x2, Double_t y2, Double_t z2, Double_t tdc2, Double_t hitX3, Double_t hitY3, Double_t hitZ3, Double_t hitX3_2, Double_t hitY3_2, Double_t hitZ3_2, UShort_t ijk2, Double_t x3, Double_t y3, Double_t z3, Double_t tdc3, Double_t hitX4, Double_t hitY4, Double_t hitZ4, Double_t hitX4_2, Double_t hitY4_2, Double_t hitZ4_2, UShort_t ijk3) {
void DchTrackCand::SetDchTrackHits(const UInt_t n, const XYZPoint xyzHit1, const XYZPoint xyzWire1, const XYZPoint xyzWire1_2, const XYZPoint xyzHit2, const XYZPoint xyzWire2, const XYZPoint xyzWire2_2, const XYZPoint xyzHit3, const XYZPoint xyzWire3, const XYZPoint xyzWire3_2, const XYZPoint xyzHit4, const XYZPoint xyzWire4, const XYZPoint xyzWire4_2, ScalarD* drifttim[], const UShort_t ijk[]) {

TClonesArray &ffDchTrackCand = *fDchTrackCand;
//TMatrix hits(4,9);
TMatrix hits(4,11);
/*
hits(0,0)=x0;hits(0,1)=y0;hits(0,2)=z0;hits(0,3)=tdc0;hits(0,4)=hitX1;hits(0,5)=hitY1;hits(0,6)=hitX1_2;hits(0,7)=hitY1_2;hits(0,8)=ijk0;
hits(1,0)=x1;hits(1,1)=y1;hits(1,2)=z1;hits(1,3)=tdc1;hits(1,4)=hitX2;hits(1,5)=hitY2;hits(1,6)=hitX2_2;hits(1,7)=hitY2_2;hits(1,8)=ijk1;
hits(2,0)=x2;hits(2,1)=y2;hits(2,2)=z2;hits(2,3)=tdc2;hits(2,4)=hitX3;hits(2,5)=hitY3;hits(2,6)=hitX3_2;hits(2,7)=hitY3_2;hits(2,8)=ijk2;
hits(3,0)=x3;hits(3,1)=y3;hits(3,2)=z3;hits(3,3)=tdc3;hits(3,4)=hitX4;hits(3,5)=hitY4;hits(3,6)=hitX4_2;hits(3,7)=hitY4_2;hits(3,8)=ijk3;
*/
/*
hits(0,0)=x0;hits(0,1)=y0;hits(0,2)=z0;hits(0,3)=tdc0;hits(0,4)=hitX1;hits(0,5)=hitY1;hits(0,6)=hitZ1;hits(0,7)=hitX1_2;hits(0,8)=hitY1_2;hits(0,9)=hitZ1_2;hits(0,10)=ijk0;
hits(1,0)=x1;hits(1,1)=y1;hits(1,2)=z1;hits(1,3)=tdc1;hits(1,4)=hitX2;hits(1,5)=hitY2;hits(1,6)=hitZ2;hits(1,7)=hitX2_2;hits(1,8)=hitY2_2;hits(1,9)=hitZ2_2;hits(1,10)=ijk1;
hits(2,0)=x2;hits(2,1)=y2;hits(2,2)=z2;hits(2,3)=tdc2;hits(2,4)=hitX3;hits(2,5)=hitY3;hits(2,6)=hitZ3;hits(2,7)=hitX3_2;hits(2,8)=hitY3_2;hits(2,9)=hitZ3_2;hits(2,10)=ijk2;
hits(3,0)=x3;hits(3,1)=y3;hits(3,2)=z3;hits(3,3)=tdc3;hits(3,4)=hitX4;hits(3,5)=hitY4;hits(3,6)=hitZ4;hits(3,7)=hitX4_2;hits(3,8)=hitY4_2;hits(3,9)=hitZ4_2;hits(3,10)=ijk3;
*/
hits(0,0)=xyzHit1.X();hits(0,1)=xyzHit1.Y();hits(0,2)=xyzHit1.Z();hits(0,3)=drifttim[ijk[0]]->GetSV();hits(0,4)=xyzWire1.X();hits(0,5)=xyzWire1.Y();hits(0,6)=xyzWire1.Z();hits(0,7)=xyzWire1_2.X();hits(0,8)=xyzWire1_2.Y();hits(0,9)=xyzWire1_2.Z();hits(0,10)=ijk[0];
hits(1,0)=xyzHit2.X();hits(1,1)=xyzHit2.Y();hits(1,2)=xyzHit2.Z();hits(1,3)=drifttim[ijk[1]]->GetSV();hits(1,4)=xyzWire2.X();hits(1,5)=xyzWire2.Y();hits(1,6)=xyzWire2.Z();hits(1,7)=xyzWire2_2.X();hits(1,8)=xyzWire2_2.Y();hits(1,9)=xyzWire2_2.Z();hits(1,10)=ijk[1];
hits(2,0)=xyzHit3.X();hits(2,1)=xyzHit3.Y();hits(2,2)=xyzHit3.Z();hits(2,3)=drifttim[ijk[2]]->GetSV();hits(2,4)=xyzWire3.X();hits(2,5)=xyzWire3.Y();hits(2,6)=xyzWire3.Z();hits(2,7)=xyzWire3_2.X();hits(2,8)=xyzWire3_2.Y();hits(2,9)=xyzWire3_2.Z();hits(2,10)=ijk[2];
hits(3,0)=xyzHit4.X();hits(3,1)=xyzHit4.Y();hits(3,2)=xyzHit4.Z();hits(3,3)=drifttim[ijk[3]]->GetSV();hits(3,4)=xyzWire4.X();hits(3,5)=xyzWire4.Y();hits(3,6)=xyzWire4.Z();hits(3,7)=xyzWire4_2.X();hits(3,8)=xyzWire4_2.Y();hits(3,9)=xyzWire4_2.Z();hits(3,10)=ijk[3];
//cout<<"hits print"<<endl;
//hits.Print();
new (ffDchTrackCand[n]) TMatrix(hits);

}
/*
//________________________________________________________________
void DchTrackCand::SetDchTrackCandNumber(UInt_t trcand,UInt_t n){

// setting number of Dch track candidate

 ScalarUI* trackN = (ScalarUI*)fDchTrackCand3->ConstructedAt(trcand);
 trackN->SetSV(n);

}
//________________________________________________________________
UInt_t DchTrackCand::GetDchTrackCandNumber(UInt_t trcand){

// getting number of Dch track candidate

 ScalarUI* trackN = (ScalarUI*)fDchTrackCand3->ConstructedAt(trcand);

 return trackN->GetSV();

}
//________________________________________________________________
void DchTrackCand::SetDchNumberOfTracksInEvent(UInt_t trcand,UInt_t n){

// setting number of Dch track candidates in event

 ScalarUI* tracksN = (ScalarUI*)fDchTrackCand4->ConstructedAt(trcand);
 tracksN->SetSV(n);

}
//________________________________________________________________
UInt_t DchTrackCand::GetDchNumberOfTracksInEvent(UInt_t trcand){

// getting number of Dch track candidates in event

 ScalarUI* tracksN = (ScalarUI*)fDchTrackCand4->ConstructedAt(trcand);
 
 return tracksN->GetSV();

}
//________________________________________________________________
void DchTrackCand::SetDchTrackCandEventNumber(UInt_t trcand,UInt_t n){

// setting event number of Dch track candidate

 ScalarUI* eventN = (ScalarUI*)fDchTrackCand2->ConstructedAt(trcand);
 eventN->SetSV(n);

}
//________________________________________________________________
UInt_t DchTrackCand::GetDchTrackCandEventNumber(UInt_t trcand){

// getting event number of Dch track candidate

 ScalarUI* eventN = (ScalarUI*)fDchTrackCand2->ConstructedAt(trcand);

 return eventN->GetSV();
}
//________________________________________________________________
void DchTrackCand::SetNumberOfEvents(UInt_t n){

// setting number of events

 nEvents=n;

}
//________________________________________________________________
UInt_t DchTrackCand::GetNumberOfEvents(){

// returns number of events

 return nEvents;

}*/




