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
fCurrentRun(-1) {
    InitServer();
    RegisterCanvases();
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

void BmnQaMonitor::DivideCanvases(Int_t nCanvs) {
    // Divide canvases by pads gor from existing number of histos to be displayed
    // Division is a subject of change if a new histo added (FIXME!)

    for (Int_t iCanvas = 0; iCanvas < nCanvs; iCanvas++) {
        TString name = TString(fCanvases[iCanvas]->GetName());

        if (name.Contains("TRIGGERS"))
            fCanvases[iCanvas]->Divide(5, 5);
        else if (name.Contains("GEM"))
            fCanvases[iCanvas]->Divide(10, 10);
        else if (name.Contains("SILICON"))
            fCanvases[iCanvas]->Divide(7, 7);
        else if (name.Contains("CSC"))
            fCanvases[iCanvas]->Divide(3, 3);
        else if (name.Contains("TOF400"))
            fCanvases[iCanvas]->Divide(3, 3);
        else if (name.Contains("TOF700"))
            fCanvases[iCanvas]->Divide(3, 3);
        else if (name.Contains("DCH"))
            fCanvases[iCanvas]->Divide(2, 2);
        else if (name.Contains("MWPC"))
            fCanvases[iCanvas]->Divide(2, 2);
        else if (name.Contains("ECAL"))
            fCanvases[iCanvas]->Divide(3, 3);
        else if (name.Contains("ZDC"))
            fCanvases[iCanvas]->Divide(3, 3);
        else if (name.Contains("DST"))
            fCanvases[iCanvas]->Divide(4, 4);
        else
            throw;
    }
}

void BmnQaMonitor::FillCanvasesWithHistos(Int_t nCanvs) {
    for (Int_t iCanvas = 0; iCanvas < nCanvs; iCanvas++) {
        TCanvas* c = fCanvases[iCanvas];
        TString name = TString(c->GetName());

        FillCanvasesWithHistos <BmnTrigDetQa> (triggers, name, iCanvas);

        FillCanvasesWithHistos <BmnCoordinateDetQa> (gem, name, iCanvas);
        FillCanvasesWithHistos <BmnCoordinateDetQa> (silicon, name, iCanvas);
        FillCanvasesWithHistos <BmnCoordinateDetQa> (csc, name, iCanvas);

        FillCanvasesWithHistos <BmnTimeDetQa> (tof400, name, iCanvas);
        FillCanvasesWithHistos <BmnTimeDetQa> (tof700, name, iCanvas);
        FillCanvasesWithHistos <BmnTimeDetQa> (dch, name, iCanvas);
        FillCanvasesWithHistos <BmnTimeDetQa> (mwpc, name, iCanvas);

        FillCanvasesWithHistos <BmnCalorimeterDetQa> (ecal, name, iCanvas);
        FillCanvasesWithHistos <BmnCalorimeterDetQa> (zdc, name, iCanvas);

        FillCanvasesWithHistos <BmnDstQa> (dst, name, iCanvas);
    }
}

void BmnQaMonitor::ShowReferenceHistos(Int_t run) {
    Int_t nCanvs = 11;

    AllHistos* currHistos = GetCurrentRun(fCurrentRun);
    vector <TH1F*> h1Curr = currHistos->Get1D();
    vector <TH2F*> h2Curr = currHistos->Get2D();

    AllHistos* refHistos = GetReferenceRun(run);
    vector <TH1F*> h1Ref = refHistos->Get1D();
    vector <TH2F*> h2Ref = refHistos->Get2D();

    vector <TNamed*> hCurr;
    vector <TNamed*> hRef;

    for (auto it : h1Curr)
        hCurr.push_back((TNamed*) it);
    for (auto it : h2Curr)
        hCurr.push_back((TNamed*) it);
    for (auto it : h1Ref)
        hRef.push_back((TNamed*) it);
    for (auto it : h2Ref)
        hRef.push_back((TNamed*) it);

    TString dets[nCanvs] = {"TRIGGERS", "GEM", "SILICON", "CSC", "TOF400", "TOF700", "DCH", "MWPC", "ECAL", "ZDC", "DST"}; // FIXME !!!

    for (Int_t iCanvas = 0; iCanvas < nCanvs; iCanvas++) {
        TCanvas* c = fCanvases[iCanvas];
        Int_t padCounter = 1;
        TString nameCanvas = (TString) c->GetName();

        for (Int_t iDet = 0; iDet < nCanvs; iDet++) {
            if (!nameCanvas.Contains(dets[iDet].Data()))
                continue;

            for (auto itCurr : hCurr)
                for (auto itRef : hRef) {
                    TString currName = (TString) itCurr->GetName();
                    TString refName = (TString) itRef->GetName();

                    if (currName != refName)
                        continue;

                    TString nameHisto = (TString) itRef->GetName();
                    if (!nameHisto.Contains(dets[iDet].Data()))
                        continue;
                    TVirtualPad* pad = c->cd(padCounter);

                    // We do not draw reference for a 2d-histo !!!
                    if (!currName.Contains(".vs") && !refName.Contains("vs.")) {
                        TH1F* cur = (TH1F*) itCurr;
                        TH1F* ref = (TH1F*) itRef;

                        Double_t maxCurr = cur->GetMaximum();
                        Double_t maxRef = ref->GetMaximum();

                        if (maxRef > maxCurr) {
                            TH1F* h = (TH1F*) pad->GetPrimitive(itCurr->GetName());
                            pad->GetListOfPrimitives()->Remove(h);

                            cur->GetYaxis()->SetRangeUser(0., maxRef * 1.1);
                            itRef->Draw();
                            itCurr->Draw("same");

                        } else
                            itRef->Draw("same");

                        ref->SetLineColor(kRed);
                    }
                    pad->Update();
                    pad->Modified();

                    padCounter++;
                }
        }
        c->Update();
        c->Modified();
    }
}

void BmnQaMonitor::ShowCurrentHistos(Int_t run) {
    Int_t nCanvs = 11;

    AllHistos* allHistos = GetCurrentRun(run);
    vector <TH1F*> h1 = allHistos->Get1D();
    vector <TH2F*> h2 = allHistos->Get2D();

    vector <TNamed*> h;
    for (auto it : h1)
        h.push_back((TNamed*) it);
    for (auto it : h2)
        h.push_back((TNamed*) it);

    TString dets[nCanvs] = {"TRIGGERS", "GEM", "SILICON", "CSC", "TOF400", "TOF700", "DCH", "MWPC", "ECAL", "ZDC", "DST"}; // FIXME !!!

    for (Int_t iCanvas = 0; iCanvas < nCanvs; iCanvas++) {
        TCanvas* c = fCanvases[iCanvas];
        Int_t padCounter = 1;
        TString nameCanvas = (TString) c->GetName();

        for (Int_t iDet = 0; iDet < nCanvs; iDet++) {
            if (!nameCanvas.Contains(dets[iDet].Data()))
                continue;

            for (auto it : h) {
                TString nameHisto = (TString) it->GetName();
                if (!nameHisto.Contains(dets[iDet].Data()))
                    continue;
                c->cd(padCounter);
                Bool_t isColz = nameHisto.Contains("vs.") ? kTRUE : kFALSE;
                if (isColz) {
                    TH2F* tmp = (TH2F*) it;
                    tmp->Draw("colz");
                } else
                    it->Draw();
                padCounter++;
            }
        }
    }
    fCurrentRun = run;
}

void BmnQaMonitor::InitServer(Int_t webPortCgi, Int_t webPortHttp) {
    const Char_t* cgiStr = Form("fastcgi:%d", webPortCgi);
    const Char_t* httpStr = Form("http:%d;noglobal", webPortHttp);

    // Start http-server ...
  
    fServer = new THttpServer(httpStr);

    // Start fast-cgi engine to the server ...
    fServer->CreateEngine(cgiStr);
    fServer->SetTimer(100, kFALSE);
}

void BmnQaMonitor::RegisterCanvases() {
    const Int_t nCanvases = 11; // dets, triggers + DST
    const TString detNames[nCanvases] = {"TRIGGERS", "GEM", "SILICON", "CSC", "TOF400", "TOF700", "DCH", "MWPC", "ECAL", "ZDC", "DST"};

    fCanvases = new TCanvas*[nCanvases];
    for (Int_t iCanvas = 0; iCanvas < nCanvases; iCanvas++)
        fCanvases[iCanvas] = new TCanvas(Form("%s", detNames[iCanvas].Data()), Form("%s", detNames[iCanvas].Data()), 800, 800);
    DivideCanvases(nCanvases);

    // Create a vector containing names of triggers to be displayed ...
    const Int_t nTrigs = 6;
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

    // Get histo names ...
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

    // Get histos by names got before ...
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

    FillCanvasesWithHistos(nCanvases);

    for (Int_t iCanvas = 0; iCanvas < nCanvases; iCanvas++)
        fServer->Register("/Objects", fCanvases[iCanvas]);

    // fillCanvasesWithHistos(nCanvases);
}

void BmnQaMonitor::RegisterUserCommands() {
    //    TString path = "GEM";
    TString cmdTitle1 = "SelectRun";
    TString cmdTitle2 = "SelectReferenceRun";
    //
    // Displaying current histograms ...
    fServer->Register("/", this);
    fName += "_";

    TString cmd1 = "/" + fName + "/->ShowCurrentHistos(%arg1%)";
    fServer->RegisterCommand(cmdTitle1.Data(), cmd1.Data(), "button;");

    TString cmd2 = "/" + fName + "/->ShowReferenceHistos(%arg1%)";
    fServer->RegisterCommand(cmdTitle2.Data(), cmd2.Data(), "button;");
    // fServer->ProcessRequests();

}

AllHistos* BmnQaMonitor::GetRun(UInt_t run) {
    TString workFile = gSystem->Getenv("VMCWORKDIR");
    workFile += TString::Format("/macro/miscellaneous/qa_files/qa_%d.root", run);

    if (fHistoNames.size() == 0)
        Fatal("BmnQaMonitor::GetHistosFromFile()", "No histos to be displayed");

    AllHistos* histos = new AllHistos();

    TFile* file = new TFile(workFile.Data(), "read");
    if (!file->IsOpen()) {
        cout << "File does not exist! Exiting ... " << endl;
        return histos;
    }

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
            histos->Set2D((TH2F*) h);
        else
            histos->Set1D((TH1F*) h);
    }
    return histos;
}