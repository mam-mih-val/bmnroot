// MpdEventManager: class for event management and navigation.

#include "MpdEventManager.h"
#include "MpdEventManagerEditor.h"
#include "constants.h"

#include "TDOMParser.h"
#include "TXMLEngine.h"
#include "TXMLAttr.h"
#include "TXMLNode.h"

#include "TDatabasePDG.h"
#include "TEveGeoNode.h"
#include "TEveManager.h"
#include "TGeoManager.h"
#include <TGLViewer.h>
#include <TGLCameraOverlay.h>
#include <TGLLightSet.h>
#include <TEveBrowser.h>

// XML
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemastypes.h>

#include <unistd.h>
#include <cerrno>
#include <iostream>
#include <sstream>

MpdEventManager* MpdEventManager::fgRinstance = 0;
MpdEventManager* MpdEventManager::Instance() { return fgRinstance; }

// convert string with hexadecimal presentation without "0x" to integer
int hex_string_to_int(string hex_string)
{
    int x;
    stringstream stream;
    stream<<hex<<hex_string;
    stream>>x;
    return x;
}

//______________________________________________________________________________
MpdEventManager::MpdEventManager()
  : TEveEventManager("EventManager", ""),
   fEventEditor(NULL),
   iCurrentEvent(0),
   fRunAna(FairRunAna::Instance()),

   fEvent(0),
   fPriOnly(kFALSE),
   fCurrentPDG(0),
   fMinEnergy(0),
   fMaxEnergy(25),
   fEvtMinEnergy(0),
   fEvtMaxEnergy(12),

   fRPhiPlane{0, 0, 1, 0},
   fRhoZPlane{-1, 0, 0, 0},
   fRPhiView(0),
   fRhoZView(0),
   fMulti3DView(0),
   fMultiRPhiView(0),
   fMultiRhoZView(0),
   fRPhiGeomScene(0),
   fRhoZGeomScene(0),
   fRPhiMng(NULL),
   fRhoZMng(NULL),
   fAxesPhi(NULL),
   fAxesRho(NULL),
   fXMLConfig(""),

   EveMCPoints(NULL),
   EveMCTracks(NULL),
   EveRecoPoints(NULL),
   EveRecoTracks(NULL),

   background_color(1),
   isDarkColor(true),

   isZDCModule(NULL),
   fgShowRecoPointsIsShow(false),
   fgRedrawRecoPointsReqired(false),
   fLastUsedColor(2001)
{
    fgRinstance = this;

    AddParticlesToPdgDataBase();

    // set colors for particles
    fPDGToColor[22] = 623;	// photon
    fPDGToColor[-2112] = 2;	// anti-neutron
    fPDGToColor[-11] = 3;	// e+
    fPDGToColor[-3122] = 4;	// anti-lambda
    fPDGToColor[11] = 5;	// e-
    fPDGToColor[-3222] = 6;	// Sigma -
    fPDGToColor[12] = 7;	// e-neutrino
    fPDGToColor[-3212] = 8;	//  Sigma0
    fPDGToColor[-13] = 9;	// mu+
    fPDGToColor[-3112] = 10;	// Sigma+ (PB
    fPDGToColor[13] = 11;	//  mu-
    fPDGToColor[-3322] = 12;	//  Xi0
    fPDGToColor[111] = 13;	// pi0
    fPDGToColor[-3312] = 14;	//  Xi+
    fPDGToColor[211] = 15;	// pi+
    fPDGToColor[-3334] = 16;	//  Omega+ (PB)
    fPDGToColor[-211] = 17;	// pi-
    fPDGToColor[-15] = 18;	// tau+
    fPDGToColor[130] = 19;	// K long
    fPDGToColor[15] = 20;	//  tau -
    fPDGToColor[321] = 21;	// K+
    fPDGToColor[411] = 22;	// D+
    fPDGToColor[-321] = 23;	// K-
    fPDGToColor[-411] = 24;	// D-
    fPDGToColor[2112] = 25;	// n
    fPDGToColor[421] = 26;	// D0
    fPDGToColor[2212] = 27;	// p
    fPDGToColor[-421] = 28;	// D0
    fPDGToColor[-2212] = 29;	//  anti-proton
    fPDGToColor[431] = 30;	// Ds+
    fPDGToColor[310] = 31;	// K short
    fPDGToColor[-431] = 32;	// anti Ds-
    fPDGToColor[221] = 33;	// eta
    fPDGToColor[4122] = 34;	// Lambda_C+
    fPDGToColor[3122] = 35;	//  Lambda
    fPDGToColor[24] = 36;	// W+
    fPDGToColor[3222] = 37;	// Sigma+
    fPDGToColor[-24] = 38;		//	W-
    fPDGToColor[3212] = 39;	//Sigma0
    fPDGToColor[23] = 40;	//	Z
    fPDGToColor[3112] = 41;	// Sigma -
    fPDGToColor[3322] = 42;	// Xi0
    fPDGToColor[3312] = 43; 	// Xi-
    fPDGToColor[3334] = 44; 	// Omega- (PB)
    fPDGToColor[50000050] = 801; //Cerenkov
    fPDGToColor[1000010020] = 45;
    fPDGToColor[1000010030] = 48;
    fPDGToColor[1000020040] = 50;
    fPDGToColor[1000020030] = 55;

    InitColorStructure();
}

