
//#include <pthread.h>
#include <thread>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <chrono>
#include <ctime>
#include <sys/inotify.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <root/TLatex.h>
#include <root/TLegend.h>

#include "BmnMonitor.h"
#include "BmnOnlineDecoder.h"

#define INOTIF_BUF_LEN (255 * (sizeof(struct inotify_event) + 255))

BmnMonitor::BmnMonitor() {
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
    bhTrig = NULL;
    bhGem_4show = NULL;
    bhToF400_4show = NULL;
    bhToF700_4show = NULL;
    bhDCH_4show = NULL;
    bhMWPC_4show = NULL;
    bhTrig_4show = NULL;
    rawDataDecoder = NULL;
}

BmnMonitor::~BmnMonitor() {

    printf("1\n");
    if (bhGem) delete bhGem;
    if (bhToF400) delete bhToF400;
    if (bhToF700) delete bhToF700;
    if (bhDCH) delete bhDCH;
    if (bhMWPC) delete bhMWPC;
    if (bhTrig) delete bhTrig;

    //    delete fRecoTree;
    if (fHistOut != NULL)
        delete fHistOut;
    printf("2\n");
    //    fServer->Unregister(infoCanvas);
    if (infoCanvas) delete infoCanvas;

    printf("3\n");
    if (bhGem_4show) delete bhGem_4show;
    printf("4\n");
    if (bhToF400_4show) delete bhToF400_4show;
    if (bhToF700_4show) delete bhToF700_4show;
    if (bhDCH_4show) delete bhDCH_4show;
    if (bhMWPC_4show) delete bhMWPC_4show;
    if (bhTrig_4show) delete bhTrig_4show;
    if (fServer) delete fServer;
    if (rawDataDecoder) {
        rawDataDecoder->DisposeDecoder();
        delete rawDataDecoder;
    }
    if (_fileList) delete _fileList;
}

void BmnMonitor::Monitor(TString dirname, TString startFile, Bool_t runCurrent) {
    _curFile = startFile;
    _curDir = dirname;
    InitServer();
    //    _inotifDir = inotify_init();
    //    _inotifDirW = inotify_add_watch(_inotifDir, dir, IN_CREATE);
    //    Int_t flags = fcntl(_inotifDir, F_GETFL, 0);
    //    fcntl(_inotifDir, F_SETFL, flags | O_NONBLOCK);
    if (!runCurrent) {
        _curFile = "";
        _curFile = WatchNext(_curDir, _curFile, RUN_FILE_CHECK_PERIOD);
        _curFile = WatchNext(_curDir, _curFile, RUN_FILE_CHECK_PERIOD);
    } else
        if (_curFile.Length() == 0) {
        _curFile = WatchNext(_curDir, _curFile, RUN_FILE_CHECK_PERIOD);
        //        _curFile = WatchNext(_inotifDir, 1e5);
    }
    //    _inotifFile = inotify_init();
    //    _inotifFileW = inotify_add_watch(_inotifFile, _curFile, IN_MODIFY);
    InitDecoder();
    RegisterAll();

    while (kTRUE) {
        ProcessFileRun(_curFile);
        _curFile = WatchNext(_curDir, _curFile, RUN_FILE_CHECK_PERIOD);
    }
    //    inotify_rm_watch(_inotifDir, _inotifDirW);
    //    close(_inotifDir);
    //    close(_inotifFile);
}

void BmnMonitor::MonitorStream(TString dirname, TString refDir, TString decoAddr) {
    //    _curFile = startFile;
    _curDir = dirname;
    if (refDir == "")
        _refDir = _curDir;
    else
        _refDir = refDir;
    DBG("started")
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
    while (kTRUE) {
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
                fDigiArrays = (DigiArrays*) mess->ReadObject(mess->GetClass());
                if (fDigiArrays->header->GetEntriesFast() == 0)
                    break;
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
        delete mess;
        gSystem->ProcessEvents();
    }
    fRawDecoSocket->Close();
    delete fRawDecoSocket;
}

