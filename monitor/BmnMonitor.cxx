
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

#include "BmnMonitor.h"

#define INOTIF_BUF_LEN (255 * (sizeof(struct inotify_event) + 255))

BmnMonitor::BmnMonitor() {
    _fileList = new vector<BmnRunInfo>();
    fRecoTree = NULL;
    fHistOut = NULL;
    fServer = NULL;
}

BmnMonitor::~BmnMonitor() {

    delete bhGem;
    delete bhToF400;
    delete bhToF700;
    delete bhDCH;
    delete bhTrig;
    //    delete fRecoTree;
    delete fHistOut;

    delete bhGem_4show;
    delete bhToF400_4show;
    delete bhToF700_4show;
    delete bhDCH_4show;
    delete bhTrig_4show;
    delete fServer;
    rawDataDecoder->DisposeDecoder();
    rawDataDecoder->DisposeConverter();
    delete rawDataDecoder;
    delete _fileList;
}

void BmnMonitor::Monitor(TString dir, TString startFile) {
    // Create server //
    if (gSystem->AccessPathName("auth.htdigest") != 0) {
        printf("Authorization file not found\n");
        return;
    }
    fServer = new THttpServer("fastcgi:9000?auth_file=auth.htdigest&auth_domain=root");
    fServer->SetTimer(100, kTRUE);
    fServer->SetItemField("/", "_monitoring", "2000");
    fServer->SetItemField("/", "_layout", "grid3x3");

    _inotifDir = inotify_init();
    _inotifDirW = inotify_add_watch(_inotifDir, dir, IN_CREATE);
    Int_t flags = fcntl(_inotifDir, F_GETFL, 0);
    fcntl(_inotifDir, F_SETFL, flags | O_NONBLOCK);
    _curFile = startFile;
    _curDir = dir;
    if (_curFile.Length() == 0) {
        _curFile = WatchNext(dir, _curFile, 1e5);
        //        _curFile = WatchNext(_inotifDir, 1e5);
    }



    _inotifFile = inotify_init();
    _inotifFileW = inotify_add_watch(_inotifFile, _curFile, IN_MODIFY);


    rawDataDecoder = new BmnRawDataDecoder(startFile, 0, 5);
    rawDataDecoder->SetRunId(1);
    rawDataDecoder->SetTrigMapping("Trig_map_Run5.txt");
    rawDataDecoder->SetTrigINLFile("TRIG_INL.txt");
    rawDataDecoder->SetTof700Mapping("TOF700_map_period_5.txt");
    rawDataDecoder->InitConverter();
    rawDataDecoder->InitDecoder();
    fDigiTree = rawDataDecoder->GetDigiTree();
    RegisterAll();

    while (kTRUE) {
        ProcessFileRun(_curFile);
        _curFile = WatchNext(dir, _curFile, 1e5);
    }
    //    BmnRunInfo test, test2, test3;
    //    test.Name =  TString(getenv("VMCWORKDIR")) + "/build/mpd_run_067.data";
    //    test2.Name = TString(getenv("VMCWORKDIR")) + "/build/mpd_run_081.data";
    //    test3.Name = "/home/ilnur/mnt/test/mpd-evb/TrigWord/mpd_run_Glob_306.data";// 65
    //    cout << test.Name << endl;
    ////    FileList->push_back(test);
    ////    FileList->push_back(test2);
    //    _fileList->push_back(test3);
    //
    //    for (auto f : *_fileList) {
    //        stat(f.Name, &(f.attr));
    //    }
    ////    for (Int_t i = 0; i < 10; i++) {
    //        for (auto f : *_fileList) {
    //            ProcessFileRun(f.Name);
    //        }
    ////    }

    inotify_rm_watch(_inotifDir, _inotifDirW);
    close(_inotifDir);
    close(_inotifFile);
}