// return integer value of color by string storing color number, RGB triple or color name (e.g. kBlue, RGB(51,63,73), RGB(#0039E1))
// support following MAIN COLOR NAMESET + some additional colors as examples:
// kWhite, kBlack, kGgray,
// kBlue, kAzure (темно-синий), kCyan (морской волны), kTeal (бирюзовый),
// kGreen, kSpring (светло-зеленый), kGreen+2 (темно-зеленый), kSpring+2 (темно-зеленый), kKhaki
// kYellow, kOrange (желтый с оттенком), kOrange+2 (оранжевый кор.), kOrange+1 (светло-оранжевый кор.), kOrange+7 (выделенно-оранжевый)
// kRed, kViolet, kMagenta (бардовый), kMagenta-6 (светло-бардовый), kPink (темно-розовый)
Int_t MpdEventManager::StringToColor(TString color)
{
    color = color.ReplaceAll(" ", "");
    color.ToLower();

    // check if color is a RGB triple
    if (color.BeginsWith("rgb"))
    {
        // parse RGB triple
        if (color.Length() < 6)
        {
            cout<<color<<" - RGB triple isn't correct. Color set to default blue"<<endl;
            return 600;
        }
        TString triple = color(3, color.Length() - 3);
        triple.Remove(TString::kLeading, '('); triple.Remove(TString::kTrailing, ')');

        int red_rgb = -1, green_rgb = -1, blue_rgb = -1;
        if (triple[0] == '#')
        {
            if (triple.Length() < 7)
            {
                cout<<triple<<" - hex triple size after '#' isn't correct (should have 6 symbols). Color set to default blue"<<endl;
                return 600;
            }

            TString str_red = triple(1,2);
            TString str_green = triple(3,2);
            TString str_blue = triple(5,2);
            if ((!str_red.IsHex()) || (!str_green.IsHex()) || (!str_blue.IsHex()))
            {
                cout<<triple<<" - hex triple after '#' has not hex format. Color set to default blue"<<endl;
                return 600;
            }

            red_rgb = hex_string_to_int(str_red.Data());
            green_rgb = hex_string_to_int(str_green.Data());
            blue_rgb = hex_string_to_int(str_blue.Data());
        }
        else
        {
            TObjArray* pRGB = triple.Tokenize(",");
            if (pRGB->GetEntriesFast() < 3)
            {
                cout<<triple<<" - RGB string doesn't include color triple. Color set to default blue"<<endl;
                return 600;
            }
            red_rgb = ((TObjString*)pRGB->At(0))->GetString().Atoi();
            green_rgb = ((TObjString*)pRGB->At(1))->GetString().Atoi();
            blue_rgb = ((TObjString*)pRGB->At(2))->GetString().Atoi();
            delete pRGB;
        }

        Int_t ci = fLastUsedColor++;
        new TColor(ci, red_rgb/255.0F, green_rgb/255.0F, blue_rgb/255.0F);
        return ci;
    }

    if (color.Contains("k"))
    {
        Ssiz_t sign_index = color.First('+');
        if (sign_index == kNPOS)
            sign_index = color.First('-');

        TString color_name = "";
        if (sign_index == kNPOS)
            color_name = color;
        else
            color_name = color(0, sign_index);

        Int_t color_value;
        if (color_name.EqualTo("kwhite")) color_value = 0;
        else if (color_name.EqualTo("kblack")) color_value = 1;
        else if (color_name.EqualTo("kgray")) color_value = 920;
        else if (color_name.EqualTo("kred")) color_value = 632;
        else if (color_name.EqualTo("kgreen")) color_value = 416;
        else if (color_name.EqualTo("kblue")) color_value = 600;
        else if (color_name.EqualTo("kyellow")) color_value = 400;
        else if (color_name.EqualTo("kmagenta")) color_value = 616;
        else if (color_name.EqualTo("kcyan")) color_value = 432;
        else if (color_name.EqualTo("korange")) color_value = 800;
        else if (color_name.EqualTo("kspring")) color_value = 820;
        else if (color_name.EqualTo("kteal")) color_value = 840;
        else if (color_name.EqualTo("kazure")) color_value = 860;
        else if (color_name.EqualTo("kviolet")) color_value = 880;
        else if (color_name.EqualTo("kpink")) color_value = 900;
        else if (color_name.EqualTo("kkhaki")) color_value = 403;
        else
        {
            cout<<color_name<<" not found. Color set to default blue"<<endl;
            color_value = 600;
        }

        if ((sign_index != kNPOS) && (color.Length() > sign_index+2))
        {
            TString strNumber = color(sign_index+1, color.Length() - sign_index-1);
            if (strNumber.IsDigit())
            {
                if (color(sign_index) == '+')
                    color_value += strNumber.Atoi();
                else
                    color_value -= strNumber.Atoi();
            }
        }

        return color_value;
    }
    else
        return color.Atoi();
}

