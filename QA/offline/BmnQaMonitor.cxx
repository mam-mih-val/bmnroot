#include <TCanvas.h>

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

    fDebug = kTRUE;

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

    delete fServer;
}

void BmnQaMonitor::DivideCanvases(Int_t nCanvs) {
    // Divide canvases by pads gor from existing number of histos to be displayed
    // Division is a subject of change if a new histo added (FIXME!)

    for (Int_t iCanvas = 0; iCanvas < nCanvs; iCanvas++) {
        TString name = TString(fCanvases[iCanvas]->GetName());

        if (name.Contains("GEM"))
            fCanvases[iCanvas]->Divide(7, 7);
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
    Int_t nCanvs = 10;

    vector <TH1F*> currHistos = GetCurrentRun(fCurrentRun);
    vector <TH1F*> refHistos = GetReferenceRun(run);

    TString dets[nCanvs] = {"GEM", "SILICON", "CSC", "TOF400", "TOF700", "DCH", "MWPC", "ECAL", "ZDC", "DST"}; // FIXME !!!

    //    MakeNormalization(currHistos, refHistos);

    for (Int_t iCanvas = 0; iCanvas < nCanvs; iCanvas++) {
        TCanvas* c = fCanvases[iCanvas];
        Int_t padCounter = 1;
        TString nameCanvas = (TString) c->GetName();

        for (Int_t iDet = 0; iDet < nCanvs; iDet++) {
            if (!nameCanvas.Contains(dets[iDet].Data()))
                continue;

            for (auto itCurr : currHistos)
                for (auto itRef : refHistos) {
                    TString currName = (TString) itCurr->GetName();
                    TString refName = (TString) itRef->GetName();

                    if (currName != refName)
                        continue;

                    TString nameHisto = (TString) itRef->GetName();
                    if (!nameHisto.Contains(dets[iDet].Data()))
                        continue;
                    TVirtualPad* pad = c->cd(padCounter);

                    Double_t maxCurr = itCurr->GetMaximum();
                    Double_t maxRef = itRef->GetMaximum();

                    if (maxRef > maxCurr) {
                        TH1F* h = (TH1F*) pad->GetPrimitive(itCurr->GetName());
                        pad->GetListOfPrimitives()->Remove(h);

                        itCurr->GetYaxis()->SetRangeUser(0., maxRef * 1.1);
                 //       itRef->SetNormFactor(itRef->Integral() / itCurr->Integral());
                        itRef->Draw();
                        itCurr->Draw("same");
                        
                    } else
                        itRef->Draw("same");
                    
                    itRef->SetLineColor(kRed);

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
    Int_t nCanvs = 10;
    vector <TH1F*> allHistos = GetCurrentRun(run); // FIXME !!!    

    TString dets[nCanvs] = {"GEM", "SILICON", "CSC", "TOF400", "TOF700", "DCH", "MWPC", "ECAL", "ZDC", "DST"}; // FIXME !!!

    for (Int_t iCanvas = 0; iCanvas < nCanvs; iCanvas++) {
        TCanvas* c = fCanvases[iCanvas];
        Int_t padCounter = 1;
        TString nameCanvas = (TString) c->GetName();

        for (Int_t iDet = 0; iDet < nCanvs; iDet++) {
            if (!nameCanvas.Contains(dets[iDet].Data()))
                continue;

            for (auto it : allHistos) {
                TString nameHisto = (TString) it->GetName();
                if (!nameHisto.Contains(dets[iDet].Data()))
                    continue;
                c->cd(padCounter);
                // TH1F* h = (TH1F*) it->Clone();
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
    const TString detNames[nCanvases] = {"GEM", "SILICON", "CSC", "TOF400", "TOF700", "DCH", "MWPC", "ECAL", "ZDC", "TRIGGERS", "DST"};

    fCanvases = new TCanvas*[nCanvases];
    for (Int_t iCanvas = 0; iCanvas < nCanvases; iCanvas++)
        fCanvases[iCanvas] = new TCanvas(Form("%s", detNames[iCanvas].Data()), Form("%s", detNames[iCanvas].Data()), 800, 800);

    DivideCanvases(nCanvases);

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
    GetHistosToBeRegistered <BmnCoordinateDetQa> (gem, 0);
    GetHistosToBeRegistered <BmnCoordinateDetQa> (silicon, 1);
    GetHistosToBeRegistered <BmnCoordinateDetQa> (csc, 2);

    GetHistosToBeRegistered <BmnTimeDetQa> (tof400, 3);
    GetHistosToBeRegistered <BmnTimeDetQa> (tof700, 4);
    GetHistosToBeRegistered <BmnTimeDetQa> (dch, 5);
    GetHistosToBeRegistered <BmnTimeDetQa> (mwpc, 6);

    GetHistosToBeRegistered <BmnCalorimeterDetQa> (ecal, 7);
    GetHistosToBeRegistered <BmnCalorimeterDetQa> (zdc, 8);

    GetHistosToBeRegistered <BmnDstQa> (dst, 9);

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

vector <TH1F*> BmnQaMonitor::GetRun(UInt_t run) {
    TString workFile = gSystem->Getenv("VMCWORKDIR");
    workFile += TString::Format("/macro/miscellaneous/qa_files/qa_%d.root", run);

    if (fHistoNames.size() == 0)
        Fatal("BmnQaMonitor::GetHistosFromFile()", "No histos to be displayed");

    TFile* file = new TFile(workFile.Data(), "read");
    if (!file->IsOpen()) {
        cout << "File does not exist! Exiting ... " << endl;
        throw;
    }

    vector <TH1F*> histos;

    for (auto it : fHistoNames) {
        TH1F* h = nullptr;

        h = (TH1F*) file->Get(TString("GEM/" + it).Data()); // FIXME!! Not elegant representation 

        if (!h)
            h = (TH1F*) file->Get(TString("SILICON/" + it).Data());
        if (!h)
            h = (TH1F*) file->Get(TString("CSC/" + it).Data());
        if (!h)
            h = (TH1F*) file->Get(TString("TOF400/" + it).Data());
        if (!h)
            h = (TH1F*) file->Get(TString("TOF700/" + it).Data());
        if (!h)
            h = (TH1F*) file->Get(TString("DCH/" + it).Data());
        if (!h)
            h = (TH1F*) file->Get(TString("MWPC/" + it).Data());
        if (!h)
            h = (TH1F*) file->Get(TString("ECAL/" + it).Data());
        if (!h)
            h = (TH1F*) file->Get(TString("ZDC/" + it).Data());
        if (!h)
            h = (TH1F*) file->Get(TString("DST/" + it).Data());

        histos.push_back(h);
    }
    // delete file;
    return histos;
}

void BmnQaMonitor::MakeNormalization(vector <TH1F*> curr, vector <TH1F*> ref) {
    if (curr.size() == 0 || ref.size() == 0) {
        cout << "Histo set is not correct! " << endl;
        return;
    }

    for (auto itCurr : curr)
        for (auto itRef : ref) {
            TString currName = (TString) itCurr->GetName();
            TString refName = (TString) itRef->GetName();

            if (currName != refName)
                continue;

            //            Double_t maxY = itCurr->GetBinContent(itCurr->GetMaximumBin());
            Double_t maxYCurr = itCurr->GetMaximum();
            Double_t maxYRef = itRef->GetMaximum();

            //            Double_t k = (itRef->Integral() > 0) ? itCurr->Integral() / (Double_t) itRef->Integral() : 1;
            //            if (k == 0)
            //                k = 1;
            //            k = k * itRef->GetBinContent(itRef->GetMaximumBin());
            //            if (maxY < k)
            //                maxY = k;

            itCurr->GetYaxis()->SetRange(0, TMath::Max(itCurr->GetMaximum(), itRef->GetMaximum()) * 1.1);
        }
}

void BmnQaMonitor::DrawRef(TCanvas* c, vector <PadInfo*>* p) {
    Double_t maxy;
    Double_t k = 1;
    for (Int_t iPad = 0; iPad < p->size(); iPad++) {
        TVirtualPad *pad = c->cd(iPad + 1);
        pad->Clear();
        PadInfo* info = p->at(iPad);
        if (!info)
            continue;
        //if (info->GetCurrentHisto()) {
        maxy = info->GetCurrentHisto()->GetBinContent(info->GetCurrentHisto()->GetMaximumBin());
        // info->GetCurrentHisto()->Draw(info->GetOption().Data());
        //if (info->GetRefHisto()) {
        k = (info->GetRefHisto()->Integral() > 0) ? info->GetCurrentHisto()->Integral() / (Double_t) info->GetRefHisto()->Integral() : 1;
        if (k == 0)
            k = 1;
        if (info->GetRefHisto()->Integral() > 0)
            info->GetRefHisto()->DrawNormalized("same hist", info->GetCurrentHisto()->Integral());
        k = k * info->GetRefHisto()->GetBinContent(info->GetRefHisto()->GetMaximumBin());
        if (maxy < k)
            maxy = k;
        // }
        info->GetCurrentHisto()->GetYaxis()->SetRange(0, maxy * 1.1);
        // }
        pad->Update();
        pad->Modified();
    }
    c->Update();
    c->Modified();
}






