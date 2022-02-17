/*
 * This macro for testing configurations of the Forward Silicon detector
 * which have some module transformations in an XML-parameter file.
 *
 * It allows as to analyze MC-points and Hits and find their ownership to
 * modules in various visual projections.
 *
 * Author: Baranov D.
 * Created: 27.01.2022
 * Updated: 09.02.2022 (Transforms added)
 */

#include "Rtypes.h"

R__ADD_INCLUDE_PATH($VMCWORKDIR)

using namespace ROOT::Math;
using namespace std;

//Global variables
TCanvas *planeXY_canv;
TCanvas *planeZX_canv;
TCanvas *planeZY_canv;
TCanvas *XYZview_canv;

BmnSiliconStationSet *gStationSet;
BmnSiliconTransform *gTransfClass;
vector<vector<Bool_t> > gConfig_visual; // [station][module] = bool (active/inactive)


//function declaration
void DrawPointsAndLines(vector<Plane3D::Point> points, TString hexcolor, Int_t marker_style, Double_t marker_size, Double_t line_size, Bool_t isPointDraw, Bool_t isLinesDraw, Bool_t isCloseLine);
void DrawPointsAndLines3D(vector<Plane3D::Point> points, TString hexcolor, Int_t marker_style, Double_t marker_size, Double_t line_size, Bool_t isPointDraw, Bool_t isLinesDraw, Bool_t isCloseLine);
void DrawBranchPoints(TTree *points_tree, TClonesArray *points_array, Int_t first_event, Int_t last_event);
void DrawBranchHits(TTree *points_tree, TClonesArray *points_array, Int_t first_event, Int_t last_event);