BmnStatus BmnMonitor::BatchDirectory(TString dirname) {
    _curDir = dirname;
    struct dirent **namelist;
    regex re("\\w+\\.data");
    Int_t n;
    n = scandir(dirname, &namelist, 0, versionsort);
    if (n < 0) {
        perror("scandir");
        return kBMNERROR;
    } else {
        for (Int_t i = 0; i < n; ++i) {
            if (regex_match(namelist[i]->d_name, re)) {
                _curFile = TString(namelist[i]->d_name);
                break;
            }
        }
    }

    InitServer();
    InitDecoder();
    RegisterAll();
    n = scandir(dirname, &namelist, 0, versionsort);
    if (n < 0) {
        perror("scandir");
        return kBMNERROR;
    } else {
        for (Int_t i = 0; i < n; ++i) {
            if (regex_match(namelist[i]->d_name, re))
                ProcessFileRun(TString(namelist[i]->d_name));
            free(namelist[i]);
        }
        free(namelist);
    }
    return kBMNSUCCESS;
}

BmnStatus BmnMonitor::BatchList(TString* files, Int_t count) {
    _curFile = files[0];
    InitServer();
    InitDecoder();
    RegisterAll();
    for (Int_t i = 0; i < count; i++) {
        ProcessFileRun(files[i]);
    }
    return kBMNSUCCESS;
}

void BmnMonitor::InitServer() {
    if (gSystem->AccessPathName("auth.htdigest") != 0) {
        printf("Authorization file not found\nStarting server without authorization\n");
        fServer = new THttpServer("fastcgi:9000");
    } else
        fServer = new THttpServer("fastcgi:9000?auth_file=auth.htdigest&auth_domain=root");
    fServer->SetTimer(100, kTRUE);
    fServer->SetItemField("/", "_monitoring", "2000");
    fServer->SetItemField("/", "_layout", "grid3x3");
}

void BmnMonitor::InitDecoder() {
    rawDataDecoder = new BmnRawDataDecoder(_curDir + _curFile, 0, 5);
    rawDataDecoder->SetTrigMapping("Trig_map_Run5.txt");
    rawDataDecoder->SetTrigINLFile("TRIG_INL.txt");
    rawDataDecoder->SetTof400Mapping("TOF400_PlaceMap_Period5.txt", "TOF400_StripMap_Period5.txt");
    rawDataDecoder->SetTof700Mapping("TOF700_map_period_5.txt");
    rawDataDecoder->SetMwpcMapping("MWPC_mapping_period_5.txt");
    rawDataDecoder->InitConverter();
    rawDataDecoder->InitDecoder();
    fDigiTree = rawDataDecoder->GetDigiTree();
}

TString BmnMonitor::WatchNext(TString dirname, TString filename, Int_t cycleWait) {
    DBG("started")
            struct dirent **namelist;
    regex re("\\w+\\.data");
    Int_t n;
    TString ret;
    while (kTRUE) {
        n = scandir(dirname, &namelist, 0, versionsort);
        if (n < 0)
            perror("scandir");
        else {
            for (Int_t i = 0; i < n; ++i) {
                if (regex_match(namelist[i]->d_name, re))
                    ret = namelist[i]->d_name;
                free(namelist[i]);
            }
            free(namelist);
        }
        if (strcmp(filename.Strip().Data(), ret.Strip().Data()) != 0)
            return ret;
        fServer->ProcessRequests();
        gSystem->ProcessEvents();
        usleep(cycleWait);
    }
}

TString BmnMonitor::WatchNext(Int_t inotifDir, Int_t cycleWait) {
    TString fileName = "";
    Char_t evBuf[INOTIF_BUF_LEN];
    while (kTRUE) {
        Int_t len, i = 0;
        len = read(inotifDir, evBuf, INOTIF_BUF_LEN);
        if ((len == -1) && (errno != EAGAIN))
            DBG("inotify read error!")
        else {
            while (i < len) {
                struct inotify_event *event = (struct inotify_event*) &evBuf[i];
                if (event->len) {
                    if ((event->mask & IN_CREATE) && !(event->mask & IN_ISDIR)) {
                        fileName = TString(event->name);
                        printf("File %s was created!\n", fileName.Data());
                        break;
                    }
                }
                i += sizeof (struct inotify_event) +event->len;
                delete event;
            }
            if (cycleWait > 0)
                usleep(cycleWait);
            else
                break;
        }
    }
    return fileName;

}