void MpdEventManager::InitColorStructure()
{
    // check and load colors from my XML file
    TString coloring_xml_path = "$VMCWORKDIR/config/eventdisplay.xml";
    TString coloring_xsd_path = "$VMCWORKDIR/eventdisplay/coloring.xsd";
    gSystem->ExpandPathName(coloring_xml_path);
    gSystem->ExpandPathName(coloring_xsd_path);

    // check XML scheme
    if (ValidateXml(coloring_xml_path.Data(), coloring_xsd_path.Data()) == true)
    {   
        xmlDoc* doc = xmlReadFile(coloring_xml_path.Data(), NULL, 0);
        
        /* Get the root element node */
        xmlNode* root_element = NULL;
        root_element = xmlDocGetRootElement(doc);
        xmlAttr* root_element_attributes = root_element->properties;
        xmlChar* value = xmlNodeListGetString(root_element->doc, root_element_attributes->children, 1);

        xmlNodePtr cur_node = root_element;
        if (strcmp((char*)value, "default") == 0)
        {
            cout<<"using default coloring"<<endl;
            gVisualizationColoring = defaultColoring;
        }
        else
        {
            structSelectedColoring* selected_coloring;
            structLevelColoring* level_coloring;
            if (strcmp((char*)value, "detector") == 0)
                gVisualizationColoring = selectedColoring;
            else
                gVisualizationColoring = levelColoring;

            cur_node = root_element->children;
            while (cur_node)
            {
                if ((strcmp((char*)cur_node->name, "text") != 0) //skipping elements with no attributes
                   && (cur_node->type != XML_COMMENT_NODE))
                {
                    // set colors for particle tracks
                    if (strcmp((char*)cur_node->name, "particle") == 0)
                    {
                        xmlAttr* attribute = cur_node->properties;
                        TString strPDG = "", strColor = "";
                        while (attribute)
                        {
                            xmlChar* attr_value = xmlNodeListGetString(root_element->doc, attribute->children, 1);
                            if (strcmp((char*)attribute->name,"pdg") == 0)
                                strPDG = (char*) attr_value;
                            if (strcmp((char*)attribute->name,"color") == 0)
                                strColor = (char*) attr_value;

                            attribute = attribute->next;
                            xmlFree(attr_value);
                        }// while (attribute)
                        xmlFree(attribute);

                        if ((strPDG != "") && (strColor != ""))
                            fPDGToColor[strPDG.Atoi()] = StringToColor(strColor);
                        cur_node = cur_node->next;
                        continue;
                    }

                    if (gVisualizationColoring == selectedColoring)
                        selected_coloring = new structSelectedColoring();
                    else
                        level_coloring = new structLevelColoring();

                    xmlAttr* attribute = cur_node->properties;
                    while (attribute)
                    {
                        xmlChar* attr_value = xmlNodeListGetString(root_element->doc, attribute->children, 1);
                        if (gVisualizationColoring == selectedColoring)
                        {
                            if (strcmp((char*)attribute->name,"name") == 0)
                                selected_coloring->detector_name = (char*) attr_value;
                            if (strcmp((char*)attribute->name,"color") == 0)
                                selected_coloring->detector_color = (char*) attr_value;
                            if (strcmp((char*)attribute->name,"isRecursiveColoring") == 0)
                                selected_coloring->isRecursiveColoring = (strcmp((char*)attr_value,"true") == 0);
                            if (strcmp((char*)attribute->name,"transparency") == 0)
                                selected_coloring->detector_transparency =  atoi((char*)attr_value);
                        }
                        else
                        {
                            if (strcmp((char*)attribute->name,"color") == 0)
                                level_coloring->fill_color = (char*) attr_value;
                            if (strcmp((char*)attribute->name,"isFillLine") == 0)
                                level_coloring->isFillLine = (strcmp((char*)attr_value,"true") == 0);
                            if (strcmp((char*)attribute->name,"visibility") == 0)
                                level_coloring->visibility = (strcmp((char*)attr_value,"true") == 0);
                            if (strcmp((char*)attribute->name,"transparency") == 0)
                                level_coloring->transparency = atoi((char*)attr_value);
                        }                    
                        attribute = attribute->next;
                        xmlFree(attr_value);
                    }// while (attribute)
                    xmlFree(attribute);

                    // add color parameters to array
                    if (gVisualizationColoring == selectedColoring)
                        vecSelectedColoring.push_back(selected_coloring);
                    else
                        vecLevelColoring.push_back(level_coloring);
                }// if ((strcmp((char*)cur_node->name, "text") != 0) && (cur_node->type != XML_COMMENT_NODE))
                cur_node = cur_node->next;
            }// while (cur_node)
        }
            
        xmlFree(cur_node);
        xmlFree(value);
        xmlFreeDoc(doc);
    }
    else
    {
        cout<<"Using default ROOT coloring"<<endl;
        gVisualizationColoring = defaultColoring;
    }
    
    return;
}

