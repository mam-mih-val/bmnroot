#include "BmnQaMonitor.h"

const Int_t nRuns = 2; // 6, 7

BmnQaMonitor::BmnQaMonitor(TString dir) :
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
fRefHistosNames(nullptr),
fCurRunInfo(nullptr),
fRefRunInfo(nullptr),
runs(nullptr),
setups(nullptr) {
    nReleases = fSteering->GetNReleases();

    nSetups = 2; // BM@N, SRC
    runs = new Int_t[nRuns]{6, 7};
    setups = new TString[nRuns]{"BM@N", "SRC"};

    nCanvases = fSteering->GetNCanvases();
    nDims = 2; // 1d, 2d

    // Default parameters if release, period, setup are not being initialized in future
    fRelease = -1;
    fPeriod = -1;
    fSetup = -1;
    if (!dir.IsNull())
        fHistoDir = dir;
    else
        Fatal("BmnQaMonitor::BmnQaMonitor(TString dir)", "Histogram directory is empty or does no exist!!!");
    isOneRefDrawn = kFALSE;

    // 1. Create lists with information on ... 
    CreateInfoLists();

    // 2. Start web-server ...
     InitServer();

    // 3. Register canvases to be shown ...
    RegisterCanvases();

    // 4. Register user's commands to be used for ...
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

void BmnQaMonitor::CreateInfoLists() {
    // Create run and reference lists for all periods and setups ...
    vector <TString> listOfReleases = fSteering->GetListOfReleases();

    fRefList = new TList();
    fRefList->SetName("refList");

    BmnRunInfo***** bri = new BmnRunInfo****[nReleases];

    for (Int_t iRelease = 0; iRelease < nReleases; iRelease++) {
        bri[iRelease] = new BmnRunInfo***[nRuns];
        TList *listForRel = new TList();
        listForRel->SetName(listOfReleases[iRelease].Data());
        fRefList->Add(listForRel);

        for (Int_t iRun = 0; iRun < nRuns; iRun++) {
            bri[iRelease][iRun] = new BmnRunInfo**[nSetups];
            TList *listForPeriod = new TList();
            listForPeriod->SetName(Form("%d", runs[iRun]));
            listForRel->Add(listForPeriod);

            for (Int_t iSetup = 0; iSetup < nSetups; iSetup++) {
                TString prefix = TString::Format("RELEASE%s_RUN%d_SETUP_%s_", listOfReleases[iRelease].Data(), runs[iRun], setups[iSetup].Data());

                pair <Int_t, Int_t> borderFiles = fSteering->GetBorderRuns(runs[iRun], setups[iSetup]);
                const Int_t nFiles = borderFiles.second - borderFiles.first + 1;

                TList *listForSetup = new TList();
                listForSetup->SetName(Form("%s", setups[iSetup].Data()));
                listForPeriod->Add(listForSetup);

                bri[iRelease][iRun][iSetup] = new BmnRunInfo*[nFiles];

                for (Int_t iFile = borderFiles.first; iFile < borderFiles.second + 1; iFile++) {
                    TFile* f = new TFile(Form("%s/%s/run%d/%s/qa_%d.root", fHistoDir.Data(), listOfReleases[iRelease].Data(), runs[iRun], setups[iSetup].Data(), iFile), "r");
                    if (!f->IsOpen()) {
                        delete f;
                        continue;
                    }

                   UniRun* run = UniRun::GetRun(runs[iRun], iFile);
                    if (run) {                        
                        bri[iRelease][iRun][iSetup][iFile - borderFiles.first] = new BmnRunInfo(run);
                        listForSetup->Add((TObject*) bri[iRelease][iRun][iSetup][iFile - borderFiles.first]);
                    }
                    delete f;
                }
            }
        }
    }

    // Create info canvas ...
    TString name = "infoCanvas";
    infoCanvas = new TCanvas(name, name, 3 * 320, 1 * 240 * 2 / 3);
    infoCanvas->Divide(3, 1);
}

void BmnQaMonitor::DivideCanvases() {
    for (Int_t iDim = 0; iDim < nDims; iDim++)
        for (Int_t iCanvas = 0; iCanvas < nCanvases; iCanvas++) {
            TString name = TString(fCanvases[iDim][iCanvas]->GetName());
            TString prefix = name.Contains("1d") ? "1d" : "2d";
            Int_t nRows = fSteering->GetCanvasSizes(runs[fPeriod], setups[fSetup].Data(), name, prefix).first;
            Int_t nColumns = fSteering->GetCanvasSizes(runs[fPeriod], setups[fSetup].Data(), name, prefix).second;

            if (nRows == -1 && nColumns == -1)
                continue;
            else {
                fCanvases[iDim][iCanvas]->Divide(nColumns, nRows);
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

    Int_t period = fSteering->GetRunAndSetupByRunId(run).first;
    TString setup = fSteering->GetRunAndSetupByRunId(run).second;
    vector <TString> detectors = fSteering->GetDetectors(period, setup);
    detectors.push_back("TRIGGERS");
    detectors.push_back("DST");

    for (Int_t iDim = 0; iDim < nDims; iDim++)
        for (Int_t iCanvas = 0; iCanvas < nCanvases; iCanvas++) {
            TCanvas * c = fCanvases[iDim][iCanvas];
            Int_t padCounter = 1;
            TString nameCanvas = (TString) c->GetName();

            for (auto det : detectors) {
                if (!nameCanvas.Contains(det.Data()))
                    continue;

                for (auto itCurr : hCurr)
                    for (auto itRef : hRef) {
                        TString currName = (TString) itCurr->GetName();
                        TString refName = (TString) itRef->GetName();

                        if (currName != refName)
                            continue;

                        TString nameHisto = (TString) itRef->GetName();
                        if (!nameHisto.Contains(det.Data()) || !nameHisto.Contains(nameCanvas.Data()))
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
                        //                        if (nameCanvas.Contains("TRIGGERS_1d"))
                        //                            pad->SetLogy();

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

    TList* periodList = (TList*) fRefList->At(fRelease);
    TList* setupList = (TList*) periodList->At(fPeriod);
    TList* runList = (TList*) setupList->At(fSetup);
    TIter it(runList);
    while (BmnRunInfo * tmp = (BmnRunInfo*) it.Next()) {
        if (tmp->GetRunNumber() == run) {
            fRefRunInfo = tmp;
            break;
        }
    }
    DrawInfoC();
}

void BmnQaMonitor::ShowCurrentHistos(Int_t run) {
    AllHistos* allHistos = GetCurrentRun(run);
    vector <TH1F*> h1 = allHistos->Get1D();
    vector <TH2F*> h2 = allHistos->Get2D();

    if (isOneRefDrawn)
        delete fRefHistosNames;

    isOneRefDrawn = kFALSE;

    // Put all histograms together
    vector <TNamed*> h;
    for (auto it : h1)
        h.push_back((TNamed*) it);
    for (auto it : h2)
        h.push_back((TNamed*) it);

    Int_t period = fSteering->GetRunAndSetupByRunId(run).first;
    TString setup = fSteering->GetRunAndSetupByRunId(run).second;
    vector <TString> detectors = fSteering->GetDetectors(period, setup);
    detectors.push_back("TRIGGERS");
    detectors.push_back("DST");
    // Divide canvases according to number of divisions assumed ...
    ClearCanvases();
    DivideCanvases();

    for (Int_t iDim = 0; iDim < nDims; iDim++)
        for (Int_t iCanvas = 0; iCanvas < nCanvases; iCanvas++) {
            TCanvas* c = fCanvases[iDim][iCanvas];
            Int_t padCounter = 1;
            TString nameCanvas = (TString) c->GetName();

            for (auto det : detectors) {
                if (!nameCanvas.Contains(det.Data()))
                    continue;

                for (auto it : h) {
                    TString nameHisto = (TString) it->GetName();
                    if (!nameHisto.Contains(det.Data()) || !nameHisto.Contains(nameCanvas.Data()))
                        continue;

                    TVirtualPad* pad = c->cd(padCounter);
                    // 1d-histograms for triggers are drawn in logarithmic scale !!!
                    //                    if (nameCanvas.Contains("TRIGGERS_1d"))
                    //                        pad->SetLogy();
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

    TList* periodList = (TList*) fRefList->At(fRelease);
    TList* setupList = (TList*) periodList->At(fPeriod);
    TList* runList = (TList*) setupList->At(fSetup);
    TIter it(runList);
    while (BmnRunInfo * tmp = (BmnRunInfo*) it.Next()) {
        if (tmp->GetRunNumber() == run) {
            fCurRunInfo = tmp;
            break;
        }
    }
    DrawInfoC();
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
    fCanvases = new TCanvas**[nDims];
    for (Int_t iDim = 0; iDim < nDims; iDim++) {
        fCanvases[iDim] = new TCanvas*[nCanvases];
        for (Int_t iCanvas = 0; iCanvas < nCanvases; iCanvas++) {
            TString prefix = (iDim == 0) ? "1d" : "2d";

            fCanvases[iDim][iCanvas] =
                    new TCanvas(Form("%s_%s", fSteering->GetListOfCanvases()[iCanvas].Data(), prefix.Data()),
                    Form("%s_%s", fSteering->GetListOfCanvases()[iCanvas].Data(), prefix.Data()), 800, 800);
        }
    }


    // Create all possible sets with detectors and triggers to be shown for all possible runs, setups ...
    gem = new BmnCoordinateDetQa**[nRuns];
    silicon = new BmnCoordinateDetQa**[nRuns];
    csc = new BmnCoordinateDetQa**[nRuns];

    tof400 = new BmnTimeDetQa**[nRuns];
    tof700 = new BmnTimeDetQa**[nRuns];
    mwpc = new BmnTimeDetQa**[nRuns];
    dch = new BmnTimeDetQa**[nRuns];

    ecal = new BmnCalorimeterDetQa**[nRuns];
    zdc = new BmnCalorimeterDetQa**[nRuns];

    triggers = new BmnTrigDetQa**[nRuns];

    dst = new BmnDstQa**[nRuns];

    for (Int_t iRun = 0; iRun < nRuns; iRun++) {
        gem[iRun] = new BmnCoordinateDetQa*[nSetups];
        silicon[iRun] = new BmnCoordinateDetQa*[nSetups];
        if (iRun != 0)
            csc[iRun] = new BmnCoordinateDetQa*[nSetups];

        tof400[iRun] = new BmnTimeDetQa*[nSetups];
        tof700[iRun] = new BmnTimeDetQa*[nSetups];
        mwpc[iRun] = new BmnTimeDetQa*[nSetups];
        dch[iRun] = new BmnTimeDetQa*[nSetups];

        ecal[iRun] = new BmnCalorimeterDetQa*[nSetups];
        zdc[iRun] = new BmnCalorimeterDetQa*[nSetups];

        triggers[iRun] = new BmnTrigDetQa*[nSetups];

        dst[iRun] = new BmnDstQa*[nSetups];

        for (Int_t iSetup = 0; iSetup < nSetups; iSetup++) {
            if (iRun == 0 && iSetup == 1) // Configuration does not exist!!!
                continue;
            vector <TString> detectors = fSteering->GetDetectors(runs[iRun], setups[iSetup]);
            vector <TString> triggs = fSteering->GetTriggers(runs[iRun], setups[iSetup]);

            if (detectors.size() == 0 && triggs.size() == 0)
                continue;

            UInt_t id = fSteering->GetBorderRuns(runs[iRun], setups[iSetup]).first;
            for (auto det : detectors) {
                if (det == "GEM")
                    gem[iRun][iSetup] = new BmnCoordinateDetQa(det, id);
                else if (det == "SILICON")
                    silicon[iRun][iSetup] = new BmnCoordinateDetQa(det, id);
                else if (det == "CSC" && iRun != 0)
                    csc[iRun][iSetup] = new BmnCoordinateDetQa(det, id);
                else if (det == "TOF400")
                    tof400[iRun][iSetup] = new BmnTimeDetQa(det, id);
                else if (det == "TOF700")
                    tof700[iRun][iSetup] = new BmnTimeDetQa(det, id);
                else if (det == "MWPC")
                    mwpc[iRun][iSetup] = new BmnTimeDetQa(det, id);
                else if (det == "DCH")
                    dch[iRun][iSetup] = new BmnTimeDetQa(det, id);
                else if (det == "ECAL")
                    ecal[iRun][iSetup] = new BmnCalorimeterDetQa(det, id);
                else if (det == "ZDC")
                    zdc[iRun][iSetup] = new BmnCalorimeterDetQa(det, id);
            }

            triggers[iRun][iSetup] = new BmnTrigDetQa(triggs, id);
            dst[iRun][iSetup] = new BmnDstQa(id);
        }
    }

    for (Int_t iRun = 0; iRun < nRuns; iRun++)
        for (Int_t iSetup = 0; iSetup < nSetups; iSetup++) {

            if (iRun == 0 && iSetup == 1) // Configuration does not exist!!!
                continue;

            // Filling vector histogram names ...
            GetHistoNames <BmnTrigDetQa> (triggers[iRun][iSetup]);

            GetHistoNames <BmnCoordinateDetQa> (gem[iRun][iSetup]);
            GetHistoNames <BmnCoordinateDetQa> (silicon[iRun][iSetup]);
            if (iRun != 0)
                GetHistoNames <BmnCoordinateDetQa> (csc[iRun][iSetup]);

            GetHistoNames <BmnTimeDetQa> (tof400[iRun][iSetup]);
            GetHistoNames <BmnTimeDetQa> (tof700[iRun][iSetup]);
            GetHistoNames <BmnTimeDetQa> (dch[iRun][iSetup]);
            GetHistoNames <BmnTimeDetQa> (mwpc[iRun][iSetup]);

            GetHistoNames <BmnCalorimeterDetQa> (ecal[iRun][iSetup]);
            GetHistoNames <BmnCalorimeterDetQa> (zdc[iRun][iSetup]);

            GetHistoNames <BmnDstQa> (dst[iRun][iSetup]);

            // Get histograms by names ...
            GetHistosToBeRegistered <BmnTrigDetQa> (triggers[iRun][iSetup]);

            GetHistosToBeRegistered <BmnCoordinateDetQa> (gem[iRun][iSetup]);
            GetHistosToBeRegistered <BmnCoordinateDetQa> (silicon[iRun][iSetup]);
            if (iRun != 0)
                GetHistosToBeRegistered <BmnCoordinateDetQa> (csc[iRun][iSetup]);

            GetHistosToBeRegistered <BmnTimeDetQa> (tof400[iRun][iSetup]);
            GetHistosToBeRegistered <BmnTimeDetQa> (tof700[iRun][iSetup]);
            GetHistosToBeRegistered <BmnTimeDetQa> (dch[iRun][iSetup]);
            GetHistosToBeRegistered <BmnTimeDetQa> (mwpc[iRun][iSetup]);

            GetHistosToBeRegistered <BmnCalorimeterDetQa> (ecal[iRun][iSetup]);
            GetHistosToBeRegistered <BmnCalorimeterDetQa> (zdc[iRun][iSetup]);

            GetHistosToBeRegistered <BmnDstQa> (dst[iRun][iSetup]);
        }

    for (Int_t iDim = 0; iDim < nDims; iDim++) {
        for (Int_t iCanvas = 0; iCanvas < nCanvases; iCanvas++)
            fServer->Register("/Objects", fCanvases[iDim][iCanvas]);
    }

    fServer->Register("/Objects", fRefList);
    fServer->Register("/Objects", infoCanvas);
}

void BmnQaMonitor::RegisterUserCommands() {
    TString cmdTitle0 = "SelectRelease";
    TString cmdTitle1 = "SelectPeriod";
    TString cmdTitle2 = "SelectSetup";
    TString cmdTitle3 = "SelectRun";
    TString cmdTitle4 = "SelectReference";
    TString cmdTitle5 = "Clear";
    //
    // Displaying current histograms ...
    fServer->Register("/", this);
    fName += "_";

    // Release
    TString cmd0 = "/" + fName + "/->SetRelease(%arg1%)";
    fServer->RegisterCommand(cmdTitle0.Data(), cmd0.Data(), "button;");

    // Period
    TString cmd1 = "/" + fName + "/->SetPeriod(%arg1%)";
    fServer->RegisterCommand(cmdTitle1.Data(), cmd1.Data(), "button;");

    // Setup
    TString cmd2 = "/" + fName + "/->SetSetup(%arg1%)";
    fServer->RegisterCommand(cmdTitle2.Data(), cmd2.Data(), "button;");

    // Run
    TString cmd3 = "/" + fName + "/->ShowCurrentHistos(%arg1%)";
    fServer->RegisterCommand(cmdTitle3.Data(), cmd3.Data(), "button;");

    // Reference
    TString cmd4 = "/" + fName + "/->ShowReferenceHistos(%arg1%)";
    fServer->RegisterCommand(cmdTitle4.Data(), cmd4.Data(), "button;");

    // Clear canvas
    TString cmd5 = "/" + fName + "/->ClearCanvases()";
    fServer->RegisterCommand(cmdTitle5.Data(), cmd5.Data(), "button;");
}

void BmnQaMonitor::ClearCanvases() {
    for (Int_t iDim = 0; iDim < nDims; iDim++)
        for (Int_t iCanvas = 0; iCanvas < nCanvases; iCanvas++)
            fCanvases[iDim][iCanvas]->Clear("C");
}

AllHistos * BmnQaMonitor::GetRun(UInt_t run) {
    Int_t period = fSteering->GetRunAndSetupByRunId(run).first;
    TString setup = fSteering->GetRunAndSetupByRunId(run).second;

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

    TString path = fHistoDir + "/" + fSteering->GetListOfReleases()[fRelease] + "/" +
            TString::Format("run%d", runs[fPeriod]) + "/" + setups[fSetup] +
            TString::Format("/qa_%d.root", run);

    TFile* file = new TFile(path.Data(), "read");
    if (!file->IsOpen()) {
        cout << "File does not exist! Exiting ... " << endl;
        return fHistos;
    }

    for (auto it : fHistoNames) {
        if (!it.Contains(TString::Format("RUN%d_SETUP_%s", period, setup.Data()).Data()))
            continue;

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

        if (h) {
            TString hName = TString::Format("%s", h->GetName());

            if (hName.Contains(".vs")) // .vs in histogram name must be present if 2d-histogram assumed
                fHistos->Set2D((TH2F*) h);
            else
                fHistos->Set1D((TH1F*) h);
        }
    }
    cout << "Run #" << run << " processed " << endl;
    return fHistos;
}

void BmnQaMonitor::DrawInfoC() {
    TLatex Tl;
    TVirtualPad *pad = infoCanvas->cd(1);
    pad->Clear();
    Tl.SetTextAlign(12);
    Tl.SetTextSize(0.10);
    Tl.SetTextColor(kBlue);
    if (fCurRunInfo) {
        Tl.DrawLatex(0.1, 0.9, Form("Current Run: %i", fCurRunInfo->GetRunNumber()));
        Tl.DrawLatex(0.1, 0.75, Form("Energy: %1.2f", fCurRunInfo->GetEnergy()));
        Tl.DrawLatex(0.1, 0.6, Form("Beam: %s", fCurRunInfo->GetBeamParticle().Data()));
        Tl.DrawLatex(0.1, 0.45, Form("Target: %s", fCurRunInfo->GetTargetParticle().Data()));
        Tl.Draw();
    }
    pad->Modified();
    pad->Update();

    pad = infoCanvas->cd(2);
    pad->Clear();
    Tl.SetTextAlign(12);
    Tl.SetTextSize(0.10);
    Tl.SetTextColor(kRed);
    if (fRefRunInfo) {
        Tl.DrawLatex(0.1, 0.9, Form("Ref. Run: %i", fRefRunInfo->GetRunNumber()));
        Tl.DrawLatex(0.1, 0.75, Form("Energy: %1.2f", fRefRunInfo->GetEnergy()));
        Tl.DrawLatex(0.1, 0.6, Form("Beam: %s", fRefRunInfo->GetBeamParticle().Data()));
        Tl.DrawLatex(0.1, 0.45, Form("Target: %s", fRefRunInfo->GetTargetParticle().Data()));
        Tl.Draw();
    }
    pad->Update();
    pad->Modified();

    pad = infoCanvas->cd(3);
    pad->Clear();
    Tl.SetTextAlign(12);
    Tl.SetTextSize(0.10);
    Tl.SetTextColor(kBlack);
    Tl.DrawLatex(0.1, 0.9, Form("Release: %s", fSteering->GetListOfReleases()[fRelease].Data()));
    Tl.DrawLatex(0.1, 0.75, Form("Period: %d", runs[fPeriod]));
    Tl.DrawLatex(0.1, 0.6, Form("Setup: %s", setups[fSetup].Data()));

    pad->Update();
    pad->Modified();

}

