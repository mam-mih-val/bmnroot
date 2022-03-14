#include <iostream>
#include <fstream>
#include <vector>
#include <TFile.h>
#include <TNtuple.h>
#include <TGraph.h>
#include <TF1.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include "TCutG.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <stdio.h>
//#include "utils.h"
#include "TString.h"
#include "TH1D.h"
#include "TMultiGraph.h"
#include "TStyle.h"
#include "TH2.h"

void drawgrid() //draw borders between cells
{
    double xs = 15; // grid step along X
    double ys = 15; // grid step along Y
    double xmin, ymin, xmax, ymax;
    gPad->GetRangeAxis(xmin, ymin, xmax, ymax);
    auto aline = new TLine();
    aline->SetLineStyle(3);
    for (double yg = ymin + ys; yg < ymax; yg = yg + ys)
        aline->PaintLine(xmin, yg, xmax, yg);
    for (double xg = xmin + xs; xg < xmax; xg = xg + xs)
        aline->PaintLine(xg, ymin, xg, ymax);
}
void drawgrid_center() //same for the central cells
{
    double xs = 7.5; // grid step along X
    double ys = 7.5; // grid step along Y
    double xmin, ymin, xmax, ymax;
    gPad->GetRangeAxis(xmin, ymin, xmax, ymax);
    auto aline = new TLine();
    aline->SetLineStyle(3);
    for (double yg = ymin + ys; yg < ymax; yg = yg + ys)
        aline->PaintLine(xmin, yg, xmax, yg);
    for (double xg = xmin + xs; xg < xmax; xg = xg + xs)
        aline->PaintLine(xg, ymin, xg, ymax);
}

