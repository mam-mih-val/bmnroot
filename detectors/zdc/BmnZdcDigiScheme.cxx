/*************************************************************************************
 *
 *         Class BmnZdcDigiScheme
 *         
 *  Author:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  18-11-2015   
 *
 ************************************************************************************/

#include "BmnZdcDigiScheme.h"
#include "TGeoBBox.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include <iostream>

using std::cout;
using std::endl;


BmnZdcDigiScheme* BmnZdcDigiScheme::fInstance = 0;
Bool_t BmnZdcDigiScheme::fInitialized = 0;
Int_t BmnZdcDigiScheme::fRefcount = 0;

static Int_t kZDC = 1; //  // hard-coded basic ZDC detector number


// -------------------------------------------------------------------------

BmnZdcDigiScheme::BmnZdcDigiScheme() : 
Nx(0),Ny(0),Nz(0)
{
  fZdcDigiPar=0;
}

// -------------------------------------------------------------------------

BmnZdcDigiScheme::~BmnZdcDigiScheme()
{
  fRefcount--;
  if(!fRefcount){
    delete this;
    fInstance = NULL;
  }
}
// -------------------------------------------------------------------------

BmnZdcDigiScheme*  BmnZdcDigiScheme::Instance()
{
  if(!fInstance) fInstance = new BmnZdcDigiScheme();
  fRefcount++;
  return fInstance;
}

// -------------------------------------------------------------------------

Bool_t BmnZdcDigiScheme::Init (BmnZdcGeoPar* geoPar, BmnZdcDigiPar* digiPar, Int_t pVerbose)
{
  if (!fInitialized){
 
    if ( ! geoPar ) {
      cout << "-W- BmnZdcDigiScheme::Init: "
	   << "No geometry parameters available!" << endl;
      return kFALSE;
    }
    if ( ! digiPar ) {
      cout << "-W-  BmnZdcDigiScheme::Init: "
	   << "No digitization parameters available!" << endl;
      //      return kFALSE;
    }
    fZdcDigiPar = digiPar;

    TObjArray* sensNodes = geoPar->GetGeoSensitiveNodes();
    if (!sensNodes) {
      cout << "-W-  BmnZdcDigiScheme::Init: "
	   << "No sensitive nodes available!" << endl;
      return kFALSE;
    }

    fPasNodes = geoPar->GetGeoPassiveNodes();
    if (!fPasNodes) {
      cout << "-W-  BmnZdcDigiScheme::Init: "
	   << "No passive nodes available!" << endl;
      return kFALSE;
    }

    fInitialized = kTRUE;

    AddNodes (sensNodes, pVerbose );  
    CalcDimensions (kZDC,Nx,Ny,Nz);

  if (pVerbose) 
      cout << endl << "-W-  BmnZdcDigiScheme::Init: finished." << endl;
  
  }
  return kTRUE;
}

// -------------------------------------------------------------------------

BmnZdcVolInfo_t* BmnZdcDigiScheme::CreateVolInfoElement (FairGeoNode* nod, Int_t pVerbose)
{
  if (!nod)
    return NULL;

  static TString root_name_copy1="";
  static Double_t volData[6]={0,0,0,0,0,0};
  Int_t i,j;

  TString shape_name = nod->getShapePointer()->GetName();
  TString root_name = nod->getRootVolume()->GetName();
  TString tmp="          ";
  TGeoBBox* shape=0;

  FairGeoVector pos= nod->getLabTransform()->getTranslation();

    for (i=0;i<3;i++)
      volData[i]=pos.getValues(i);     // [cm]

  if (root_name!=root_name_copy1) {

    root_name_copy1 = root_name;

    shape =   (TGeoBBox*) nod->getRootVolume()->GetShape(); 

    if (shape_name.Contains("BOX")) {
      volData[3]= shape->GetDX();  // [cm]
      volData[4]= shape->GetDY();  // [cm]
      volData[5]= shape->GetDZ();  // [cm]      
    }
    else {
      if (shape_name.Contains("PGON")) {
	volData[3]= nod->getPoint(2)->getZ() /10.;  // [cm]
	volData[4]= volData[3];
	volData[5]= shape->GetDZ();                 // [cm]
      }
    }
  }

  BmnZdcVolInfo_t *volInfo = new BmnZdcVolInfo_t;
  for (i=0;i<6;i++)
    volInfo->push_back(volData[i]);
  
  if (pVerbose>1) {
    if (pVerbose>2)
      tmp = "  root: "+root_name + ", shape: " + shape_name;
    tmp += "X,Y,Z,   Dx,Dy,Dz: ";
    cout << tmp << 
      volData[0] << "," << volData[1] << "," << volData[2] << ",   " <<
      volData[3] << "," << volData[4] << "," << volData[5]  << endl;
  }
  return volInfo;
}


