/**
 * \file BmnTrackingGeometryConstructor.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2008-2014
 **/

#include "BmnTrackingGeometryConstructor.h"
//#include "CbmLitFieldGridCreator.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoShape.h"
#include "TGeoMedium.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "TGeoCone.h"
#include "TGeoSphere.h"
#include "TGeoArb8.h"
#include "TGeoPgon.h"
#include "TProfile2D.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TRegexp.h"

#include <set>
#include <map>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <limits>

using namespace std;

BmnTrackingGeometryConstructor::BmnTrackingGeometryConstructor():
   fGeo(NULL),
   fNofTrdStations(-1),
   fNofMuchStations(-1),
   fNofMvdStations(-1),
   fNofStsStations(-1),
   fDet()
{
	fGeo = gGeoManager;
	fDet.DetermineSetup();
}

BmnTrackingGeometryConstructor::~BmnTrackingGeometryConstructor()
{
}

BmnTrackingGeometryConstructor* BmnTrackingGeometryConstructor::Instance()
{
   static BmnTrackingGeometryConstructor instance;
   return &instance;
}

//void BmnTrackingGeometryConstructor::GetMuchLayoutVec(
//   lit::parallel::LitDetectorLayoutVec& layout)
//{
//   GetMuchLayout(layout);
//}
//
//void BmnTrackingGeometryConstructor::GetMuchLayoutScal(
//   lit::parallel::LitDetectorLayoutScal& layout)
//{
//   GetMuchLayout(layout);
//}

//template<class T> void BmnTrackingGeometryConstructor::GetMuchLayout(
//   lit::parallel::LitDetectorLayout<T>& layout)
//{
//   cout << "Getting MUCH layout for parallel version of tracking...\n";
//
//   // Read file with TProfile2D containing silicon equivalent of the material
//   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
//   TString matBudgetFile = parDir + "/littrack/much_v12b.silicon.root";
//   TFile* oldFile = gFile;
//   TDirectory* oldDirectory = gDirectory;
//   TFile* file = new TFile(matBudgetFile, "READ");
//   CbmHistManager hm;
//   hm.ReadFromFile(file);
//
//   CbmLitFieldGridCreator gridCreator;
//
//   const TGeoNode* much = static_cast<const TGeoNode*>(fGeo->GetTopNode()->GetNodes()->FindObject("much_0"));
//   TObjArray* muchNodes = much->GetNodes();
//   Int_t currentStation = 0;
//   Int_t currentLayer = 0;
//   for (Int_t iMuchNode = 0; iMuchNode < muchNodes->GetEntriesFast(); iMuchNode++) {
//      const TGeoNode* muchNode = static_cast<const TGeoNode*>(muchNodes->At(iMuchNode));
//
//      if (TString(muchNode->GetName()).Contains("station")) {
//         TObjArray* layerNodes = muchNode->GetNodes();
//
//         for (Int_t iLayerNode = 0; iLayerNode < layerNodes->GetEntriesFast(); iLayerNode++) {
//            const TGeoNode* layer = static_cast<const TGeoNode*>(layerNodes->At(iLayerNode));
//
//            Double_t z = much->GetMatrix()->GetTranslation()[2]
//                       + muchNode->GetMatrix()->GetTranslation()[2]
//                       + layer->GetMatrix()->GetTranslation()[2];
//
//            // Convert material for this station
//            TProfile2D* profile = (iLayerNode == 0)
//                  ? hm.P2("hrl_ThicknessSilicon_MuchAbsorber_" + lit::ToString<Int_t>(currentStation) + "_P2")
//                  : hm.P2("hrl_ThicknessSilicon_Much_" + lit::ToString<Int_t>(currentLayer) + "_P2");
//            //profile->Rebin2D(200, 200);
//            lit::parallel::LitMaterialGrid material;
//            ConvertTProfile2DToLitMaterialGrid(profile, &material);
//
//            lit::parallel::LitVirtualStation<T> vs;
//            vs.SetMaterial(material);
//
//            lit::parallel::LitStation<T> station;
//            station.AddVirtualStation(vs);
//
//            layout.AddStation(station);
//
//
//            currentLayer++;
//         }
//         currentStation++;
//      }
//   }
//
//   gFile = oldFile;
//   gDirectory = oldDirectory;
//   file->Close();
//   delete file;
//
//   cout << layout;
//   cout << "Finish getting MUCH layout for parallel version of tracking\n";
//}

