#include "CbmKF.h"
#include "CbmKFFieldMath.h"
#include "CbmKFHit.h"
#include "CbmStsStation.h"
#include "CbmMvdGeoPar.h"
#include "CbmGeoStsPar.h"
#include "CbmGeoSttPar.h" //AZ
#include "BmnFieldPar.h"
#include "BmnTOFGeoPar.h" //YP

#include "FairGeoNode.h"
#include "FairRunAna.h"
#include "FairBaseParSet.h"
#include "FairGeoPassivePar.h"
#include "FairRuntimeDb.h"

#include <iostream>
#include <cmath>

using namespace std;

CbmKF *CbmKF::fInstance = 0;

CbmKF::CbmKF(const char *name, Int_t iVerbose ):
  FairTask(name,iVerbose),
  vMaterial(), 
  
  vMvdMaterial(),
  vStsMaterial(),
  vSttMaterial(),
  vTargets(),
  vPipe(),

  vPassiveTube(),
  vPassiveWall(),
  vPassiveBox(),

  MvdStationIDMap(),
  StsStationIDMap(),
  SttStationIDMap(),

  StsDigi(),
  fMagneticField(0),
  fMethod(2),
  fMaterialID2IndexMap()
{
  if( !fInstance ) fInstance = this;
  StsDigi = CbmStsDigiScheme::Instance(); //AZ
}

CbmKF::~CbmKF(){
  fInstance = 0;
}

void CbmKF::SetParContainers()
{
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  rtdb->getContainer("FairBaseParSet");
  rtdb->getContainer("FairGeoParSet");
  rtdb->getContainer("FairGeoPassivePar");
  rtdb->getContainer("CbmMvdGeoPar");
  rtdb->getContainer("CbmGeoStsPar");
  //rtdb->getContainer("CbmGeoTofPar");
   rtdb->getContainer("BmnTOFGeoPar");//YP
  rtdb->getContainer("CbmGeoSttPar"); //AZ
  rtdb->getContainer("BmnFieldPar");
  rtdb->getContainer("CbmStsDigiPar");
}

InitStatus CbmKF::ReInit()
{
  //AZ StsDigi.Clear();
  StsDigi->Clear();
  return Init();
}