// -------------------------------------------------------------------------

BmnZdcVolId_t* BmnZdcDigiScheme::CreateVolElement (FairGeoNode* nod, Int_t nodeNumber,
						   BmnZdcDigiId_t* right, Int_t pVerbose)
{
  if (!nod)
    return NULL;

  FairGeoNode *nod0, *nod1;
  TString mother_name, tmp;

  //  FairGeoTransform *center;

  TString nod_name=nod->GetName();
  mother_name = nod->getMother();

  //  if (nod_name.Contains("boxs") {
  if (1)
  {

    //   (*right).push_back(nod->getRootVolume()->GetNumber()); // GroupID(now=box volume ID)
    if (nod_name.Contains("boxs"))
      (*right).push_back(1);                               //  GroupID ("size" of small ZDC module)
    else
      (*right).push_back(2);                               //  GroupID ("size" of big ZDC module)
   (*right).push_back(nod->getCopyNo());                  // ModuleID inside group
   //   center = nod->getLabTransform(); 
    (*right).push_back(nodeNumber);                        // ChannelID

    BmnZdcVolId_t* left = new BmnZdcVolId_t;
    (*left).push_back(nod->getRootVolume()->GetNumber()+3);  // sensitive volume ID
    (*left).push_back(nod->getCopyNo());                    // MotherMotherCopyNo

    if (pVerbose>1) {
      tmp = "  mother: "+mother_name + ", me: " + (nod->getName()) +"    ";
      cout << tmp << 
	(*left)[0] << "," << (*left)[1]<<  " : " << 
	(*right)[0] << "," << (*right)[1]<< "," << (*right)[2] << endl;
    }

    return left;

  }
  else {
    cout << "-E-  BmnZdcDigiScheme::CreateVolInfoElement:  Strange for me node: "
	 << nod->GetName() << "  Node number:" << nodeNumber  << "  Mother:" << mother_name << endl;
    return NULL;
  }

}

// -------------------------------------------------------------------------

Bool_t BmnZdcDigiScheme::AddNodes (TObjArray* sensNodes, Int_t pVerbose)
{
  Int_t nNodes = sensNodes->GetEntriesFast();
  Int_t nPasNodes = fPasNodes->GetEntriesFast();
  FairGeoNode *nod=0;
  Int_t nodeNumber,nodeCopyNo,nodeVolumeId, chanId2=0, chanId1=0, zdc_channel=0;
  BmnZdcVolId_t *left1,*left2;
  BmnZdcDigiId_t *right1,*right2;

  // if (pVerbose) {
  //     cout << "-W-  BmnZdcDigiScheme::AddNodes: started.  nNodes:" << nNodes  << endl;
  // }

  // for (nodeNumber=0;nodeNumber<nNodes;nodeNumber++) {

  if (pVerbose) {
    //      cout << "-W-  BmnZdcDigiScheme::AddNodes: started.  nNodes:" << nNodes  << endl;
      cout << "-W-  BmnZdcDigiScheme::AddNodes: started.  nNodes:" << nPasNodes  << endl;
  }

  TString nod_name;

  for (nodeNumber=1;nodeNumber<nPasNodes;nodeNumber++) {
    //    nod = (FairGeoNode*)sensNodes->At(nodeNumber);
    nod = (FairGeoNode*)fPasNodes->At(nodeNumber);  // we will use boxes, not layers
    if (nod) {

      nod_name = nod->getName();
      if (nod_name.Contains("box")) {

        zdc_channel++;

	right1 = new BmnZdcDigiId_t;
	//	left1 = CreateVolElement(nod, nodeNumber, right1, pVerbose);
	left1 = CreateVolElement(nod, zdc_channel, right1, pVerbose);

	fVolToDigiIdMap[*left1]=*right1;


	CreateVolInfoElement(nod,1);
	// left2 = new BmnZdcVolId_t ((*left1).begin(),(*left1).end());
	// (*left2)[0]=(*left1)[0]+1;
	// right2 = new BmnZdcDigiId_t (right1->begin(),right1->end());
	// (*right2)[0] = (*right1)[0]+1;

	// fVolToDigiIdMap[*left2]=*right2;

	// CreateVolCopyElements (left1, right1); 
      }

    }
    else {
      cout << "-W-  BmnZdcDigiScheme::AddNodes: "
	//	   << "Node number "<< nodeNumber << " from " << nNodes  << " not found!" << endl;
	   << "Node number "<< nodeNumber << " from " << nPasNodes  << " not found!" << endl;
      return kFALSE;
    }
  }
  return kTRUE;
}

