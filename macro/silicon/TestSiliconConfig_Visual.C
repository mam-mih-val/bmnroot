/*
 * Macro:
 * Visual test of your FORWARD SILICON configuration, including:
 *   - view of selected station
 *   - views of each layer in the selected station
 *   - positions, rotations, dead zones, strip numbering order, strip angle inclination,
 *     strip borders, etc...
 *
 * Press <SHIFT>+<LEFT_MOUSE_BUTTON> to add and draw points in the station
 * Press <C> to clear the area of the station
 *
 * Author: D. Baranov
 * Created: 08.02.2022
 */

#include "Rtypes.h"

R__ADD_INCLUDE_PATH($VMCWORKDIR)
R__LOAD_LIBRARY(libBmnData.so)

#include <fstream>
#include <vector>

using namespace TMath;
using namespace std;

class BmnSiliconStation;

BmnSiliconStation *g_current_station = 0;

TMarker *g_hits_marker = 0;
Int_t g_n_hits_marker = 0;

vector<TMarker*> g_points_marker;

//Enable the ability to add points by pressing keyboard and mouse buttons
Bool_t g_EnableAddingPoints = true;

//Enable drawing points on the canvas of separate layers
Bool_t g_DrawOnSeparateLayers = false;

//Print information about added points and reconstructed hits
Bool_t g_ShowIntersectionPointInfo = true;

void DrawCreatedStationFront(BmnSiliconStation *station, vector<TLorentzVector> &points);
void RecognizeModuleNumFromPointAndDrawIt(TCanvas *station_front_canv);
void DrawCreatedStationTop(BmnSiliconStation *station, vector<TLorentzVector> &points);
//------------------------------------------------------------------------------

void TestSiliconConfig_Visual() {

    //Enable transparency OpenGL via gStyle
    //gStyle->SetCanvasPreferGL(1);

    //Select the current configuration
    TString gPathDetectorConfig = gSystem->Getenv("VMCWORKDIR");
    gPathDetectorConfig += "/parameters/silicon/XMLConfigs/";
    gPathDetectorConfig += "SiliconRun8_3stations.xml";
    //gPathDetectorConfig += "SiliconRun8_mods_10_14rot_18.xml";

    BmnSiliconStationSet *StationSet = new BmnSiliconStationSet(gPathDetectorConfig);
    //BmnSiliconStationSet *StationSet = new BmnSiliconStationSet("SiliconRun_TestSRC.xml");

    //Select the current station to be shown
    Int_t currentStationNumber = 0;

    //Define a set of test points if required
    vector<TLorentzVector> test_points;
    //TLorentzVector test_point;
    //test_point.SetXYZT(0, 0, 0, 0);
    //test_points.push_back(test_point);


    Int_t station_num = -1;
    Int_t module_num = -1;

    if ( (currentStationNumber >= 0)   && (currentStationNumber < StationSet->GetNStations()) ) {
        station_num = currentStationNumber;
    }

    if(station_num != -1) {
        BmnSiliconStation *station = StationSet->GetSiliconStation(station_num);

        DrawCreatedStationFront(station, test_points);
        DrawCreatedStationTop(station, test_points);
    }
    else {
        cout << "Current station number is incorrect!!!\n";
    }


    //delete StationSet;

    cout << "It's the end" << "\n";
    //gApplication->Terminate();
}

