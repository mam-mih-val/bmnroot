#include <thread>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <chrono>
#include <ctime>
#include <sys/epoll.h>
#include <fcntl.h>
#include <zmq.h>

#include <root/TLatex.h>
#include <root/TLegend.h>

#include "BmnMonitor.h"
#include "BmnOnlineDecoder.h"

BmnMonitor::BmnMonitor() {
    keepWorking = kTRUE;
    fRecoTree = NULL;
    fRecoTree4Show = NULL;
    fHistOut = NULL;
    fHistOutTemp = NULL;
    fServer = NULL;
    fRawDecoSocket = NULL;
    fRunID = 0;
    fEvents = 0;
    fState = kBMNRECON;
    itersToUpdate = 1000;
    TString name = "infoCanvas";
    infoCanvas = new TCanvas(name, name);
    refList = new TList();
    refList->SetName("refList");
    fDigiArrays = NULL;
    _ctx = NULL;
}

BmnMonitor::~BmnMonitor() {
    DBG("started")
    for (auto h : bhVec)
        if (h) delete h;
    bhVec.clear();
    DBG("hists deleted")
            //    delete fRecoTree;
    if (fHistOut != NULL)
        delete fHistOut;
    DBG("delete fHistOut")
    if (infoCanvas) delete infoCanvas;
    for (auto h : bhVec4show)
        if (h) delete h;
    bhVec4show.clear();
    DBG("hists_4show deleted")
    if (fServer) delete fServer;
    if (_ctx) {
        zmq_ctx_destroy(_ctx);
        _ctx = NULL;
    }
}

void BmnMonitor::MonitorStreamZ(TString dirname, TString refDir, TString decoAddr, Int_t webPort) {
    DBG("started")
    fState = kBMNWAIT;
    _ctx = zmq_ctx_new();
    _decoSocket = zmq_socket(_ctx, ZMQ_SUB);
    if (_decoSocket == NULL) {
        DBGERR("zmq socket")
        return;
    }
    if (zmq_setsockopt(_decoSocket, ZMQ_SUBSCRIBE, NULL, 0) == -1) {
        DBGERR("zmq subscribe")
        return;
    }
    Int_t rcvBuf = MAX_BUF_LEN;
    if (zmq_setsockopt(_decoSocket, ZMQ_RCVBUF, &rcvBuf, sizeof (rcvBuf)) == -1)
        DBGERR("zmq_setsockopt of ZMQ_RCVBUF")
    if (zmq_setsockopt(_decoSocket, ZMQ_SNDBUF, &rcvBuf, sizeof (rcvBuf)) == -1)
        DBGERR("zmq_setsockopt of ZMQ_SNDBUF")
    _webPort = webPort;
    _curDir = dirname;
    if (refDir == "")
        _refDir = _curDir;
    else
        _refDir = refDir;
    printf("Ref dir set to %s\n", _refDir.Data());
    InitServer();
    RegisterAll();
    fRawDecoAddr = decoAddr;
    TString conStr = TString("tcp://") + fRawDecoAddr + ":5555";
    if (zmq_connect(_decoSocket, conStr.Data()) != 0) {
        DBGERR("zmq connect")
        return;
    }
    zmq_msg_t msg;
    TBufferFile t(TBuffer::kRead);
    Int_t frame_size = 0;
    decoTimeout = 0;
    keepWorking = kTRUE;

    while (keepWorking) {
        gSystem->ProcessEvents();
        zmq_msg_init(&msg);
        frame_size = zmq_msg_recv(&msg, _decoSocket, ZMQ_DONTWAIT); // ZMQ_DONTWAIT
        if (frame_size == -1) {
            if (errno == EAGAIN) {
                usleep(DECO_SOCK_WAIT_PERIOD * 1000);
                decoTimeout += DECO_SOCK_WAIT_PERIOD;
                if ((decoTimeout > DECO_SOCK_WAIT_LIMIT) && (fState == kBMNWORK)) {
                    FinishRun();
                    fState = kBMNWAIT;
                    DBG("state changed to kBMNWAIT")
                }
            } else {
                printf("Receive error № %d #%s\n", errno, zmq_strerror(errno));
                return;
            }
        } else {
            decoTimeout = 0;
            t.Reset();
            t.SetWriteMode();
            t.SetBuffer(zmq_msg_data(&msg), zmq_msg_size(&msg));
            t.SetReadMode();
            fDigiArrays = (DigiArrays*) (t.ReadObject(DigiArrays::Class()));
            gSystem->ProcessEvents();
            if (fDigiArrays->header->GetEntriesFast() > 0) {
                BmnEventHeader* head = (BmnEventHeader*) fDigiArrays->header->At(0);
                Int_t runID = head->GetRunId();
                switch (fState) {
                    case kBMNWAIT:
                        fRunID = runID;
                        CreateFile(fRunID);
                        DBG("state changed to kBMNWORK")
                        fState = kBMNWORK;
                        ProcessDigi(0);
                        break;
                    case kBMNWORK:
                        if (fRunID != runID) {
                            FinishRun();
                            fRunID = runID;
                            CreateFile(fRunID);
                        }
                        ProcessDigi(0);
                        break;
                    default:
                        break;
                }
            }
            fDigiArrays->Clear();
            delete fDigiArrays;
            t.DetachBuffer();
        }
        zmq_msg_close(&msg);
    }
    zmq_close(_decoSocket);
    zmq_ctx_destroy(_ctx);
    _ctx = NULL;
}