InitStatus CbmKF::Init()
{
  fMagneticField = 0;
  
  vMvdMaterial.clear();
  vStsMaterial.clear();
  vSttMaterial.clear();
  vPipe.clear();
  vTargets.clear();    
  vPassiveTube.clear();
  vPassiveWall.clear();
  vPassiveBox.clear();
  vMaterial.clear();

  StsStationIDMap.clear();
  SttStationIDMap.clear();
  MvdStationIDMap.clear();

  fMaterialID2IndexMap.clear();

  FairRunAna *Run = FairRunAna::Instance();
  FairRuntimeDb *RunDB = Run->GetRuntimeDb();

  {
    CbmGeoStsPar* StsPar = reinterpret_cast<CbmGeoStsPar*>(RunDB->findContainer("CbmGeoStsPar"));
    CbmStsDigiPar *digiPar = reinterpret_cast<CbmStsDigiPar*>(RunDB->findContainer("CbmStsDigiPar"));
    //AZ StsDigi.Init(StsPar, digiPar);
    StsDigi->Init(StsPar, digiPar);
  }

  if( fVerbose ) cout<<"KALMAN FILTER : === INIT MAGNETIC FIELD ==="<<endl;
//BmnNewFieldMap *field = (BmnNewFieldMap*) fMagneticField; // GP

  fMagneticField =  (BmnNewFieldMap*) (Run->GetField());//reinterpret_cast<FairField*>(Run->GetField());
  if( fVerbose && fMagneticField ) cout << "Magnetic field done" << endl;

  if( !fMagneticField ) cout<<"No Magnetic Field Found"<<endl;


/** 					*/
  // fill vector of material
    
  //=== Mvd ===
  
  CbmMvdGeoPar* MvdPar = reinterpret_cast<CbmMvdGeoPar*>(RunDB->findContainer("CbmMvdGeoPar"));
  
  if( MvdPar ){ //=== Mvd stations ===
    
    if( fVerbose ) cout<<"KALMAN FILTER : === READ MVD MATERIAL ==="<<endl;
    
    int NMvdStations;
    
    TObjArray* mvdNodes = MvdPar->GetGeoSensitiveNodes();
    if ( ! mvdNodes ) {
      cout << "-E- " << GetName() << "::GetGeometry: No MVD node array"
	   << endl;
      NMvdStations = 0;
      return kERROR;
    }
    
    NMvdStations = mvdNodes->GetEntries();
    
    for ( Int_t ist = 0 ; ist < NMvdStations ; ist++ ) {
      FairGeoNode* mvdNode = reinterpret_cast<FairGeoNode*>(mvdNodes->At(ist));
      if ( ! mvdNode ) {
	cout << "-W- CbmKF::Init: station#" << ist
	     << " not found among sensitive nodes " << endl;
	continue;
      }
      
      CbmKFTube tube;
      if( ReadTube( tube, mvdNode ) ) continue;
      tube.ID = 1101+ist;
      vMvdMaterial.push_back(tube);
      MvdStationIDMap.insert(pair<Int_t,Int_t>(tube.ID, ist ) );
      
      if( fVerbose ) cout<<" Mvd detector "<<tube.Info()<<endl;
    }
  }


  CbmGeoStsPar* StsPar = reinterpret_cast<CbmGeoStsPar*>(RunDB->findContainer("CbmGeoStsPar"));

  if( StsPar ){ //=== Sts stations ===
    
    if( fVerbose ) cout<<"KALMAN FILTER : === READ STS MATERIAL ==="<<endl;

    //AZ int NStations = StsDigi.GetNStations();
    int NStations = StsDigi->GetNStations();

    for ( Int_t ist = 0; ist<NStations; ist++ )
      {
	//AZ CbmStsStation *st = StsDigi.GetStation(ist);
	CbmStsStation *st = StsDigi->GetStation(ist);
	if ( !st ) continue;

	CbmKFTube tube;

	tube.ID = 1000+st->GetStationNr();
	tube.F = 1.;
	tube.z  = st->GetZ();
	tube.dz = st->GetD();
	tube.RadLength = st->GetRadLength();
	tube.r  = st->GetRmin();
	tube.R  = st->GetRmax();
	tube.rr = tube.r * tube.r;
	tube.RR = tube.R * tube.R;
	tube.ZThickness = tube.dz;
	tube.ZReference = tube.z;

	vStsMaterial.push_back(tube);
	StsStationIDMap.insert(pair<Int_t,Int_t>(tube.ID, ist ) );
	
	if( fVerbose ) cout<<" Sts material ( id, z, dz, r, R, RadL )= ( "
			   << tube.ID<<", " << tube.z<<", " << tube.dz
			   <<", " << tube.r<<", " << tube.R<<", " << tube.RadLength<<" )"<<endl;	
      }
  }

  //=== Stt stations ===
    
  CbmGeoSttPar *sttPar = reinterpret_cast<CbmGeoSttPar*>(RunDB->findContainer("CbmGeoSttPar"));
 
 
  if( sttPar ){

    /*
    if( fVerbose ) cout<<"KALMAN FILTER : === READ STT PASSIVE MATERIAL ==="<<endl;
    if( fVerbose==1 ) cout<<"printout skipped for Verbose mode 1"<<endl;
    TObjArray *Nodes = sttPar->GetGeoPassiveNodes();
    for( Int_t i=0;i<Nodes->GetEntries(); i++) {
      FairGeoNode *node = dynamic_cast<FairGeoNode*> (Nodes->At(i));
      if ( !node ) continue;	
      TString name = node->getName();
      CbmKFMaterial * kfmat = ReadPassive( node );
      if( !kfmat ) continue;
      if( fVerbose>=1 ) cout<<" Stt material "<<name<<" : "<<kfmat->Info()<<endl;
    }
    */

    if( fVerbose ) cout<<"KALMAN FILTER : === READ STT DETECTORS ==="<<endl;

    TObjArray *Nodes = sttPar->GetGeoSensitiveNodes();
    //double zold = 0;
    int ista = 0; //-1;
    for (Int_t i = 0; i < Nodes->GetEntries(); ++i) {
      FairGeoNode *node = dynamic_cast<FairGeoNode*> (Nodes->At(i));
      if ( !node ) continue;
	
      TString name = node->getName();	
      if (name.Contains("C2gas")) continue; // chamber #2 - skip it 
      CbmKFTube tube;
      if ( ReadTube( tube, node) ) continue;
      CbmKFWall wall;
      wall.RadLength = tube.RadLength;
      //wall.F = 1.015*1.1*1.0322;
      Double_t dist = 1.;
      if (name.Contains("stt1")) wall.F = 7.552e-4 / 1.653e-4;
      else {
	wall.F = 8.378e-4 / 2.479e-4;
	dist = 1.2;
      }
      wall.F = TMath::Sqrt (wall.F);
      wall.ZThickness = tube.ZThickness;

      for (Int_t lay = 0; lay < 6; ++lay) {
	// Loop over layers 
	Int_t iii = lay % 2;
	Int_t jjj = lay / 2;
	wall.ZReference = tube.ZReference + iii * dist + jjj * 6.; // distance between layers
	wall.ID = tube.ID + lay;
	
	//Int_t jsta = ( TMath::Abs( wall.ZReference - zold ) <10. ) ?ista :ista+1;
	//Int_t jsta = ( TMath::Abs( wall.ZReference - zold ) <1. ) ?ista :ista+1;

	if( fVerbose ) cout<<" Stt detector "<<name<<": "<<wall.Info()<<", station "<<ista<<endl;
      
	//if( jsta==ista ) continue; // same station	
    //zold = wall.ZReference;
	//SttStationIDMap.insert(pair<Int_t,Int_t>(ista+1, ista));
	SttStationIDMap.insert(pair<Int_t,Int_t>(wall.ID, ista));
	ista++;
	vSttMaterial.push_back(wall);	
      }
    }
  }


  //=== Tof material ===
    
 // CbmGeoTofPar *TofPar = reinterpret_cast<CbmGeoTofPar*>(RunDB->findContainer("CbmGeoTofPar"));
BmnTOFGeoPar *TofPar = reinterpret_cast<BmnTOFGeoPar*>(RunDB->findContainer("BmnTOFGeoPar"));
 
  if( TofPar ){

    if( fVerbose ) cout<<"KALMAN FILTER : === READ TOF PASSIVE MATERIAL ==="<<endl;
    if( fVerbose==1 ) cout<<"printout skipped for Verbose mode 1"<<endl;
    TObjArray *Nodes = TofPar->GetGeoPassiveNodes();
    for( Int_t i=0;i<Nodes->GetEntries(); i++)
      {
	FairGeoNode *node = dynamic_cast<FairGeoNode*> (Nodes->At(i));
	if ( !node ) continue;	
	TString name = node->getName();
	CbmKFMaterial * kfmat = ReadPassive( node );
	if( !kfmat ) continue;
	if( fVerbose>1 ) cout<<" Tof material "<<name<<" : "<<kfmat->Info()<<endl;
      }

    if( fVerbose ) cout<<"KALMAN FILTER : === READ TOF DETECTORS ==="<<endl;

    Nodes = TofPar->GetGeoSensitiveNodes(); //AZ
    for( Int_t i=0;i<Nodes->GetEntries(); i++)
      {
	FairGeoNode *node = dynamic_cast<FairGeoNode*> (Nodes->At(i));
	if ( !node ) continue;	
	TString name = node->getName();
	CbmKFMaterial * kfmat = ReadPassive( node );
	if( !kfmat ) continue;
	if( fVerbose>1 ) cout<<" Tof material "<<name<<" : "<<kfmat->Info()<<endl;
      }
  }


  // === Passive Material ===

    
  FairGeoPassivePar* PassivePar=reinterpret_cast<FairGeoPassivePar*>(RunDB->findContainer("FairGeoPassivePar"));
  
  if( PassivePar ){
    
    TObjArray *Nodes = PassivePar->GetGeoPassiveNodes();
    FairGeoNode *node=0;
    
    if( fVerbose ) cout<<"KALMAN FILTER : === READ BEAM PIPE MATERIAL ==="<<endl;

    node = dynamic_cast<FairGeoNode*> (Nodes->FindObject("pipe1")); 

    if( node ){ 
      
      TString name = node->getName();
      TString Sname = node->getShapePointer()->GetName();
      
      FairGeoVector nodeV(0,0,0);
      if( node->getMotherNode() ) nodeV = node->getLabTransform()->getTranslation(); //in cm
      FairGeoVector centerV = node->getCenterPosition().getTranslation();
      TArrayD *P = node->getParameters();
      Int_t NP = node->getShapePointer()->getNumParam();
      FairGeoMedium *material = node->getMedium();
      material->calcRadiationLength();

      Double_t z = nodeV.Z()+centerV.Z();
      
      if ( Sname=="PCON" )
	{
	  Int_t Nz = (NP-3)/3;
	  for( Int_t i=0; i<Nz-1; i++ )
	    {
	      CbmKFCone cone;
	      cone.ID = node->getMCid();
	      cone.z1 = P->At(3+i*3+0) + z;
	      cone.r1 = P->At(3+i*3+1);
	      cone.R1 = P->At(3+i*3+2);
	      cone.z2 = P->At(3+(i+1)*3+0) + z;
	      cone.r2 = P->At(3+(i+1)*3+1);
	      cone.R2 = P->At(3+(i+1)*3+2);
	      cone.RadLength =  material->getRadiationLength();
	      cone.F = 1.0322;
	      cone.ZReference  = (cone.z1+cone.z2)/2;	      
	      cone.ZThickness  = 0;
	      
	      if( i<=3) vPipe.push_back(cone);
	      
	      if( fVerbose ) cout <<" Pipe material ( id, {z1, r1, R1}, {z2, r2, R2}, RL )= ( "
				  << node->getMCid()<<", {" 
				  << cone.z1<<", "<<cone.r1<<", "<<cone.R1<<"}, {"
				  << cone.z2<<", "<<cone.r2<<", "<<cone.R2<<"}, "<<cone.RadLength
				  <<" )"<<endl;
	    }
	}
      else
	{
	  cout <<" unknown pipe shape : "<<Sname<<endl;
	}
    }
    

    
    if( fVerbose ) cout<<"KALMAN FILTER : === READ TARGET MATERIAL ==="<<endl;
      Nodes=gGeoManager->GetTopVolume()->GetNodes(); //GP
      

//TGeoVolume *trg=gGeoManager->GetVolume("targ");
 //node->setRootVolume(trg); //why can't use "classical root geometry ?"
//node = dynamic_cast<FairGeoNode*> (trg);

      //AZ-170722 CbmKFTube tube1( 100500, 0,0,0/*-21.9*/, 2.*0.25, 0, 0.5, 0.5 ); //hard-coded target with Pb rad. lenght, -23.4 z pos.
    CbmKFTube tube1( 100500, 0,0,0/*-21.9*/, 2.*0.25, 0, 0.5, 30420. ); //AZ-170722 - hard-coded target with Air rad. length, 0.0 z pos.
    vTargets.push_back( tube1 );
    if( fVerbose ) cout<<" Target material "<<tube1.Info()<<endl;

    node = dynamic_cast<FairGeoNode*> (Nodes->FindObject("targ_0"));  // classes can't be cast
   // cout<<" node: "<<node<<endl;
     if( node ){
      CbmKFTube tube;
      if( !ReadTube( tube, node) ){
	vTargets.push_back( tube );
	if( fVerbose ) if( true ) cout<<" Target material "<<tube.Info()<<endl;
      }
    }
  }
    

  {
    for( vector <CbmKFCone>::iterator i = vPipe.begin(); i!=vPipe.end(); ++i )
      {
	vMaterial.push_back( &*i );
      }
    
    for( vector <CbmKFTube>::iterator i = vTargets.begin(); i!=vTargets.end();++i )
      {
	vMaterial.push_back( &*i );
      }
    
    for( vector <CbmKFTube>::iterator i=vMvdMaterial.begin();i!=vMvdMaterial.end();++i )
      {
	vMaterial.push_back( &*i );
      }

    for( vector <CbmKFTube>::iterator i=vStsMaterial.begin();i!=vStsMaterial.end();++i )
      {
	vMaterial.push_back( &*i );
      }

    for( vector <CbmKFWall>::iterator i=vSttMaterial.begin();i!=vSttMaterial.end();++i )
      {
	vMaterial.push_back( &*i );
      }    

    for( vector <CbmKFTube>::iterator i=vPassiveTube.begin();i!=vPassiveTube.end();++i )
      {
	vMaterial.push_back( &*i );
      }
     for( vector <CbmKFWall>::iterator i=vPassiveWall.begin();i!=vPassiveWall.end();++i )
      {
	vMaterial.push_back( &*i );
      }
     for( vector <CbmKFBox>::iterator i=vPassiveBox.begin();i!=vPassiveBox.end();++i )
      {
	vMaterial.push_back( &*i );
      }

    sort( vMaterial.begin(), vMaterial.end(), CbmKFMaterial::comparePDown );
    for( unsigned i=0; i<vMaterial.size(); i++ ){
      fMaterialID2IndexMap.insert(pair<Int_t,Int_t>(vMaterial[i]->ID, i));
      if (fVerbose) cout<<" KF  TOTAL MATIREAL: "<<vMaterial[i]->ID<<endl;
    }    
    
  }
  return kSUCCESS;
}