void DrawCreatedStationFront(BmnSiliconStation *station, vector<TLorentzVector> &points) {

    TString description_text = "";
        description_text += "FRONT: station "; description_text += station->GetStationNumber();
        description_text += ", xpos = "; description_text += station->GetXPosition();
        description_text += ", ypos = "; description_text += station->GetYPosition();
        description_text += ", zpos = "; description_text += station->GetZPosition();
    TCanvas *station_front_canv = new TCanvas("station_front_canv", description_text, 10, 10, 1000, 1000);
    station_front_canv->SetGrid();

    Int_t StationNumber = station->GetStationNumber();
    Double_t XSize = station->GetXSize();
    Double_t YSize = station->GetYSize();
    Double_t ZSize = station->GetZSize();
    Double_t XMinStation = station->GetXMinStation();
    Double_t XMaxStation = station->GetXMaxStation();
    Double_t YMinStation = station->GetYMinStation();
    Double_t YMaxStation = station->GetYMaxStation();
    Double_t ZMinStation = station->GetZMinStation();
    Double_t ZMaxStation = station->GetZMaxStation();
    Double_t XPosition = station->GetXPosition();
    Double_t YPosition = station->GetYPosition();
    Double_t ZPosition = station->GetZPosition();
    Int_t NModules = station->GetNModules();

    Double_t *XShiftsOfModules = new Double_t[NModules];
    Double_t *YShiftsOfModules = new Double_t[NModules];
    Double_t *ZShiftsOfModules = new Double_t[NModules];
    Double_t maxXShift = 0.0;
    Double_t maxYShift = 0.0;
    Double_t maxZShift = 0.0;
    for(Int_t iModule = 0; iModule < NModules; ++iModule) {
        XShiftsOfModules[iModule] = station->GetXShiftOfModule(iModule);
        YShiftsOfModules[iModule] = station->GetYShiftOfModule(iModule);
        ZShiftsOfModules[iModule] = station->GetZShiftOfModule(iModule);

        if( fabs(XShiftsOfModules[iModule]) > maxXShift ) maxXShift = fabs(XShiftsOfModules[iModule]);
        if( fabs(YShiftsOfModules[iModule]) > maxYShift ) maxYShift = fabs(YShiftsOfModules[iModule]);
        if( fabs(ZShiftsOfModules[iModule]) > maxZShift ) maxZShift = fabs(ZShiftsOfModules[iModule]);
    }

    if(1) {
        cout << "StationNumber = " << StationNumber << "\n";
        cout << "  XSize = " << XSize << "\n";
        cout << "  YSize = " << YSize << "\n";
        cout << "  ZSize = " << ZSize << "\n";
        cout << "\n";
        cout << "  XMinStation = " << XMinStation << "\n";
        cout << "  XMaxStation = " << XMaxStation << "\n";
        cout << "  YMinStation = " << YMinStation << "\n";
        cout << "  YMaxStation = " << YMaxStation << "\n";
        cout << "  ZMinStation = " << ZMinStation << "\n";
        cout << "  ZMaxStation = " << ZMaxStation << "\n";
        cout << "\n";
        cout << "  XPosition = " << XPosition << "\n";
        cout << "  YPosition = " << YPosition << "\n";
        cout << "  ZPosition = " << ZPosition << "\n";
        cout << "\n";
        cout << "  NModules = " << NModules << "\n";
        for(Int_t iModule = 0; iModule < NModules; ++iModule) {
            cout << "    XShiftsOfModules[" << iModule << "] = " << XShiftsOfModules[iModule] << "\n";
            cout << "    YShiftsOfModules[" << iModule << "] = " << YShiftsOfModules[iModule] << "\n";
            cout << "    ZShiftsOfModules[" << iModule << "] = " << ZShiftsOfModules[iModule] << "\n";

            BmnSiliconModule *module = station->GetModule(iModule);

            Double_t XModuleSize = module->GetXSize();
            Double_t YModuleSize = module->GetYSize();
            Double_t XMinModule = module->GetXMinModule();
            Double_t XMaxModule = module->GetXMaxModule();
            Double_t YMinModule = module->GetYMinModule();
            Double_t YMaxModule = module->GetYMaxModule();

            cout << "    XModuleSize = " << XModuleSize << "\n";
            cout << "    YModuleSize = " << YModuleSize << "\n";
            cout << "    XMinModule = " << XMinModule << "\n";
            cout << "    XMaxModule = " << XMaxModule << "\n";
            cout << "    YMinModule = " << YMinModule << "\n";
            cout << "    YMaxModule = " << YMaxModule << "\n";
            cout << "\n";

            Int_t NLayers = module->GetNStripLayers();
            for(Int_t iLayer = 0; iLayer < NLayers; ++iLayer) {
                BmnSiliconLayer layer = module->GetStripLayer(iLayer);

                Double_t stripAngleDeg = layer.GetAngleDeg();
                Double_t pitch = layer.GetPitch();
                Double_t XLayerSize = layer.GetXSize();
                Double_t YLayerSize = layer.GetYSize();
                Double_t XMinLayer = layer.GetXMinLayer();
                Double_t XMaxLayer = layer.GetXMaxLayer();
                Double_t YMinLayer = layer.GetYMinLayer();
                Double_t YMaxLayer = layer.GetYMaxLayer();
                StripLayerType layerType = layer.GetType();
                Double_t xLeftBorderLayer = layer.GetXLeftStripBorderPoint();
                Double_t xRightBorderLayer = layer.GetXRightStripBorderPoint();
                Double_t yLeftBorderLayer = layer.GetYLeftStripBorderPoint();
                Double_t yRightBorderLayer = layer.GetYRightStripBorderPoint();
                StripNumberingDirection stripDirection = layer.GetStripNumberingOrder();

                cout << "    Layer[" << iLayer << "]:\n";
                cout << "       stripAngleDeg = " << stripAngleDeg << "\n";
                cout << "       pitch = " << pitch << "\n";
                cout << "\n";
                cout << "       XLayerSize = " << XLayerSize << "\n";
                cout << "       YLayerSize = " << YLayerSize << "\n";
                cout << "\n";
                cout << "       XMinLayer = " << XMinLayer << "\n";
                cout << "       XMaxLayer = " << XMaxLayer << "\n";
                cout << "       YMinLayer = " << YMinLayer << "\n";
                cout << "       YMaxLayer = " << YMaxLayer << "\n";
                cout << "\n";
                if(layerType == StripLayerType::LowerStripLayer) {
                    cout << "       layerType = " << "lower" << "\n";
                }
                if(layerType == StripLayerType::UpperStripLayer) {
                    cout << "       layerType = " << "upper" << "\n";
                }
                cout << "\n";
                cout << "       xLeftBorderLayer = " << xLeftBorderLayer << "\n";
                cout << "       xRightBorderLayer = " << xRightBorderLayer << "\n";
                cout << "       yLeftBorderLayer = " << yLeftBorderLayer << "\n";
                cout << "       yRightBorderLayer = " << yRightBorderLayer << "\n";
                if(stripDirection == StripNumberingDirection::LeftToRight) {
                    cout << "       stripDirection = " << "LeftToRight" << "\n";
                }
                if(stripDirection == StripNumberingDirection::RightToLeft) {
                    cout << "       stripDirection = " << "RightToLeft" << "\n";
                }
                cout << "\n";

                vector<DeadZoneOfStripLayer> deadZones = layer.GetDeadZones();

                for(Int_t idead = 0; idead < deadZones.size(); ++idead) {
                    cout << "       DeadZone[" << idead << "]:\n";

                    Int_t NDeadPoints = deadZones[idead].GetNPoints();

                    for(Int_t ip = 0; ip < NDeadPoints; ++ip) {
                        cout << "         (x,y) = ( " << deadZones[idead].GetXPoint(ip) << ", " << deadZones[idead].GetYPoint(ip) << " )\n";
                    }
                    cout << "\n";
                }
            }
        }
        cout << "-----------------------------------------------------------\n";
    }

    BmnSiliconModule **Modules = new BmnSiliconModule*[NModules];
    for(Int_t imod = 0; imod < NModules; imod++) {
        Modules[imod] = station->GetModule(imod);
    }

    //histo limits
    Double_t xmin = XMinStation - XSize*0.3;
    Double_t xmax = XMaxStation + XSize*0.3;
    Double_t ymin = YMinStation - YSize*0.3;
    Double_t ymax = YMaxStation + YSize*0.3;

    //gStyle->SetOptStat(0);
    TH2F *station_front_2dhist = new TH2F("station_front_2dhist", description_text, 100, xmin, xmax, 100, ymin, ymax);
    station_front_2dhist->GetXaxis()->SetTitle("+x (classical)");
    station_front_2dhist->GetYaxis()->SetTitle("+y");
    station_front_2dhist->SetStats(0);
    station_front_canv->cd();
    station_front_2dhist->Draw();

    TPolyLine **bordersOfModules = new TPolyLine*[NModules];

    for(Int_t im = 0; im < NModules; im++) {

        //local module borders -------------------------------------------------
        Double_t x_min_module = Modules[im]->GetXMinModule();
        Double_t x_max_module = Modules[im]->GetXMaxModule();
        Double_t y_min_module = Modules[im]->GetYMinModule();
        Double_t y_max_module = Modules[im]->GetYMaxModule();

        Double_t x_module_border_points[5] = {x_min_module, x_min_module, x_max_module, x_max_module, x_min_module};
        Double_t y_module_border_points[5] = {y_min_module, y_max_module, y_max_module, y_min_module, y_min_module};

        //if we have the rotation of the station
        if( std::fabs(Modules[im]->GetModuleRotationAngleDeg()) >= std::numeric_limits<double>::epsilon() ) {
            Double_t x_p0 = Modules[im]->ConvertLocalToRotatedX(x_min_module, y_min_module);
            Double_t y_p0 = Modules[im]->ConvertLocalToRotatedY(x_min_module, y_min_module);

            Double_t x_p1 = Modules[im]->ConvertLocalToRotatedX(x_min_module, y_max_module);
            Double_t y_p1 = Modules[im]->ConvertLocalToRotatedY(x_min_module, y_max_module);

            Double_t x_p2 = Modules[im]->ConvertLocalToRotatedX(x_max_module, y_max_module);
            Double_t y_p2 = Modules[im]->ConvertLocalToRotatedY(x_max_module, y_max_module);

            Double_t x_p3 = Modules[im]->ConvertLocalToRotatedX(x_max_module, y_min_module);
            Double_t y_p3 = Modules[im]->ConvertLocalToRotatedY(x_max_module, y_min_module);

            x_module_border_points[0] = x_p0;
            x_module_border_points[1] = x_p1;
            x_module_border_points[2] = x_p2;
            x_module_border_points[3] = x_p3;
            x_module_border_points[4] = x_p0;

            y_module_border_points[0] = y_p0;
            y_module_border_points[1] = y_p1;
            y_module_border_points[2] = y_p2;
            y_module_border_points[3] = y_p3;
            y_module_border_points[4] = y_p0;
        }

        bordersOfModules[im] = new TPolyLine(5, x_module_border_points, y_module_border_points);
        bordersOfModules[im]->SetLineColor(TColor::GetColor("#000000"));
        bordersOfModules[im]->SetLineStyle(2);
        bordersOfModules[im]->SetLineWidth(1);
        bordersOfModules[im]->SetFillColor(TColor::GetColor("#cafced"));
        bordersOfModules[im]->SetFillStyle(0); // 0 - not filled, 1001 - solid

        station_front_canv->cd();
        //bordersOfModules[im]->Draw("l");
        //----------------------------------------------------------------------

        //layers ---------------------------------------------------------------
        Int_t NLayers = Modules[im]->GetNStripLayers();

        for(Int_t ilayer = 0; ilayer < NLayers; ++ilayer) {

            //Separate canvas for current layer
            description_text = "";
            description_text += "FRONT: station "; description_text += station->GetStationNumber();
            description_text += ", module = "; description_text += im;
            description_text += ", layer = "; description_text += ilayer;
            TCanvas *layer_canv = new TCanvas(TString(TString("layer_canv_")+=ilayer)+=im, description_text, 10, 10, 1000, 1000);
            layer_canv->SetGrid();

            //TH2F *layer_2dhist = new TH2F(TString(TString("layer_2dhist_")+=ilayer)+=im, description_text, 100, -range_size, range_size, 100, -range_size, range_size);
            TH2F *layer_2dhist = new TH2F(TString(TString("layer_2dhist_")+=ilayer)+=im, description_text, 100, xmin, xmax, 100, ymin, ymax);
            layer_2dhist->GetXaxis()->SetTitle("+x (classical)");
            layer_2dhist->GetYaxis()->SetTitle("+y");
            layer_2dhist->SetStats(0);
            layer_canv->cd();
            layer_2dhist->Draw();
            //------------------------------------------------------------------

            BmnSiliconLayer layer = Modules[im]->GetStripLayer(ilayer);

            Double_t x_min_layer = layer.GetXMinLayer();
            Double_t x_max_layer = layer.GetXMaxLayer();
            Double_t y_min_layer = layer.GetYMinLayer();
            Double_t y_max_layer = layer.GetYMaxLayer();

            TPolyLine *bordersOfLayer = nullptr;

            Double_t x_layer_border_points[5] = {x_min_layer, x_min_layer, x_max_layer, x_max_layer, x_min_layer};
            Double_t y_layer_border_points[5] = {y_min_layer, y_max_layer, y_max_layer, y_min_layer, y_min_layer};

            //if we have the rotation of the station
            if( std::fabs(Modules[im]->GetModuleRotationAngleDeg()) >= std::numeric_limits<double>::epsilon() ) {
                Double_t x_p0 = Modules[im]->ConvertLocalToRotatedX(x_min_layer, y_min_layer);
                Double_t y_p0 = Modules[im]->ConvertLocalToRotatedY(x_min_layer, y_min_layer);

                Double_t x_p1 = Modules[im]->ConvertLocalToRotatedX(x_min_layer, y_max_layer);
                Double_t y_p1 = Modules[im]->ConvertLocalToRotatedY(x_min_layer, y_max_layer);

                Double_t x_p2 = Modules[im]->ConvertLocalToRotatedX(x_max_layer, y_max_layer);
                Double_t y_p2 = Modules[im]->ConvertLocalToRotatedY(x_max_layer, y_max_layer);

                Double_t x_p3 = Modules[im]->ConvertLocalToRotatedX(x_max_layer, y_min_layer);
                Double_t y_p3 = Modules[im]->ConvertLocalToRotatedY(x_max_layer, y_min_layer);

                x_layer_border_points[0] = x_p0;
                x_layer_border_points[1] = x_p1;
                x_layer_border_points[2] = x_p2;
                x_layer_border_points[3] = x_p3;
                x_layer_border_points[4] = x_p0;

                y_layer_border_points[0] = y_p0;
                y_layer_border_points[1] = y_p1;
                y_layer_border_points[2] = y_p2;
                y_layer_border_points[3] = y_p3;
                y_layer_border_points[4] = y_p0;
            }

            bordersOfLayer = new TPolyLine(5, x_layer_border_points, y_layer_border_points);
            bordersOfLayer->SetLineColorAlpha(TColor::GetColor("#009900"), 0.3);
            bordersOfLayer->SetLineStyle(1);
            bordersOfLayer->SetLineWidth(1);
            //bordersOfLayer->SetFillColor(TColor::GetColor("#cafced"));
            bordersOfLayer->SetFillColorAlpha(TColor::GetColor("#cccccc"), 0.3);
            bordersOfLayer->SetFillStyle(1001); // 0 - not filled, 1001 - solid


            station_front_canv->cd();
            bordersOfLayer->Draw("f");
            bordersOfLayer->Draw("l");

            layer_canv->cd();
            bordersOfLayer->Draw("f");

            //layer dead zones--------------------------------------------------
            vector<DeadZoneOfStripLayer> DeadZones = layer.GetDeadZones();
            Int_t NDeadZones = DeadZones.size();

            TPolyLine *dead_zones_polylines = new TPolyLine[NDeadZones];

            for(Int_t izone = 0; izone < NDeadZones; ++izone) {
                DeadZoneOfStripLayer dead_zone  = DeadZones[izone];

                for(Int_t ipoint = 0; ipoint < dead_zone.GetNPoints(); ++ipoint) {
                    Double_t x = dead_zone.GetXPoint(ipoint);
                    Double_t y = dead_zone.GetYPoint(ipoint);

                    //if we have the rotation of the station
                    if( std::fabs(Modules[im]->GetModuleRotationAngleDeg()) >= std::numeric_limits<double>::epsilon() ) {
                        Double_t x_p = Modules[im]->ConvertLocalToRotatedX(x, y);
                        Double_t y_p = Modules[im]->ConvertLocalToRotatedY(x, y);
                        x = x_p;
                        y = y_p;
                    }

                    dead_zones_polylines[izone].SetNextPoint(x,y);
                    if(ipoint == dead_zone.GetNPoints()-1) {
                        dead_zones_polylines[izone].SetNextPoint(dead_zones_polylines->GetX()[0], dead_zones_polylines->GetY()[0]);
                    }
                }

                dead_zones_polylines[izone].SetFillStyle(1001);
                dead_zones_polylines[izone].SetLineColorAlpha(TColor::GetColor("#ff0000"), 0.3);
                dead_zones_polylines[izone].SetFillColorAlpha(TColor::GetColor("#ffc7c7"), 0.3);
                dead_zones_polylines[izone].SetLineWidth(1);

                station_front_canv->cd();
                dead_zones_polylines[izone].Draw("f");
                dead_zones_polylines[izone].Draw("");

                layer_canv->cd();
                dead_zones_polylines[izone].Draw("f");
                dead_zones_polylines[izone].Draw("");
            }
            //------------------------------------------------------------------

            //strip borders ----------------------------------------------------
            Double_t x_left_border = layer.GetXLeftStripBorderPoint();
            Double_t y_left_border = layer.GetYLeftStripBorderPoint();
            Double_t x_right_border = layer.GetXRightStripBorderPoint();
            Double_t y_right_border = layer.GetYRightStripBorderPoint();

            StripNumberingDirection strip_direction = layer.GetStripNumberingOrder();

            Double_t strip_angle_deg = layer.GetAngleDeg();
            Double_t x_shift_up_left = Tan(strip_angle_deg*DegToRad())*(y_max_layer-y_left_border);
            Double_t x_shift_down_left = Tan(strip_angle_deg*DegToRad())*(y_left_border-y_min_layer);
            Double_t x_shift_up_right = Tan(strip_angle_deg*DegToRad())*(y_max_layer-y_right_border);
            Double_t x_shift_down_right = Tan(strip_angle_deg*DegToRad())*(y_right_border-y_min_layer);

            //left strip line
            Double_t x1_left_strip_line = x_left_border - x_shift_down_left;
            Double_t y1_left_strip_line = y_min_layer;
            Double_t x2_left_strip_line = x_left_border + x_shift_up_left;
            Double_t y2_left_strip_line = y_max_layer;
            if( std::fabs(Modules[im]->GetModuleRotationAngleDeg()) >= std::numeric_limits<double>::epsilon() ) {
                Double_t x1_p = Modules[im]->ConvertLocalToRotatedX(x1_left_strip_line, y1_left_strip_line);
                Double_t y1_p = Modules[im]->ConvertLocalToRotatedY(x1_left_strip_line, y1_left_strip_line);
                Double_t x2_p = Modules[im]->ConvertLocalToRotatedX(x2_left_strip_line, y2_left_strip_line);
                Double_t y2_p = Modules[im]->ConvertLocalToRotatedY(x2_left_strip_line, y2_left_strip_line);
                x1_left_strip_line = x1_p;
                y1_left_strip_line = y1_p;
                x2_left_strip_line = x2_p;
                y2_left_strip_line = y2_p;
            }
            TLine *left_strip_line = new TLine();
            left_strip_line->SetX1(x1_left_strip_line);
            left_strip_line->SetY1(y1_left_strip_line);
            left_strip_line->SetX2(x2_left_strip_line);
            left_strip_line->SetY2(y2_left_strip_line);
            left_strip_line->SetLineColor(TColor::GetColor("#033cc0"));
            left_strip_line->Draw();

            //right strip line
            Double_t x1_right_strip_line = x_right_border - x_shift_down_right;
            Double_t y1_right_strip_line = y_min_layer;
            Double_t x2_right_strip_line = x_right_border + x_shift_up_right;
            Double_t y2_right_strip_line = y_max_layer;
            if( std::fabs(Modules[im]->GetModuleRotationAngleDeg()) >= std::numeric_limits<double>::epsilon() ) {
                Double_t x1_p = Modules[im]->ConvertLocalToRotatedX(x1_right_strip_line, y1_right_strip_line);
                Double_t y1_p = Modules[im]->ConvertLocalToRotatedY(x1_right_strip_line, y1_right_strip_line);
                Double_t x2_p = Modules[im]->ConvertLocalToRotatedX(x2_right_strip_line, y2_right_strip_line);
                Double_t y2_p = Modules[im]->ConvertLocalToRotatedY(x2_right_strip_line, y2_right_strip_line);
                x1_right_strip_line = x1_p;
                y1_right_strip_line = y1_p;
                x2_right_strip_line = x2_p;
                y2_right_strip_line = y2_p;
            }
            TLine *right_strip_line = new TLine();
            right_strip_line->SetX1(x1_right_strip_line);
            right_strip_line->SetY1(y1_right_strip_line);
            right_strip_line->SetX2(x2_right_strip_line);
            right_strip_line->SetY2(y2_right_strip_line);
            right_strip_line->SetLineColor(TColor::GetColor("#033cc0"));
            right_strip_line->Draw();

            //left border marker
            Double_t x_left_border_marker = x_left_border;
            Double_t y_left_border_marker = y_left_border;
            if( std::fabs(Modules[im]->GetModuleRotationAngleDeg()) >= std::numeric_limits<double>::epsilon() ) {
                Double_t x_p = Modules[im]->ConvertLocalToRotatedX(x_left_border, y_left_border);
                Double_t y_p = Modules[im]->ConvertLocalToRotatedY(x_left_border, y_left_border);
                x_left_border_marker = x_p;
                y_left_border_marker = y_p;
            }
            TMarker *left_border_marker = new TMarker(x_left_border_marker, y_left_border_marker, 20);
            left_border_marker->SetMarkerSize(1.2);
            left_border_marker->SetMarkerColor(TColor::GetColor("#ff0000"));
            left_border_marker->Draw();

            //right border marker
            Double_t x_right_border_marker = x_right_border;
            Double_t y_right_border_marker = y_right_border;
            if( std::fabs(Modules[im]->GetModuleRotationAngleDeg()) >= std::numeric_limits<double>::epsilon() ) {
                Double_t x_p = Modules[im]->ConvertLocalToRotatedX(x_right_border, y_right_border);
                Double_t y_p = Modules[im]->ConvertLocalToRotatedY(x_right_border, y_right_border);
                x_right_border_marker = x_p;
                y_right_border_marker = y_p;
            }
            TMarker *right_border_marker = new TMarker(x_right_border_marker, y_right_border_marker, 20);
            right_border_marker->SetMarkerSize(1.2);
            right_border_marker->SetMarkerColor(TColor::GetColor("#ff0000"));
            right_border_marker->Draw();

            //strip numbering direction
            TArrow *strip_direction_arrow = new TArrow();
            if(strip_direction == LeftToRight) {
                strip_direction_arrow->SetX1(x_left_border_marker);
                strip_direction_arrow->SetY1(y_left_border_marker);

                //finding arrow ending
                Double_t x_arrend = x_left_border + layer.GetXSize();
                Double_t y_arrend = y_left_border;
                if( std::fabs(Modules[im]->GetModuleRotationAngleDeg()) >= std::numeric_limits<double>::epsilon() ) {
                    Double_t x_p = Modules[im]->ConvertLocalToRotatedX(x_arrend, y_arrend);
                    Double_t y_p = Modules[im]->ConvertLocalToRotatedY(x_arrend, y_arrend);
                    x_arrend = x_p;
                    y_arrend = y_p;
                }
                strip_direction_arrow->SetX2(x_arrend);
                strip_direction_arrow->SetY2(y_arrend);
            }
            else {
                strip_direction_arrow->SetX1(x_right_border_marker);
                strip_direction_arrow->SetY1(y_right_border_marker);

                //finding arrow ending
                Double_t x_arrend = x_right_border - layer.GetXSize();
                Double_t y_arrend = y_right_border;
                if( std::fabs(Modules[im]->GetModuleRotationAngleDeg()) >= std::numeric_limits<double>::epsilon() ) {
                    Double_t x_p = Modules[im]->ConvertLocalToRotatedX(x_arrend, y_arrend);
                    Double_t y_p = Modules[im]->ConvertLocalToRotatedY(x_arrend, y_arrend);
                    x_arrend = x_p;
                    y_arrend = y_p;
                }
                strip_direction_arrow->SetX2(x_arrend);
                strip_direction_arrow->SetY2(y_arrend);
            }
            strip_direction_arrow->SetArrowSize(0.02);
            strip_direction_arrow->SetLineColor(TColor::GetColor("#f4a900"));
            strip_direction_arrow->SetFillColor(TColor::GetColor("#f4a900"));
            strip_direction_arrow->SetLineWidth(1);
            strip_direction_arrow->Draw("|>");
            //------------------------------------------------------------------

            //center line of a layer------------------------------------------------
            Double_t x_center_layer = (x_min_layer + x_max_layer)*0.5;
            Double_t y_center_layer = (y_min_layer + y_max_layer)*0.5;

            Double_t x1_center_vline =  x_center_layer;
            Double_t y1_center_vline =  y_max_layer;
            Double_t x2_center_vline =  x_center_layer;
            Double_t y2_center_vline =  y_min_layer;

            Double_t x1_center_hline =  x_min_layer;
            Double_t y1_center_hline =  y_center_layer;
            Double_t x2_center_hline =  x_max_layer;
            Double_t y2_center_hline =  y_center_layer;

            if( std::fabs(Modules[im]->GetModuleRotationAngleDeg()) >= std::numeric_limits<double>::epsilon() ) {
                Double_t x1_vline_p = Modules[im]->ConvertLocalToRotatedX(x_center_layer, y_max_layer);
                Double_t y1_vline_p = Modules[im]->ConvertLocalToRotatedY(x_center_layer, y_max_layer);
                Double_t x2_vline_p = Modules[im]->ConvertLocalToRotatedX(x_center_layer, y_min_layer);
                Double_t y2_vline_p = Modules[im]->ConvertLocalToRotatedY(x_center_layer, y_min_layer);
                Double_t x1_hline_p = Modules[im]->ConvertLocalToRotatedX(x1_center_hline, y1_center_hline);
                Double_t y1_hline_p = Modules[im]->ConvertLocalToRotatedY(x1_center_hline, y1_center_hline);
                Double_t x2_hline_p = Modules[im]->ConvertLocalToRotatedX(x2_center_hline, y2_center_hline);
                Double_t y2_hline_p = Modules[im]->ConvertLocalToRotatedY(x2_center_hline, y2_center_hline);
                x1_center_vline =  x1_vline_p;
                y1_center_vline =  y1_vline_p;
                x2_center_vline =  x2_vline_p;
                y2_center_vline =  y2_vline_p;
                x1_center_hline =  x1_hline_p;
                y1_center_hline =  y1_hline_p;
                x2_center_hline =  x2_hline_p;
                y2_center_hline =  y2_hline_p;
            }

            TLine *vert_center_layer_line = new TLine(x1_center_vline, y1_center_vline, x2_center_vline, y2_center_vline);
            TLine *horiz_center_layer_line = new TLine(x1_center_hline, y1_center_hline, x2_center_hline, y2_center_hline);
            vert_center_layer_line->SetLineColor(TColor::GetColor("#00ccff"));
            horiz_center_layer_line->SetLineColor(TColor::GetColor("#00ccff"));

            layer_canv->cd();
            vert_center_layer_line->Draw();
            horiz_center_layer_line->Draw();
            //------------------------------------------------------------------

            //strip angle slope of a layer -------------------------------------
            strip_angle_deg = layer.GetAngleDeg();
            Double_t strip_angle_rad = strip_angle_deg*TMath::Pi()/180;
            Double_t vert_leg = fabs(y_center_layer - y_max_layer);
            Double_t hor_leg = vert_leg*Tan(strip_angle_rad);
            Double_t x_edge_layer = x_center_layer + hor_leg;
            Double_t y_edge_layer = y_max_layer;

            Double_t x1_slope_line =  x_center_layer;
            Double_t y1_slope_line =  y_center_layer;
            Double_t x2_slope_line =  x_edge_layer;
            Double_t y2_slope_line =  y_edge_layer;

            if( std::fabs(Modules[im]->GetModuleRotationAngleDeg()) >= std::numeric_limits<double>::epsilon() ) {
                Double_t x1_line_p = Modules[im]->ConvertLocalToRotatedX(x1_slope_line, y1_slope_line);
                Double_t y1_line_p = Modules[im]->ConvertLocalToRotatedY(x1_slope_line, y1_slope_line);
                Double_t x2_line_p = Modules[im]->ConvertLocalToRotatedX(x2_slope_line, y2_slope_line);
                Double_t y2_line_p = Modules[im]->ConvertLocalToRotatedY(x2_slope_line, y2_slope_line);

                x1_slope_line =  x1_line_p;
                y1_slope_line =  y1_line_p;
                x2_slope_line =  x2_line_p;
                y2_slope_line =  y2_line_p;
            }

            TLine *strip_slope_layer_line = new TLine(x1_slope_line, y1_slope_line, x2_slope_line, y2_slope_line);
            strip_slope_layer_line->SetLineColor(TColor::GetColor("#00ff33"));

            layer_canv->cd();
            strip_slope_layer_line->Draw();
            //------------------------------------------------------------------

            //Нарисовать точку на слое, по которой мы определяем в какой станции находимся
            //draw a point in the layer ----------------------------------------
            TMarker *mark_point_front = new TMarker[points.size()];
            for(int ip = 0; ip < points.size(); ip++) {
                mark_point_front[ip].SetX(points.at(ip).X());
                mark_point_front[ip].SetY(points.at(ip).Y());
                mark_point_front[ip].SetMarkerStyle(5);
                mark_point_front[ip].SetMarkerSize(1.0);
                mark_point_front[ip].SetMarkerColor(TColor::GetColor("#0000ff"));

                layer_canv->cd();
                mark_point_front[ip].Draw();
            }
            //------------------------------------------------------------------
        }
        //----------------------------------------------------------------------
    }

    //Нарисовать точку на станции, по которой мы определяем в какой станции находимся и какую рисовать
    //draw a point in the station ----------------------------------------------
    TMarker *mark_point_front = new TMarker[points.size()];
    for(int ip = 0; ip < points.size(); ip++) {
        mark_point_front[ip].SetX(points.at(ip).X());
        mark_point_front[ip].SetY(points.at(ip).Y());
        mark_point_front[ip].SetMarkerStyle(5);
        mark_point_front[ip].SetMarkerSize(1.0);
        mark_point_front[ip].SetMarkerColor(TColor::GetColor("#0000ff"));

        station_front_canv->cd();
        mark_point_front[ip].Draw();
    }
    //--------------------------------------------------------------------------

    station_front_canv->cd();
    TPaveText *notice_pt = new TPaveText(0.08, 0.91, 0.93, 0.94, "nbNDC");
    notice_pt->AddText("Press <SHIFT+LeftMouseClick>: to add a new point; <C>: to clear the area");
    notice_pt->SetTextColor(kRed);
    notice_pt->Draw();

    g_current_station = station;

    if(g_EnableAddingPoints) {
        station_front_canv->AddExec("ex", "RecognizeModuleNumFromPointAndDrawIt(station_front_canv)");
        //station_front_canv->AddExec("ex", "RecognizeModuleNumFromPointAndDrawIt()");
    }

}
//------------------------------------------------------------------------------

