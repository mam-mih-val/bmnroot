
//#include <pthread.h>
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
    _fileList = new vector<BmnRunInfo>();
    fRecoTree = NULL;
    fRecoTree4Show = NULL;
    fHistOut = NULL;
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
    bhGem = NULL;
    bhToF400 = NULL;
    bhToF700 = NULL;
    bhDCH = NULL;
    bhMWPC = NULL;
    bhZDC = NULL;
    bhTrig = NULL;
    bhGem_4show = NULL;
    bhToF400_4show = NULL;
    bhToF700_4show = NULL;
    bhDCH_4show = NULL;
    bhMWPC_4show = NULL;
    bhZDC_4show = NULL;
    bhTrig_4show = NULL;
    rawDataDecoder = NULL;
    _ctx = NULL;
}

BmnMonitor::~BmnMonitor() {
    if (bhGem) delete bhGem;
    if (bhToF400) delete bhToF400;
    if (bhToF700) delete bhToF700;
    if (bhDCH) delete bhDCH;
    if (bhMWPC) delete bhMWPC;
    if (bhZDC) delete bhZDC;
    if (bhTrig) delete bhTrig;

    //    delete fRecoTree;
    if (fHistOut != NULL)
        delete fHistOut;
    //    fServer->Unregister(infoCanvas);
    if (infoCanvas) delete infoCanvas;
    if (bhGem_4show) delete bhGem_4show;
    if (bhToF400_4show) delete bhToF400_4show;
    if (bhToF700_4show) delete bhToF700_4show;
    if (bhDCH_4show) delete bhDCH_4show;
    if (bhMWPC_4show) delete bhMWPC_4show;
    if (bhZDC_4show) delete bhZDC_4show;
    if (bhTrig_4show) delete bhTrig_4show;
    if (fServer) delete fServer;
    if (rawDataDecoder) {
        rawDataDecoder->DisposeDecoder();
        delete rawDataDecoder;
    }
    if (_fileList) delete _fileList;
    if (_ctx) {
        zmq_ctx_destroy(_ctx);
        _ctx = NULL;
    }
}

void BmnMonitor::MonitorStreamZ(TString dirname, TString refDir, TString decoAddr, Int_t webPort) {
    _ctx = zmq_ctx_new();
    _decoSocket = zmq_socket(_ctx, ZMQ_SUB);
    if (_decoSocket == NULL) {
        DBGERR("zmq socket")
        return;
    }
    _webPort = webPort;
    _curDir = dirname;
    if (refDir == "")
        _refDir = _curDir;
    else
        _refDir = refDir;
    DBG("started")
    printf("Ref dir set to %s\n", _refDir.Data());
    InitServer();
    RegisterAll();
    fRawDecoAddr = decoAddr;
    TString conStr = TString("tcp://") + fRawDecoAddr + ":5555";
    if (zmq_connect(_decoSocket, conStr.Data()) != 0) {
        DBGERR("zmq connect")
        return;
    }
    Int_t len;
    decoTimeout = 0;
    keepWorking = kTRUE;
    while (keepWorking) {
        gSystem->ProcessEvents();
        
    }
    zmq_close(_decoSocket);
    zmq_ctx_destroy(_ctx);
    _ctx = NULL;
}

