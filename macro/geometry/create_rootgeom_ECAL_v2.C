/******************************************************************************
 *
 *  File: create_rootgeom_ECAL_v2.C
 *  Last modified: 09-08-2019 by Petr Alekseev <pnaleks@gmail.com>
 *
 *  Example of use: 
 *  root -l '$VMCWORKDIR/macro/geometry/create_rootgeom_ECAL_v2.C(7,4)'
 *	This will generate root geometry file 
 *  $VMCWORKDIR/geometry/ECAL_v2_run7_pos4.root
 ******************************************************************************/

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMedium.h"
#include "TFile.h"

#include "../run/bmnloadlibs.C"

//________________________________________________________________________________
//ECAL = 8x7x9x215
//ECAL = 8 SuperModules 
//SuperModule = 7 Modules
//Module = 9 ADCmodules
//ADCmodule = X:4cm x  Y:4cm x Z:215 layers * (0.045 Pb + 0.15 Sci) 
//front steel cover ---PIPEC FIXME
//ADCmodules == modules in the code
                                             
//________________________________________________________________________________
//ECAL Position and rotation in BM@N
Double_t ecalX = -175;          //-175;     //-55   //ecalX = -167.18
Double_t ecalY = 12.4;          //12.4;     //12.4  //ecalY = 13.155;
Double_t ecalZ = 283.1;         //283.1;    //283.1 //ecalZ = 263.437;
Double_t rotationAngle = -20;       //-20;  //10;   //rotationAngle = -15.;

void SetEcalPosition(Int_t run, Int_t pos) {
    if (run == 7) {
        switch (pos) {
            case 1:
                ecalX = -44; ecalY = 12.4; ecalZ = 843.1; rotationAngle = 0;
                return;
            case 2:
                ecalX = -175; ecalY = 12.4; ecalZ = 883.1; rotationAngle = -10;
                return;
            case 3:
                ecalX = -175; ecalY = 12.4; ecalZ = 283.1; rotationAngle = -10;
                return;
            case 4:
                ecalX = -55; ecalY = 12.4; ecalZ = 283.1; rotationAngle = -10;
                return;
            case 5:
                ecalX = -175; ecalY = 12.4; ecalZ = 283.1; rotationAngle = -20;
                return;
        }
    }
    Fatal("Main", "ECAL run %d position %d is not defined in " __FILE__, run, pos);
}

//________________________________________________________________________________
//ECAL Parameters	
int Xmodules = 24; //X modules = ADCmodules 
int Ymodules = 21;
int Zlayers = 215;
Double_t thickPb = 0.030; //lead
Double_t thickSci = 0.15; //scintilator
Double_t thickSteel = 0.2; //front cover FIXME
Double_t thickPaint = 0.0075; //paint
Double_t thickPbSciPaint = thickPaint + thickPb + thickPaint + thickSci;
Double_t modwidth = 4;
Double_t modheight = 4;
Double_t modlength = thickSteel + thickPbSciPaint * Zlayers;

//media ============================================
struct {
    TGeoMedium *vacuum = 0;
    TGeoMedium *air = 0;
    TGeoMedium *iron = 0;
    TGeoMedium *lead = 0;
    TGeoMedium *scint = 0;
    TGeoMedium *steel = 0;
    TGeoMedium *tyvec = 0;
    TGeoMedium *slot = 0;
    TGeoMedium *TiO2 = 0;
} mMed;

TGeoMedium * DefineMedia(FairGeoMedia* geoMedia, FairGeoBuilder* geoBuild, const char * name) {
    FairGeoMedium* pFairGeoMedium = geoMedia->getMedium(name);
    if (!pFairGeoMedium) Fatal("Main", "FairMedium %s not found", name);
    geoBuild->createMedium(pFairGeoMedium);
    TGeoMedium * pTGeoMedium = gGeoManager->GetMedium(name);
    if (!pTGeoMedium) Fatal("Main", "Medium %s not found", name);
    return pTGeoMedium;
}

void DefineRequiredMedia(FairGeoMedia* geoMedia, FairGeoBuilder* geoBuild) {

    mMed.vacuum = DefineMedia(geoMedia, geoBuild, "vacuum");
    //mMed.air = DefineMedia(geoMedia, geoBuild, "air");
    //mMed.iron = DefineMedia(geoMedia, geoBuild, "iron");
    //mMed.tyvec = DefineMedia(geoMedia, geoBuild, "tyvec_ZDC");
    mMed.lead = DefineMedia(geoMedia, geoBuild, "lead");
    //mMed.scint = DefineMedia(geoMedia, geoBuild, "polystyrene"); // medium polystyrene (Scint.)
    mMed.scint = DefineMedia(geoMedia, geoBuild, "FscScint"); // medium FscScint (Scint.)
    //mMed.slot = DefineMedia(geoMedia, geoBuild, "plasticWLS_ZDC"); // medium plastic (slot)
    //mMed.steel = DefineMedia(geoMedia, geoBuild, "steel");
    mMed.steel = DefineMedia(geoMedia, geoBuild, "steel");
    mMed.TiO2 = DefineMedia(geoMedia, geoBuild, "TiO2");

}//DefineRequiredMedia
	