void TestSiliconClassTransform() {

    TString file_prefix = "";
    //file_prefix = "_CC_4gev_b0_16fm";

    // Input MC-points for Silicon
    TClonesArray* fSiliconMCPoints_Array = new TClonesArray("");
    TString fSiliconMCPoints_FileName = "$VMCWORKDIR/macro/run/bmnsim";
        fSiliconMCPoints_FileName += file_prefix;
        fSiliconMCPoints_FileName += ".root";
    TFile *fSiliconMCPoints_File = new TFile(fSiliconMCPoints_FileName, "READ");
    TTree *fSiliconMCPoints_Tree = (TTree*) fSiliconMCPoints_File->Get("bmndata");
    fSiliconMCPoints_Tree->GetBranch("SiliconPoint")->SetAutoDelete(kTRUE);
    fSiliconMCPoints_Tree->SetBranchAddress("SiliconPoint", &fSiliconMCPoints_Array);
    Int_t NEvents_fSiliconMCPoints = fSiliconMCPoints_Tree->GetEntries();


    // Input hits for Silicon
    TClonesArray* fSiliconHits_Array = new TClonesArray("");
    TString fSiliconHits_FileName = "$VMCWORKDIR/macro/run/bmndst";
        fSiliconHits_FileName += file_prefix;
        fSiliconHits_FileName += ".root";
    TFile *fSiliconHits_File = new TFile(fSiliconHits_FileName, "READ");
    TTree *fSiliconHits_Tree = (TTree*) fSiliconHits_File->Get("bmndata");
    fSiliconHits_Tree->GetBranch("BmnSiliconHit")->SetAutoDelete(kTRUE);
    fSiliconHits_Tree->SetBranchAddress("BmnSiliconHit", &fSiliconHits_Array);
    Int_t NEvents_fSiliconHits = fSiliconHits_Tree->GetEntries();

//------------------------------------------------------------------------------

    Double_t scale_coeff = 1;
    Double_t xmin_view = -40.0*scale_coeff;
    Double_t xmax_view = +40.0*scale_coeff;
    Double_t ymin_view = -40.0*scale_coeff;
    Double_t ymax_view = +40.0*scale_coeff;
    Double_t zmin_view = -10.0*scale_coeff; //was -100
    Double_t zmax_view = +100.0*scale_coeff;

    //XY plane
    planeXY_canv = new TCanvas("planeXY_canv", "planeXY_canv", 10, 10, 900, 900);
    planeXY_canv->SetGrid();
    TH2D *planeXY_h2 = new TH2D("planeXY_h2", "planeXY_h2 (to the target(back))", 10, xmin_view, xmax_view,    10, ymin_view, ymax_view);
    planeXY_h2->GetXaxis()->SetTitle("X(global)");
    planeXY_h2->GetYaxis()->SetTitle("Y(global)");
    planeXY_h2->SetStats(0);

    //ZX plane
    planeZX_canv = new TCanvas("planeZX_canv", "planeZX_canv", 1000, 10, 900, 900);
    planeZX_canv->SetGrid();
    TH2D *planeZX_h2 = new TH2D("planeZX_h2", "planeZX_h2(top)", 10, zmin_view, zmax_view,    10, xmin_view, xmax_view);
    planeZX_h2->GetXaxis()->SetTitle("Z(global)");
    planeZX_h2->GetYaxis()->SetTitle("X(global)");
    planeZX_h2->SetStats(0);

    //ZY plane
    planeZY_canv = new TCanvas("planeZY_canv", "planeZY_canv", 1800, 10, 900, 900);
    planeZY_canv->SetGrid();
    TH2D *planeZY_h2 = new TH2D("planeZY_h2", "planeZY_h2(side)", 10, zmin_view, zmax_view,    10, ymin_view, ymax_view);
    planeZY_h2->GetXaxis()->SetTitle("Z(global)");
    planeZY_h2->GetYaxis()->SetTitle("Y(global)");
    planeZY_h2->SetStats(0);

    //3D view (y and z - swapped)
    XYZview_canv = new TCanvas("XYZview_canv", "XYZview_canv", 2900, 10, 900, 900);
    //XYZview_canv->SetGrid();
    TH3D *XYZview_h3 = new TH3D("XYZview_h3", "XYZview_h3", 10, zmin_view, zmax_view,    10, xmin_view, xmax_view,      10, ymin_view, ymax_view);
    XYZview_h3->GetXaxis()->SetTitle("Z");
    XYZview_h3->GetYaxis()->SetTitle("X");
    XYZview_h3->GetZaxis()->SetTitle("Y");
    XYZview_h3->SetStats(0);

//------------------------------------------------------------------------------


    TString gPathDetectorConfig = gSystem->Getenv("VMCWORKDIR");
    gPathDetectorConfig += "/parameters/silicon/XMLConfigs/";
    //gPathDetectorConfig += "SiliconRun8_mods_6_10_14_18.xml";
    gPathDetectorConfig += "SiliconRun8_3stations.xml";
    //gPathDetectorConfig += "SiliconRun8_mods_10_14rot_18.xml";
    //gPathDetectorConfig += "SiliconRun8_mods_6_10_14_18.xml";


    //Current configuration
    gStationSet = new BmnSiliconStationSet(gPathDetectorConfig);

    //Current transform configuration
    gTransfClass = new BmnSiliconTransform();
    gTransfClass->LoadFromXMLFile(gPathDetectorConfig);


    //Selecting modules to visualize -------------------------------------------
    gConfig_visual.resize(gStationSet->GetNStations());  //quantity of stations
    for(Int_t istation = 0; istation < gConfig_visual.size(); ++istation) {
        gConfig_visual[istation].resize(gStationSet->GetStation(istation)->GetNModules(), false); //default: inactive
    }

    for(Int_t istation = 0; istation < gStationSet->GetNStations(); ++istation) {
        for(Int_t imodule = 0; imodule < gStationSet->GetStation(istation)->GetNModules(); ++imodule) {
            gConfig_visual[istation][imodule] = true;
        }
    }

    //gConfig_visual[0][0] = true;
    //gConfig_visual[0][1] = true;
    //gConfig_visual[0][2] = true;
    //gConfig_visual[0][3] = true;
    //gConfig_visual[1][0] = true;
    //gConfig_visual[2][0] = true;
    //gConfig_visual[3][0] = true;
    //gConfig_visual[4][0] = true;
    //gConfig_visual[4][1] = true;
    //gConfig_visual[5][0] = true;
    //gConfig_visual[5][1] = true;
    //gConfig_visual[6][0] = true;
    //gConfig_visual[6][1] = true;
    //gConfig_visual[7][0] = true;
    //gConfig_visual[7][1] = true;
    //gConfig_visual[8][0] = true;
    //gConfig_visual[8][1] = true;
    //gConfig_visual[9][0] = true;
    //gConfig_visual[9][1] = true;

    //Printing the info about the visual status of modules
    if(1) {
        cout << "Info about the visual status of modules:\n";
        for(Int_t istation = 0; istation < gConfig_visual.size(); ++istation) {
            for(Int_t imodule = 0; imodule < gConfig_visual[istation].size(); ++imodule) {
                cout << "det[" << istation << "][" << imodule << "] = " << gConfig_visual[istation][imodule] << "\n";
            }
        }
        cout << "\n";
    }
    //--------------------------------------------------------------------------

    planeXY_canv->cd();
    planeXY_h2->Draw();

    planeZX_canv->cd();
    planeZX_h2->Draw();

    planeZY_canv->cd();
    planeZY_h2->Draw();

    XYZview_canv->cd();
    XYZview_h3->Draw();


    for(Int_t istation = 0; istation < gStationSet->GetNStations(); ++istation) {

        for(Int_t imodule = 0; imodule < gStationSet->GetStation(istation)->GetNModules(); ++imodule) {

            Bool_t visual_status = gConfig_visual[istation][imodule];

            if(visual_status == true) {

                BmnSiliconModule *module = gStationSet->GetStation(istation)->GetModule(imodule);

                //parameters of the current module in local coordinates
                Double_t xmin_module = module->GetXMinModule();
                Double_t xmax_module = module->GetXMaxModule();
                Double_t ymin_module = module->GetYMinModule();
                Double_t ymax_module = module->GetYMaxModule();
                Double_t z_start_pos_module = gStationSet->GetStation(istation)->GetModule(imodule)->GetZStartModulePosition();
                Double_t z_reg_module = gStationSet->GetStation(istation)->GetModule(imodule)->GetZPositionRegistered();

                //print parameters of the module
                if(1) {
                    cout << "  station[" << istation << "], module[" << imodule << "]:\n";
                    cout << "    xloc_module(min:max) = ( " << xmin_module << " : " << xmax_module << ")\n";
                    cout << "    yloc_module(min:max) = ( " << ymin_module << " : " << ymax_module << ")\n";
                    cout << "    zloc_start_pos_module = " << z_start_pos_module << "\n";
                    cout << "    zloc_reg_module = " << z_reg_module << "\n";
                    cout << "\n";
                }

                vector<Plane3D::Point> module_borders;
                vector<Plane3D::Point> tmodule_borders; //transformed
                vector<Plane3D::Point> rtmodule_borders; //inverse transformed

                if( std::fabs(module->GetModuleRotationAngleDeg()) < std::numeric_limits<double>::epsilon() ) {
                    module_borders.push_back(Plane3D::Point(-xmin_module, ymin_module, z_reg_module)); //x converted to global
                    module_borders.push_back(Plane3D::Point(-xmin_module, ymax_module, z_reg_module)); //x converted to global
                    module_borders.push_back(Plane3D::Point(-xmax_module, ymax_module, z_reg_module)); //x converted to global
                    module_borders.push_back(Plane3D::Point(-xmax_module, ymin_module, z_reg_module)); //x converted to global
                }
                else {
                    Double_t x_p0 = module->ConvertLocalToRotatedX(xmin_module, ymin_module);
                    Double_t y_p0 = module->ConvertLocalToRotatedY(xmin_module, ymin_module);

                    Double_t x_p1 = module->ConvertLocalToRotatedX(xmin_module, ymax_module);
                    Double_t y_p1 = module->ConvertLocalToRotatedY(xmin_module, ymax_module);

                    Double_t x_p2 = module->ConvertLocalToRotatedX(xmax_module, ymax_module);
                    Double_t y_p2 = module->ConvertLocalToRotatedY(xmax_module, ymax_module);

                    Double_t x_p3 = module->ConvertLocalToRotatedX(xmax_module, ymin_module);
                    Double_t y_p3 = module->ConvertLocalToRotatedY(xmax_module, ymin_module);

                    module_borders.push_back(Plane3D::Point(-x_p0, y_p0, z_reg_module)); //x converted to global
                    module_borders.push_back(Plane3D::Point(-x_p1, y_p1, z_reg_module)); //x converted to global
                    module_borders.push_back(Plane3D::Point(-x_p2, y_p2, z_reg_module)); //x converted to global
                    module_borders.push_back(Plane3D::Point(-x_p3, y_p3, z_reg_module)); //x converted to global
                }

                for(Int_t ipoint = 0; ipoint < module_borders.size(); ++ipoint) {
                    tmodule_borders.push_back(gTransfClass->ApplyTransforms(module_borders[ipoint], istation, imodule));
                    //tmodule_borders.push_back(module_borders[ipoint]);
                }


                //DrawPointsAndLines(module_borders, "#cc0000", 20, 1.0, 1.0, false, true, true);
                DrawPointsAndLines(tmodule_borders, "#00cc00", 20, 1.0, 1.0, false, true, true);
                //DrawPointsAndLines(rtmodule_borders, "#0000cc", 20, 1.0, 1.0, true, true, true);


                //DrawPointsAndLines3D(module_borders, "#cc0000", 20, 0.5, 1.0, false, true, true);
                DrawPointsAndLines3D(tmodule_borders, "#00cc00", 20, 0.5, 1.0, false, true, true);
                //DrawPointsAndLines3D(rtmodule_borders, "#0000cc", 20, 0.5, 1.0, true, true, true);

            }
        }
    }
;
    //DrawBranchPoints(fSiliconMCPoints_Tree, fSiliconMCPoints_Array, 0, NEvents_fSiliconMCPoints);
    //DrawBranchHits(fSiliconHits_Tree, fSiliconHits_Array, 0, NEvents_fSiliconMCPoints);

    DrawBranchPoints(fSiliconMCPoints_Tree, fSiliconMCPoints_Array, 0, 1);
    DrawBranchHits(fSiliconHits_Tree, fSiliconHits_Array, 0, 1);

    //delete gTransfClass;
    delete gStationSet;

    cout << "It's the end" << "\n";
    //gApplication->Terminate();
}
//------------------------------------------------------------------------------