BmnStatus BmnMonitor::CreateFile(Int_t runID) {
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
    if (bhTrig != NULL) delete bhTrig;
    bhGem = new BmnHistGem(refName + "GEM");
    bhDCH = new BmnHistDch(refName + "DCH");
    bhMWPC = new BmnHistMwpc(refName + "MWPC");
    bhToF400 = new BmnHistToF(refName + "ToF400");
    bhToF700 = new BmnHistToF700(refName + "ToF700");
    bhTrig = new BmnHistTrigger(refName + "Triggers");
    bhGem->SetDir(fHistOut, fRecoTree);
    bhDCH->SetDir(fHistOut, fRecoTree);
    bhMWPC->SetDir(fHistOut, fRecoTree);
    bhToF400->SetDir(fHistOut, fRecoTree);
    bhToF700->SetDir(fHistOut, fRecoTree);
    bhTrig->SetDir(fHistOut, fRecoTree);

    bhGem_4show->SetDir(NULL, fRecoTree4Show);
    bhDCH_4show->SetDir(NULL, fRecoTree4Show);
    bhMWPC_4show->SetDir(NULL, fRecoTree4Show);
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
    bhTrig_4show->Reset();
    bhGem_4show->Reset();
}

BmnStatus BmnMonitor::OpenStream() {
    //    dataReceiver = new BmnDataReceiver();
    //    rawDataDecoder = new BmnRawDataDecoder();
    //    fDataQue = &(dataReceiver->data_queue);
    //    rawDataDecoder->SetTrigMapping("Trig_map_Run5.txt");
    //    //    FILE *data_stream = istream_iterator<UInt_t>(data_queue);
    //    //    istream<UInt_t> qstream(data_queue);
    //    //    rdd->SetRawFileIn(data_stream);
    //    rawDataDecoder->SetRunId(84);
    //    rawDataDecoder->InitConverter(fDataQue);
    //    rawDataDecoder->InitDecoder();



    //    fDigiTree = rawDataDecoder->GetDigiTree();
}

void BmnMonitor::ProcessStreamRun() {
    Int_t iEv = 0;
    BmnStatus convertResult;

    OpenStream();
    RegisterAll();
    thread rcvThread(threadReceiveWrapper, dataReceiver);

    while (kTRUE && iEv < 100) {

        convertResult = rawDataDecoder->ConvertRawToRootIterate();
        if (convertResult == kBMNTIMEOUT) {
            printf("Connection timeout!");
            break;
        }
        rawDataDecoder->DecodeDataToDigiIterate();
        ProcessDigi(iEv++);
        fServer->ProcessRequests();
        gSystem->ProcessEvents();
    }
    rcvThread.join();

    //    rawDataDecoder->DisposeDecoder();
    //    rawDataDecoder->DisposeConverter();
    delete dataReceiver;
    delete rawDataDecoder;
    FinishRun();

}

void BmnMonitor::ProcessFileRun(TString rawFileName) {
    printf("File %s \n", TString(_curDir + rawFileName).Data());
    Int_t iEv = 0;
    Int_t lastEv = 0;
    TString nextFile;
    BmnStatus convertResult = kBMNSUCCESS;

    const UInt_t kRUNNUMBERSYNC = 0x236E7552;
    const size_t kWORDSIZE = sizeof (UInt_t);
    const Short_t kNBYTESINWORD = 4;
    Int_t runId = -1;
    FILE * file = fopen(TString(_curDir + rawFileName).Data(), "rb");
    if (file == NULL) {
        printf("File %s is not open!!!\n", TString(_curDir + rawFileName).Data());
        return;
    }
    UInt_t word;
    while (fread(&word, kWORDSIZE, 1, file)) {
        if (word == kRUNNUMBERSYNC) {
            fread(&word, kWORDSIZE, 1, file); //skip word
            fread(&runId, kWORDSIZE, 1, file);
            break;
        }
    }
    fclose(file);
    printf("run id = %d\n", runId);

    rawDataDecoder->ResetDecoder(_curDir + rawFileName);
    CreateFile(rawDataDecoder->GetRunId());

    while (kTRUE) {
        convertResult = rawDataDecoder->ConvertRawToRootIterateFile();
        if (convertResult == kBMNFINISH) {
            printf("finish\n");
            //_curFile = "";
            break;
        }
        fServer->ProcessRequests();
        gSystem->ProcessEvents();
        lastEv = iEv;
        iEv = rawDataDecoder->GetEventId();
        if (iEv > lastEv) {
            rawDataDecoder->DecodeDataToDigiIterate();
            ProcessDigi(iEv);
        }
        if (convertResult == kBMNTIMEOUT) {
            printf("timeout\n");
            //_curFile = "";
            break;
        }
    }
    FinishRun();
}