// -------------------------------------------------------------------------

Bool_t BmnZdcDigiScheme::CreateVolCopyElements  (BmnZdcVolId_t* left, BmnZdcDigiId_t* right)
{
  BmnZdcVolId_t *left1,*left2;
  BmnZdcDigiId_t *right1,*right2;

  if (!fPasNodes) 
    return kFALSE;

  FairGeoNode *nod1 =(FairGeoNode*) fPasNodes->At(fPasNodes->GetEntries()-2);

  if (!nod1) 
    return kFALSE;

  Int_t moduleID,nModules; // {MotherMotherCopyNo, MotherCopyNo, VolumeId, CopyNo},{GroupID, ModuleID, ChannelID}

  TString lm_name = nod1->getName();
  TString last_module_number (&(lm_name[lm_name.Last('#')+1]));
  nModules = last_module_number.Atoi();

  for (moduleID=1; moduleID<nModules;  moduleID++) {

      left1 = new BmnZdcVolId_t ((*left).begin(),(*left).end());
      (*left1)[1]=(*left)[1]+moduleID;
      right1 = new BmnZdcDigiId_t (right->begin(),right->end());
      (*right1)[1] = (*right)[1]+moduleID;

      fVolToDigiIdMap[*left1]=*right1;

  }

  return kTRUE;
}


// -------------------------------------------------------------------------

Bool_t BmnZdcDigiScheme::CreateVolInfoCopyElements  (BmnZdcDigiId_t* right, BmnZdcVolInfo_t *volInfo )
{


  if (!fPasNodes) 
    return kFALSE;

  FairGeoNode *nod1 =(FairGeoNode*)fPasNodes->At(fPasNodes->GetEntries()-2);

  if (!nod1) 
    return kFALSE;

  Int_t moduleID,nModules; 

  TString lm_name = nod1->getName();
  TString last_module_number (&(lm_name[lm_name.Last('#')+1]));
  nModules = last_module_number.Atoi();

  for (moduleID=1; moduleID<nModules;  moduleID++) {

    nod1 = (FairGeoNode*) fPasNodes->At(fPasNodes->GetEntries()-2 - nModules+moduleID);

    FairGeoVector pos= nod1->getLabTransform()->getTranslation();

    BmnZdcVolInfo_t *volInfo1 = new BmnZdcVolInfo_t ((*volInfo).begin(),(*volInfo).end());
    (*volInfo1)[0]=pos.getValues(0);    // X [cm]
    (*volInfo1)[1]=pos.getValues(1);    // Y [cm]

    BmnZdcDigiId_t *right1 = new BmnZdcDigiId_t (right->begin(),right->end());
    (*right1)[1] = (*right)[1]+moduleID;

    BmnZdcDigiId_t *right2 = new BmnZdcDigiId_t (right1->begin(),right1->end());
    (*right2)[0] = (*right1)[0]+1;

    BmnZdcVolInfo_t *volInfo2 = new BmnZdcVolInfo_t (*volInfo1);
    (*volInfo2)[2]=-(*volInfo2)[2];  // Z [cm]

    fDigiToVolInfoMap[*right1]=volInfo1;
    fDigiToVolInfoMap[*right2]=volInfo2;
  }

  return kTRUE;
}


// -------------------------------------------------------------------------

Bool_t BmnZdcDigiScheme::GetVolCenterXYZ (BmnZdcDigiId_t* pDigiID, Double_t &x, Double_t &y,Double_t &z)
{
  if (!pDigiID)
      return kFALSE;
  if (fDigiToVolInfoMap.find(*pDigiID)==fDigiToVolInfoMap.end())
    return kFALSE;
  else {
    BmnZdcVolInfo_t* volInfo = fDigiToVolInfoMap[*pDigiID];
    if (volInfo) {
      x=(*volInfo)[0];
      y=(*volInfo)[1];
      z=(*volInfo)[2];
      return kTRUE;
    }
    else
      return kFALSE;
  }
}

