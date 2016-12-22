
//#include <pthread.h>
#include <thread>
#include <mutex>
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

#define INOTIF_BUF_LEN (255 * (sizeof(struct inotify_event) + 255))

BmnMonitor::BmnMonitor() {
    _fileList = new vector<BmnRunInfo>();
    fRecoTree = NULL;
    fRecoTree4Show = NULL;
    fHistOut = NULL;
    fServer = NULL;
    TString name = "infoCanvas";
    infoCanvas = new TCanvas(name, name);
}

BmnMonitor::~BmnMonitor() {

    delete bhGem;
    delete bhToF400;
    delete bhToF700;
    delete bhDCH;
    delete bhMWPC;
    delete bhTrig;

    //    delete fRecoTree;
    fRecoTree4Show->Clear();
    delete fRecoTree4Show;
    delete fHistOut;

    fServer->Unregister(infoCanvas);
    delete infoCanvas;

    delete bhGem_4show;
    delete bhToF400_4show;
    delete bhToF700_4show;
    delete bhDCH_4show;
    delete bhMWPC_4show;
    delete bhTrig_4show;
    delete fServer;
    rawDataDecoder->DisposeDecoder();
    rawDataDecoder->DisposeConverter();
    delete rawDataDecoder;
    delete _fileList;
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
        printf("first wn returned %s\n", _curFile.Data());
        _curFile = WatchNext(_curDir, _curFile, RUN_FILE_CHECK_PERIOD);
        printf("found new file %s\n", _curFile.Data());
    } else
        if (_curFile.Length() == 0) {
        _curFile = WatchNext(_curDir, _curFile, RUN_FILE_CHECK_PERIOD);
        printf("WN returned %s\n", _curFile.Data());
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

BmnStatus BmnMonitor::BatchDirectory(TString dirname) {
    _curDir = dirname;
    TSystemDirectory dir0(_curDir, _curDir);
    TList *files0 = dir0.GetListOfFiles();
    if (files0) {
        files0->Sort(kSortAscending);
        TSystemFile *file0;
        TString fname0;
        TIter next0(files0);
        while ((file0 = (TSystemFile*) next0())) {
            fname0 = TString(file0->GetName());
            if (!file0->IsDirectory() && fname0.EndsWith("data")) {
                _curFile = fname0;
                break;
            }
        }
        delete file0;
        //        delete files0;
    } else
        return kBMNERROR;

    InitServer();
    InitDecoder();
    RegisterAll();
    TSystemDirectory dir(_curDir, _curDir);
    TList *files = dir.GetListOfFiles();
    if (files) {
        files->Sort(kSortAscending);
        TSystemFile *file;
        TString fname, retFname;
        TIter next(files);
        while ((file = (TSystemFile*) next())) {
            fname = TString(file->GetName());
            printf("fname = %s\n", fname.Data());
            if (!file->IsDirectory() && fname.EndsWith("data"))
                ProcessFileRun(fname);
        }
        delete file;
        delete files;
    } else
        return kBMNERROR;
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
    TSystemDirectory dir(dirname, dirname);
    while (kTRUE) {
        TList *files = dir.GetListOfFiles();
        if (files) {
            files->Sort(kSortAscending);
            TSystemFile *file;
            TString fname, retFname;
            TIter next(files);
            while ((file = (TSystemFile*) next())) {
                fname = TString(file->GetName());
                if (!file->IsDirectory() && fname.EndsWith("data"))
                    retFname = fname;
            }
            delete file;
            delete files;
            if (strcmp(filename.Strip().Data(), retFname.Strip().Data()) != 0)
                return retFname;
        }
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

void BmnMonitor::CheckFileTime(TString Dir, vector<BmnRunInfo>* FileList) {
    if (FileList == NULL)
        FileList = new vector<BmnRunInfo>();
    else
        FileList->clear();
}

BmnStatus BmnMonitor::OpenFile(TString rawFileName) {
    DBG("opening file")
    TString outHistName = Form("bmn_run%04d_hist.root", runIndex);
    fHistOut = new TFile(outHistName, "recreate");
    fRecoTree = new TTree("BmnMon", "BmnMon");
    fRecoTree->SetMaxTreeSize(TTREE_MAX_SIZE); // file will not be divided
    fRecoTree4Show = new TTree("BmnMon4Show", "BmnMon");
    fRecoTree4Show->SetDirectory(NULL); // tree will not be saved
    bhGem->SetDir(fHistOut, fRecoTree);
    bhDCH->SetDir(fHistOut, fRecoTree);
    bhMWPC->SetDir(fHistOut, fRecoTree);
    bhToF400->SetDir(fHistOut, fRecoTree);
    bhToF700->SetDir(fHistOut, fRecoTree);
    bhTrig->SetDir(fHistOut, fRecoTree);
    DBG("directory set for saved hist")

    bhGem_4show->SetDir(NULL, fRecoTree4Show);
    bhDCH_4show->SetDir(NULL, fRecoTree4Show);
    bhMWPC_4show->SetDir(NULL, fRecoTree4Show);
    bhToF400_4show->SetDir(NULL, fRecoTree4Show);
    bhToF700_4show->SetDir(NULL, fRecoTree4Show);
    bhTrig_4show->SetDir(NULL, fRecoTree4Show);
    
    bhToF400->Reset();
    bhToF700->Reset();
    bhDCH->Reset();
    bhMWPC->Reset();
    bhTrig->Reset();
    bhGem->Reset();

    bhToF400_4show->Reset();
    bhToF700_4show->Reset();
    bhDCH_4show->Reset();
    bhMWPC_4show->Reset();
    bhTrig_4show->Reset();
    bhGem_4show->Reset();
    DBG("directory set for show hists")
}

BmnStatus BmnMonitor::OpenStream() {
    dataReceiver = new BmnDataReceiver();
    rawDataDecoder = new BmnRawDataDecoder();
    fDataQue = &(dataReceiver->data_queue);
    //    dataReceiver-> SetQueMutex(fDataMutex);
    //    rawDataDecoder->SetQueMutex(fDataMutex);
    rawDataDecoder->SetTrigMapping("Trig_map_Run5.txt");
    //    FILE *data_stream = istream_iterator<UInt_t>(data_queue);
    //    istream<UInt_t> qstream(data_queue);
    //    rdd->SetRawFileIn(data_stream);
    rawDataDecoder->SetRunId(84);
    rawDataDecoder->InitConverter(fDataQue);
    rawDataDecoder->InitDecoder();
    fDigiTree = rawDataDecoder->GetDigiTree();
}

void BmnMonitor::ProcessStreamRun() {
    Int_t iEv = 0;
    BmnStatus convertResult;

    OpenStream();
    RegisterAll();
    thread rcvThread(threadWrapper, dataReceiver);

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
    runIndex = TString(rawFileName(rawFileName.Length() - 8, 3)).Atoi();
    Int_t iEv = 0;
    Int_t lastEv = 0;
    TString nextFile;
    BmnStatus convertResult = kBMNSUCCESS;
    OpenFile(rawFileName);
    rawDataDecoder->ResetDecoder(_curDir + rawFileName);

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
    fDigiTree->GetEntry(iEv);
    // histograms fill//
    BmnEventHeader* head = (BmnEventHeader*) fDigiArrays.header->At(0);
    bhTrig->FillFromDigi(
            fDigiArrays.bc1,
            fDigiArrays.t0,
            fDigiArrays.bc2,
            fDigiArrays.veto,
            fDigiArrays.fd,
            fDigiArrays.bd);
    bhGem->FillFromDigi(fDigiArrays.gem);
    bhToF400->FillFromDigi(fDigiArrays.tof400);
    bhToF700->FillFromDigi(fDigiArrays.tof700);
    bhDCH->FillFromDigi(fDigiArrays.dch);
    bhMWPC->FillFromDigi(fDigiArrays.mwpc);
    // Fill data Tree //
    fRecoTree->Fill();
    // fill histograms what will be shown on the site//
    bhTrig_4show->FillFromDigi(
            fDigiArrays.bc1,
            fDigiArrays.t0,
            fDigiArrays.bc2,
            fDigiArrays.veto,
            fDigiArrays.fd,
            fDigiArrays.bd);
    bhGem_4show->FillFromDigi(fDigiArrays.gem);
//    bhGem_4show->FillFromDigiMasked(fDigiArrays.gem, &(bhGem->histGemStrip), iEv, head);
    bhToF400_4show->FillFromDigi(fDigiArrays.tof400);
    bhToF700_4show->FillFromDigi(fDigiArrays.tof700);
    bhDCH_4show->FillFromDigi(fDigiArrays.dch);
    bhMWPC_4show->FillFromDigi(fDigiArrays.mwpc);
    fRecoTree4Show->Fill();
    // print info canvas //
    infoCanvas->Clear();
    infoCanvas->cd(1);
    TString  runType;
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
        Tl.DrawLatex(0.5, 0.9, Form("Run: %04d", rawDataDecoder->GetRunId()));
        Tl.DrawLatex(0.5, 0.6, Form("Event: %d", rawDataDecoder->GetEventId()));
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
    fDigiArrays = rawDataDecoder->GetDigiArrays();
    //    header = (TClonesArray*)(fDigiTree->GetBranch("EventHeader"));
    //    gemDigits = fDigiArrays.gem;//(TClonesArray*)(fDigiTree->GetBranch("GEM"));
    //    ToF4Digits = fDigiArrays.gem;//(TClonesArray*)(fDigiTree->GetBranch("TOF400"));
    //    ToF7Digits = fDigiArrays.gem;//(TClonesArray*)fDigiTree->GetBranch("TOF700");
    //    DchDigits = fDigiArrays.dch;//(TClonesArray*)fDigiTree->GetBranch("DCH");
    //    trigBC1Digits = fDigiArrays.bc1;//(TClonesArray*)fDigiTree->GetBranch("BC1");
    //    trigBC2Digits = fDigiArrays.bc2;//(TClonesArray*)fDigiTree->GetBranch("BC2");
    //    trigVDDigits = fDigiArrays.veto;//(TClonesArray*)fDigiTree->GetBranch("VETO");
    // digiTree->GetBranch("BD", &trigBDDigits);
    //    digiTree->GetBranch("GEM", &trigBC1Digits);
    //    digiTree->GetBranch("GEM", &trigBC1Digits);
    // histograms init //
    bhGem = new BmnHistGem("GEM");
    bhDCH = new BmnHistDch("DCH");
    bhMWPC = new BmnHistMwpc("MWPC");
    bhToF400 = new BmnHistToF("ToF400");
    bhToF700 = new BmnHistToF700("ToF700");
    bhTrig = new BmnHistTrigger("Triggers");

    bhGem_4show = new BmnHistGem("GEM_");
    bhDCH_4show = new BmnHistDch("DCH_");
    bhMWPC_4show = new BmnHistMwpc("MWPC_");
    bhToF400_4show = new BmnHistToF("ToF400_");
    bhToF700_4show = new BmnHistToF700("ToF700_");
    bhTrig_4show = new BmnHistTrigger("Triggers_");

    fServer->Register("/", infoCanvas);
    bhGem_4show->Register(fServer);
    bhDCH_4show->Register(fServer);
    bhMWPC_4show->Register(fServer);
    bhToF400_4show->Register(fServer);
    bhToF700_4show->Register(fServer);
    bhTrig_4show->Register(fServer);
    DBG("histograms registered")
}

void BmnMonitor::FinishRun() {
    DBG("started")
//    bhGem->SetDir(NULL, fRecoTree);
//    bhToF400->SetDir(NULL, fRecoTree);
//    bhToF700->SetDir(NULL, fRecoTree);
//    bhDCH->SetDir(NULL, fRecoTree);
//    bhMWPC->SetDir(NULL, fRecoTree);
//    bhTrig->SetDir(NULL, fRecoTree);
    fRecoTree->Write();
    fHistOut->Write();
    //    fHistOut->Close();
    //    fRecoTree->Clear();
    //    delete fRecoTree;
    fRecoTree4Show->Clear();
    delete fRecoTree4Show;
    string cmd;
    cmd = string("chmod 775 ") + fHistOut->GetName();
    system(cmd.c_str());

    cmd = string("hadd -f shorter.root ") + fHistOut->GetName() +
            string("; mv shorter.root ") + fHistOut->GetName();

    system(cmd.c_str());
}

void BmnMonitor::threadWrapper(BmnDataReceiver* dr) {
    dr->ConnectRaw();
}

void BmnMonitor::threadDecodeWrapper(BmnRawDataDecoder* rdd) {
}

ClassImp(BmnMonitor);