TString BmnMonitor::WatchNext(TString dirname, TString filename, Int_t cycleWait) {
    TSystemDirectory dir(dirname, dirname);
    TList *files = dir.GetListOfFiles();
    while (kTRUE) {
        if (files) {
            files->Sort(kSortDescending);
            TSystemFile *file;
            TString fname;
            TIter next(files);
            while ((file = (TSystemFile*) next())) {
                if (!file->IsDirectory() && fname.EndsWith("data"))
                    fname = file->GetName();
            }
            if (filename != fname)
                return fname;
        }
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
    //    struct dirent

}

BmnStatus BmnMonitor::OpenFile(TString rawFileName) {
    DBG("opening file")
    TString outHistName = Form("bmn_run%04d_hist.root", runIndex);
    fHistOut = new TFile(outHistName, "recreate");
    DBG("file created")
    fRecoTree = new TTree("BmnMon", "BmnMon");
    bhGem->SetDir(fHistOut, fRecoTree);
    bhDCH->SetDir(fHistOut, fRecoTree);
    bhToF400->SetDir(fHistOut, fRecoTree);
    bhToF700->SetDir(fHistOut, fRecoTree);
    bhTrig->SetDir(fHistOut, fRecoTree);
    DBG("directory set for saved hist")

    bhGem_4show->SetDir(NULL, NULL);
    bhDCH_4show->SetDir(NULL, NULL);
    bhToF400_4show->SetDir(NULL, NULL);
    bhToF700_4show->SetDir(NULL, NULL);
    bhTrig_4show->SetDir(NULL, NULL);
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

    rawDataDecoder->DisposeDecoder();
    rawDataDecoder->DisposeConverter();
    delete dataReceiver;
    delete rawDataDecoder;
    FinishRun();

}

void BmnMonitor::ProcessFileRun(TString rawFileName) {
    runIndex = TString(rawFileName(rawFileName.Length() - 8, 3)).Atoi();
    Int_t iEv = -1;
    Int_t lastEv = -1;
    TString nextFile;
    BmnStatus convertResult = kBMNSUCCESS;

    OpenFile(rawFileName);
    rawDataDecoder->ResetDecoder(_curDir + rawFileName);

    while (kTRUE) {
        convertResult = rawDataDecoder->ConvertRawToRootIterateFile();
        fServer->ProcessRequests();
        gSystem->ProcessEvents();
        lastEv = iEv;
        iEv = rawDataDecoder->GetNevents() - 1;
        if (iEv > lastEv) {
            rawDataDecoder->DecodeDataToDigiIterate();
            ProcessDigi(iEv);
        }
        if (convertResult == kBMNTIMEOUT) {
            //_curFile = "";
            break;
        }
        if (convertResult == kBMNFINISH) {
            //_curFile = "";
            break;
        }
        //        nextFile = WatchNext(_inotifDir, 0);
        //        if (nextFile.Length() > 0){
        //            inotify_rm_watch(_inotifFile, _inotifFileW);
        //            _curFile = nextFile;
        //            break;
        //        }
    }
    FinishRun();
}

void BmnMonitor::ProcessDigi(Int_t iEv) {
    fDigiTree->GetEntry(iEv);
    // histograms fill//
    bhTrig->FillFromDigi(
            fDigiArrays.bc1,
            fDigiArrays.t0,
            fDigiArrays.bc2,
            fDigiArrays.veto,
            fDigiArrays.fd,
            fDigiArrays.bd,
            (BmnEventHeader*) (fDigiArrays.header->At(0)), iEv);
    bhGem->FillFromDigi(fDigiArrays.gem);
    bhToF400->FillFromDigi(fDigiArrays.tof400);
    bhToF700->FillFromDigi(fDigiArrays.tof700);
    bhDCH->FillFromDigi(fDigiArrays.dch);
    // Fill data Tree //
    fRecoTree->Fill();
    // fill histograms what will be shown on the site//
    bhTrig_4show->FillFromDigi(
            fDigiArrays.bc1,
            fDigiArrays.t0,
            fDigiArrays.bc2,
            fDigiArrays.veto,
            fDigiArrays.fd,
            fDigiArrays.bd,
            (BmnEventHeader*) (fDigiArrays.header->At(0)), iEv);
    bhGem_4show->FillFromDigiMasked(fDigiArrays.gem, &(bhGem->histGemStrip), iEv, (BmnEventHeader*) (fDigiArrays.header->At(0)));
    bhToF400_4show->FillFromDigi(fDigiArrays.tof400);
    bhToF700_4show->FillFromDigi(fDigiArrays.tof700);
    bhDCH_4show->FillFromDigi(fDigiArrays.dch);

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
    bhToF400 = new BmnHistToF("ToF400");
    bhToF700 = new BmnHistToF700("ToF700");
    bhTrig = new BmnHistTrigger("Triggers");

    bhGem_4show = new BmnHistGem("GEM_");
    bhDCH_4show = new BmnHistDch("DCH_");
    bhToF400_4show = new BmnHistToF("ToF400_");
    bhToF700_4show = new BmnHistToF700("ToF700_");
    bhTrig_4show = new BmnHistTrigger("Triggers_");


    bhGem_4show->Register(fServer);
    bhDCH_4show->Register(fServer);
    bhToF400_4show->Register(fServer);
    bhToF700_4show->Register(fServer);
    bhTrig_4show->Register(fServer);
    DBG("histograms registered")
}

void BmnMonitor::FinishRun() {
    fRecoTree->Write();
    fHistOut->Write();
    fHistOut->GetName();
    string cmd;
    cmd = string("chmod 775 ") + fHistOut->GetName();
    system(cmd.c_str());

    cmd = string("hadd -f shorter.root ") + fHistOut->GetName() +
            string("; mv shorter.root ") + fHistOut->GetName();
    //    bhGem->SetDir(NULL, fRecoTree);
    //    bhToF400->SetDir(NULL, fRecoTree);
    //    bhToF700->SetDir(NULL, fRecoTree);
    //    bhDCH->SetDir(NULL, fRecoTree);
    //    bhTrig->SetDir(NULL, fRecoTree);
    ////    fHistOut->Close();
    //    
    bhToF400->Reset();
    bhToF700->Reset();
    bhDCH->Reset();
    bhTrig->Reset();
    bhGem->Reset();

    bhToF400_4show->Reset();
    bhToF700_4show->Reset();
    bhDCH_4show->Reset();
    bhTrig_4show->Reset();
    bhGem_4show->Reset();

    system(cmd.c_str());
}

void BmnMonitor::threadWrapper(BmnDataReceiver* dr) {
    dr->ConnectRaw();
}

void BmnMonitor::threadDecodeWrapper(BmnRawDataDecoder* rdd) {
}

ClassImp(BmnMonitor);