// -------------------------------------------------------------------------

Bool_t BmnZdcDigiScheme::GetVolDxDyDz   (BmnZdcDigiId_t* pDigiID, Double_t &Dx, Double_t &Dy, Double_t &Dz)
{
  if (!pDigiID)
      return kFALSE;
  if (fDigiToVolInfoMap.find(*pDigiID)==fDigiToVolInfoMap.end())
    return kFALSE;
  else {
    BmnZdcVolInfo_t* volInfo = fDigiToVolInfoMap[*pDigiID];
    if (volInfo) {
      Dx=(*volInfo)[3];
      Dy=(*volInfo)[4];
      Dz=(*volInfo)[5];
      return kTRUE;
    }
    else
      return kFALSE;
  }
}

// -------------------------------------------------------------------------

Bool_t BmnZdcDigiScheme::IsVolumeExist     (BmnZdcVolId_t* pVolId)
{
  if (!pVolId)
    return kFALSE;
  else
    return (!(fVolToDigiIdMap.find(*pVolId)==fVolToDigiIdMap.end()));
}

// -------------------------------------------------------------------------

BmnZdcDigiId_t BmnZdcDigiScheme::GetDigiId     (BmnZdcVolId_t* pVolId)
{
  static const BmnZdcDigiId_t not_found (4,-1);

  if (IsVolumeExist(pVolId))
    return fVolToDigiIdMap[*pVolId]; 
  else 
    return not_found;
}

// -------------------------------------------------------------------------

Int_t BmnZdcDigiScheme::GetGroupID (BmnZdcVolId_t* pVolId)
{
  BmnZdcDigiId_t  digiID = GetDigiId (pVolId);
  //  return digiID.first;
  return digiID[0];
}

// -------------------------------------------------------------------------

Int_t BmnZdcDigiScheme::GetChannelID  (BmnZdcVolId_t* pVolId)
{
  BmnZdcDigiId_t  digiID = GetDigiId (pVolId);
  //  return digiID.second;
  return digiID[2];
}

// -------------------------------------------------------------------------

BmnZdcVolInfo_t* BmnZdcDigiScheme::GetVolInfo(BmnZdcVolId_t* pVolId)
{
  if (IsVolumeExist(pVolId)) {

    BmnZdcDigiId_t pDigiID = GetDigiId(pVolId);

    if (fDigiToVolInfoMap.find(pDigiID)==fDigiToVolInfoMap.end())
      return NULL;
    else 
      return fDigiToVolInfoMap[pDigiID]; 
  }
  else 
    return NULL;
}


// -------------------------------------------------------------------------

void BmnZdcDigiScheme::PrintVolume (Int_t volID, Int_t copyNoMotherMother)
{
  Int_t content[]={volID,copyNoMotherMother};
  BmnZdcVolId_t pVolId (content,content+sizeof(content)/sizeof(Int_t));

  BmnZdcDigiId_t pDigiID = GetDigiId(&pVolId);

  cout << " BmnZdc Volume: "  <<   volID<< "," << copyNoMotherMother << 
    "   DigiID: " << pDigiID[0] << "," << pDigiID[1]<< "," << pDigiID[2] ;

if (IsVolumeExist(&pVolId))
  cout << " found ";

  BmnZdcVolInfo_t* pVolInfo = GetVolInfo (&pVolId);

  if (pVolInfo)
    cout << "    X,Y,Z [cm]: " <<  (*pVolInfo)[0]<< "," << (*pVolInfo)[1]<< "," << (*pVolInfo)[2]<< 
      "    Dx,Dy,Dz [cm]: " <<  (*pVolInfo)[3]<< "," << (*pVolInfo)[4]<< "," << (*pVolInfo)[5] ;
  else
    cout << " (no VolInfo) ";

  cout << endl;
}



// -------------------------------------------------------------------------