Int_t CbmKF::GetMaterialIndex( Int_t uid )
{
  map<Int_t,Int_t>::iterator i = fMaterialID2IndexMap.find(uid);
  if( i != fMaterialID2IndexMap.end()){      
    return i->second;
  }
  return -1;
}


Int_t CbmKF::ReadTube( CbmKFTube &tube, FairGeoNode *node){

  if ( !node ) return 1;	

  TString name = node->getName();
  TString Sname = node->getShapePointer()->GetName();	
  FairGeoVector nodeV = node->getLabTransform()->getTranslation(); //in cm
  FairGeoVector centerV = node->getCenterPosition().getTranslation();
  TArrayD *P = node->getParameters();
  Int_t NP = node->getShapePointer()->getNumParam();
  FairGeoMedium *material = node->getMedium();
  material->calcRadiationLength();

  tube.ID = node->getMCid();
  tube.RadLength = material->getRadiationLength();
  tube.F = 1.;
  
  tube.Fe = 0.02145;
  TString Mname = material->GetName();
  if( Mname=="MUCHWolfram" ){
    tube.Fe = 0.009029;
  }else if( Mname=="MUCHiron" ){
    tube.Fe = 0.02219;
  }else if( Mname=="carbon" ){
    tube.Fe = 0.08043;
  }
  
  tube.x  = nodeV.X()+centerV.X();
  tube.y  = nodeV.Y()+centerV.Y();
  tube.z  = nodeV.Z()+centerV.Z();
  /*   
  int np = node->getNumPoints();
  cout<<"points:"<<endl;
  for( int i=0; i<np; i++ ){
    FairGeoVector *v = node->getPoint(i);
    cout<<v->X()<<" "<<v->Y()<<" "<<v->Z()<<endl;
  }
  */
  if ( Sname=="TUBS" || Sname=="TUBE" )
    {
      tube.r  = P->At(0);
      tube.R  = P->At(1);
      tube.dz = 2.*P->At(2);
    }
  else if ( Sname=="TRAP" )
    {
      tube.r  = 0;
      tube.R  = 1000;
      tube.dz = 2.*P->At(0);
    }
  else if ( Sname=="SPHE" )
    {
      tube.r  = 0;
      tube.R  = 1000;
      tube.z += 0.5*( P->At(0) + P->At(1) ); // inner & outer radius
      tube.dz = (P->At(1) - P->At(0));
    }
  else if ( Sname=="PCON" )
    {
      Int_t Nz = (NP-3)/3;
      double Z = -100000, R = -100000, z = 100000, r = 100000;
      for( Int_t i=0; i<Nz; i++ )
	{
	  double z1 = P->At(3+i*3+0);
	  double r1 = P->At(3+i*3+1);
	  double R1 = P->At(3+i*3+2);
	  if( r1<r ) r = r1;
	  if( R1>R ) R = R1;
	  if( z1<z ) z = z1;
	  if( z1>Z ) Z = z1;		  
	}
      
      tube.r  = r;
      tube.R  = R;
      tube.z += (z+Z)/2.;
      tube.dz = (Z-z);
    }  
  else if ( Sname=="PGON" )
    {
      Int_t Nz = (NP-4)/3;
      double Z = -100000, R = -100000, z = 100000, r = 100000;
      for( Int_t i=0; i<Nz; i++ )
	{
	  double z1 = P->At(4+i*3+0);
	  double r1 = P->At(4+i*3+1);
	  double R1 = P->At(4+i*3+2);
	  if( r1<r ) r = r1;
	  if( R1>R ) R = R1;
	  if( z1<z ) z = z1;
	  if( z1>Z ) Z = z1;		  
	}      
      tube.r  = r;
      tube.R  = R;
      tube.z += (z+Z)/2.;
      tube.dz = (Z-z);
    }  
  else if ( Sname=="BOX " )
    {
      double dx = 2*P->At(0);
      double dy = 2*P->At(1);
      double dz = 2*P->At(2);
      tube.r  = 0;
      tube.R  = TMath::Sqrt(dx*dx+dy*dy);
      tube.dz = dz;
    }
  else {
    cout <<" -E- unknown shape : "<<Sname<<endl;
    cout <<" -E- It does not make sense to go on" <<endl;
    cout <<" -E- Stop execution here" <<endl;
    Fatal("CbmKF::ReadTube","Unknown Shape");
  }
  tube.rr = tube.r * tube.r;
  tube.RR = tube.R * tube.R;
  tube.ZThickness = tube.dz;
  tube.ZReference = tube.z;
  return 0;
}


