#include "BmnQaMonitor.h"

BmnQaMonitor::BmnQaMonitor() :
fServer(nullptr),
gem(nullptr),
silicon(nullptr),
csc(nullptr),
tof400(nullptr),
tof700(nullptr),
dch(nullptr),
mwpc(nullptr),
ecal(nullptr),
zdc(nullptr),
dst(nullptr),
triggers(nullptr),
fCanvases(nullptr),
fHistos(nullptr),
fSteering(new BmnOfflineQaSteering()),
fRefHistosNames(nullptr) {
    fHistoDir = "";
    fCurrentRun = -1;
    isOneRefDrawn = kFALSE;

    // 1. Start server 
    InitServer();

    // 2. Register canvases to be shown 
    RegisterCanvases();

    // 3. Register user's commands to show current and reference runs
    RegisterUserCommands();
}

BmnQaMonitor::~BmnQaMonitor() {
    delete gem;
    delete silicon;
    delete csc;

    delete tof400;
    delete tof700;
    delete dch;
    delete mwpc;

    delete ecal;
    delete zdc;

    delete dst;

    delete triggers;

    delete fServer;
}

void BmnQaMonitor::DivideCanvases() {
    // Divide canvases by pads got from existing number of histos to be displayed
    // Division is a subject of change if a new histo added (FIXME!)

    vector <TString> canvases = fSteering->GetCanvNames();

    for (Int_t iCanvas = 0; iCanvas < fSteering->GetCanvNames().size(); iCanvas++) {
        TString name = TString(fCanvases[iCanvas]->GetName());

        if (name.Contains(canvases[iCanvas].Data())) {
            Int_t idxDim = name.Contains("2d") ? 1 : 0;
            fCanvases[iCanvas]->Divide(fSteering->GetDetCanvas()[iCanvas / 2][idxDim][0], fSteering->GetDetCanvas()[iCanvas / 2][idxDim][1]);
        }
    }
}

void BmnQaMonitor::ShowReferenceHistos(Int_t run) {
    AllHistos* currHistos = GetCurrentRun(fCurrentRun);
    vector <TH1F*> h1Curr = currHistos->Get1D();
    vector <TH2F*> h2Curr = currHistos->Get2D();

    AllHistos* refHistos = GetReferenceRun(run);
    vector <TH1F*> h1Ref = refHistos->Get1D();
    vector <TH2F*> h2Ref = refHistos->Get2D();

    vector <TNamed*> hCurr;
    vector <TNamed*> hRef;

    if (!isOneRefDrawn)
        fRefHistosNames = new vector <TString>;

    for (auto it : h1Curr)
        hCurr.push_back((TNamed*) it);
    for (auto it : h2Curr)
        hCurr.push_back((TNamed*) it);
    for (auto it : h1Ref)
        hRef.push_back((TNamed*) it);
    for (auto it : h2Ref)
        hRef.push_back((TNamed*) it);

    for (Int_t iCanvas = 0; iCanvas < fSteering->GetCanvNames().size(); iCanvas++) {
        TCanvas* c = fCanvases[iCanvas];
        Int_t padCounter = 1;
        TString nameCanvas = (TString) c->GetName();

        for (auto itMap : fSteering->GetCorrMap()) {
            if (!nameCanvas.Contains(itMap.first.Data()))
                continue;

            for (auto itCurr : hCurr)
                for (auto itRef : hRef) {
                    TString currName = (TString) itCurr->GetName();
                    TString refName = (TString) itRef->GetName();

                    if (currName != refName)
                        continue;

                    TString nameHisto = (TString) itRef->GetName();
                    if (!nameHisto.Contains(itMap.first.Data()) || !nameHisto.Contains(nameCanvas.Data()))
                        continue;
                    TVirtualPad* pad = c->cd(padCounter);

                    // Remove a reference histo if drawn ...
                    if (isOneRefDrawn) {
                        vector <TString>& vec = *fRefHistosNames;
                        for (auto it : vec) {
                            TObject* obj = pad->GetPrimitive(it.Data());
                            if (!obj)
                                continue;

                            pad->GetListOfPrimitives()->Remove(obj);
                        }
                    } else {
                        TString baseName = TString(itRef->GetName());
                        baseName += ", Ref";
                        itRef->SetName(baseName.Data());
                        fRefHistosNames->push_back(itRef->GetName());
                    }

                    // 1d histos for triggers are drawn in logarithmic scale !!!
                    if (nameCanvas.Contains("TRIGGERS_1d"))
                        pad->SetLogy();

                    // We do not draw reference for a 2d-histo !!!
                    if (!currName.Contains("vs.") && !refName.Contains("vs.")) {
                        TH1F* cur = (TH1F*) itCurr;

                        if (!refName.Contains(", Ref")) {
                            refName += ", Ref";
                            itRef->SetName(refName.Data());
                        }

                        TH1F* ref = (TH1F*) itRef;

                        Double_t maxCurr = cur->GetMaximum();
                        Double_t maxRef = ref->GetMaximum();

                        if (maxRef > maxCurr) {
                            TH1F* h = (TH1F*) pad->GetPrimitive(cur->GetName());
                            pad->GetListOfPrimitives()->Remove(h);

                            ref->Draw();
                            ref->SetLineColor(kRed);
                            cur->GetYaxis()->SetRangeUser(0., maxRef * 1.3);

                            if (ref->GetSumOfWeights() > FLT_EPSILON)
                                cur->DrawNormalized("same", ref->Integral());
                        } else {
                            if (ref->GetSumOfWeights() > FLT_EPSILON)
                                ref->DrawNormalized("same", cur->Integral())->SetLineColor(kRed);
                        }
                    }

                    pad->Update();
                    pad->Modified();

                    padCounter++;
                }
        }
        c->Update();
        c->Modified();
    }
    isOneRefDrawn = kTRUE;
}