void BmnMonitor::MonitorStream(TString dirname, TString refDir, TString decoAddr, Int_t webPort) {
    _webPort = webPort;
    _curDir = dirname;
    if (refDir == "")
        _refDir = _curDir;
    else
        _refDir = refDir;
    DBG("started")
    printf("Ref dir set to %s\n", _refDir.Data());
    InitServer();
    RegisterAll();
    //    thread threadDeco(threadDecodeWrapper, dirname, startFile, runCurrent);
    //    if (threadDeco.joinable())
    //        threadDeco.detach();
    fRawDecoAddr = decoAddr;
    usleep(1e6);
    Int_t len;
    decoTimeout = 0;
    TMonitor *mon = new TMonitor;
    while (keepWorking) {
        gSystem->ProcessEvents();
        //        fServer->ProcessRequests();
        switch (fState) {
            case kBMNRECON:
                fRawDecoSocket = new TSocket(fRawDecoAddr, RAW_DECODER_SOCKET_PORT);
                if (fRawDecoSocket == NULL) {
                    DBGERR("TSocket")
                    return;
                } else
                    if (!fRawDecoSocket->IsValid()) {
                    usleep(DECO_SOCK_WAIT_PERIOD * 1000);
                    continue;
                }
                printf("Connected to %s\n", fRawDecoAddr.Data());
                fRawDecoSocket->GetInetAddress().Print();
                fRawDecoSocket->SetOption(kKeepAlive, 1);
                mon->Add(fRawDecoSocket);
                fState = kBMNWAIT;
                break;
            default:
                break;

        }
        TSocket *sel;
        sel = mon->Select(DECO_SOCK_WAIT_PERIOD);
        if (sel == (TSocket *) - 1) { // timeout
            decoTimeout += DECO_SOCK_WAIT_PERIOD;
            //            DBG("mon select timeout")
            if ((decoTimeout > DECO_SOCK_WAIT_LIMIT) && (fState == kBMNWORK)) {
                FinishRun();
                fState = kBMNWAIT;
            }
            continue;
        }
        if (sel == (TSocket *) 0) { // error
            printf("TSocket error: %s\n", strerror(errno));
            DBGERR("mon select")
            continue;
        }
        decoTimeout = 0;
        TMessage *mess;
        len = sel->Recv(mess);
        if (len <= 0) {
            if (len == 0)
                printf("Connection closed by decoder\n");
            else
                DBGERR("Recv error")
                FinishRun();
            keepWorking = kFALSE;
            break; // @TODO remove
            delete mess;
            fState = kBMNRECON;
            mon->Remove(fRawDecoSocket);
            delete fRawDecoSocket;
            fRawDecoSocket = NULL;
            continue;
        }
        gSystem->ProcessEvents();
        switch (mess->What()) {
            case kMESS_STRING:
                char str[64];
                mess->ReadString(str, 64);
                printf("message: %s\n", str);
                break;
            case kMESS_OBJECT:
            {
                //                if (mess->GetClass() != DigiArrays::Class()){
                //                    printf("!wrong class!\n");
                //                    continue;
                //                }
                fDigiArrays = (DigiArrays*) mess->ReadObject(mess->GetClass());
                if (fDigiArrays->header->GetEntriesFast() == 0) {
                    fDigiArrays->Clear();
                    delete fDigiArrays;
                    break;
                }
                BmnEventHeader* head = (BmnEventHeader*) fDigiArrays->header->At(0);
                Int_t runID = head->GetRunId();
                switch (fState) {
                    case kBMNWAIT:
                        fRunID = runID;
                        CreateFile(fRunID);
                        fState = kBMNWORK;
                        ProcessDigi(0);
                        break;
                    case kBMNWORK:
                        if (fRunID != runID) {
                            FinishRun();
                            keepWorking = kFALSE;
                            break; // @TODO remove
                            fRunID = runID;
                            CreateFile(fRunID);
                        }
                        ProcessDigi(0);
                        break;
                    default:
                        break;
                }
                fDigiArrays->Clear();
                delete fDigiArrays;
                break;
            }

            default:
                printf("*** Unexpected message ***\n");
                break;

        }
        mess->Clear();
        mess->Delete();
        //        delete mess;
        gSystem->ProcessEvents();
    }
    if (fRawDecoSocket) {
        fRawDecoSocket->Close();
        delete fRawDecoSocket;
    }
    delete mon;
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
    fRecoTree = NULL;
    fHistOut = new TFile(outHistName, "recreate");
    if (fHistOut)
        printf("file %s created\n", outHistName.Data());
    fRecoTree = new TTree("BmnMon", "BmnMon");
    fRecoTree->SetMaxTreeSize(TTREE_MAX_SIZE); // file will not be divided
    if (fRecoTree4Show) {
        fRecoTree4Show->Clear();
        delete fRecoTree4Show;
        fRecoTree4Show = NULL;
    }
    fRecoTree4Show = new TTree("BmnMon4Show", "BmnMon");
    fRecoTree4Show->SetDirectory(NULL); // tree will not be saved

    TString refName = Form("ref%06d_", fRunID);
    if (bhGem != NULL) delete bhGem;
    if (bhToF400 != NULL) delete bhToF400;
    if (bhToF700 != NULL) delete bhToF700;
    if (bhDCH != NULL) delete bhDCH;
    if (bhMWPC != NULL) delete bhMWPC;
    if (bhZDC != NULL) delete bhZDC;
    if (bhTrig != NULL) delete bhTrig;
    bhGem    = new BmnHistGem(refName + "GEM");
    bhDCH    = new BmnHistDch(refName + "DCH");
    bhMWPC   = new BmnHistMwpc(refName + "MWPC");
    bhZDC    = new BmnHistZDC(refName + "ZDC");
    bhToF400 = new BmnHistToF(refName + "ToF400");
    bhToF700 = new BmnHistToF700(refName + "ToF700");
    bhTrig   = new BmnHistTrigger(refName + "Triggers");
    bhGem->SetDir(fHistOut, fRecoTree);
    bhDCH->SetDir(fHistOut, fRecoTree);
    bhMWPC->SetDir(fHistOut, fRecoTree);
    bhZDC->SetDir(fHistOut, fRecoTree);
    bhToF400->SetDir(fHistOut, fRecoTree);
    bhToF700->SetDir(fHistOut, fRecoTree);
    bhTrig->SetDir(fHistOut, fRecoTree);

    bhGem_4show->SetDir(NULL, fRecoTree4Show);
    bhDCH_4show->SetDir(NULL, fRecoTree4Show);
    bhMWPC_4show->SetDir(NULL, fRecoTree4Show);
    bhZDC_4show->SetDir(NULL, fRecoTree4Show);
    bhToF400_4show->SetDir(NULL, fRecoTree4Show);
    bhToF700_4show->SetDir(NULL, fRecoTree4Show);
    bhTrig_4show->SetDir(NULL, fRecoTree4Show);

    //    bhToF400->Reset();
    //    bhToF700->Reset();
    //    bhDCH->Reset();
    //    bhMWPC->Reset();
    //    bhTrig->Reset();
    //    bhGem->Reset();
    bhToF400_4show->Reset();
    bhToF700_4show->Reset();
    bhDCH_4show->Reset();
    bhMWPC_4show->Reset();
    bhZDC_4show->Reset();
    bhTrig_4show->Reset();
    bhGem_4show->Reset();
}