void DrawPointsAndLines(vector<Plane3D::Point> points, TString hexcolor, Int_t marker_style, Double_t marker_size, Double_t line_size, Bool_t isPointDraw, Bool_t isLinesDraw, Bool_t isCloseLine) {
    //Drawing the points as the markers ----------------------------------------
    for(Int_t ipoint = 0; ipoint < points.size(); ++ipoint) {

        //XY-plane -------------------------------------------------------------
        //Markers on the XY plane
        {
            TMarker *marker = new TMarker(points[ipoint].X(), points[ipoint].Y(), marker_style);
            marker->SetMarkerSize(marker_size);
            marker->SetMarkerColor(TColor::GetColor(hexcolor));
            planeXY_canv->cd();
            if(isPointDraw) marker->Draw();

            //Lines between points on XY plane
            Double_t x1, y1, x2, y2;
            if(ipoint != points.size()-1) {
                x1 = points[ipoint].X();
                y1 = points[ipoint].Y();
                x2 = points[ipoint+1].X();
                y2 = points[ipoint+1].Y();
            }
            else {
                if(isCloseLine) {
                    x1 = points[ipoint].X();
                    y1 = points[ipoint].Y();
                    x2 = points[0].X();
                    y2 = points[0].Y();
                }
            }
            TLine *line = new TLine(x1, y1, x2, y2);
            line->SetLineColor(TColor::GetColor(hexcolor));
            line->SetLineWidth(line_size);
            planeXY_canv->cd();
            if(isLinesDraw) line->Draw();
        }
        //----------------------------------------------------------------------

        //ZX-plane -------------------------------------------------------------
        //Markers on the ZX plane
        {
            TMarker *marker = new TMarker(points[ipoint].Z(), points[ipoint].X(), marker_style);
            marker->SetMarkerSize(marker_size);
            marker->SetMarkerColor(TColor::GetColor(hexcolor));
            planeZX_canv->cd();
            if(isPointDraw) marker->Draw();

            //Lines between points on ZX plane
            Double_t z1, x1, z2, x2;
            if(ipoint != points.size()-1) {
                z1 = points[ipoint].Z();
                x1 = points[ipoint].X();
                z2 = points[ipoint+1].Z();
                x2 = points[ipoint+1].X();
            }
            else {
                if(isCloseLine) {
                    z1 = points[ipoint].Z();
                    x1 = points[ipoint].X();
                    z2 = points[0].Z();
                    x2 = points[0].X();
                }
            }
            TLine *line = new TLine(z1, x1, z2, x2);
            line->SetLineColor(TColor::GetColor(hexcolor));
            line->SetLineWidth(line_size);
            planeZX_canv->cd();
            if(isLinesDraw) line->Draw();
        }
        //----------------------------------------------------------------------

        //ZY-plane -------------------------------------------------------------
        //Markers on the XY plane
        {
            TMarker *marker = new TMarker(points[ipoint].Z(), points[ipoint].Y(), marker_style);
            marker->SetMarkerSize(marker_size);
            marker->SetMarkerColor(TColor::GetColor(hexcolor));
            planeZY_canv->cd();
            if(isPointDraw) marker->Draw();

            //Lines between points on XY plane
            Double_t z1, y1, z2, y2;
            if(ipoint != points.size()-1) {
                z1 = points[ipoint].Z();
                y1 = points[ipoint].Y();
                z2 = points[ipoint+1].Z();
                y2 = points[ipoint+1].Y();
            }
            else {
                if(isCloseLine) {
                    z1 = points[ipoint].Z();
                    y1 = points[ipoint].Y();
                    z2 = points[0].Z();
                    y2 = points[0].Y();
                }
            }
            TLine *line = new TLine(z1, y1, z2, y2);
            line->SetLineColor(TColor::GetColor(hexcolor));
            line->SetLineWidth(line_size);
            planeZY_canv->cd();
            if(isLinesDraw) line->Draw();
        }
        //----------------------------------------------------------------------
    }
}

