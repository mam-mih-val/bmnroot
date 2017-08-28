// FairEventManager: class for event management and navigation.

#include "FairEventManager.h"
#include "FairEventManagerEditor.h"
#include "constants.h"

#include "TDatabasePDG.h"
#include "TEveGeoNode.h"
#include "TEveManager.h"
#include "TGeoManager.h"
#include <TGLViewer.h>
#include <TGLCameraOverlay.h>
#include <TGLLightSet.h>
#include <TEveProjectionAxes.h>
#include <TEveBrowser.h>

// XML
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemastypes.h>

#include <unistd.h>
#include <cerrno>
#include <iostream>
#include <sstream>

FairEventManager* FairEventManager::fgRinstance = 0;
FairEventManager* FairEventManager::Instance() { return fgRinstance; }

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
FairEventManager::FairEventManager()
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

   fRPhiMng(0),
   fRhoZMng(0),
   fRPhiView(0),
   fRhoZView(0),
   fMulti3DView(0),
   fMultiRPhiView(0),
   fMultiRhoZView(0),
   fRPhiGeomScene(0),
   fRhoZGeomScene(0),

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
    InitColorStructure();
}

// COLOR SET:
// white, black, gray,
// blue, azure (темно-синий), cyan (морской волны), teal (бирюзовый),
// green, spring (светло-зеленый), green+2 (темно-зеленый), spring+2 (темно-зеленый), khaki
// yellow, orange (желтый с оттенком), orange+2 (оранжевый кор.), orange+1 (светло-оранжевый кор.), orange+7 (выделенно-оранжевый)
// red, violet, magenta (бардовый), magenta-6 (светло-бардовый), pink (темно-розовый)
void FairEventManager::InitColorStructure()
{
    // load colors from XML file
    TString coloring_xml_path = "$VMCWORKDIR/config/eventdisplay.xml";
    TString coloring_xsd_path = "$VMCWORKDIR/eventdisplay/coloring.xsd";
    gSystem->ExpandPathName(coloring_xml_path);
    gSystem->ExpandPathName(coloring_xsd_path);

    // check XML
    if (ValidateXml(coloring_xml_path.Data(), coloring_xsd_path.Data()) == true)
    {   
        xmlDoc* doc = xmlReadFile(coloring_xml_path.Data(), NULL, 0);
        
        /* Get the root element node */
        xmlNode* root_element = NULL;
        root_element = xmlDocGetRootElement(doc);
        xmlAttr* root_element_attributes = root_element->properties;
        xmlChar* value = xmlNodeListGetString(root_element->doc, root_element_attributes->children, 1);
        xmlFree(root_element_attributes);

        xmlNodePtr cur_node = root_element;
        if (strcmp((char*)value, "default") == 0)
        {
            cout<<"using default coloring"<<endl;
            gVisualizationColoring = defaultColoring;
        }
        else
        {          
            if (strcmp((char*)value, "detector") == 0)
                gVisualizationColoring = selectedColoring;
            else
                gVisualizationColoring = levelColoring;
            structSelectedColoring* selected_coloring;
            structLevelColoring* level_coloring;

            cur_node = root_element->children;
            while (cur_node)
            {
                if ((strcmp((char*)cur_node->name, "text") != 0) //skipping elements with no attributes
                   && (cur_node->type != XML_COMMENT_NODE))
                {
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
                }
                cur_node = cur_node->next;
            }// while (cur_node)
        }
            
        xmlFree(value);
        xmlFree(cur_node);
        xmlCleanupParser();
        xmlFreeDoc(doc);
    }
    else
    {
        cout<<"using default coloring"<<endl;
        gVisualizationColoring = defaultColoring;
    }
    
    return;
}