//______________________________________________________________________________
void MpdEventManager::Init(Int_t visopt, Int_t vislvl, Int_t maxvisnds)
{
    TEveManager::Create();
    fRunAna->Init();

    if (gGeoManager == NULL)
        return;
    TGeoNode* N = gGeoManager->GetTopNode();
    TEveGeoTopNode* TNod = new TEveGeoTopNode(gGeoManager, N, visopt, vislvl, maxvisnds);

    // change color and visibility of geometry nodes
    if (!fXMLConfig.EqualTo(""))
        LoadXMLSettings();
    else
    {
        if (gVisualizationColoring != defaultColoring)
        {
            if (gVisualizationColoring == selectedColoring)
                SelectedGeometryColoring();
            else
                LevelChangeNodeProperty(N, 0);
        }
    }

    gEve->AddGlobalElement(TNod);
    gEve->FullRedraw3D(kTRUE);
    fEvent = gEve->AddEvent(this);

    // create projection managers
    fRPhiMng = new TEveProjectionManager(TEveProjection::kPT_RPhi);
    fRhoZMng = new TEveProjectionManager(TEveProjection::kPT_RhoZ);
    gEve->AddToListTree(fRPhiMng, kFALSE);
    gEve->AddToListTree(fRhoZMng, kFALSE);

    // create axes for viewers
    fAxesPhi = new TEveProjectionAxes(fRPhiMng);
    fAxesPhi->SetMainColor(kRed);
    fAxesRho = new TEveProjectionAxes(fRhoZMng);
    fAxesRho->SetMainColor(kRed);

    // first 3D viewer
    gEve->GetDefaultViewer()->SetElementName("3D View");
    // display axes
    //gEve->GetDefaultViewer()->GetGLViewer()->SetGuideState(TGLUtil::kAxesEdge, kTRUE, kFALSE, 0);
    // switch off left and right light sources for first window
    gEve->GetDefaultViewer()->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightLeft, false);
    gEve->GetDefaultViewer()->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightRight, false);
    if (!isDarkColor)
        gEve->GetDefaultViewer()->GetGLViewer()->UseLightColorSet();
    gEve->GetDefaultViewer()->GetGLViewer()->SetClearColor(background_color);

    // use only one View in Online mode
    if (isOnline)
        return;

    // ADD WINDOW in EventDisplay for RPhi projection
    TEveWindowSlot* RPhiSlot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
    TEveWindowPack* RPhiPack = RPhiSlot->MakePack();
    RPhiPack->SetElementName("RPhi View");
    RPhiPack->SetShowTitleBar(kFALSE);
    RPhiPack->NewSlot()->MakeCurrent();
    fRPhiView = gEve->SpawnNewViewer("RPhi View", "");
    // create scene holding projected geometry for the RPhi view
    fRPhiGeomScene  = gEve->SpawnNewScene("RPhi", "Scene holding geometry for RPhi.");
    // add axes for scene of RPhi view
    fRPhiGeomScene->AddElement(fAxesPhi);

    // ADD WINDOW in EvenDisplay for RhoZ projection
    TEveWindowSlot* RhoZSlot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
    TEveWindowPack* RhoZPack = RhoZSlot->MakePack();
    RhoZPack->SetElementName("RhoZ View");
    RhoZPack->SetShowTitleBar(kFALSE);
    RhoZPack->NewSlot()->MakeCurrent();
    fRhoZView = gEve->SpawnNewViewer("RhoZ View", "");
    // create scene holding projected geometry for the RhoZ view.
    fRhoZGeomScene  = gEve->SpawnNewScene("RhoZ", "Scene holding geometry for RhoZ.");
    // add axes for scene of RPhoZ view
    fRhoZGeomScene->AddElement(fAxesRho);

    SetViewers(fRPhiView, fRhoZView);

    // ADD WINDOW in EvenDisplay for MultiView
    TEveWindowSlot* MultiSlot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
    TEveWindowPack* MultiPack = MultiSlot->MakePack();
    MultiPack->SetElementName("Multi View");
    MultiPack->SetHorizontal();
    MultiPack->SetShowTitleBar(kFALSE);
    MultiPack->NewSlot()->MakeCurrent();
    fMulti3DView = gEve->SpawnNewViewer("3D View (multi)", "");
    // switch off left and right light sources for 3D MultiView
    fMulti3DView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightLeft, false);
    fMulti3DView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightRight, false);
    if (!isDarkColor)
        fMulti3DView->GetGLViewer()->UseLightColorSet();
    fMulti3DView->GetGLViewer()->SetClearColor(background_color);
    // add 3D scenes (first tab) to 3D MultiView
    fMulti3DView->AddScene(gEve->GetGlobalScene());
    fMulti3DView->AddScene(gEve->GetEventScene());

    // add slots for RPhi and RhoZ projections on Multi View tab
    MultiPack = MultiPack->NewSlot()->MakePack();
    MultiPack->SetShowTitleBar(kFALSE);
    MultiPack->NewSlot()->MakeCurrent();
    fMultiRPhiView = gEve->SpawnNewViewer("RPhi View (multi)", "");
    MultiPack->NewSlot()->MakeCurrent();
    fMultiRhoZView = gEve->SpawnNewViewer("RhoZ View (multi)", "");

    SetViewers(fMultiRPhiView, fMultiRhoZView);

    // don't change reposition camera on each update
    fRPhiView->GetGLViewer()->SetResetCamerasOnUpdate(kFALSE);
    fRhoZView->GetGLViewer()->SetResetCamerasOnUpdate(kFALSE);
    fMulti3DView->GetGLViewer()->SetResetCamerasOnUpdate(kFALSE);
    fMultiRPhiView->GetGLViewer()->SetResetCamerasOnUpdate(kFALSE);
    fMultiRhoZView->GetGLViewer()->SetResetCamerasOnUpdate(kFALSE);

    // from FairRoot (not checked yet)
    fMulti3DView->GetEveFrame()->HideAllDecorations();
    fMultiRPhiView->GetEveFrame()->HideAllDecorations();
    fMultiRhoZView->GetEveFrame()->HideAllDecorations();
}//MpdEventManager::Init