void scan() //main program
{
    //list of cells in blocks
    int A_block[15] = {42, 43, 44,
                       59, 60, 61, 62,
                       77, 78, 79, 80,
                       95, 96, 97, 98};
    int B_block[15] = {45, 46, 47, 48, 49,
                       63, 64, 65, 66, 67,
                       81, 82, 83, 84, 85};
    int C_block[15] = {50, 51, 52, 53, 54,
                       68, 69, 70, 71, 72,
                       86, 87, 88, 89, 90};
    int D_block[15] = {55, 56, 57,
                       73, 74, 75, 76,
                       91, 92, 93, 94,
                       104, 105, 106, 107};
    int I_block[15] = {108, 109, 110, 111,
                       121, 122, 123, 124,
                       139, 140, 141, 142,
                       158, 159, 160};
    int J_block[15] = {125, 126, 127, 128, 129,
                       143, 144, 145, 146, 147,
                       161, 162, 163, 164, 165};
    int K_block[15] = {130, 131, 132, 133, 134,
                       148, 149, 150, 151, 152,
                       166, 167, 168, 169, 170};
    int L_block[15] = {117, 118, 119, 120,
                       135, 136, 137, 138,
                       153, 154, 155, 156,
                       171, 172, 173};
    int E_block[13] = {99, 100, 101,
                       1, 2, 3, 4, 5,
                       11, 12, 13, 14, 15};
    int F_block[12] = {6, 7, 8, 9, 10,
                       16, 17, 18, 19, 20,
                       102, 103};
    int G_block[13] = {112, 113, 114,
                       21, 22, 23, 24, 25,
                       31, 32, 33, 34, 35};
    int H_block[12] = {26, 27, 28, 29, 30,
                       36, 37, 38, 39, 40,
                       115, 116};

    TH2F *cell_v_signal = new TH2F("cell_v_signal", "cell_v_signal", 200, 0, 200, 200, 0, 2000);
    TH2F *ScintWall = new TH2F("ScintWall", "ScintWall", 18, -135, 135, 8, -60, 60);
    TH2F *ScintWall_CellID = new TH2F("ScintWall_CellID", "ScintWall_CellID", 18, -135, 135, 8, -60, 60);
    TH2F *ScintWall_center = new TH2F("ScintWall_center", "ScintWall_center", 10, -45, 30, 4, -15, 15);
    TH2F *ScintWall_center_CellID = new TH2F("ScintWall_center_CellID", "ScintWall_center_CellID", 10, -45, 30, 4, -15, 15);
    TH1F *cell_v_signal1D = new TH1F("cell_v_signal1D", "cell_v_signal1D", 200, 0, 200);
    TH1F *cells[200];
    for (int i = 0; i < 200; i++)
        cells[i] = new TH1F(TString::Format("Cell_%d", i), TString::Format("Cell_%d", i), 500, -2.0, 200);

    TFile *_file0 = TFile::Open("/lhep/users/vvolkov/BMN/src1.root"); //insert .root file path and name here
    TTree *tree = (TTree *)_file0->Get("bmndata");

    TClonesArray *BmnScWallDigiArray = nullptr; //leave nullptr!!!
    tree->SetBranchAddress("ScWallDigi", &BmnScWallDigiArray);
    BmnScWallRaw2Digit *tempScWallMapper = new BmnScWallRaw2Digit();
    tempScWallMapper->ParseConfig("SCWALL_map_dry_run_2022.txt"); //path to the scwall map

    int counter = 0;
    auto ThatVectorX = tempScWallMapper->GetUniqueXpositions();
    auto ThatVectorY = tempScWallMapper->GetUniqueYpositions();
    delete tempScWallMapper;
    int n_events = tree->GetEntries();
    cout << n_events << endl;
    for (int ev = 0; ev < tree->GetEntries(); ev++) ////nt1->GetEntries() //get entries from .root
    {
        ScintWall_CellID->Reset();        //for cell id number in pdf
        ScintWall_center_CellID->Reset(); //for cell id number in pdf
        counter = 0;
        tree->GetEntry(ev);
        for (int i = 0; i < BmnScWallDigiArray->GetEntriesFast(); i++) //cells iterations
        {
            BmnScWallDigi *ThisDigi = (BmnScWallDigi *)BmnScWallDigiArray->At(i);
            float realXpos = ThatVectorX.at(ThisDigi->GetX());
            float realYpos = ThatVectorY.at(ThisDigi->GetY());
            float signal = ThisDigi->GetSignal();  //get what you need here (ZL, Amplitude etc., see the conv.root (input file) for methods or ask Nikolay. You can use fSignal instead GetSignal() for example)
            float cell_id = ThisDigi->GetCellId(); //same
            if (cell_id > 200)                     // we don't have scwall out of this range, so skip
                continue;
            counter++; // for future
            //cout << ev << " cell id " << cell_id << " X " << realXpos / 10 << " Y " << realYpos / 10 << " Signal " << signal << endl; //control output

            if (cell_id <= 40) //central block of the wall
            {
                //cout << ev << "CC cell id " << cell_id << " X " << realXpos / 10 << " Y " << realYpos / 10 << endl;
                ScintWall_center->Fill(realXpos / 10, realYpos / 10, signal / n_events);
                ScintWall_center_CellID->Fill(realXpos / 10, realYpos / 10, cell_id);
                cell_v_signal->Fill(cell_id, signal);
                cell_v_signal1D->Fill(cell_id, signal / n_events);
                cells[(int)cell_id]->Fill(signal);
            }
            else //noncentral
            {
                //cout << ev << " cell id " << cell_id << " X " << realXpos / 10 << " Y " << realYpos / 10 << endl;
                ScintWall->Fill(realXpos / 10, realYpos / 10, signal / n_events); //Ox: signal normilized to 1 event, be careful, as the actual number of events is different for different cells,
                                                                                  //so if you need exact values, you should obtain number of events for EVERY cell and then divide signal on that value with respect to each cell
                ScintWall_CellID->Fill(realXpos / 10, realYpos / 10, cell_id);    //for the cell id numbers on the histogram
                cell_v_signal->Fill(cell_id, signal);                             //TH2F
                cell_v_signal1D->Fill(cell_id, signal / n_events);                //TH1F
                cells[(int)cell_id]->Fill(signal);                                //spectrum
                //if ((int)cell_id == 77) // control output of the cell you need
                //cout << ev << " | " << cell_id << " int " << (int)cell_id << " s " << signal << endl;
            }
        }
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    /////////////////Draw///////////////////
    ////////////////////////////////////////
    ////////////////////////////////////////

    gStyle->SetOptStat(0);
    gStyle->SetPaintTextFormat("4.0f"); //how many numbers in output and how many after dot

    TCanvas *c2 = new TCanvas();
    ScintWall->SetBarOffset(-0.2);
    ScintWall->Draw("colzTEXT L");
    ScintWall_CellID->SetMarkerSize(1.3);
    ScintWall_CellID->SetMarkerColor(kRed);
    ScintWall_CellID->SetBarOffset(0.2);
    ScintWall_CellID->Draw("TEXT SAME");
    ScintWall->GetXaxis()->SetTitle("X [cm]");
    ScintWall->GetYaxis()->SetTitle("Y [cm]");
    auto grid = new TExec("grid", "drawgrid()"); //add borders
    grid->Draw();
    c2->Print("h1.pdf(", "pdf");

    TCanvas *c3 = new TCanvas();
    ScintWall_center->SetBarOffset(-0.2);
    ScintWall_center->Draw("colzTEXT");
    ScintWall_center_CellID->SetMarkerSize(1.5);
    ScintWall_center_CellID->SetMarkerColor(kRed);
    ScintWall_center_CellID->SetBarOffset(0.2);
    ScintWall_center_CellID->Draw("TEXT SAME");
    ScintWall_center->GetXaxis()->SetTitle("X [cm]");
    ScintWall_center->GetYaxis()->SetTitle("Y [cm]");
    auto grid_center = new TExec("grid_center", "drawgrid_center()");
    grid_center->Draw();
    c3->Print("h1.pdf", "pdf");

    //draw spectra
    TCanvas *cA = new TCanvas();
    cA->DivideSquare(15);
    for (int i = 1; i <= 15; i++)
    {
        cA->cd(i);
        cells[A_block[i - 1]]->Draw();
        cells[A_block[i - 1]]->GetXaxis()->SetTitle("signal");
        cells[A_block[i - 1]]->GetYaxis()->SetTitle("counts");
    }
    cA->cd(); // c1 is the TCanvas
    TPad *pad5 = new TPad("all", "all", 0, 0, 1, 1);
    pad5->SetFillStyle(4000); // transparent
    pad5->Draw();
    pad5->cd();
    TLatex *lat = new TLatex();
    lat->DrawLatexNDC(.005, .96, "A");
    cA->Print("h1.pdf", "pdf");

    TCanvas *cB = new TCanvas();
    cB->DivideSquare(15);
    for (int i = 1; i <= 15; i++)
    {
        cB->cd(i);
        cells[B_block[i - 1]]->Draw();
    }
    cB->cd(); // c1 is the TCanvas

    lat->DrawLatexNDC(.005, .96, "B");
    cB->Print("h1.pdf", "pdf");

    TCanvas *cC = new TCanvas();
    cC->DivideSquare(15);
    for (int i = 1; i <= 15; i++)
    {
        cC->cd(i);
        cells[C_block[i - 1]]->Draw();
    }
    cC->cd(); // c1 is the TCanvas

    lat->DrawLatexNDC(.005, .96, "C");
    cC->Print("h1.pdf", "pdf");

    TCanvas *cD = new TCanvas();
    cD->DivideSquare(15);
    for (int i = 1; i <= 15; i++)
    {
        cD->cd(i);
        cells[D_block[i - 1]]->Draw();
    }
    cD->cd(); // c1 is the TCanvas
    lat->DrawLatexNDC(.005, .96, "D");
    cD->Print("h1.pdf", "pdf");

    TCanvas *cI = new TCanvas();
    cI->DivideSquare(15);
    for (int i = 1; i <= 15; i++)
    {
        cI->cd(i);
        cells[I_block[i - 1]]->Draw();
    }
    cI->cd(); // c1 is the TCanvas
    lat->DrawLatexNDC(.005, .96, "I");
    cI->Print("h1.pdf", "pdf");

    TCanvas *cJ = new TCanvas();
    cJ->DivideSquare(15);
    for (int i = 1; i <= 15; i++)
    {
        cJ->cd(i);
        cells[J_block[i - 1]]->Draw();
    }
    cJ->cd(); // c1 is the TCanvas
    lat->DrawLatexNDC(.005, .96, "J");
    cJ->Print("h1.pdf", "pdf");

    TCanvas *cK = new TCanvas();
    cK->DivideSquare(15);
    for (int i = 1; i <= 15; i++)
    {
        cK->cd(i);
        cells[K_block[i - 1]]->Draw();
    }
    cK->cd(); // c1 is the TCanvas
    lat->DrawLatexNDC(.005, .96, "K");
    cK->Print("h1.pdf", "pdf");

    TCanvas *cL = new TCanvas();
    cL->DivideSquare(15);
    for (int i = 1; i <= 15; i++)
    {
        cL->cd(i);
        cells[L_block[i - 1]]->Draw();
    }
    cL->cd(); // c1 is the TCanvas
    lat->DrawLatexNDC(.005, .96, "L");
    cL->Print("h1.pdf", "pdf");

    TCanvas *cE = new TCanvas();
    cE->DivideSquare(13);
    for (int i = 1; i <= 13; i++)
    {
        cE->cd(i);
        cells[E_block[i - 1]]->Draw();
    }
    cE->cd(); // c1 is the TCanvas
    lat->DrawLatexNDC(.005, .96, "E");
    cE->Print("h1.pdf", "pdf");

    TCanvas *cG = new TCanvas();
    cG->DivideSquare(13);
    for (int i = 1; i <= 13; i++)
    {
        cG->cd(i);
        cells[G_block[i - 1]]->Draw();
    }
    cG->cd(); // c1 is the TCanvas
    lat->DrawLatexNDC(.005, .96, "G");
    cG->Print("h1.pdf", "pdf");

    TCanvas *cF = new TCanvas();
    cF->DivideSquare(12);
    for (int i = 1; i <= 12; i++)
    {
        cF->cd(i);
        cells[F_block[i - 1]]->Draw();
    }
    cF->cd(); // c1 is the TCanvas
    lat->DrawLatexNDC(.005, .96, "F");
    cF->Print("h1.pdf", "pdf");

    TCanvas *cH = new TCanvas();
    cH->DivideSquare(12);
    for (int i = 1; i <= 12; i++)
    {
        cH->cd(i);
        cells[H_block[i - 1]]->Draw();
    }
    cH->cd(); // c1 is the TCanvas
    lat->DrawLatexNDC(.005, .96, "H");
    cH->Print("h1.pdf", "pdf");

    //draw TH2F
    TCanvas *c4 = new TCanvas();
    cell_v_signal->Draw("colz");
    cell_v_signal->GetXaxis()->SetTitle("cell id");
    cell_v_signal->GetYaxis()->SetTitle("signal");
    c4->Print("h1.pdf", "pdf");

    //draw TH1F
    TCanvas *c5 = new TCanvas();
    cell_v_signal1D->Draw("hist");
    cell_v_signal1D->GetXaxis()->SetTitle("cell id");
    cell_v_signal1D->GetYaxis()->SetTitle("signal/n_{events}");
    //cell_v_signal1D->GetXaxis()->SetTitleSize(0.05);
    //cell_v_signal1D->GetYaxis()->SetTitleSize(0.05);
    c5->Print("h1.pdf)", "pdf");

    //make pdf with map, if the map not needed, comment this line
    gSystem->Exec("pdfunite h1.pdf map.pdf result.pdf");
}