void DrawPointsAndLines3D(vector<Plane3D::Point> points, TString hexcolor, Int_t marker_style, Double_t marker_size, Double_t line_size, Bool_t isPointDraw, Bool_t isLinesDraw, Bool_t isCloseLine) {

    //system coord XYZ---->ZXY

    //points as the markers ----------------------------------------
    TPolyMarker3D *markers = new TPolyMarker3D(points.size());
    markers->SetMarkerColor(TColor::GetColor(hexcolor));
    markers->SetMarkerStyle(marker_style);
    markers->SetMarkerSize(marker_size);

    //lines between points
    TPolyLine3D *lines = new TPolyLine3D(points.size());
    lines->SetLineColor(TColor::GetColor(hexcolor));
    lines->SetLineWidth(line_size);

    for(Int_t ipoint = 0; ipoint < points.size(); ++ipoint) {

        markers->SetPoint(ipoint, points[ipoint].Z(), points[ipoint].X(), points[ipoint].Y());

        if(ipoint != points.size()-1) {
            lines->SetPoint(ipoint, points[ipoint].Z(), points[ipoint].X(), points[ipoint].Y());
        }
        else {
            lines->SetPoint(ipoint, points[ipoint].Z(), points[ipoint].X(), points[ipoint].Y());
            if(isCloseLine) {
                lines->SetPoint(ipoint+1, points[0].Z(), points[0].X(), points[0].Y());
            }
        }
    }

    XYZview_canv->cd();
    if(isPointDraw) markers->Draw();
    if(isLinesDraw) lines->Draw();
}