void MpdEventManager::SetViewers(TEveViewer* RPhi, TEveViewer* RhoZ)
{
    RPhi->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
    // set camera parameters
    RPhi->GetGLViewer()->GetCameraOverlay()->SetOrthographicMode(TGLCameraOverlay::kAxis);
    RPhi->GetGLViewer()->GetCameraOverlay()->SetShowOrthographic(kTRUE);
    // switch off left, right, top and bottom light sources
    RPhi->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightLeft, false);
    RPhi->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightRight, false);
    RPhi->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightTop, false);
    RPhi->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightBottom, false);
    if (!isDarkColor)
        RPhi->GetGLViewer()->UseLightColorSet();
    RPhi->GetGLViewer()->SetClearColor(background_color);

    RhoZ->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoZOY);
    // set camera parameters
    RhoZ->GetGLViewer()->GetCameraOverlay()->SetOrthographicMode(TGLCameraOverlay::kAxis);
    RhoZ->GetGLViewer()->GetCameraOverlay()->SetShowOrthographic(kTRUE);
    // switch off left, right and front light sources
    RhoZ->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightLeft, false);
    RhoZ->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightRight, false);
    RhoZ->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightFront, false);
    if (!isDarkColor)
        RhoZ->GetGLViewer()->UseLightColorSet();
    RhoZ->GetGLViewer()->SetClearColor(background_color);

    RPhi->AddScene(fRPhiGeomScene);
    RPhi->AddScene(gEve->GetGlobalScene());
    RPhi->AddScene(gEve->GetEventScene());
    RhoZ->AddScene(fRhoZGeomScene);
    RhoZ->AddScene(gEve->GetGlobalScene());
    RhoZ->AddScene(gEve->GetEventScene());

    // set clip planes
    RPhi->GetGLViewer()->GetClipSet()->SetClipType(TGLClip::kClipPlane);
    RPhi->GetGLViewer()->GetClipSet()->SetClipState(TGLClip::kClipPlane, fRPhiPlane);
    RhoZ->GetGLViewer()->GetClipSet()->SetClipType(TGLClip::kClipPlane);
    RhoZ->GetGLViewer()->GetClipSet()->SetClipState(TGLClip::kClipPlane, fRhoZPlane);
}