void BmnMonitor::InitServer() {
    TString cgiStr = Form("fastcgi:%d", _webPort);
    if (gSystem->AccessPathName("auth.htdigest") != 0) {
        printf("Authorization file not found\nStarting server without authorization\n");
        fServer = new THttpServer(cgiStr.Data());
    } else
        fServer = new THttpServer(TString(cgiStr + "?auth_file=auth.htdigest&auth_domain=root").Data());
    fServer->SetTimer(100, kTRUE);
    fServer->SetItemField("/", "_monitoring", "10000");
    fServer->SetItemField("/", "_layout", "grid3x3");
}

BmnStatus BmnMonitor::CreateFile(Int_t runID) {
    fEvents = 0;
    UpdateRuns();
    TString outHistName = Form("bmn_run%04d_hist.root", runID);
    fHistOut = new TFile(outHistName, "recreate");
    if (fHistOut)
        printf("file %s created\n", outHistName.Data());
    fRecoTree = new TTree("BmnMon", "BmnMon");
    fRecoTree->SetMaxTreeSize(TTREE_MAX_SIZE); // file will not be divided
    //    if (fRecoTree4Show) {
    //        fRecoTree4Show->Clear();
    //        delete fRecoTree4Show;
    //        fRecoTree4Show = NULL;
    //    }
    fHistOutTemp = new TFile("tempo.root", "recreate");
    if (fHistOutTemp)
        printf("file tempo.root created\n");
    fRecoTree4Show = new TTree("BmnMon4Show", "BmnMon");
    //    fRecoTree4Show->SetDirectory(NULL); // tree will not be saved

    TString refName = Form("ref%06d_", fRunID);
    bhVec.push_back(new BmnHistGem(refName + "GEM", _curDir));
    bhVec.push_back(new BmnHistDch(refName + "DCH"));
    bhVec.push_back(new BmnHistMwpc(refName + "MWPC"));
    bhVec.push_back(new BmnHistZDC(refName + "ZDC"));
    bhVec.push_back(new BmnHistToF(refName + "ToF400"));
    bhVec.push_back(new BmnHistToF700(refName + "ToF700"));
    bhVec.push_back(new BmnHistTrigger(refName + "Triggers"));
    for (auto h : bhVec)
        h->SetDir(fHistOut, fRecoTree);
    for (auto h : bhVec4show){
        h->SetDir(fHistOutTemp, fRecoTree4Show);
        h->Reset();
    }
}

void BmnMonitor::ProcessDigi(Int_t iEv) {
    // histograms fill//
    fEvents++;
    BmnEventHeader* head = (BmnEventHeader*) fDigiArrays->header->At(0);
    for (auto h : bhVec)
        if (h)
            h->FillFromDigi(fDigiArrays);
//    //    clock_t prev = clock();
//    //    printf("zdc fill %f\n", (clock() - prev)/ (double) CLOCKS_PER_SEC);
    // Fill data Tree //
    fRecoTree->Fill();
    // fill histograms what will be shown on the site//
    for (auto h : bhVec4show)
        if (h)
            h->FillFromDigi(fDigiArrays);
    fRecoTree4Show->Fill();
    if (fEvents % 200 == 0) {
        // print info canvas //
        infoCanvas->Clear();
        infoCanvas->cd(1);
        TString runType;
        switch (head->GetTrig()) {
            case kBMNBEAM:
                runType = "beam";
                break;
            case kBMNMINBIAS:
                runType = "target";
                break;
            default:
                runType = "???";
                break;
        }
        TLatex Tl;
        Tl.SetTextAlign(23);
        Tl.SetTextSize(0.16);
        Tl.DrawLatex(0.5, 0.9, Form("Run: %04d", head->GetRunId()));
        Tl.DrawLatex(0.5, 0.6, Form("Event: %d", head->GetEventId()));
        Tl.DrawLatex(0.5, 0.3, Form("Run Type: %s", runType.Data()));
        Tl.Draw();
        infoCanvas->Modified();
        infoCanvas->Update();
        for (auto h : bhVec4show)
            h->DrawBoth();
    }
}