//void BmnTrackingGeometryConstructor::GetTrdLayoutVec(
//   lit::parallel::LitDetectorLayoutVec& layout)
//{
//   GetTrdLayout(layout);
//}
//
//void BmnTrackingGeometryConstructor::GetTrdLayoutScal(
//   lit::parallel::LitDetectorLayoutScal& layout)
//{
//   GetTrdLayout(layout);
//}

//template<class T>
//void BmnTrackingGeometryConstructor::GetTrdLayout(
//   lit::parallel::LitDetectorLayout<T>& layout)
//{
//   cout << "Getting TRD layout for parallel version of tracking...\n";
//
//   // Read file with TProfile2D containing silicon equivalent of the material
//   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
//   TString matBudgetFile = parDir + "/littrack/trd_v13p_3e.silicon.root";
//   TFile* oldFile = gFile;
//   TDirectory* oldDirectory = gDirectory;
//   TFile* file = new TFile(matBudgetFile, "READ");
//   CbmHistManager hm;
//   hm.ReadFromFile(file);
//
//   CbmLitFieldGridCreator gridCreator;
//
//   Double_t startZPosition = 100.;// Last STS station
////   Double_t detectorZPosition = 400.; // We want to extrapolate up to here using virtual stations
//   Double_t dZ = 10.; // Distance between neighboring virtual stations
//
//   lit::parallel::LitMaterialGrid richMaterial;
//   GetRichMaterial(&richMaterial);
//
//   // Virtual stations
//   Int_t nofVirtualStations = 31;
//   for (Int_t iStation = 0; iStation < nofVirtualStations; iStation++) {
//      Double_t z = startZPosition + iStation * dZ;
//      lit::parallel::LitFieldGrid fieldGrid;
//      gridCreator.CreateGrid(z, fieldGrid);
//
//      lit::parallel::LitVirtualStation<T> station;
//      station.SetZ(z);
//      station.SetField(fieldGrid);
//      if (iStation == 10) station.SetMaterial(richMaterial);
//
//      layout.AddVirtualStation(station);
//   }
//
//   // Stations
//   Int_t nofStations = GetNofTrdStations();
//   for (Int_t iStation = 0; iStation < nofStations; iStation++) {
//      // Convert material for this station
//      TProfile2D* profile = hm.P2("hrl_ThicknessSilicon_Trd_" + lit::ToString<Int_t>(iStation) + "_P2");
//      //profile->Rebin2D(200, 200);
//      lit::parallel::LitMaterialGrid material;
//      ConvertTProfile2DToLitMaterialGrid(profile, &material);
//
//      lit::parallel::LitVirtualStation<T> vs;
//      vs.SetMaterial(material);
//
//      lit::parallel::LitStation<T> station;
//      station.AddVirtualStation(vs);
//
//      layout.AddStation(station);
//   }
//
//   gFile = oldFile;
//   gDirectory = oldDirectory;
//   file->Close();
//   delete file;
//
//   cout << layout;
//   cout << "Finish getting TRD layout for parallel version of tracking\n";
//}