void DrawBranchPoints(TTree *points_tree, TClonesArray *points_array, Int_t first_event, Int_t last_event) {

    vector<Plane3D::Point> tracks;
    vector<Plane3D::Point> points;

    for(Int_t ievent = first_event; ievent < last_event; ++ievent) {

        points_tree->GetEntry(ievent);

        Int_t npoints = points_array->GetEntries();

        Int_t trackIdPrev = -1;
        tracks.clear();

        for(Int_t ipoint = 0; ipoint < npoints; ++ipoint) {

            BmnSiliconPoint *mc_point = (BmnSiliconPoint*) points_array->At(ipoint);

            Double_t x = ((FairMCPoint*)mc_point)->GetX();
            Double_t y = ((FairMCPoint*)mc_point)->GetY();
            Double_t z = ((FairMCPoint*)mc_point)->GetZ();

            Double_t px = ((FairMCPoint*)mc_point)->GetPx();
            Double_t py = ((FairMCPoint*)mc_point)->GetPy();
            Double_t pz = ((FairMCPoint*)mc_point)->GetPz();

            Int_t trackId = mc_point->GetTrackID();

            Int_t station_num = mc_point->GetStation();
            Int_t module_num = mc_point->GetModule();

            //cout << "station_num = " << station_num << "\n";
            //cout << "module_num = " << module_num << "\n";

            //needs to determine point ownership using local positions
            Plane3D::Point loc_point(x, y, z);
            Plane3D::Point loc_direct(px+x, py+y, pz+z);
            if(station_num < gStationSet->GetNStations()) {
                if(module_num < gStationSet->GetStation(station_num)->GetNModules()) {
                    loc_point = gTransfClass->ApplyInverseTransforms(loc_point, station_num, module_num);
                    loc_direct = gTransfClass->ApplyInverseTransforms(loc_direct, station_num, module_num);
                }
            }
            Double_t xloc = loc_point.X();
            Double_t yloc = loc_point.Y();
            Double_t zloc = loc_point.Z();

            Double_t pxloc = loc_direct.X() - loc_point.X();
            Double_t pyloc = loc_direct.Y() - loc_point.Y();
            Double_t pzloc = loc_direct.Z() - loc_point.Z();

            if(0) {
                cout << "ipoint = " << ipoint << "\n";
                cout << "  x = " << x << "\n";
                cout << "  y = " << y << "\n";
                cout << "  z = " << z << "\n";
                cout << "  px = " << px << "\n";
                cout << "  py = " << py << "\n";
                cout << "  pz = " << pz << "\n";
                cout << "\n";
                cout << "  xloc = " << xloc << "\n";
                cout << "  yloc = " << yloc << "\n";
                cout << "  zloc = " << zloc << "\n";
                cout << "  pxloc = " << pxloc << "\n";
                cout << "  pyloc = " << pyloc << "\n";
                cout << "  pzloc = " << pzloc << "\n";
                cout << "\n";
            }

            Int_t current_station_num = -1;
            Int_t current_module_num = -1;
            Bool_t visual_status = false;
            current_station_num  = gStationSet->GetPointStationOwnership(-xloc, yloc, zloc);
            //current_station_num  = gStationSet->GetPointStationOwnership(zloc);

            if(current_station_num >= 0) current_module_num = gStationSet->GetStation(current_station_num)->GetPointModuleOwnership(-xloc, yloc, zloc);
            if(current_station_num >= 0 && current_module_num >= 0) visual_status = gConfig_visual[current_station_num][current_module_num];

            TString point_color;
            if(visual_status == true) point_color = "#cc0000";
            else point_color = "#999999";

            if(trackId != trackIdPrev) {
                DrawPointsAndLines(tracks, "#ffbcad", 6, 1.0, 1.0, false, true, false);
                DrawPointsAndLines3D(tracks, "#ffbcad", 6, 1.0, 1.0, false, true, false);
                tracks.clear();
            }
            trackIdPrev = trackId;

            tracks.push_back(Plane3D::Point(x, y, z));
            points.push_back(Plane3D::Point(x, y, z));

            DrawPointsAndLines(points, point_color, 20, 0.4, 1.0, true, false, false);
            DrawPointsAndLines3D(points, point_color, 20, 0.4, 1.0, true, false, false);
            points.clear();

            //drawing directions of mc-points
            vector<Plane3D::Point> direct_points;
            direct_points.push_back(Plane3D::Point(x, y, z));
            direct_points.push_back(Plane3D::Point(x+px*10, y+py*10, z+pz*10)); //10x scaling
            DrawPointsAndLines(direct_points, point_color, 20, 0.4, 0.5, false, true, false);
            DrawPointsAndLines3D(direct_points, point_color, 20, 0.4, 0.5, false, true, false);
            direct_points.clear();

        }
    }
}