void BmnMonitor::RegisterAll() {
    bhVec4show.push_back(new BmnHistGem("GEM", _curDir));
    bhVec4show.push_back(new BmnHistDch("DCH"));
    bhVec4show.push_back(new BmnHistMwpc("MWPC"));
    bhVec4show.push_back(new BmnHistZDC("ZDC"));
    bhVec4show.push_back(new BmnHistToF("ToF400"));
    bhVec4show.push_back(new BmnHistToF700("ToF700"));
    bhVec4show.push_back(new BmnHistTrigger("Triggers"));

    fServer->Register("/", infoCanvas);
    fServer->Register("/", refList);
    for (auto h : bhVec4show){
        h->Register(fServer);
        h->SetRefPath(_refDir);
    }
}

void BmnMonitor::UpdateRuns() {
    struct dirent **namelist;
    regex re(".*bmn_run0*(\\d+)_hist.root");
    Int_t n;
    refList->Clear();
    n = scandir(_refDir, &namelist, 0, versionsort);
    if (n < 0)
        perror("scandir");
    else {
        for (Int_t i = 0; i < n; ++i) {
            if (regex_match(namelist[i]->d_name, re))
                refList->Add(new TObjString(
                    TString(regex_replace(namelist[i]->d_name, re, "$1"))));
            free(namelist[i]);
        }
        free(namelist);
    }
}

void BmnMonitor::FinishRun() {
    DBG("started")
    if (fRecoTree)
        printf("fRecoTree Write result = %d\n", fRecoTree->Write());
    if (fHistOut) {
        printf("fHistOut  Write result = %d\n", fHistOut->Write());
        fHistOut->Close();
        //        string cmd;
        //        cmd = string("chmod 775 ") + _curDir + fHistOut->GetName();
        //        printf("system result = %d\n", system(cmd.c_str()));
        //
        //        cmd = string("hadd -f " + _curDir + "shorter.root ") + _curDir + fHistOut->GetName() +
        //                string("; mv " + _curDir + "shorter.root ") + _curDir + fHistOut->GetName();
        //        //           printf("system result = %d\n", system(cmd.c_str()));
        //        std::thread threadHAdd(BmnMonitor::threadCmdWrapper, cmd);
        //        if (threadHAdd.joinable())
        //            threadHAdd.detach();
    }
    for (auto h : bhVec)
        if (h) delete h;
    bhVec.clear();
    if (fRecoTree4Show)
        printf("fRecoTree4Show Write result = %d\n", fRecoTree4Show->Write());
    if (fHistOutTemp) {
        printf("fHistOutMem Write result = %d\n", fHistOutTemp->Write());
    for (auto h : bhVec4show)
        h->SetDir(NULL, NULL);
        fHistOutTemp->Close();
        delete fHistOut;
    }
}

TObjArray* BmnMonitor::GetAlikeRuns(BmnEventHeader* header){
    TString beamParticle = "C";
    TString targetParticle = "Pb";
    Float_t beamEnergy = 4.5;
    TObjArray arrayConditions;
    UniDbSearchCondition* searchCondition = new UniDbSearchCondition(columnBeamParticle, conditionEqual, beamParticle);
    arrayConditions.Add((TObject*)searchCondition);
    searchCondition = new UniDbSearchCondition(columnTargetParticle, conditionEqual, targetParticle);
    arrayConditions.Add((TObject*)searchCondition);
//    searchCondition = new UniDbSearchCondition(columnEnergy, conditionEqual, beamEnergy);
//    arrayConditions.Add((TObject*)searchCondition);

    TObjArray* refRuns = UniDbRun::Search(arrayConditions);

    arrayConditions.SetOwner(kTRUE);
    arrayConditions.Delete();
    
    return refRuns;
}

void BmnMonitor::threadDecodeWrapper(TString dirname, TString startFile, Bool_t runCurrent) {
    BmnOnlineDecoder *deco = new BmnOnlineDecoder();
    deco->Decode(dirname, startFile, runCurrent);
    delete deco;
    //    deco->BatchDirectory(dirname);
}

void BmnMonitor::threadCmdWrapper(string cmd) {
    printf("system result = %d\n", system(cmd.c_str()));
}
ClassImp(BmnMonitor);