//void BmnTrackingGeometryConstructor::GetRichMaterial(
//      lit::parallel::LitMaterialGrid* material)
//{
//   if (!fDet.GetDet(kRICH)) return;
//   // Read file with TProfile2D containing silicon equivalent of the material
//   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
//   TString matBudgetFile = parDir + "/littrack/rich_v08a.silicon.root";
//   TFile* oldFile = gFile;
//   TDirectory* oldDirectory = gDirectory;
//   TFile* file = new TFile(matBudgetFile, "READ");
//   CbmHistManager hm;
//   hm.ReadFromFile(file);
//
//   TProfile2D* profile = hm.P2("hrl_ThicknessSilicon_Rich_P2");
//  // profile->Rebin2D(200, 200);
//   ConvertTProfile2DToLitMaterialGrid(profile, material, 3.);
//
//   gFile = oldFile;
//   gDirectory = oldDirectory;
//   file->Close();
//   delete file;
//}

//void BmnTrackingGeometryConstructor::ConvertTProfile2DToLitMaterialGrid(
//      const TProfile2D* profile,
//      lit::parallel::LitMaterialGrid* grid,
//      Double_t maximumValue)
//{
////   Int_t nofBinsX = profile->GetNbinsX();
////   Int_t nofBinsY = profile->GetNbinsY();
////   vector<vector<fscal> >material(nofBinsX);
////   for (Int_t i = 0; i < nofBinsX; i++) material[i].resize(nofBinsY);
////   for (Int_t iX = 1; iX <= nofBinsX; iX++) {
////      for (Int_t iY = 1; iY <= nofBinsY; iY++) {
////         Double_t content = profile->GetBinContent(iX, iY);
////         if (maximumValue > 0 && content > maximumValue) content = maximumValue;
////         material[iX - 1][iY - 1] = content;
////      }
////   }
////   Double_t xmin = profile->GetXaxis()->GetXmin();
////   Double_t xmax = profile->GetXaxis()->GetXmax();
////   Double_t ymin = profile->GetYaxis()->GetXmin();
////   Double_t ymax = profile->GetYaxis()->GetXmax();
////   grid->SetMaterial(material, xmin, xmax, ymin, ymax, nofBinsX, nofBinsY);
//
//   Int_t nofBinsX = profile->GetNbinsX();
//   Int_t nofBinsY = profile->GetNbinsY();
//   Int_t minShrinkBinX = std::numeric_limits<Double_t>::max();
//   Int_t maxShrinkBinX = std::numeric_limits<Double_t>::min();
//   Int_t minShrinkBinY = std::numeric_limits<Double_t>::max();
//   Int_t maxShrinkBinY = std::numeric_limits<Double_t>::min();
//   Bool_t isSet = false;
//   for (Int_t iBinX = 1; iBinX <= nofBinsX; iBinX++) {
//      for (Int_t iBinY = 1; iBinY <= nofBinsY; iBinY++) {
//         Double_t content = profile->GetBinContent(iBinX, iBinY);
//         if (content != 0.) {
//            minShrinkBinX = std::min(iBinX, minShrinkBinX);
//            maxShrinkBinX = std::max(iBinX, maxShrinkBinX);
//            minShrinkBinY = std::min(iBinY, minShrinkBinY);
//            maxShrinkBinY = std::max(iBinY, maxShrinkBinY);
//            isSet = true;
//         }
//      }
//   }
//
//   Int_t nofShrinkBinsX = maxShrinkBinX - minShrinkBinX + 1;
//   Int_t nofShrinkBinsY = maxShrinkBinY - minShrinkBinY + 1;
//   vector<vector<fscal> >material(nofShrinkBinsX);
//   for (Int_t i = 0; i < nofShrinkBinsX; i++) material[i].resize(nofShrinkBinsY);
//
//   for (Int_t iX = minShrinkBinX; iX <= maxShrinkBinX; iX++) {
//      for (Int_t iY = minShrinkBinY; iY <= maxShrinkBinY; iY++) {
//         Double_t content = profile->GetBinContent(iX, iY);
//         if (maximumValue > 0 && content > maximumValue) content = maximumValue;
//         material[iX - minShrinkBinX][iY - minShrinkBinY] = content;
//      }
//   }
//   Double_t xmin = profile->GetXaxis()->GetBinLowEdge(minShrinkBinX);
//   Double_t xmax = profile->GetXaxis()->GetBinUpEdge(maxShrinkBinX);
//   Double_t ymin = profile->GetYaxis()->GetBinLowEdge(minShrinkBinY);
//   Double_t ymax = profile->GetYaxis()->GetBinUpEdge(maxShrinkBinY);
//   grid->SetMaterial(material, xmin, xmax, ymin, ymax, nofShrinkBinsX, nofShrinkBinsY);
//}