void BmnQaMonitor::ShowCurrentHistos(Int_t run) {
    AllHistos* allHistos = GetCurrentRun(run);
    vector <TH1F*> h1 = allHistos->Get1D();
    vector <TH2F*> h2 = allHistos->Get2D();

    if (isOneRefDrawn)
        delete fRefHistosNames;

    isOneRefDrawn = kFALSE;
   
    vector <TNamed*> h;
    for (auto it : h1)
        h.push_back((TNamed*) it);
    for (auto it : h2)
        h.push_back((TNamed*) it);

    for (Int_t iCanvas = 0; iCanvas < fSteering->GetCanvNames().size(); iCanvas++) {
        TCanvas* c = fCanvases[iCanvas];
        Int_t padCounter = 1;
        TString nameCanvas = (TString) c->GetName();

        for (auto itMap : fSteering->GetCorrMap()) {
            if (!nameCanvas.Contains(itMap.first.Data()))
                continue;

            for (auto it : h) {
                TString nameHisto = (TString) it->GetName();
                if (!nameHisto.Contains(itMap.first.Data()) || !nameHisto.Contains(nameCanvas.Data()))
                    continue;

                TVirtualPad* pad = c->cd(padCounter);
                // 1d histos for triggers are drawn in logarithmic scale !!!
                if (nameCanvas.Contains("TRIGGERS_1d"))
                    pad->SetLogy();
                Bool_t isColz = nameHisto.Contains("vs.") ? kTRUE : kFALSE;
                if (isColz) {
                    TH2F* tmp = (TH2F*) it;
                    tmp->Draw("colz");
                } else {
                    TH1F* tmp = (TH1F*) it;
                    it->Draw();
                    tmp->GetYaxis()->UnZoom();
                }
                padCounter++;
            }
        }
    }
    fCurrentRun = run;
}

void BmnQaMonitor::InitServer() {
    const Char_t* cgiStr = Form("fastcgi:%d", 9000);
    const Char_t* httpStr = Form("http:%d;noglobal", 8080);

    // Start http-server ...  
    fServer = new THttpServer(httpStr);

    // Start fast-cgi engine to the server ...
    fServer->CreateEngine(cgiStr);
    fServer->SetTimer(100, kFALSE);
}

void BmnQaMonitor::RegisterCanvases() {
    Int_t nCanvases = fSteering->GetCanvNames().size();

    fCanvases = new TCanvas*[nCanvases];

    for (Int_t iCanvas = 0; iCanvas < nCanvases; iCanvas++)
        fCanvases[iCanvas] = new TCanvas(Form("%s", fSteering->GetCanvNames()[iCanvas].Data()), Form("%s", fSteering->GetCanvNames()[iCanvas].Data()), 800, 800);

    DivideCanvases();

    // Create a vector containing names of triggers to be displayed ...
    const Int_t nTrigs = 6; // FIXME !!!
    TString _trigNames[nTrigs] = {"BC1", "BC2", "BC3", "BD", "SI", "VETO"};

    vector <TString> trigNames;
    for (Int_t iTrig = 0; iTrig < nTrigs; iTrig++)
        trigNames.push_back(_trigNames[iTrig]);

    triggers = new BmnTrigDetQa(trigNames);

    gem = new BmnCoordinateDetQa("GEM");
    silicon = new BmnCoordinateDetQa("SILICON");
    csc = new BmnCoordinateDetQa("CSC");

    tof400 = new BmnTimeDetQa("TOF400");
    tof700 = new BmnTimeDetQa("TOF700");
    dch = new BmnTimeDetQa("DCH");
    mwpc = new BmnTimeDetQa("MWPC");

    ecal = new BmnCalorimeterDetQa("ECAL");
    zdc = new BmnCalorimeterDetQa("ZDC");

    dst = new BmnDstQa();

    // 1. Get names ...
    GetHistoNames <BmnTrigDetQa> (triggers);

    GetHistoNames <BmnCoordinateDetQa> (gem);
    GetHistoNames <BmnCoordinateDetQa> (silicon);
    GetHistoNames <BmnCoordinateDetQa> (csc);

    GetHistoNames <BmnTimeDetQa> (tof400);
    GetHistoNames <BmnTimeDetQa> (tof700);
    GetHistoNames <BmnTimeDetQa> (dch);
    GetHistoNames <BmnTimeDetQa> (mwpc);

    GetHistoNames <BmnCalorimeterDetQa> (ecal);
    GetHistoNames <BmnCalorimeterDetQa> (zdc);

    GetHistoNames <BmnDstQa> (dst);

    // 2. Get histos by their names got before ...
    GetHistosToBeRegistered <BmnTrigDetQa> (triggers, 0);

    GetHistosToBeRegistered <BmnCoordinateDetQa> (gem, 1);
    GetHistosToBeRegistered <BmnCoordinateDetQa> (silicon, 2);
    GetHistosToBeRegistered <BmnCoordinateDetQa> (csc, 3);

    GetHistosToBeRegistered <BmnTimeDetQa> (tof400, 4);
    GetHistosToBeRegistered <BmnTimeDetQa> (tof700, 5);
    GetHistosToBeRegistered <BmnTimeDetQa> (dch, 6);
    GetHistosToBeRegistered <BmnTimeDetQa> (mwpc, 7);

    GetHistosToBeRegistered <BmnCalorimeterDetQa> (ecal, 8);
    GetHistosToBeRegistered <BmnCalorimeterDetQa> (zdc, 9);

    GetHistosToBeRegistered <BmnDstQa> (dst, 10);

    for (Int_t iCanvas = 0; iCanvas < nCanvases; iCanvas++)
        fServer->Register("/Objects", fCanvases[iCanvas]);
}