//______________________________________________________________________________
void FairEventManager::Init(Int_t visopt, Int_t vislvl, Int_t maxvisnds)
{
    TEveManager::Create();
    fRunAna->Init();

    TGeoNode* N = gGeoManager->GetTopNode();
    TEveGeoTopNode* TNod = new TEveGeoTopNode(gGeoManager, N, visopt, vislvl, maxvisnds);

    // change color and visibility of geometry nodes
    if (gVisualizationColoring != defaultColoring)
    {
        if (gVisualizationColoring == selectedColoring)
            SelectedGeometryColoring();
        else
            LevelChangeNodeProperty(N, 0);
    }

    gEve->AddGlobalElement(TNod);
    gEve->FullRedraw3D(kTRUE);
    fEvent = gEve->AddEvent(this);

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

    // different views and projections for Offline mode
    if (!isOnline)
    {
        // create projection managers
        fRPhiMng = new TEveProjectionManager(TEveProjection::kPT_RPhi);
        gEve->AddToListTree(fRPhiMng, kFALSE);

        fRhoZMng = new TEveProjectionManager(TEveProjection::kPT_RhoZ);
        gEve->AddToListTree(fRhoZMng, kFALSE);

        // create axes for viewers
        TEveProjectionAxes* fAxesPhi = new TEveProjectionAxes(fRPhiMng);
        fAxesPhi->SetMainColor(kRed);
        TEveProjectionAxes* fAxesRho = new TEveProjectionAxes(fRhoZMng);
        fAxesRho->SetMainColor(kRed);

        // ADD WINDOW in EventDisplay for RPhi projection
        TEveWindowSlot *RPhiSlot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
        TEveWindowPack *RPhiPack = RPhiSlot->MakePack();
        RPhiPack->SetElementName("RPhi View");
        RPhiPack->SetShowTitleBar(kFALSE);
        RPhiPack->NewSlot()->MakeCurrent();
        fRPhiView = gEve->SpawnNewViewer("RPhi View", "");
        fRPhiView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
        // set camera parameters
        fRPhiView->GetGLViewer()->GetCameraOverlay()->SetOrthographicMode(TGLCameraOverlay::kAxis);
        fRPhiView->GetGLViewer()->GetCameraOverlay()->SetShowOrthographic(kTRUE);
        // switch off left, right, top and bottom light sources
        fRPhiView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightLeft, false);
        fRPhiView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightRight, false);
        fRPhiView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightTop, false);
        fRPhiView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightBottom, false);
        if (!isDarkColor)
            fRPhiView->GetGLViewer()->UseLightColorSet();
        fRPhiView->GetGLViewer()->SetClearColor(background_color);

        // create scene holding projected geometry for the RPhi view
        fRPhiGeomScene  = gEve->SpawnNewScene("RPhi", "Scene holding geometry for RPhi.");
        // add axes for scene of RPhi view
        fRPhiGeomScene->AddElement(fAxesPhi);
        // add geometry scene to RPhi View
        fRPhiView->AddScene(fRPhiGeomScene);
        // create scene holding projected event-data for the RPhi view
        //fRPhiEventScene = gEve->SpawnNewScene("RPhi Event Data", "Scene holding event-data for RPhi.");
        //fRPhiView->AddScene(fRPhiEventScene);
        fRPhiView->AddScene(gEve->GetGlobalScene());
        fRPhiView->AddScene(gEve->GetEventScene());

        // set clip plane
        Double_t eqRPhi[4] = {0, 0, 1, 0};
        fRPhiView->GetGLViewer()->GetClipSet()->SetClipType(TGLClip::kClipPlane);
        fRPhiView->GetGLViewer()->GetClipSet()->SetClipState(TGLClip::kClipPlane, eqRPhi);

        // ADD WINDOW in EvenDisplay for RhoZ projection
        TEveWindowSlot *RhoZSlot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
        TEveWindowPack *RhoZPack = RhoZSlot->MakePack();
        RhoZPack->SetElementName("RhoZ View");
        RhoZPack->SetShowTitleBar(kFALSE);
        RhoZPack->NewSlot()->MakeCurrent();
        fRhoZView = gEve->SpawnNewViewer("RhoZ View", "");
        fRhoZView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoZOY);
        // set camera parameters
        fRhoZView->GetGLViewer()->GetCameraOverlay()->SetOrthographicMode(TGLCameraOverlay::kAxis);
        fRhoZView->GetGLViewer()->GetCameraOverlay()->SetShowOrthographic(kTRUE);
        // switch off left, right and front light sources
        fRhoZView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightLeft, false);
        fRhoZView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightRight, false);
        fRhoZView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightFront, false);
        if (!isDarkColor)
            fRhoZView->GetGLViewer()->UseLightColorSet();
        fRhoZView->GetGLViewer()->SetClearColor(background_color);

        // create scene holding projected geometry for the RhoZ view.
        fRhoZGeomScene  = gEve->SpawnNewScene("RhoZ", "Scene holding geometry for RhoZ.");
        // add axes for scene of RPhoZ view
        fRhoZGeomScene->AddElement(fAxesRho);
        // add geometry scenes to RhoZView
        fRhoZView->AddScene(fRhoZGeomScene);
        // create scene holding projected event-data for the RhoZ view
        //fRhoZEventScene = gEve->SpawnNewScene("RhoZ Event Data", "Scene holding event-data for RhoZ.");
        //fRhoZView->AddScene(fRhoZEventScene);
        fRhoZView->AddScene(gEve->GetGlobalScene());
        fRhoZView->AddScene(gEve->GetEventScene());

        // set clip plane
        Double_t eqRhoZ[4] = {-1, 0, 0, 0};
        fRhoZView->GetGLViewer()->GetClipSet()->SetClipType(TGLClip::kClipPlane);
        fRhoZView->GetGLViewer()->GetClipSet()->SetClipState(TGLClip::kClipPlane, eqRhoZ);

        // ADD WINDOW in EvenDisplay for MultiView
        TEveWindowSlot *MultiSlot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
        TEveWindowPack *MultiPack = MultiSlot->MakePack();
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

        // add slot for RPhi projection on Multi View tab
        MultiPack = MultiPack->NewSlot()->MakePack();
        MultiPack->SetShowTitleBar(kFALSE);
        MultiPack->NewSlot()->MakeCurrent();
        fMultiRPhiView = gEve->SpawnNewViewer("RPhi View (multi)", "");
        fMultiRPhiView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
        // set camera parameters
        fMultiRPhiView->GetGLViewer()->GetCameraOverlay()->SetOrthographicMode(TGLCameraOverlay::kAxis);
        fMultiRPhiView->GetGLViewer()->GetCameraOverlay()->SetShowOrthographic(kTRUE);
        // switch off left, right, top and bottom light sources
        fMultiRPhiView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightLeft, false);
        fMultiRPhiView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightRight, false);
        fMultiRPhiView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightTop, false);
        fMultiRPhiView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightBottom, false);
        if (!isDarkColor)
            fMultiRPhiView->GetGLViewer()->UseLightColorSet();
        fMultiRPhiView->GetGLViewer()->SetClearColor(background_color);

        // add RPhi scenes (second tab) to RPhi MultiView
        fMultiRPhiView->AddScene(fRPhiGeomScene);
        //fMultiRPhiView->AddScene(fRPhiEventScene);
        fMultiRPhiView->AddScene(gEve->GetGlobalScene());
        fMultiRPhiView->AddScene(gEve->GetEventScene());

        // set clip plane
        fMultiRPhiView->GetGLViewer()->GetClipSet()->SetClipType(TGLClip::kClipPlane);
        fMultiRPhiView->GetGLViewer()->GetClipSet()->SetClipState(TGLClip::kClipPlane, eqRPhi);

        // add slot for RhoZ projection on Multi View tab
        MultiPack->NewSlot()->MakeCurrent();
        fMultiRhoZView = gEve->SpawnNewViewer("RhoZ View (multi)", "");
        fMultiRhoZView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoZOY);
        // set camera parameters
        fMultiRhoZView->GetGLViewer()->GetCameraOverlay()->SetOrthographicMode(TGLCameraOverlay::kAxis);
        fMultiRhoZView->GetGLViewer()->GetCameraOverlay()->SetShowOrthographic(kTRUE);
        // switch off left, right and front light sources
        fMultiRhoZView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightLeft, false);
        fMultiRhoZView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightRight, false);
        fMultiRhoZView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightFront, false);
        if (!isDarkColor)
            fMultiRhoZView->GetGLViewer()->UseLightColorSet();
        fMultiRhoZView->GetGLViewer()->SetClearColor(background_color);

        // add RhoZ scenes (second tab) to RhoZ MultiView
        fMultiRhoZView->AddScene(fRhoZGeomScene);
        //fMultiRhoZView->AddScene(fRhoZEventScene);
        fMultiRhoZView->AddScene(gEve->GetGlobalScene());
        fMultiRhoZView->AddScene(gEve->GetEventScene());

        // set clip plane
        fMultiRhoZView->GetGLViewer()->GetClipSet()->SetClipType(TGLClip::kClipPlane);
        fMultiRhoZView->GetGLViewer()->GetClipSet()->SetClipState(TGLClip::kClipPlane, eqRhoZ);

        // copy geometry and event scene for RPhi and RhoZ views from global scene (3D)
        //fRPhiGeomScene->AddElement(gEve->GetGlobalScene());
        //fRPhiEventScene->AddElement(gEve->GetEventScene());
        //fRhoZGeomScene->AddElement(gEve->GetGlobalScene());
        //fRhoZEventScene->AddElement(gEve->GetEventScene());

        // update all scenes
        //fRPhiView->GetGLViewer()->UpdateScene(kTRUE);
        //fRhoZView->GetGLViewer()->UpdateScene(kTRUE);
        //fMulti3DView->GetGLViewer()->UpdateScene(kTRUE);
        //fMultiRPhiView->GetGLViewer()->UpdateScene(kTRUE);
        //fMultiRhoZView->GetGLViewer()->UpdateScene(kTRUE);

        // don't change reposition camera on each update
        fRPhiView->GetGLViewer()->SetResetCamerasOnUpdate(kFALSE);
        fRhoZView->GetGLViewer()->SetResetCamerasOnUpdate(kFALSE);
        fMulti3DView->GetGLViewer()->SetResetCamerasOnUpdate(kFALSE);
        fMultiRPhiView->GetGLViewer()->SetResetCamerasOnUpdate(kFALSE);
        fMultiRhoZView->GetGLViewer()->SetResetCamerasOnUpdate(kFALSE);
    }//if (!isOnline)
}//FairEventManager::Init