CbmKFMaterial *CbmKF::ReadPassive( FairGeoNode *node){

  if ( !node ) return 0;	

  TString name = node->getName();
  TString Sname = node->getShapePointer()->GetName();	

  FairGeoTransform trans( *node->getLabTransform() );
  FairGeoNode *nxt = node;
  while( (nxt=nxt->getMotherNode()) ){
    FairGeoTransform* tm=nxt->getLabTransform();
    if( !tm ) break;
    trans.transFrom(*tm);
  }

  //FairGeoVector nodeV = node->getLabTransform()->getTranslation(); //in cm
  //FairGeoVector centerV = node->getCenterPosition().getTranslation();

  FairGeoVector nodeV = trans.getTranslation(); //in cm
  FairGeoVector centerV = nodeV + node->getCenterPosition().getTranslation();

  TArrayD *P = node->getParameters();
  Int_t NP = node->getShapePointer()->getNumParam();
  FairGeoMedium *material = node->getMedium();
  material->calcRadiationLength();

  Int_t ID = node->getMCid();
  Double_t RadLength = material->getRadiationLength();
  // Double_t F = 1.;
  Double_t x0  = centerV.X();
  Double_t y0  = centerV.Y();
  Double_t z0  = centerV.Z();

  CbmKFMaterial *ret = 0;

  if ( Sname=="TUBS" || Sname=="TUBE" )
    {
      CbmKFTube tube( ID, x0,y0,z0, 2.*P->At(2), P->At(0), P->At(1), RadLength );
      vPassiveTube.push_back(tube);
      ret = &(vPassiveTube.back());
    }
  else if ( Sname=="SPHE" )
    {
      CbmKFTube tube( ID, x0,y0,z0+0.5*( P->At(0) + P->At(1) ), 
		      (P->At(1) - P->At(0)), 0, 1000, RadLength );
      vPassiveTube.push_back(tube);
      ret = &(vPassiveTube.back());
    }
  else if ( Sname=="PCON" )
    {
      Int_t Nz = (NP-3)/3;
      double Z = -100000, R = -100000, z = 100000, r = 100000;
      for( Int_t i=0; i<Nz; i++ )
	{
	  double z1 = P->At(3+i*3+0);
	  double r1 = P->At(3+i*3+1);
	  double R1 = P->At(3+i*3+2);
	  if( r1<r ) r = r1;
	  if( R1>R ) R = R1;
	  if( z1<z ) z = z1;
	  if( z1>Z ) Z = z1;		  
	}      
      CbmKFTube tube( ID, x0,y0,z0+0.5*(z+Z), (Z-z), r, R, RadLength );
      vPassiveTube.push_back(tube);
      ret = &(vPassiveTube.back());
    }  
  else if ( Sname=="PGON" )
    {
      Int_t Nz = (NP-4)/3;
      double Z = -100000, R = -100000, z = 100000, r = 100000;
      for( Int_t i=0; i<Nz; i++ )
	{
	  double z1 = P->At(4+i*3+0);
	  double r1 = P->At(4+i*3+1);
	  double R1 = P->At(4+i*3+2);
	  if( r1<r ) r = r1;
	  if( R1>R ) R = R1;
	  if( z1<z ) z = z1;
	  if( z1>Z ) Z = z1;		  
	}      
      CbmKFTube tube( ID, x0,y0,z0+0.5*(z+Z), (Z-z), r, R, RadLength );
      vPassiveTube.push_back(tube);
      ret = &(vPassiveTube.back());
    }  
  else if ( Sname=="BOX " )
    {
      CbmKFBox box( ID, x0, y0, z0, 2*P->At(0),2*P->At(1),2*P->At(2), RadLength );
      vPassiveBox.push_back(box);
      ret = &(vPassiveBox.back());
    }
  else if ( Sname=="TRAP" )
    {
      int np = node->getNumPoints();
      FairGeoVector vMin=*node->getPoint(0), vMax=vMin;
      for( int i=0; i<np; i++ ){
	FairGeoVector *v = node->getPoint(i);
	for( int j=0; j<3; j++){
	  if( vMin(j) > (*v)(j) ) (&vMin.X())[j]=(*v)(j);
	  if( vMax(j) < (*v)(j) ) (&vMax.X())[j]=(*v)(j);
	}
      }
      FairGeoVector v0 = (vMin+vMax);
      v0*=.5/10.;
      FairGeoVector dv = vMax-vMin;
      dv/=10.;
      CbmKFBox box( ID, x0+v0(0), y0+v0(1), z0+v0(2), dv(0),dv(1), dv(2), RadLength );
      vPassiveBox.push_back(box);
      ret = &(vPassiveBox.back());
    }
  else {
    cout <<" -E- unknown shape : "<<Sname<<endl;
    cout <<" -E- It does not make sense to go on" <<endl;
    cout <<" -E- Stop execution here" <<endl;
    Fatal("CbmKF::ReadPassive","Unknown Shape");
  }
  return ret;
}


