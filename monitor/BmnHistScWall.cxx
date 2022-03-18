#include "TGaxis.h"

#include "BmnHistScWall.h"

BmnHistScWall::BmnHistScWall(TString title, TString path) : BmnHist() {
    fTitle = title;
    fName = title + "_cl";
    TGaxis::SetMaxDigits(2);
    CreateHistos();
}

void BmnHistScWall::CreateHistos() {
    BmnScWallRaw2Digit *tempScWallMapper = new BmnScWallRaw2Digit();
    
    tempScWallMapper->ParseConfig("SCWALL_map_dry_run_2022.txt"); // scwall map
    auto UniqueXpos = tempScWallMapper->GetUniqueXpositions();
    auto UniqueYpos = tempScWallMapper->GetUniqueYpositions();
    std::copy(UniqueXpos.begin(), UniqueXpos.end(), back_inserter(fUniqueXpos));
    std::copy(UniqueYpos.begin(), UniqueYpos.end(), back_inserter(fUniqueYpos));
    delete tempScWallMapper;

    TString name;
    name = fTitle + "_h2d_Grid";
    h2d_grid = new TH2D(name, "ScWall Grid", fUniqueXpos.size()-1, &fUniqueXpos[0], fUniqueYpos.size()-1, &fUniqueYpos[0]);
    h2d_grid->GetXaxis()->SetTitle("X position");
    h2d_grid->GetXaxis()->SetTitleColor(kOrange + 10);
    h2d_grid->GetYaxis()->SetTitle("Y position");
    h2d_grid->GetYaxis()->SetTitleColor(kOrange + 10);
    h2d_grid->GetZaxis()->SetNoExponent(kFALSE);
    name = fTitle + "_h2d_profile";
    h2d_profile = new TH2D(name, "ScWall Profile", 200, 0, 200, 200, 0, 5000);
    h2d_profile->GetXaxis()->SetTitle("Cell id");
    h2d_profile->GetXaxis()->SetTitleColor(kOrange + 10);
    h2d_profile->GetYaxis()->SetTitle("Average Signal [MeV]");
    h2d_profile->GetYaxis()->SetTitleColor(kOrange + 10);
    name = fTitle + "CanvasAmplitudes";
    canAmps = new TCanvas(name, name, PAD_WIDTH * ScWall_ROWS, PAD_HEIGHT * ScWall_COLS);
    canAmps->Divide(ScWall_ROWS, ScWall_COLS);
    canAmpsPads.resize(ScWall_ROWS * ScWall_COLS);
    NamesAmps.resize(ScWall_ROWS * ScWall_COLS);

    PadInfo *p = new PadInfo();
    p->current = h2d_grid;
    p->opt = "colz";
    canAmpsPads[0] = p;
    PadInfo *p1 = new PadInfo();
    p1->current = h2d_profile;
    canAmpsPads[1] = p1;
}



void BmnHistScWall::Register(THttpServer *serv) {
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, h2d_grid);
    fServer->Register(path, h2d_profile);
    fServer->Register(path, canAmps);
    TString cmd = "/" + fName + "/->Reset()";
    fServer->SetItemField(path.Data(), "_monitoring", "2000");
    fServer->SetItemField(path.Data(), "_layout", "grid3x3");
    TString cmdTitle = path + "Reset";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    fServer->Restrict(cmdTitle, "visible=shift");
    fServer->Restrict(cmdTitle, "allow=shift");
    cmd = "/" + fName + "/->SetRefRun(%arg1%)";
    cmdTitle = path + "SetRefRun";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
}

void BmnHistScWall::SetDir(TFile *outFile = NULL, TTree *recoTree = NULL) {
    frecoTree = recoTree;
    fDir = NULL;
    if (outFile != NULL)
        fDir = outFile->mkdir(fTitle + "_hists");
    h2d_grid->SetDirectory(fDir);
    h2d_profile->SetDirectory(fDir);
}

void BmnHistScWall::DrawBoth() {
    BmnHist::DrawRef(canAmps, &canAmpsPads);
    return;
}

void BmnHistScWall::FillFromDigi(DigiArrays *fDigiArrays) {
    TClonesArray * digits = fDigiArrays->scwall;
    if (!digits)
        return;
    
    // Loop over digis
    for (Int_t iDig = 0; iDig < digits->GetEntriesFast(); iDig++) {
        BmnScWallDigi* digi = (BmnScWallDigi*) digits->At(iDig);
        double signal = abs(digi->GetSignal());
        h2d_profile->Fill(digi->GetCellId(), signal);

        int x_pos = fUniqueXpos.at(digi->GetX());
        int y_pos = fUniqueYpos.at(digi->GetY());
        h2d_grid->Fill(x_pos, y_pos, signal);
    }
    fEventCounter += digits->GetEntriesFast();

}

BmnStatus BmnHistScWall::SetRefRun(Int_t id) {
    TString FileName = Form("bmn_run%04d_hist.root", id);
    printf("SetRefRun: %s\n", FileName.Data());
    if (refRunName != FileName) {
        refRunName = FileName;
        refID = id;
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canAmpsPads, NamesAmps);
        DrawBoth();
    }
    return kBMNSUCCESS;
}

void BmnHistScWall::ClearRefRun() {
    for (auto pad : canAmpsPads){
        if (pad->ref) delete pad->ref;
        pad->ref = NULL;
    }
    refID = 0;
}

void BmnHistScWall::Reset() {
    printf("BmnHistScWall : Reset histos\n");
    h2d_grid->Reset();
    h2d_profile->Reset();
}

BmnHistScWall::~BmnHistScWall() {
    delete canAmps;
    if (fDir != NULL)
        return;
    delete h2d_grid;
    delete h2d_profile;
}

ClassImp(BmnHistScWall);