void RecognizeModuleNumFromPointAndDrawIt(TCanvas *station_front_canv) {
//void RecognizeModuleNumFromPointAndDrawIt() {

    Int_t event = gPad->GetEvent();
    Int_t eventX = gPad->GetEventX();
    Int_t eventY = gPad->GetEventY();
    TObject *object = gPad->GetSelected();

    //if(event == kButtonDown) {
    if(event == kButton1Shift) { //mouse click with shift button
        Double_t x = gPad->AbsPixeltoX(eventX);
        Double_t y = gPad->AbsPixeltoY(eventY);

        //which module does the current point belong to?
        Int_t module_num = -1;
        Double_t z_current_pos = 0.0;
        for(Int_t imod = 0; imod < g_current_station->GetNModules(); ++imod) {
            BmnSiliconModule *module = g_current_station->GetModule(imod);
            Double_t module_zpos = module->GetZStartModulePosition();
            module_num = g_current_station->GetPointModuleOwnership(x, y, module_zpos);
            z_current_pos = module_zpos;
            if(module_num != -1) break;
        }

        cout << "\n-------------------------------------------------------------\n";

        if(module_num != -1) {


            cout << " point ( " << x << " : " << y << " ) is in:\n";
            cout << " module = " << module_num << "\n";

            BmnSiliconModule *module = g_current_station->GetModule(module_num);
            Int_t NLayers = module->GetNStripLayers();

            for(Int_t ilayer = 0; ilayer < NLayers; ++ilayer) {

                BmnSiliconLayer layer = module->GetStripLayer(ilayer);

                Double_t xloc = module->ConvertRotatedToLocalX(x,y);
                Double_t yloc = module->ConvertRotatedToLocalY(x,y);

                if( layer.IsPointInsideStripLayer(xloc, yloc) ) {
                    cout << "      layer[" << ilayer << "] : ";
                    if(layer.GetType() == LowerStripLayer) cout << "LowerStripLayer";
                    if(layer.GetType() == UpperStripLayer) cout << "UpperStripLayer";
                    cout << ", angle = " << layer.GetAngleDeg();
                    cout << ", pitch = " << layer.GetPitch();
                    cout << ", nstrips = " << layer.GetNStrips();
                    cout << ", sizes = (" << layer.GetXSize() << " x " << layer.GetYSize() << ")";
                    cout << ", strip_pos = " << layer.ConvertPointToStripPosition(xloc, yloc);
                    cout << "\n";

                }
            }

        }
        else {
            cout << " point ( " << x << " : " << y << " ) is out of any modules!!!\n";
        }

        TMarker *point_marker = new TMarker(x, y, 2);
        point_marker->SetMarkerSize(1.0);
        point_marker->SetMarkerColor(TColor::GetColor("#ff0000"));

        g_points_marker.push_back(point_marker);

        station_front_canv->cd();
        g_points_marker[g_points_marker.size()-1]->Draw();

        //other canvas drawing
        if(g_DrawOnSeparateLayers) {
            TSeqCollection *collect = gROOT->GetListOfCanvases();
            Int_t nentries = collect->GetEntries();
            for(Int_t ie = 0; ie < nentries; ++ie) {
                TObject *obj = collect->At(ie);
                if(TString(obj->GetName()).Contains("layer_canv_")) {
                    ((TCanvas*)obj)->cd();
                    point_marker->Draw();
                    ((TCanvas*)obj)->Update();
                }
            }
        }

        if(module_num != -1) {

            g_current_station->AddPointToStation(x, y, z_current_pos,   0.0, 0.0, 1.0,    1.0, 0);
            g_current_station->ProcessPointsInStation();

            if(g_hits_marker)  {
                delete [] g_hits_marker;

                g_hits_marker = 0;
                g_n_hits_marker = 0;
            }

            Int_t NModules = g_current_station->GetNModules();

            if(g_ShowIntersectionPointInfo) cout << "\n  Number of intersection points:\n";

            for(Int_t im = 0; im < NModules; ++im) {
                BmnSiliconModule *module = g_current_station->GetModule(im);
                Int_t NHits = module->GetNIntersectionPoints();
                g_n_hits_marker += NHits;

                if(g_ShowIntersectionPointInfo) cout << "   in module[" << im << "] : " << NHits << "\n";
            }

            if(g_ShowIntersectionPointInfo) cout << "   in all modules : " << g_n_hits_marker << "\n\n";


            if(g_n_hits_marker == 0) return;

            g_hits_marker = new TMarker[g_n_hits_marker];
            Int_t hit_marker_cnt = 0;

            if(g_ShowIntersectionPointInfo) cout << "  Intersection points:\n";

            for(Int_t im = 0; im < NModules; ++im) {

                if(g_ShowIntersectionPointInfo) cout << "   module[" << im << "]:\n";

                BmnSiliconModule *module = g_current_station->GetModule(im);

                Int_t NHits = module->GetNIntersectionPoints();

                for(Int_t ihit = 0; ihit < NHits; ++ihit) {

                    Double_t x_hit = module->GetIntersectionPointX(ihit);
                    Double_t y_hit = module->GetIntersectionPointY(ihit);
                    Double_t z_hit = module->GetZPositionRegistered();

                    if(g_ShowIntersectionPointInfo) cout << "     hit[" << ihit << "] = ( " << x_hit << " : " << y_hit << " : " << z_hit << " ) " << "\n";

                    //which layers?
                    Int_t NLayers = module->GetNStripLayers();
                    for(Int_t ilayer = 0; ilayer < NLayers; ++ilayer) {
                        BmnSiliconLayer layer = module->GetStripLayer(ilayer);
                        if( layer.IsPointInsideStripLayer(x_hit,y_hit) ) {
                            if(g_ShowIntersectionPointInfo) cout << "      layer[" << ilayer << "] : ";
                            if(layer.GetType() == LowerStripLayer) cout << "LowerStripLayer";
                            if(layer.GetType() == UpperStripLayer) cout << "UpperStripLayer";
                            cout << ", angle = " << layer.GetAngleDeg();
                            cout << ", pitch = " << layer.GetPitch();
                            cout << ", nstrips = " << layer.GetNStrips();
                            cout << ", sizes = (" << layer.GetXSize() << " x " << layer.GetYSize() << ")";
                            if(g_ShowIntersectionPointInfo) cout << "\n";
                        }
                    }

                    if(g_ShowIntersectionPointInfo) cout << "\n";

                    g_hits_marker[hit_marker_cnt].SetX(x_hit);
                    g_hits_marker[hit_marker_cnt].SetY(y_hit);
                    g_hits_marker[hit_marker_cnt].SetMarkerSize(1.0);
                    g_hits_marker[hit_marker_cnt].SetMarkerStyle(5);
                    g_hits_marker[hit_marker_cnt].SetMarkerColor(TColor::GetColor("#009900"));

                    station_front_canv->cd();
                    g_hits_marker[hit_marker_cnt].Draw();


                    //other canvas drawing
                    if(g_DrawOnSeparateLayers) {
                        TSeqCollection *collect = gROOT->GetListOfCanvases();
                        Int_t nentries = collect->GetEntries();
                        for(Int_t ie = 0; ie < nentries; ++ie) {
                            TObject *obj = collect->At(ie);
                            if(TString(obj->GetName()).Contains("layer_canv_")) {

                                Int_t NLayers = module->GetNStripLayers();

                                for(Int_t ilayer = 0; ilayer < NLayers; ++ilayer) {

                                    BmnSiliconLayer layer = module->GetStripLayer(ilayer);

                                    if( layer.IsPointInsideStripLayer(x_hit,y_hit) ) {
                                        TString laymod = "";
                                        laymod += ilayer;
                                        laymod += im;

                                        if(TString(obj->GetName()).Contains(laymod)) {
                                            ((TCanvas*)obj)->cd();
                                            g_hits_marker[hit_marker_cnt].Draw();
                                            ((TCanvas*)obj)->Update();
                                        }
                                    }
                                }
                            }
                        }
                    }

                    hit_marker_cnt++;
                }
                cout << "\n";
            }

        }
        cout << "-------------------------------------------------------------\n";
    }

    if(event == kKeyPress) {
        cout << "key button was pressed\n";
        cout << "px = " << eventX << "\n";
        cout << "py = " << (char)eventY << "\n";

        if(eventX == (int)'c') {

            g_current_station->Reset();

            if(g_hits_marker != 0)  {
                delete [] g_hits_marker;
                g_hits_marker = 0;
                g_n_hits_marker = 0;
            }

            for(Int_t i = 0; i < g_points_marker.size(); ++i) {
                g_points_marker[i]->Delete();
            }
            g_points_marker.clear();

            station_front_canv->Update();

            //other canvas drawing
            if(g_DrawOnSeparateLayers) {
                TSeqCollection *collect = gROOT->GetListOfCanvases();
                Int_t nentries = collect->GetEntries();
                for(Int_t ie = 0; ie < nentries; ++ie) {
                    TObject *obj = collect->At(ie);
                    if(TString(obj->GetName()).Contains("layer_canv_")) {
                        ((TCanvas*)obj)->Update();
                    }
                }
            }
        }

    }

    /*if(object) {
        if(object->InheritsFrom("TMarker")) {
            TMarker *marker = (TMarker*)object;
            //cout << "marker (x,y) = ( " << marker->GetX() << " : " << marker->GetY() << " )\n";
        }
    }*/
}