Int_t BmnTrackingGeometryConstructor::GetNofTrdStations()
{
   static Bool_t firstTime = true;
   if (firstTime) {
      Int_t layerCounter = 0;
      TObjArray* topNodes = fGeo->GetTopNode()->GetNodes();
      for (Int_t iTopNode = 0; iTopNode < topNodes->GetEntriesFast(); iTopNode++) {
         TGeoNode* topNode = static_cast<TGeoNode*>(topNodes->At(iTopNode));
         if (TString(topNode->GetName()).Contains("trd")) {
            TObjArray* layers = topNode->GetNodes();
            for (Int_t iLayer = 0; iLayer < layers->GetEntriesFast(); iLayer++) {
               TGeoNode* layer = static_cast<TGeoNode*>(layers->At(iLayer));
               if (TString(layer->GetName()).Contains("layer")) {
                  layerCounter++;
               }
            }
         }
      }
      fNofTrdStations = layerCounter;
      firstTime = false;
   }
   return fNofTrdStations;
}

Int_t BmnTrackingGeometryConstructor::GetNofMuchStations()
{
   static Bool_t firstTime = true;
   if (firstTime) {
      fNofMuchStations = 0;
      const TGeoNode* much = static_cast<const TGeoNode*>(fGeo->GetTopNode()->GetNodes()->FindObject("much_0"));
      if (NULL == much) { // No MUCH detector return 0 stations
         firstTime = false;
         return fNofMuchStations;
      }
      TObjArray* muchNodes = much->GetNodes();
      for (Int_t iMuchNode = 0; iMuchNode < muchNodes->GetEntriesFast(); iMuchNode++) {
         const TGeoNode* muchNode = static_cast<const TGeoNode*>(muchNodes->At(iMuchNode));
         if (TString(muchNode->GetName()).Contains("station")) {
            TObjArray* layerNodes = muchNode->GetNodes();
            fNofMuchStations += layerNodes->GetEntriesFast();
         }
      }
      firstTime = false;
   }
   return fNofMuchStations;
}

Int_t BmnTrackingGeometryConstructor::GetNofMuchAbsorbers()
{
   static Bool_t firstTime = true;
   if (firstTime) {
      fNofMuchAbsorbers = 0;
      const TGeoNode* much = static_cast<const TGeoNode*>(fGeo->GetTopNode()->GetNodes()->FindObject("much_0"));
      if (NULL == much) { // No MUCH detector return 0 stations
         firstTime = false;
         return fNofMuchAbsorbers;
      }
      TObjArray* muchNodes = much->GetNodes();
      for (Int_t iMuchNode = 0; iMuchNode < muchNodes->GetEntriesFast(); iMuchNode++) {
         const TGeoNode* muchNode = static_cast<const TGeoNode*>(muchNodes->At(iMuchNode));
         if (TString(muchNode->GetName()).Contains("absorber")) fNofMuchAbsorbers++;
      }
      firstTime = false;
   }
   return fNofMuchAbsorbers;
}

Int_t BmnTrackingGeometryConstructor::GetNofMuchTrdStations()
{
   return GetNofMuchStations() + GetNofTrdStations();
}