// setting of geometry colors for DETECTOR COLORING MODE
void MpdEventManager::SelectedGeometryColoring()
{
    TGeoVolume* curVolume;
    for (int i = 0; i < vecSelectedColoring.size(); i++)
    {
        structSelectedColoring* selected_coloring = vecSelectedColoring[i];
        curVolume = gGeoManager->GetVolume(selected_coloring->detector_name);
        if (!curVolume)
        {
            cout<<"There is no volume with given name: "<<selected_coloring->detector_name<<endl;
            // delete wrong detector name from the array
            vecSelectedColoring.erase(vecSelectedColoring.begin() + i);
            i--;
            continue;
        }
        Int_t curColor = StringToColor(selected_coloring->detector_color);
        Int_t curTransparency = selected_coloring->detector_transparency;

        curVolume->SetFillColor(curColor);
        curVolume->SetLineColor(curColor);
        curVolume->SetTransparency(curTransparency);

        if (selected_coloring->isRecursiveColoring)
        {
            for (int j = 0; j < curVolume->GetNdaughters(); j++)
            {
                TGeoNode* child = curVolume->GetNode(j);
                TGeoVolume* subVolume = child->GetVolume();

                subVolume->SetFillColor(curColor);
                subVolume->SetLineColor(curColor);
                subVolume->SetTransparency(curTransparency);

                if (child->GetNdaughters() != 0)
                    RecursiveChangeNodeProperty(child, curColor, curTransparency);
            }// for (int j = 0; j < curVolume->GetNdaughters(); j++)
        }// if (selected_coloring->isRecursiveColoring)
    }// for (int i = 0; i < vecSelectedColoring.size(); i++)

    return;
}

void MpdEventManager::RecursiveChangeNodeProperty(TGeoNode* node, Int_t color, int transparency)
{
    for (int i = 0; i < node->GetNdaughters(); i++)
    {
        TGeoNode* child = node->GetDaughter(i);
        TGeoVolume* curVolume = child->GetVolume();

        curVolume->SetFillColor(color);
        curVolume->SetLineColor(color);
        curVolume->SetTransparency(transparency);

        if (child->GetNdaughters() != 0) RecursiveChangeNodeProperty(child, color, transparency);
    }
}

// set transparent geometry
void MpdEventManager::SetTransparentGeometry(bool is_on)
{
    switch (gVisualizationColoring)
    {
    case selectedColoring:
    {
        TGeoVolume* curVolume;
        for (int i = 0; i < vecSelectedColoring.size(); i++)
        {
            structSelectedColoring* selected_coloring = vecSelectedColoring[i];
            curVolume = gGeoManager->GetVolume(selected_coloring->detector_name);
            if (!curVolume)
            {
                cout<<"There is no volume with given name: "<< selected_coloring->detector_name<<endl;
                // delete wrong detector name from the array
                vecSelectedColoring.erase(vecSelectedColoring.begin() + i);
                i--;
                continue;
            }

            Int_t curTransparency = 80;
            if (!is_on)
                curTransparency = selected_coloring->detector_transparency;

            curVolume->SetTransparency(curTransparency);

            for (int j = 0; j < curVolume->GetNdaughters(); j++)
            {
                TGeoNode* child = curVolume->GetNode(j);
                TGeoVolume* subVolume = child->GetVolume();

                subVolume->SetTransparency(curTransparency);

                if (child->GetNdaughters() != 0)
                    RecursiveChangeNodeTransparent(child, curTransparency);
            }// or (int j = 0; j < curVolume->GetNdaughters(); j++)
        }// for (int i = 0; i < vecSelectedColoring.size(); i++)

        break;
    }// case selectedColoring:
    case levelColoring:
    {
        // NOT IMPLEMENTED
        break;
    }
    case defaultColoring:
    {
        // NOT IMPLEMENTED
        break;
    }
    }// switch (gVisualizationColoring)

    return;
}

void MpdEventManager::RecursiveChangeNodeTransparent(TGeoNode* node, int transparency)
{
    for (int i = 0; i < node->GetNdaughters(); i++)
    {
        TGeoNode* child = node->GetDaughter(i);
        TGeoVolume* curVolume = child->GetVolume();

        curVolume->SetTransparency(transparency);

        if (child->GetNdaughters() != 0) RecursiveChangeNodeTransparent(child, transparency);
    }
}

// hierarchical changing of nodes' properties: visibility, transparency, fill color and line color
void MpdEventManager::LevelChangeNodeProperty(TGeoNode* node, int level)
{
    for (int i = 0; i < node->GetNdaughters(); i++)
    {
        TGeoNode* child = node->GetDaughter(i);
        if (level < vecLevelColoring.size())
        {
            TGeoVolume* curVolume = child->GetVolume();

            structLevelColoring* level_coloring = vecLevelColoring[level];
            curVolume->SetVisibility(level_coloring->visibility);
            curVolume->SetTransparency(level_coloring->transparency);
            curVolume->SetFillColor(StringToColor(level_coloring->fill_color));
            if (level_coloring->isFillLine) curVolume->SetLineColor(StringToColor(level_coloring->fill_color));

            if (child->GetNdaughters() != 0)
                LevelChangeNodeProperty(child, ++level);
        }//if (level < arr_size)
    }
}