void DrawCreatedStationTop(BmnSiliconStation *station, vector<TLorentzVector> &points) {

    TString description_text = "";
        description_text += "TOP: station "; description_text += station->GetStationNumber();
        description_text += ", xpos = "; description_text += station->GetXPosition();
        description_text += ", ypos = "; description_text += station->GetYPosition();
        description_text += ", zpos = "; description_text += station->GetZPosition();
    TCanvas *station_top_canv = new TCanvas("station_top_canv", description_text, 10, 10, 1000, 1000);
    station_top_canv->SetGrid();

    Int_t StationNumber = station->GetStationNumber();
    Double_t XSize = station->GetXSize();
    Double_t YSize = station->GetYSize();
    Double_t ZSize = station->GetZSize();
    Double_t XMinStation = station->GetXMinStation();
    Double_t XMaxStation = station->GetXMaxStation();
    Double_t YMinStation = station->GetYMinStation();
    Double_t YMaxStation = station->GetYMaxStation();
    Double_t ZMinStation = station->GetZMinStation();
    Double_t ZMaxStation = station->GetZMaxStation();
    Double_t XPosition = station->GetXPosition();
    Double_t YPosition = station->GetYPosition();
    Double_t ZPosition = station->GetZPosition();
    Int_t NModules = station->GetNModules();

    Double_t *XShiftsOfModules = new Double_t[NModules];
    Double_t *YShiftsOfModules = new Double_t[NModules];
    Double_t *ZShiftsOfModules = new Double_t[NModules];
    Double_t maxXShift = 0.0;
    Double_t maxYShift = 0.0;
    Double_t maxZShift = 0.0;
    for(Int_t iModule = 0; iModule < NModules; ++iModule) {
        XShiftsOfModules[iModule] = station->GetXShiftOfModule(iModule);
        YShiftsOfModules[iModule] = station->GetYShiftOfModule(iModule);
        ZShiftsOfModules[iModule] = station->GetZShiftOfModule(iModule);

        if( fabs(XShiftsOfModules[iModule]) > maxXShift ) maxXShift = fabs(XShiftsOfModules[iModule]);
        if( fabs(YShiftsOfModules[iModule]) > maxYShift ) maxYShift = fabs(YShiftsOfModules[iModule]);
        if( fabs(ZShiftsOfModules[iModule]) > maxZShift ) maxZShift = fabs(ZShiftsOfModules[iModule]);
    }

    BmnSiliconModule **Modules = new BmnSiliconModule*[NModules];
    for(Int_t imod = 0; imod < NModules; imod++) {
        Modules[imod] = station->GetModule(imod);
    }

    //Double_t xrange_size = XSize + fabs(XPosition) + maxXShift;
    //    xrange_size *= 0.5 + 0.1;

    Double_t xmin = XMinStation - XSize*0.3;
    Double_t xmax = XMaxStation + XSize*0.3;
    Double_t zmin = ZMinStation - ZSize*5.0;
    Double_t zmax = ZMaxStation + ZSize*5.0;

    //TH2F *station_top_2dhist = new TH2F("station_top_2dhist", description_text, 100, -xrange_size, xrange_size, 100, ZPosition-xrange_size*0.5, ZPosition+xrange_size*0.5);
    TH2F *station_top_2dhist = new TH2F("station_top_2dhist", description_text, 100, xmin, xmax, 100, zmin, zmax);
    station_top_2dhist->GetXaxis()->SetTitle("+x (classical)");
    station_top_2dhist->GetYaxis()->SetTitle("+z");
    station_top_2dhist->SetStats(0);
    station_top_2dhist->Draw();

    TBox **modules_box = new TBox*[NModules];

    TPolyLine **bordersOfModules = new TPolyLine*[NModules];

    for(Int_t im = 0; im < NModules; im++) {

        //module borders -------------------------------------------------------
        Double_t x_min_module = Modules[im]->GetXMinModule();
        Double_t x_max_module = Modules[im]->GetXMaxModule();
        Double_t y_min_module = Modules[im]->GetYMinModule();
        Double_t y_max_module = Modules[im]->GetYMaxModule();
        Double_t z_min_module = Modules[im]->GetZStartModulePosition();
        Double_t z_max_module = Modules[im]->GetZStartModulePosition() + Modules[im]->GetModuleThickness();

        Double_t x_module_border_points[5] = {x_min_module, x_min_module, x_max_module, x_max_module, x_min_module};
        Double_t y_module_border_points[5] = {y_min_module, y_max_module, y_max_module, y_min_module, y_min_module};

        //if we have the rotation of the station
        if( std::fabs(Modules[im]->GetModuleRotationAngleDeg()) >= std::numeric_limits<double>::epsilon() ) {
            Double_t x_p0 = Modules[im]->ConvertLocalToRotatedX(x_min_module, y_min_module);
            Double_t x_p1 = Modules[im]->ConvertLocalToRotatedX(x_min_module, y_max_module);
            Double_t x_p2 = Modules[im]->ConvertLocalToRotatedX(x_max_module, y_max_module);
            Double_t x_p3 = Modules[im]->ConvertLocalToRotatedX(x_max_module, y_min_module);
            x_module_border_points[0] = x_p0;
            x_module_border_points[1] = x_p1;
            x_module_border_points[2] = x_p2;
            x_module_border_points[3] = x_p3;
            x_module_border_points[4] = x_p0;
        }

        TPolyLine *frontBorders = new TPolyLine(5);
        frontBorders->SetPoint(0, x_module_border_points[0], z_min_module);
        for(Int_t ip = 1; ip < 4; ip++) frontBorders->SetNextPoint(x_module_border_points[ip], z_min_module);
        frontBorders->SetNextPoint(x_module_border_points[0], z_min_module);
        frontBorders->SetLineStyle(1);
        frontBorders->SetLineWidth(1);
        frontBorders->SetFillStyle(0); // 0 - not filled, 1001 - solid

        TPolyLine *backBorders  = new TPolyLine(5);;
        backBorders->SetPoint(0, x_module_border_points[0], z_max_module);
        for(Int_t ip = 1; ip < 4; ip++) backBorders->SetNextPoint(x_module_border_points[ip], z_max_module);
        backBorders->SetNextPoint(x_module_border_points[0], z_max_module);
        backBorders->SetLineStyle(1);
        backBorders->SetLineWidth(1);
        backBorders->SetFillStyle(0); // 0 - not filled, 1001 - solid

        station_top_canv->cd();
        frontBorders->Draw("l");
        backBorders->Draw("l");
        //----------------------------------------------------------------------
    }

    TMarker *mark_point_top = new TMarker[points.size()];
    for(int ip = 0; ip < points.size(); ip++) {
        mark_point_top[ip].SetX(points.at(ip).X());
        mark_point_top[ip].SetY(points.at(ip).Z());
        mark_point_top[ip].SetMarkerStyle(5);
        mark_point_top[ip].SetMarkerSize(1.0);
        mark_point_top[ip].SetMarkerColor(TColor::GetColor("#0000ff"));
        mark_point_top[ip].Draw();
    }
}