void BmnMonitor::ProcessStreamRun() {
    //    Int_t iEv = 0;
    //    BmnStatus convertResult;
    //
    //    OpenStream();
    //    RegisterAll();
    //    thread rcvThread(threadReceiveWrapper, dataReceiver);
    //
    //    while (kTRUE && iEv < 100) {
    //
    //        convertResult = rawDataDecoder->ConvertRawToRootIterate();
    //        if (convertResult == kBMNTIMEOUT) {
    //            printf("Connection timeout!");
    //            break;
    //        }
    //        rawDataDecoder->DecodeDataToDigiIterate();
    //        ProcessDigi(iEv++);
    //        fServer->ProcessRequests();
    //        gSystem->ProcessEvents();
    //    }
    //    rcvThread.join();
    //
    //    //    rawDataDecoder->DisposeDecoder();
    //    //    rawDataDecoder->DisposeConverter();
    //    delete dataReceiver;
    //    delete rawDataDecoder;
    //    FinishRun();

}

void BmnMonitor::ProcessDigi(Int_t iEv) {
    //    fDigiTree->GetEntry(iEv);
    // histograms fill//
    //    DBG("started")
    fEvents++;
    if (fDigiArrays->header == NULL) {
        printf("Wrong header!\n");
        return;
    }
    BmnEventHeader* head = (BmnEventHeader*) fDigiArrays->header->At(0);
    bhTrig->FillFromDigi(
            fDigiArrays->bc1,
            fDigiArrays->t0,
            fDigiArrays->bc2,
            fDigiArrays->veto,
            fDigiArrays->fd,
            fDigiArrays->bd);
    bhGem->FillFromDigi(fDigiArrays->gem);
    bhToF400->FillFromDigi(fDigiArrays->tof400);
    bhToF700->FillFromDigi(fDigiArrays->tof700);
    bhDCH->FillFromDigi(fDigiArrays->dch);
    bhMWPC->FillFromDigi(fDigiArrays->mwpc);
    bhZDC->FillFromDigi(fDigiArrays->zdc);
    // Fill data Tree //
    fRecoTree->Fill();
    // fill histograms what will be shown on the site//
    bhTrig_4show->FillFromDigi(
            fDigiArrays->bc1,
            fDigiArrays->t0,
            fDigiArrays->bc2,
            fDigiArrays->veto,
            fDigiArrays->fd,
            fDigiArrays->bd);
    bhGem_4show->FillFromDigi(fDigiArrays->gem);
    bhToF400_4show->FillFromDigi(fDigiArrays->tof400);
    bhToF700_4show->FillFromDigi(fDigiArrays->tof700);
    bhDCH_4show->FillFromDigi(fDigiArrays->dch);
    bhMWPC_4show->FillFromDigi(fDigiArrays->mwpc);
    bhZDC_4show->FillFromDigi(fDigiArrays->zdc);
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
        bhGem_4show->DrawBoth();
        bhDCH_4show->DrawBoth();
        bhMWPC_4show->DrawBoth();
        bhZDC_4show->DrawBoth();
        bhTrig_4show->DrawBoth();
        bhToF400_4show->DrawBoth();
        bhToF700_4show->DrawBoth();
    }
}