Int_t BmnTrackingGeometryConstructor::GetNofMvdStations()
{
   static Bool_t firstTime = true;
   if (firstTime) {
      fNofMvdStations = 0;
      TGeoNode* topNode = gGeoManager->GetTopVolume()->FindNode("pipevac1_0");
      if (topNode) {
         TObjArray* nodes = topNode->GetNodes();
         for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
            TGeoNode* node = (TGeoNode*) nodes->At(iNode);
            if (TString(node->GetName()).Contains("mvdstation")) {
               fNofMvdStations++;
            }
         }
      } else {
         TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
         for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
            TGeoNode* node = (TGeoNode*) nodes->At(iNode);
            TString nodeName = node->GetName();
            nodeName.ToLower();
            if (nodeName.Contains("pipe")) {
               TObjArray* nodes2 = node->GetNodes();
               for (Int_t iiNode = 0; iiNode < nodes2->GetEntriesFast(); iiNode++) {
                  TGeoNode* node2 = (TGeoNode*) nodes2->At(iiNode);
                  TString nodeName2 = node2->GetName();
                  nodeName2.ToLower();
                  if (nodeName2.Contains("pipevac1")) {
                     // check if there is a mvd in the pipevac
                     TObjArray* nodes3 = node2->GetNodes();
                     for (Int_t iiiNode = 0; iiiNode < nodes3->GetEntriesFast(); iiiNode++) {
                        TGeoNode* node3 = (TGeoNode*) nodes3->At(iiiNode);
                        TString nodeName3 = node3->GetName();
                        nodeName3.ToLower();
                        if (nodeName3.Contains("mvd")) {
                           fNofMvdStations = node3->GetNodes()->GetEntriesFast();
                           break;
                        }
                     }
                     break;
                  }
               }
               break;
            }
         }
      }
      firstTime = false;
   }
   return fNofMvdStations;
}

Int_t BmnTrackingGeometryConstructor::GetNofStsStations()
{
   static Bool_t firstTime = true;
   if (firstTime) {
      fNofStsStations = 0;
      TObjArray* nodes = gGeoManager->GetTopVolume()->GetNodes();
      for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
         TGeoNode* node = (TGeoNode*) nodes->At(iNode);
         if (TString(node->GetName()).Contains("STS_")) { // Top STS node
            fNofStsStations = node->GetNodes()->GetEntriesFast();
            break;
         }
      }
      firstTime = false;
   }
   return fNofStsStations;
}

Int_t BmnTrackingGeometryConstructor::ConvertMuchToAbsoluteStationNr(
      Int_t station,
      Int_t layer)
{
   static Bool_t firstTime = true;
   static vector<Int_t> sumOfLayers;
   if (firstTime) {
      fNofMuchStations = 0;
      const TGeoNode* much = static_cast<const TGeoNode*>(fGeo->GetTopNode()->GetNodes()->FindObject("much_0"));
      if (NULL == much) { // No MUCH detector return 0
         firstTime = false;
         return 0;
      }
      map<Int_t, Int_t> nofLayersPerStation;
      TObjArray* muchNodes = much->GetNodes();
      for (Int_t iMuchNode = 0; iMuchNode < muchNodes->GetEntriesFast(); iMuchNode++) {
         const TGeoNode* muchNode = static_cast<const TGeoNode*>(muchNodes->At(iMuchNode));
         if (TString(muchNode->GetName()).Contains("station")) {
            Int_t station = std::atoi(string(muchNode->GetName() + 11, 2).c_str()) - 1;
            TObjArray* layerNodes = muchNode->GetNodes();
            nofLayersPerStation[station] = layerNodes->GetEntriesFast();
         }
      }
      map<Int_t, Int_t>::const_iterator it;
      Int_t sum = 0;
      sumOfLayers.push_back(0);
      for (it = nofLayersPerStation.begin(); it != nofLayersPerStation.end(); it++) {
         sum += (*it).second;
         sumOfLayers.push_back(sum);
      }
      firstTime = false;
   }
   return sumOfLayers[station] + layer;
}