// setting of geometry colors for DETECTOR COLORING MODE
void FairEventManager::SelectedGeometryColoring()
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
        Int_t curColor = GetColor(selected_coloring->detector_color);
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
            }
        }
    }

    return;
}

void FairEventManager::RecursiveChangeNodeProperty(TGeoNode* node, Int_t color, int transparency)
{
    for (int i = 0; i < node->GetNdaughters(); i++)
    {
        TGeoNode* child = node->GetDaughter(i);
        TGeoVolume* curVolume = child->GetVolume();

        curVolume->SetFillColor(color);
        curVolume->SetLineColor(color);
        curVolume->SetTransparency(transparency);

        if (child->GetNdaughters() != 0)
            RecursiveChangeNodeProperty(child, color, transparency);
    }
}

void FairEventManager::RecursiveChangeNodeTransparent(TGeoNode* node, int transparency)
{
    for (int i = 0; i < node->GetNdaughters(); i++)
    {
        TGeoNode* child = node->GetDaughter(i);
        TGeoVolume* curVolume = child->GetVolume();

        curVolume->SetTransparency(transparency);

        if (child->GetNdaughters() != 0)
            RecursiveChangeNodeTransparent(child, transparency);
    }
}

// set transparent geometry
void FairEventManager::SetTransparentGeometry(bool is_on)
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
            }
        }

        break;
    }
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