void BmnMonitor::RegisterAll() {
    // histograms init //
    bhGem_4show = new BmnHistGem("GEM", _curDir);
    bhDCH_4show = new BmnHistDch("DCH");
    bhMWPC_4show = new BmnHistMwpc("MWPC");
    bhZDC_4show = new BmnHistZDC("ZDC");
    bhToF400_4show = new BmnHistToF("ToF400");
    bhToF700_4show = new BmnHistToF700("ToF700");
    bhTrig_4show = new BmnHistTrigger("Triggers");

    fServer->Register("/", infoCanvas);
    bhGem_4show->Register(fServer);
    bhDCH_4show->Register(fServer);
    bhMWPC_4show->Register(fServer);
    bhZDC_4show->Register(fServer);
    bhToF400_4show->Register(fServer);
    bhToF700_4show->Register(fServer);
    bhTrig_4show->Register(fServer);
    fServer->Register("/", refList);
    DBG("histograms registered")

    bhGem_4show->SetRefPath(_refDir);
    bhDCH_4show->SetRefPath(_refDir);
    bhMWPC_4show->SetRefPath(_refDir);
    bhZDC_4show->SetRefPath(_refDir);
    bhToF400_4show->SetRefPath(_refDir);
    bhToF700_4show->SetRefPath(_refDir);
    bhTrig_4show->SetRefPath(_refDir);
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
            //    bhGem->SetDir(NULL, fRecoTree);
            //    bhToF400->SetDir(NULL, fRecoTree);
            //    bhToF700->SetDir(NULL, fRecoTree);
            //    bhDCH->SetDir(NULL, fRecoTree);
            //    bhMWPC->SetDir(NULL, fRecoTree);
            //    bhTrig->SetDir(NULL, fRecoTree);
    if (fRecoTree)
        printf("fRecoTree Write result = %d\n", fRecoTree->Write());
    if (fHistOut) {
        printf("fHistOut  Write result = %d\n", fHistOut->Write());
        //    DBG("list deleted")
        //            fRecoTree->Clear();
        //    DBG("tree cleared")
        //            delete fRecoTree;
        //    DBG("tree deleted")
        //    DBG("fHist closed")
        string cmd;
        cmd = string("chmod 775 ") + _curDir + fHistOut->GetName();
        printf("system result = %d\n", system(cmd.c_str()));

        cmd = string("hadd -f " + _curDir + "shorter.root ") + _curDir + fHistOut->GetName() +
                string("; mv " + _curDir + "shorter.root ") + _curDir + fHistOut->GetName();
        //           printf("system result = %d\n", system(cmd.c_str()));
        std::thread threadHAdd(BmnMonitor::threadCmdWrapper, cmd);
        if (threadHAdd.joinable())
            threadHAdd.detach();
    }
    //    bhGem->SetDir(NULL, fRecoTree);
    //    bhDCH->SetDir(NULL, fRecoTree);
    //    bhMWPC->SetDir(NULL, fRecoTree);
    //    bhToF400->SetDir(NULL, fRecoTree);
    //    bhToF700->SetDir(NULL, fRecoTree);
    //    bhTrig->SetDir(NULL, fRecoTree);
    //            fHistOut->Close();
    //           delete fHistOut;
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