void BmnMonitor::ProcessDigi(Int_t iEv) {
    //    fDigiTree->GetEntry(iEv);
    // histograms fill//
    //    DBG("started")
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
    //    bhGem_4show->FillFromDigiMasked(fDigiArrays->gem, &(bhGem->histGemStrip), iEv);
    bhToF400_4show->FillFromDigi(fDigiArrays->tof400);
    bhToF700_4show->FillFromDigi(fDigiArrays->tof700);
    bhDCH_4show->FillFromDigi(fDigiArrays->dch);
    bhMWPC_4show->FillFromDigi(fDigiArrays->mwpc);
    fRecoTree4Show->Fill();
    if (head->GetEventId() % 100 == 0) {
        bhGem_4show->DrawBoth();
        bhDCH_4show->DrawBoth();
        bhMWPC_4show->DrawBoth();
        bhTrig_4show->DrawBoth();
        bhToF400_4show->DrawBoth();
        bhToF700_4show->DrawBoth();
    }
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
    //    if ((iEv % itersToUpdate == 0) && (iEv > 1)) {
    ////        bhGem->UpdateNoiseMask(0.5 * iEv);
    ////        bhGem_4show->ApplyNoiseMask(bhGem->GetNoiseMask());
    //        bhGem_4show->ApplyNoiseMask(&(bhGem->histGemStrip), 0.5 * iEv);
    //        cout << " mask " << (*(bhGem->GetNoiseMask()))[0][0][0][199] << endl;
    //    }
    //    gSystem->ProcessEvents();
    //    fServer->ProcessRequests();
    //            usleep(1e5);
}

void BmnMonitor::RegisterAll() {
    // histograms init //
    bhGem_4show = new BmnHistGem("GEM", _curDir);
    bhDCH_4show = new BmnHistDch("DCH");
    bhMWPC_4show = new BmnHistMwpc("MWPC");
    bhToF400_4show = new BmnHistToF("ToF400");
    bhToF700_4show = new BmnHistToF700("ToF700");
    bhTrig_4show = new BmnHistTrigger("Triggers");

    fServer->Register("/", infoCanvas);
    bhGem_4show->Register(fServer);
    bhDCH_4show->Register(fServer);
    bhMWPC_4show->Register(fServer);
    bhToF400_4show->Register(fServer);
    bhToF700_4show->Register(fServer);
    bhTrig_4show->Register(fServer);
    fServer->Register("/", refList);
    DBG("histograms registered")

    bhGem_4show->SetRefPath(_refDir);
    bhDCH_4show->SetRefPath(_refDir);
    bhMWPC_4show->SetRefPath(_refDir);
    bhToF400_4show->SetRefPath(_refDir);
    bhToF700_4show->SetRefPath(_refDir);
    bhTrig_4show->SetRefPath(_refDir);
}

void BmnMonitor::UpdateRuns() {
    struct dirent **namelist;
    regex re("bmn_run0*(\\d+)_hist.root");
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
        cmd = string("chmod 775 ") + fHistOut->GetName();
        printf("system result = %d\n", system(cmd.c_str()));

        cmd = string("hadd -f shorter.root ") + fHistOut->GetName() +
                string("; mv shorter.root ") + fHistOut->GetName();
           printf("system result = %d\n", system(cmd.c_str()));
        //std::thread threadHAdd(BmnMonitor::threadCmdWrapper, cmd);
        //if (threadHAdd.joinable())
        //    threadHAdd.detach();
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

void BmnMonitor::threadReceiveWrapper(BmnDataReceiver* dr) {
    dr->ConnectRaw();
}

void BmnMonitor::threadDecodeWrapper(TString dirname, TString startFile, Bool_t runCurrent) {

    BmnOnlineDecoder *deco = new BmnOnlineDecoder();
    deco->Decode(dirname, startFile, runCurrent);
    //    deco->BatchDirectory(dirname);
}

void BmnMonitor::threadCmdWrapper(string cmd) {
    printf("system result = %d\n", system(cmd.c_str()));
}
ClassImp(BmnMonitor);