// hierarchical changing of nodes' properties: visibility, transparency, fill color and line color
void FairEventManager::LevelChangeNodeProperty(TGeoNode* node, int level)
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
            curVolume->SetFillColor(GetColor(level_coloring->fill_color));
            if (level_coloring->isFillLine) curVolume->SetLineColor(GetColor(level_coloring->fill_color));

            if (child->GetNdaughters() != 0)
            {
                level++;
                LevelChangeNodeProperty(child, level);
            }
        }//if (level < arr_size)
    }
}

// validate XML file with geometry colors
// returns true if successful or false if XML validation failed
bool FairEventManager::ValidateXml(const char *XMLFileName, const char *XSDFileName)
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

// return integer value of color by color name (default, blue)
// support following colors:
// white, black, gray,
// green, spring,
// blue, cyan (бирюзовый), azure, teal,
// red, pink (розовый), magenta, violet (фиолетовый),
// yellow, orange
Int_t FairEventManager::GetColor(TString colorName)
{
    colorName = colorName.ReplaceAll(" ", "");
    colorName.ToLower();

    // check if instead of color name we have an RGB triple
    if (colorName.BeginsWith("rgb"))
    {
        // parse rgb triple
        if (colorName < 6)
        {
            cout<<colorName<<" - RGB triple isn't correct. Color set to default blue"<<endl;
            return 600;
        }
        TString triple = colorName(3, colorName.Length() - 3);
        triple.Remove(TString::kLeading, '('); triple.Remove(TString::kTrailing, ')');

        int red_rgb = -1, green_rgb = -1, blue_rgb = -1;
        if (triple[0] == '#')
        {
            if (triple < 7)
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

    if (colorName == "white") return 0;
    else if (colorName == "black") return 1;
    else if (colorName == "gray") return 920;
    else if (colorName == "blue") return 600;
    else if (colorName == "red") return 632;
    else if (colorName == "green") return 416;
    else if (colorName == "yellow") return 400;
    else if (colorName == "magenta") return 616;
    else if (colorName == "cyan") return 432;
    else if (colorName == "orange") return 800;
    else if (colorName == "pink") return 900;
    else if (colorName == "violet") return 880;
    else if (colorName == "azure") return 860;
    else if (colorName == "teal") return 840;
    else if (colorName == "spring") return 820;

    else if (colorName == "green+2") return 418;
    else if (colorName == "spring+2") return 823;
    else if (colorName == "orange+1") return 801;
    else if (colorName == "orange+2") return 802;
    else if (colorName == "orange+7") return 807;
    else if (colorName == "magenta-6") return 610;
    else if (colorName == "khaki") return 403;
    else
    {
        cout<<colorName<<" not found. Color set to default blue"<<endl;
        return 600;
    }
}

//______________________________________________________________________________
void FairEventManager::Open()
{
}

//______________________________________________________________________________
void FairEventManager::Close()
{
}

//______________________________________________________________________________
void FairEventManager::DisplaySettings()
{
}

//______________________________________________________________________________
void FairEventManager::UpdateEditor()
{
}

// FairEventManager destructor
FairEventManager::~FairEventManager()
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
void FairEventManager::GotoEvent(Int_t event)
{
    iCurrentEvent = event;
    fRunAna->Run((Long64_t)event);
}

// go to next FairRunAna event for scene data getting
void FairEventManager::NextEvent()
{
    fRunAna->Run((Long64_t)++iCurrentEvent);
}

// go to previous FairRunAna event for scene data getting
void FairEventManager::PrevEvent()
{
    fRunAna->Run((Long64_t)--iCurrentEvent);
}

// assign different colors for differrent particles
// return integer value of color for track by particle pdg (default, white)
Int_t FairEventManager::Color(int pdg)
{
    switch (pdg)
    {
    case   22     :
        return  623;    // photon
    case   -2112  :
        return  2 ;   // anti-neutron
    case   -11    :
        return  3;    // e+
    case   -3122  :
        return  4;   // anti-Lambda
    case   11     :
        return  5;    // e-
    case   -3222  :
        return  6;   // Sigma-
    case   12     :
        return  7;    // e-neutrino (NB: flavour undefined by Geant)
    case   -3212  :
        return  8;   // Sigma0
    case   -13    :
        return  9;    // mu+
    case   -3112  :
        return  10;   // Sigma+ (PB)*/
    case   13     :
        return  11;    // mu-
    case   -3322  :
        return  12;   // Xi0MWPCDigit
    case   111    :
        return  13;    // pi0
    case   -3312  :
        return  14;   // Xi+
    case   211    :
        return  15;    // pi+
    case   -3334  :
        return  16;   // Omega+ (PB)
    case   -211   :
        return  17;    // pi-
    case   -15    :
        return  18;   // tau+
    case   130    :
        return  19;   // K long
    case   15     :
        return  20;   // tau-
    case   321    :
        return  21;   // K+
    case   411    :
        return  22;   // D+
    case   -321   :
        return  23;   // K-
    case   -411   :
        return  24;   // D-
    case   2112   :
        return  25;   // n
    case   421    :
        return  26;   // D0
    case   2212   :
        return  27;   // p
    case   -421   :
        return  28;   // D0
    case   -2212  :
        return  29;   // anti-proton
    case   431    :
        return  30;   // Ds+
    case   310    :
        return  31;   // K short
    case   -431   :
        return  32;   // anti Ds-
    case   221    :
        return  33;   // eta
    case   4122   :
        return  34;   // Lamba_c+
    case   3122   :
        return  35;   // Lambda
    case   24     :
        return  36;   // W+
    case   3222   :
        return  37;   // Sigma+
    case   -24    :
        return  38;   // W-
    case   3212   :
        return  39;   // Sigma0
    case   23     :
        return  40;   // Z
    case   3112   :
        return  41;   // Sigma-
    case   3322   :
        return  42;   // Xi0
    case   3312   :
        return  43;   // Xi-
    case   3334   :
        return  44;   // Omega- (PB)
    case   50000050   :
        return  801;   // Cerenkov
    case   1000010020  :
        return  45;
    case   1000010030  :
        return  48;
    case   1000020040   :
        return  50;
    case   1000020030   :
        return  55;
    case   0:
        return  391;    // Rootino
    default  :
        return 0;
    }//switch
}

// add particles to the PDG data base: Deuteron, Triton, Alpha, HE3; Cherenkov, FeedbackPhoton
void FairEventManager::AddParticlesToPdgDataBase(Int_t pdg)
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

void FairEventManager::AddEventElement(TEveElement* element, ElementList element_list)
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
    }
}

ClassImp(FairEventManager)