// validate XML file with geometry colors
// returns true if successful or false if XML validation failed
bool MpdEventManager::ValidateXml(const char* XMLFileName, const char* XSDFileName)
{
    bool ok = false;

    xmlSchemaParserCtxtPtr ctxt = xmlSchemaNewParserCtxt(XSDFileName);
    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);

    xmlSchemaPtr schema = NULL;
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);
    //xmlSchemaDump(stdout, schema);    //to print schema dump

    xmlDoc* doc = NULL;
    doc = xmlReadFile(XMLFileName, NULL, 0);
    if (doc == NULL)
        cout<<"Error: could not parse file"<<XMLFileName<<endl;
    else
    {
        xmlSchemaValidCtxtPtr cvalid = xmlSchemaNewValidCtxt(schema);
        xmlSchemaSetValidErrors(cvalid, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
        int ret = xmlSchemaValidateDoc(cvalid, doc);
        if (ret == 0)
        {
            //cout<<XMLFileName<<" is validated"<<endl;
            ok = true;
        }
        else if (ret > 0)
        {
            cout<<XMLFileName<<" failed to validate"<<endl;
        }
        else
        {
            cout<<XMLFileName<<" validation generated an internal error"<<endl;
        }
        xmlSchemaFreeValidCtxt(cvalid);
    }

    if (schema != NULL)
        xmlSchemaFree(schema);
    xmlSchemaCleanupTypes();

    return ok;
}

void MpdEventManager::Open()
{}

void MpdEventManager::Close()
{}

void MpdEventManager::DisplaySettings()
{}

void MpdEventManager::UpdateEditor()
{}

// MpdEventManager destructor
MpdEventManager::~MpdEventManager()
{
    if (!vecSelectedColoring.empty())
    {
        for (int i = 0; i < vecSelectedColoring.size(); i++)
            delete (vecSelectedColoring[i]);
        vecSelectedColoring.clear();
    }
    if (!vecLevelColoring.empty())
    {
        for (int i = 0; i < vecLevelColoring.size(); i++)
            delete (vecLevelColoring[i]);
        vecLevelColoring.clear();
    }
}

// go to FairRunAna event with given number for scene data getting
void MpdEventManager::GotoEvent(Int_t event)
{
    iCurrentEvent = event;
    fRunAna->Run((Long64_t)event);
}

// go to next FairRunAna event for scene data getting
void MpdEventManager::NextEvent()
{
    fRunAna->Run((Long64_t)++iCurrentEvent);
}

// go to previous FairRunAna event for scene data getting
void MpdEventManager::PrevEvent()
{
    fRunAna->Run((Long64_t)--iCurrentEvent);
}

// assign different colors for differrent particles
// return integer value of color for track by particle pdg (default, white)
Int_t MpdEventManager::Color(int pdg)
{
    if (fPDGToColor.find(pdg) != fPDGToColor.end())
        return fPDGToColor[pdg];

    return 0;
}

// add particles to the PDG data base: Deuteron, Triton, Alpha, HE3; Cherenkov, FeedbackPhoton
void MpdEventManager::AddParticlesToPdgDataBase()
{
    TDatabasePDG* pdgDB = TDatabasePDG::Instance();

    const Double_t kAu2Gev = 0.9314943228;
    const Double_t khSlash = 1.0545726663e-27;
    const Double_t kErg2Gev = 1/1.6021773349e-3;
    const Double_t khShGev = khSlash*kErg2Gev;
    const Double_t kYear2Sec = 3600*24*365.25;

    // Ions
    if (!pdgDB->GetParticle(1000010020))
        pdgDB->AddParticle("Deuteron","Deuteron", 2*kAu2Gev+8.071e-3,kTRUE, 0, 3, "Ion", 1000010020);

    if (!pdgDB->GetParticle(1000010030))
        pdgDB->AddParticle("Triton","Triton", 3*kAu2Gev+14.931e-3, kFALSE, khShGev/(12.33*kYear2Sec), 3, "Ion", 1000010030);

    if (!pdgDB->GetParticle(1000020040))
        pdgDB->AddParticle("Alpha","Alpha", 4*kAu2Gev+2.424e-3, kTRUE, khShGev/(12.33*kYear2Sec), 6, "Ion", 1000020040);

    if (!pdgDB->GetParticle(1000020030))
        pdgDB->AddParticle("HE3","HE3", 3*kAu2Gev+14.931e-3, kFALSE, 0, 6, "Ion", 1000020030);

    // Special particles
    if (!pdgDB->GetParticle(50000050))
        pdgDB->AddParticle("Cherenkov","Cherenkov", 0, kFALSE, 0, 0, "Special", 50000050);

    if (!pdgDB->GetParticle(50000051))
        pdgDB->AddParticle("FeedbackPhoton","FeedbackPhoton", 0, kFALSE, 0, 0, "Special", 50000051);
}