///
/// \param[in] run    BMN run number (only run 7 is currently supported)
/// \param[in] pos    ECAL position in the run (five positions for run 7 are defined)
///
void create_rootgeom_ECAL_v2(Int_t run, Int_t pos){

    bmnloadlibs(); // load libraries

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");
    
    // ---- Geometry file name (output) ----------------------------------------
    const TString geoDetectorName = "ECAL";
    const TString geoDetectorVersion = "v2";
    TString geoFileName;
    geoFileName.Form("%s/geometry/%s_%s_run%d_pos%d.root", gPath.Data(), geoDetectorName.Data(), geoDetectorVersion.Data(), run, pos);
    cout << geoFileName << endl;
	
    SetEcalPosition(run, pos);
//  TGeoRotation   *rot = new TGeoRotation("rot1", rotationAngle, 0., 0.);
//  TGeoRotation   *rot = new TGeoRotation("rot1", 0., rotationAngle, 0.);
//  TGeoRotation   *rot = new TGeoRotation("rot1", 0., 0., rotationAngle);
	
    TGeoRotation *rot = new TGeoRotation("rot1", 0., 0., 0.);
    rot->Print();
    rot->RotateY(rotationAngle);
    
    TGeoCombiTrans *combi1 = new TGeoCombiTrans(ecalX, ecalY, ecalZ, rot);

    // ----  global geometry parameters  ---------------------------------------
    FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
	
    // -------   Load media from media file   ----------------------------------
    TString medFile = gPath + "/geometry/media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    FairGeoMedia*   geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();
    geoMedia->list();
    geoMedia->print();
    DefineRequiredMedia(geoMedia, geoBuild);

    // ------------------------ VOLUMES ----------------------------------------
    
    // --------------   Create geometry and global top volume  -----------------
    
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(mMed.air); // ??? it was vacuum ???
//    top->SetLineColor(kBlue);
//    top->SetVisibility(kTRUE);
//    top->SetTransparency(100);

    gGeoManager = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoManager->SetName(geoDetectorName + "_geom");
    gGeoManager->SetTopVolume(top);
//    gGeoManager->SetTopVisible(1);
    
    TGeoVolume *ecal = gGeoManager->MakeBox("ecal", mMed.vacuum, Xmodules*modwidth/2, Ymodules*modheight/2, modlength/2);
//    ecal->SetVisibility(kTRUE);
//    ecal->SetTransparency(100);
//    combi1->RotateY(rotationAngle);
    top->AddNode(ecal, 0, combi1);
	
    TGeoVolume *module = gGeoManager->MakeBox("mod", mMed.vacuum, modwidth/2, modheight/2, modlength/2);
//    module->SetVisibility(kTRUE);
//    module->SetTransparency(100);
//    module->SetLineColor(kRed);
//    module->SetLineWidth(0);
    
    TGeoVolume *layerPb = gGeoManager->MakeBox("layPb", mMed.lead, modwidth/2, modheight/2, thickPb/2);
//    layerPb->SetVisibility(kTRUE);
//    layerPb->SetLineColor(kBlue);
//    layerPb->SetLineWidth(0);
//    layerPb->SetTransparency(100);  	
	
    TGeoVolume *layerPaint = gGeoManager->MakeBox("layPaint", mMed.TiO2, modwidth/2, modheight/2, thickPaint/2); 
//    layerPaint->SetVisibility(kTRUE);
//    layerPaint->SetLineColor(kWhite);
//    layerPaint->SetLineWidth(0);
//    layerPaint->SetTransparency(100);  	
	
    TGeoVolume *layerSci = gGeoManager->MakeBox("laySci", mMed.scint, modwidth/2, modheight/2, thickSci/2); 
//    layerSci->SetLineColor(kWhite+1);
//    layerSci->SetTransparency(100);

    TGeoVolume *covSteel = gGeoManager->MakeBox("covSteel", mMed.air, modwidth/2, modheight/2, thickSteel/2); // ??? should be steel ??? 
//    covSteel->SetVisibility(kTRUE);
//    covSteel->SetLineColor(kWhite);
//    covSteel->SetTransparency(100);
  	

    // ------------------ STRUCTURE  -------------------------------------------
    module->AddNode(covSteel, 0, new TGeoTranslation(0,0, thickSteel/2 - modlength/2));
    Double_t z;
//    Zlayers = 1;
    Int_t nlay;
    for(int zlayer = 0; zlayer < Zlayers; zlayer++){
        z = thickSteel + thickPbSciPaint * zlayer;
        //TString layerNamePb = TString::Format("layerPb%d",zlayer);
        //TString layerNameSci = TString::Format("layerSci%d",zlayer);	
        module->AddNode(layerPaint, nlay, new TGeoTranslation(0,0, z + thickPaint/2 -modlength/2));
        module->AddNode(layerPb, nlay, new TGeoTranslation(0,0, z + thickPaint + thickPb/2 - modlength/2));
        module->AddNode(layerPaint, nlay, new TGeoTranslation(0,0, z + thickPaint + thickPb + thickPaint/2 -modlength/2));
        module->AddNode(layerSci, zlayer, new TGeoTranslation(0,0, z + thickPaint + thickPb + thickPaint + thickSci/2 - modlength/2));
    }
	
    int nmod = 0;
    //Xmodules = 1;
    //Ymodules = 1;
    Double_t x0 = modwidth*Xmodules/2 - modwidth/2;
    Double_t y0 = modheight*Ymodules/2 - modheight/2;
    for(int xmod = 0 ; xmod < Xmodules; xmod++){
        for(int ymod = 0; ymod < Ymodules; ymod++){
            TString moduleName = TString::Format("mod_X:%d_Y:%d",xmod,ymod);
            module->SetName(moduleName);
            cout<<module->GetName()<<endl;
            ecal->AddNode(module, nmod++, new TGeoTranslation(x0 - modwidth * xmod,	-y0 + modheight * ymod,0));
        }//ymod
    }//xmod			

    top->SetVisContainers(kTRUE);
    gGeoManager->CloseGeometry();
    gGeoManager->CheckOverlaps(0.001);
    gGeoManager->PrintOverlaps();
    gGeoManager->Test();
    //gGeoMan->SetVisLevel(3);
    //gGeoMan->SetVisOption(1);
    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();
    //top->Draw("ogl");
}