Bool_t BmnZdcDigiScheme::CalcDimensions (Int_t pGlobalDetectorNumber, Int_t &nx, Int_t &ny, Int_t &nz)
{

  if (fDigiToVolInfoMap.empty())
    return kFALSE;

  Bool_t result = kFALSE;
  std::map<BmnZdcDigiId_t,BmnZdcVolInfo_t*>::iterator it;
  std::map<Double_t,Int_t>  xmap, ymap, zmap;
  Double_t x,y,z;
  nx = ny = nz = 0;

  for ( it=fDigiToVolInfoMap.begin() ; it != fDigiToVolInfoMap.end(); ++it ) {

    //    cout << ". ";

    //    if ((*it).first.first==pGlobalDetectorNumber) {
    //    if ((*it).first[0]==1) {
    if (1) {

      result = kTRUE;

      x = (*((*it).second))[0];
      if (xmap.count(x))
	xmap[x]= xmap[x]+1;
      else
	xmap[x]= 1;

      y = (*((*it).second))[1];
      if (ymap.count(y))
	ymap[y]= ymap[y]+1;
      else
	ymap[y]= 1;

      z = (*((*it).second))[2];
      if (zmap.count(z))
	zmap[z]= zmap[z]+1;
      else
	zmap[z]= 1;
    }
  }

  nx = xmap.size();
  ny = ymap.size();
  nz = zmap.size();

  return result;
}

// -------------------------------------------------------------------------

void BmnZdcDigiScheme::GetZdcDimensions (Int_t &nx, Int_t &ny, Int_t &nz)
{
  nx=Nx; ny=Ny; nz=Nz;
}


// -------------------------------------------------------------------------

BmnZdcDigiId_t  BmnZdcDigiScheme::GetDigiIdFromCoords  (Double_t x, Double_t y, Double_t z)
{
  Int_t content[]={-1,-1};
  BmnZdcVolId_t resultmc (content,content+sizeof(content)/sizeof(Int_t));

  BmnZdcDigiId_t result (-1,-1);
  if (gGeoManager) {
    TGeoNode *tgn = gGeoManager->FindNode(x,y,z);
    if (tgn) {
      resultmc[0] = (gGeoManager->GetMother(2))->GetNumber();
      resultmc[1] = (gGeoManager->GetMother(1))->GetMotherVolume()->GetNumber();
      resultmc[2] = tgn->GetVolume()->GetNumber();
      resultmc[3] = tgn->GetNumber();
      if (fVolToDigiIdMap.find(resultmc)!=fVolToDigiIdMap.end())
	result = fVolToDigiIdMap[resultmc];
    }
  }
  return result;
}


// -------------------------------------------------------------------------

BmnZdcDigiId_t  BmnZdcDigiScheme::GetDigiIdFromVolumeData  (Int_t pMcVolumeNumber, Int_t pMotherMotherCopyNumber)
{
  Int_t content[]={pMcVolumeNumber,pMotherMotherCopyNumber};
  BmnZdcVolId_t pVolId (content,content+sizeof(content)/sizeof(Int_t));
  BmnZdcDigiId_t  digiID = GetDigiId (&pVolId);
  return digiID;
}

// -------------------------------------------------------------------------

void BmnZdcDigiScheme::SplitDigiID (BmnZdcDigiId_t digiID, Int_t &module_groupID, Int_t &modID, Int_t &chanID)
{
  module_groupID = digiID[0];
  modID = digiID[1];
  chanID = digiID[2];
}

// -------------------------------------------------------------------------

void BmnZdcDigiScheme::Print() 
{
  cout << "*********************************************" << endl;

  cout <<  "***  BmnZdcDigiScheme:" << endl;
  cout << " BmnZdc  Nx,Ny,Nz:  " << Nx << "," << Ny<< "," << Nz;

  std::map<BmnZdcVolId_t,BmnZdcDigiId_t>::iterator it;

  for ( it=fVolToDigiIdMap.begin() ; it != fVolToDigiIdMap.end(); ++it)
    PrintVolume((*it).first[0],(*it).first[1]);

  cout << "*********************************************" << endl;
}

// -------------------------------------------------------------------------

Bool_t  BmnZdcDigiScheme::GetGroupIdModIdChanId (Int_t pMcVolumeNumber, Int_t pMcCopyNumber, Int_t pMotherCopyNumber, Int_t pMotherMotherCopyNumber, Int_t &pGroupId, Int_t &pChanId, Int_t &pModId)
{
  Int_t content[]={pMotherMotherCopyNumber,pMotherCopyNumber,pMcVolumeNumber,pMcCopyNumber};
  BmnZdcVolId_t pVolId (content,content+sizeof(content)/sizeof(Int_t));
  BmnZdcDigiId_t  digiID = GetDigiId (&pVolId);
  pGroupId = digiID[0];
  pModId = digiID[1];
  pChanId = digiID[2];

  return kTRUE;
} 

// -------------------------------------------------------------------------
ClassImp(BmnZdcDigiScheme)