void BmnQaMonitor::RegisterUserCommands() {
    TString cmdTitle1 = "SelectRun";
    TString cmdTitle2 = "SelectReferenceRun";
    TString cmdTitle3 = "Clear";
    //
    // Displaying current histograms ...
    fServer->Register("/", this);
    fName += "_";

    // Current run
    TString cmd1 = "/" + fName + "/->ShowCurrentHistos(%arg1%)";
    fServer->RegisterCommand(cmdTitle1.Data(), cmd1.Data(), "button;");

    // Reference run
    TString cmd2 = "/" + fName + "/->ShowReferenceHistos(%arg1%)";
    fServer->RegisterCommand(cmdTitle2.Data(), cmd2.Data(), "button;");

    // Clear pads
    TString cmd3 = "/" + fName + "/->ClearCanvases()";
    fServer->RegisterCommand(cmdTitle3.Data(), cmd3.Data(), "button;");
}

void BmnQaMonitor::ClearCanvases() {
    Int_t nCanvases = fSteering->GetCanvNames().size();
    for (Int_t iCanvas = 0; iCanvas < nCanvases; iCanvas++)
        fCanvases[iCanvas]->Clear("D");
}

AllHistos * BmnQaMonitor::GetRun(UInt_t run) {
    if (fHistos)
        delete fHistos;

    fHistos = new AllHistos();
    if (run == 0) {
        ClearCanvases();
        return fHistos;
    }

    if (fHistoNames.size() == 0) {
        cout << "BmnQaMonitor::GetHistosFromFile(), No histos to be displayed" << endl;
        return fHistos;
    }

    fPathToData += fHistoDir + TString::Format("/qa_%d.root", run);
    TFile* file = new TFile(fPathToData.Data(), "read");
    if (!file->IsOpen()) {
        cout << "File does not exist! Exiting ... " << endl;
        fPathToData = "";
        return fHistos;
    }

    fPathToData = "";

    for (auto it : fHistoNames) {
        TNamed* h = nullptr;
        h = (TNamed*) file->Get(TString("TRIGGERS/" + it).Data()); // FIXME!! Not elegant representation 

        if (!h)
            h = (TNamed*) file->Get(TString("GEM/" + it).Data());
        if (!h)
            h = (TNamed*) file->Get(TString("SILICON/" + it).Data());
        if (!h)
            h = (TNamed*) file->Get(TString("CSC/" + it).Data());
        if (!h)
            h = (TNamed*) file->Get(TString("TOF400/" + it).Data());
        if (!h)
            h = (TNamed*) file->Get(TString("TOF700/" + it).Data());
        if (!h)
            h = (TNamed*) file->Get(TString("DCH/" + it).Data());
        if (!h)
            h = (TNamed*) file->Get(TString("MWPC/" + it).Data());
        if (!h)
            h = (TNamed*) file->Get(TString("ECAL/" + it).Data());
        if (!h)
            h = (TNamed*) file->Get(TString("ZDC/" + it).Data());
        if (!h)
            h = (TNamed*) file->Get(TString("DST/" + it).Data());

        TString hName = TString::Format("%s", h->GetName());

        if (hName.Contains(".vs")) // .vs in histo name must be present if 2d-histo assumed
            fHistos->Set2D((TH2F*) h);
        else
            fHistos->Set1D((TH1F*) h);
    }
    cout << "Run #" << run << " processed " << endl;
    return fHistos;
}