Int_t CbmKF::Propagate( Double_t *T, Double_t *C, Double_t z_out, Double_t QP0, Bool_t line ) 
{


if(line) fMethod=0;
else fMethod=2;

  Bool_t err = 0;
  if( fabs(T[5]-z_out)<1.e-5 ) return 0;

  //AZ
  Double_t zmax = 0.0;
  BmnNewFieldMap *field = nullptr;
  if (fMagneticField) {
    field = (BmnNewFieldMap*) fMagneticField; // GP
    zmax = field->GetZmax() + field->GetPositionZ();
   // cout <<"asdassd: " << field->GetZmax() << " " << field->GetPositionZ() << " " << zmax << endl;
  }
  //AZ
 // cout<<"extrap1: "<<z_out<<endl;
  //AZ if ( !fMagneticField || (300<=z_out && 300<=T[5]) )
  if ( !fMagneticField || z_out>400 )//(zmax<=z_out ))// && zmax<=T[5]) )
    {
      //cout<<"extrap2: "<<z_out<<endl;
   // CbmKFFieldMath::ExtrapolateLine( T, C, T, C, z_out );
      CbmKFFieldMath::ExtrapolateALight( T, C, T, C, z_out , QP0, field);//fMagneticField );
      return 0;
    }
  Double_t zz = z_out;
  //AZ if ( z_out<300. && 300<=T[5] )  CbmKFFieldMath::ExtrapolateLine( T, C, T, C, 300. );
  if ( z_out<zmax && zmax<=T[5] )  CbmKFFieldMath::ExtrapolateLine( T, C, T, C, zmax );

  //AZ if ( T[5]<300. && 300.<z_out )
  if ( T[5]<zmax && zmax<z_out )
    {
      //AZ zz = 300.;
      zz = zmax;
    }
  Bool_t repeat = 1;
  while(!err && repeat)
    {
      const Double_t max_step = 5.;
      Double_t zzz;
      if( fabs(T[5]-zz)>max_step ) zzz = T[5]+ ( (zz>T[5]) ?max_step :-max_step);
      else { zzz = zz; repeat = 0; }
      switch( fMethod )
	{
	case 0:
	  {
	    CbmKFFieldMath::ExtrapolateLine( T, C, T, C, zzz );
	    break;
	  }
	case 1:
	  { 
	    err = err || CbmKFFieldMath::ExtrapolateALight( T, C, T, C, zzz , QP0, field );
	    break;
	  }
	case 2:
	  { 
	    err = err || CbmKFFieldMath::ExtrapolateRK4( T, C, T, C, zzz , QP0, field );
	    break;
	  }
	  /*
	case 3:
	  { 
	    CbmKFFieldMath::ExtrapolateACentral( T, C, T, C, zzz , QP0, fMagneticField );
	    break;
	  }
	case 4:
	  { 
	    CbmKFFieldMath::ExtrapolateAnalytic( T, C, T, C, zzz , QP0, fMagneticField, 1 );
	    break;
	  }
	case 5:
	  { 
	    CbmKFFieldMath::ExtrapolateAnalytic( T, C, T, C, zzz , QP0, fMagneticField, 2 );
	    break;
	  }
	case 6:
	  { 
	    CbmKFFieldMath::ExtrapolateAnalytic( T, C, T, C, zzz , QP0, fMagneticField, 3 );
	    break;
	  }
	  */
	}
    }
  if ( T[5]!=z_out ) CbmKFFieldMath::ExtrapolateLine( T, C, T, C, z_out );  
  return err;
}

Int_t CbmKF::PassMaterial( CbmKFTrackInterface &track, Double_t &QP0, Int_t ifst, Int_t ilst ){
  Bool_t downstream = ( ilst > ifst );
  Bool_t err = 0;
  Int_t iend = downstream ? ilst+1 :ilst-1;
  for( Int_t i = ifst; i!=iend; downstream ?++i :--i ){
    err = err || vMaterial[i]->Pass( track, downstream, QP0 );
  }
  return err;
}

Int_t CbmKF::PassMaterialBetween( CbmKFTrackInterface &track, Double_t &QP0, Int_t ifst, Int_t ilst ){
  Bool_t downstream = ( ilst > ifst );
  Bool_t err = 0;
  Int_t istart = downstream ? ifst+1 :ifst-1;
  for( Int_t i = istart; i!=ilst; downstream ?++i :--i ){
    err = err || vMaterial[i]->Pass( track, downstream, QP0 );
  }
  return err;
}

Int_t CbmKF::PassMaterialBetween( CbmKFTrackInterface &track, Double_t &QP0, CbmKFHit *fst, CbmKFHit *lst ){
  return PassMaterialBetween( track, QP0, fst->MaterialIndex, lst->MaterialIndex);
}

ClassImp(CbmKF)