void DrawBranchHits(TTree *points_tree, TClonesArray *points_array, Int_t first_event, Int_t last_event) {

    vector<Plane3D::Point> points;

    for(Int_t ievent = first_event; ievent < last_event; ++ievent) {

        points_tree->GetEntry(ievent);

        Int_t npoints = points_array->GetEntries();

        for(Int_t ihit = 0; ihit < npoints; ++ihit) {

            BmnSiliconHit *hit = (BmnSiliconHit*) points_array->At(ihit);

            Double_t x = hit->GetX();
            Double_t y = hit->GetY();
            Double_t z = hit->GetZ();

            Int_t station_num = hit->GetStation();
            Int_t module_num = hit->GetModule();

            //needs to determine hit ownership using local positions
            Plane3D::Point loc_hit(x, y, z);
            if(station_num < gStationSet->GetNStations()) {
                if(module_num < gStationSet->GetStation(station_num)->GetNModules()) {
                    loc_hit = gTransfClass->ApplyInverseTransforms(loc_hit, station_num, module_num);
                }
            }
            Double_t xloc = loc_hit.X();
            Double_t yloc = loc_hit.Y();
            Double_t zloc = loc_hit.Z();

            Int_t current_station_num = -1;
            Int_t current_module_num = -1;
            Bool_t visual_status = false;
            current_station_num  = gStationSet->GetPointStationOwnership(-xloc, yloc, zloc);
            //current_station_num  = gStationSet->GetPointStationOwnership(zloc);

            if(current_station_num >= 0) current_module_num = gStationSet->GetStation(current_station_num)->GetPointModuleOwnership(-xloc, yloc, zloc);
            if(current_station_num >= 0 && current_module_num >= 0) visual_status = gConfig_visual[current_station_num][current_module_num];

            TString point_color;
            if(visual_status == true) point_color = "#0000cc";
            else point_color = "#ccccff";

            points.push_back(Plane3D::Point(x, y, z));

            DrawPointsAndLines(points, point_color, 6, 1.0, 1.0, true, false, false);
            DrawPointsAndLines3D(points, point_color, 6, 1.0, 1.0, true, false, false);
            points.clear();
        }
    }
}
