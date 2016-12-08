
//#include <pthread.h>
#include <thread>
#include <mutex>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <chrono>
#include <ctime>

#include "BmnMonitor.h"

BmnMonitor::BmnMonitor() {
    //    if (pthread_mutex_init((pthread_mutex_t*)fDataMutex, NULL))
    //        printf("Error initializing mutex!\n");
//    fDataMutex = new mutex();
    _fileList = new vector<BmnRunInfo>();
}

BmnMonitor::~BmnMonitor() {
    //    pthread_mutex_destroy((pthread_mutex_t*) fDataMutex);
//    delete (mutex*) fDataMutex;
    delete fServer;
    delete fHistOut;
    
    delete bhGem;
    delete bhToF400;
    delete bhToF700;
    delete bhDCH;
    delete bhTrig;
   
    delete bhGem_4show;
    delete bhToF400_4show;
    delete bhToF700_4show;
    delete bhDCH_4show;
    delete bhTrig_4show;
    //    delete fRecoTree;
    rawDataDecoder->DisposeDecoder();
    rawDataDecoder->DisposeConverter();
    delete rawDataDecoder;
    delete _fileList;
}


void BmnMonitor::Monitor() {
    // Create server //
   if (gSystem->AccessPathName("auth.htdigest")!=0) {
      printf("Authorization file not found\n");
      return;
   }
    fServer = new THttpServer("fastcgi:9000?auth_file=auth.htdigest&auth_domain=root");
    fServer->SetTimer(100, kTRUE);
//    fServer->SetItemField("/", "_monitoring","2000");
//    fServer->SetItemField("/", "_layout","grid3x3");
//    fServer->Restrict("/", "visible=all");
    
    rawDataDecoder = new BmnRawDataDecoder("mpd_run_067.data", 0);
    rawDataDecoder->SetTrigMapping("Trig_map_Run4.txt");
    rawDataDecoder->InitConverter();
    rawDataDecoder->InitDecoder();
    fDigiTree = rawDataDecoder->GetDigiTree();
    RegisterAll();
    BmnRunInfo test, test2, test3;
    test.Name =  TString(getenv("VMCWORKDIR")) + "/build/mpd_run_067.data";
    test2.Name = TString(getenv("VMCWORKDIR")) + "/build/mpd_run_081.data";
    test3.Name = TString(getenv("VMCWORKDIR")) + "/build/mpd_run_065.data";// 65
    cout << test.Name << endl;
//    FileList->push_back(test);
//    FileList->push_back(test2);
    _fileList->push_back(test3);

    for (auto f : *_fileList) {
        stat(f.Name, &(f.attr));
    }
//    for (Int_t i = 0; i < 10; i++) {
        for (auto f : *_fileList) {
            ProcessFileRun(f.Name);
        }
//    }

}

void BmnMonitor::CheckFileTime(TString Dir, vector<BmnRunInfo>* FileList){
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
    rawDataDecoder->SetTrigMapping("Trig_map_Run4.txt");
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
    BmnStatus convertResult = kBMNSUCCESS;

    OpenFile(rawFileName);
    rawDataDecoder->ResetDecoder(rawFileName);

    while (kTRUE && convertResult != kBMNFINISH) {
        convertResult = rawDataDecoder->ConvertRawToRootIterateFile();
        rawDataDecoder->DecodeDataToDigiIterate();
        lastEv = iEv;
        iEv = rawDataDecoder->GetNevents() - 1;
        if (iEv > lastEv)
            ProcessDigi(iEv);
    }
    FinishRun();

}

void BmnMonitor::ProcessDigi(Int_t iEv) {
    fDigiTree->GetEntry(iEv);
    // histograms fill//
    bhTrig->FillFromDigi(
            fDigiArrays.bc1,
            NULL,
            fDigiArrays.bc2,
            fDigiArrays.veto,
            NULL,
            NULL);
    bhGem->FillFromDigi(fDigiArrays.gem);
    bhToF400->FillFromDigi(fDigiArrays.tof400);
    bhToF700->FillFromDigi(fDigiArrays.tof700);
    bhDCH->FillFromDigi(fDigiArrays.dch);
    // Fill data Tree //
    fRecoTree->Fill();
    // fill histograms what will be showed on the site//
    bhTrig_4show->FillFromDigi(
            fDigiArrays.bc1,
            NULL,
            fDigiArrays.bc2,
            fDigiArrays.veto,
            NULL,
            NULL);
    bhGem_4show->FillFromDigiMasked(fDigiArrays.gem, &(bhGem->histGemStrip), iEv);
    bhToF400_4show->FillFromDigi(fDigiArrays.tof400);
    bhToF700_4show->FillFromDigi(fDigiArrays.tof700);
    bhDCH_4show->FillFromDigi(fDigiArrays.dch);
    
//    if ((iEv % itersToUpdate == 0) && (iEv > 1)) {
////        bhGem->UpdateNoiseMask(0.5 * iEv);
////        bhGem_4show->ApplyNoiseMask(bhGem->GetNoiseMask());
//        bhGem_4show->ApplyNoiseMask(&(bhGem->histGemStrip), 0.5 * iEv);
//        cout << " mask " << (*(bhGem->GetNoiseMask()))[0][0][0][199] << endl;
//    }
    gSystem->ProcessEvents();
    fServer->ProcessRequests();
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
    cmd += string("hadd -f shorter.root ") + fHistOut->GetName() +
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
    
    system (cmd.c_str());
}

void BmnMonitor::threadWrapper(BmnDataReceiver* dr) {
    dr->ConnectRaw();
}

void BmnMonitor::threadDecodeWrapper(BmnRawDataDecoder* rdd) {
}

ClassImp(BmnMonitor);