void MpdEventManager::AddEventElement(TEveElement* element, ElementList element_list)
{
    switch (element_list)
    {
        case MCPointList:
        {
            if (EveMCPoints == NULL)
            {
                EveMCPoints = new TEveElementList("MC points");
                gEve->AddElement(EveMCPoints, this);
                EveMCPoints->SetRnrState(kFALSE);
                GetEventEditor()->fShowMCPoints->SetEnabled(kTRUE);
            }

            gEve->AddElement(element, EveMCPoints);
            break;
        }
        case MCTrackList:
        {
            if (EveMCTracks == NULL)
            {
                EveMCTracks = new TEveElementList("MC tracks");
                gEve->AddElement(EveMCTracks, this);
                EveMCTracks->SetRnrState(kFALSE);
                GetEventEditor()->fShowMCTracks->SetEnabled(kTRUE);
            }

            gEve->AddElement(element, EveMCTracks);
            break;
        }
        case RecoPointList:
        {
            if (EveRecoPoints == NULL)
            {
                EveRecoPoints = new TEveElementList("Reco points");
                gEve->AddElement(EveRecoPoints, this);
                EveRecoPoints->SetRnrState(kFALSE);
                GetEventEditor()->fShowRecoPoints->SetEnabled(kTRUE);
            }

            gEve->AddElement(element, EveRecoPoints);
            break;
        }
        case RecoTrackList:
        {
            if (EveRecoTracks == NULL)
            {
                EveRecoTracks = new TEveElementList("Reco tracks");
                gEve->AddElement(EveRecoTracks, this);
                EveRecoTracks->SetRnrState(kFALSE);
                GetEventEditor()->fShowRecoTracks->SetEnabled(kTRUE);
            }

            gEve->AddElement(element, EveRecoTracks);
            break;
        }
    }// switch (element_list)
}


void MpdEventManager::LoadXMLSettings()
{
    TDOMParser* Parser = new TDOMParser();
    Parser->SetValidate(kFALSE);
    Parser->ParseFile(fXMLConfig);

    TXMLNode* MainNode = Parser->GetXMLDocument()->GetRootNode();
    FairXMLNode* xml = new FairXMLNode(MainNode);
    for (int i = 0; i < xml->GetNChildren(); i++)
    {
        TString nodename = xml->GetChild(i)->GetName();
        if (nodename.EqualTo("Detectors"))
        {
            TGeoNode* top = gGeoManager->GetTopNode();
            FairXMLNode* top_xml = xml->GetChild(i)->GetChild(0);
            if (!top_xml->IsNull())
                LoadXMLDetector(top, top_xml);
        }
        else if (nodename.EqualTo("MCTracksColors"))
        {
            FairXMLNode* colors = xml->GetChild(i);
            for (int j = 0; j < colors->GetNChildren(); j++)
            {
                FairXMLNode* color = colors->GetChild(j);
                TString pgd_code = color->GetAttribValue("pdg");
                TString color_code = color->GetAttribValue("color");
                fPDGToColor[pgd_code.Atoi()] = StringToColor(color_code);
            }
        }
    }

    delete xml;
    delete Parser;
}

void MpdEventManager::LoadXMLDetector(TGeoNode* node, FairXMLNode* xml, Int_t depth)
{
    TString name = xml->GetAttribValue("name");
    TString node_name = node->GetName();

    Bool_t recursive = (xml->GetAttribValue("recursive").Length() != 0 && !name.EqualTo(node_name));
    if (recursive && depth == 0)
        return;

    TString color = xml->GetAttribValue("color");
    if (!color.EqualTo(""))
    {
        node->GetVolume()->SetFillColor(StringToColor(color));
        node->GetVolume()->SetLineColor(StringToColor(color));
    }

    TString transparency = xml->GetAttribValue("transparency");
    if (!transparency.EqualTo(""))
        node->GetVolume()->SetTransparency((Char_t)(transparency.Atoi()));

    if (xml->GetAttribValue("recursive").Length() > 0)
    {
        TString val = xml->GetAttribValue("recursive");
        Int_t xml_depth = val.Atoi();
        if (recursive)
            xml_depth =depth-1;

        for (int i = 0; i < node->GetNdaughters(); i++)
        {
            TGeoNode* daughter_node = node->GetDaughter(i);
            LoadXMLDetector(daughter_node, xml, xml_depth);
        }
    }

    if (xml->GetNChildren() > 0 && !recursive)
    {
        for (int i = 0; i < node->GetNdaughters(); i++)
        {
            TString subdetector_name = node->GetDaughter(i)->GetName();
            for (int j = 0; j < xml->GetNChildren(); j++)
            {
                FairXMLNode* subnode = xml->GetChild(j);
                TString subnode_name = subnode->GetAttribValue("name");
                if (subnode_name == subdetector_name)
                    LoadXMLDetector(node->GetDaughter(i), subnode);
            }
        }
    }
}

ClassImp(MpdEventManager)
