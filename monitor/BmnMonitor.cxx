
//#include <pthread.h>
#include <thread>
#include <mutex>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "BmnMonitor.h"

BmnMonitor::BmnMonitor() {
    //    if (pthread_mutex_init((pthread_mutex_t*)fDataMutex, NULL))
    //        printf("Error initializing mutex!\n");
    fDataMutex = new mutex();
}

BmnMonitor::~BmnMonitor() {
    //    pthread_mutex_destroy((pthread_mutex_t*) fDataMutex);
    delete (mutex*) fDataMutex;
    delete fServer;

    delete fHistOut;
    delete bhGem;
    delete bhToF400;
    delete bhToF700;
    delete bhDCH1;
    delete bhTrig;
    //    delete fRecoTree;
    rawDataDecoder->DisposeDecoder();
    rawDataDecoder->DisposeConverter();
    delete rawDataDecoder;
}


struct BmnRunInfo {
    TString Name;
    struct stat attr;
};

void BmnMonitor::Monitor() {
    // Create server //
    fServer = new THttpServer("fastcgi:9000");
    fServer->SetTimer(100, kTRUE);
    vector<BmnRunInfo> FileList;
    rawDataDecoder = new BmnRawDataDecoder("mpd_run_067.data", 0);
    rawDataDecoder->SetTrigMapping("Trig_map_Run4.txt");
    rawDataDecoder->InitConverter();
    rawDataDecoder->InitDecoder();
    fDigiTree = rawDataDecoder->GetDigiTree();
    RegisterAll();
    BmnRunInfo test, test2;
    test.Name =  TString(getenv("VMCWORKDIR")) + "/build/mpd_run_067.data";
    test2.Name = TString(getenv("VMCWORKDIR")) + "/build/mpd_run_081.data";
    cout << test.Name << endl;
    FileList.push_back(test);
    FileList.push_back(test2);

    for (auto f : FileList) {
        stat(f.Name, &(f.attr));
    }
//    for (Int_t i = 0; i < 10; i++) {
        for (auto f : FileList) {
            ProcessFileRun(f.Name);
        }
//    }

}

BmnStatus BmnMonitor::OpenFile(TString rawFileName) {
    TString outHistName = Form("bmn_run%04d_hist.root", runIndex);
    fHistOut = new TFile(outHistName, "recreate");
    fRecoTree = new TTree("BmnMon", "BmnMon");
    bhGem->SetDir(fHistOut, fRecoTree);
    bhToF400->SetDir(fHistOut, fRecoTree);
    bhToF700->SetDir(fHistOut, fRecoTree);
    bhDCH1->SetDir(fHistOut, fRecoTree);
    bhTrig->SetDir(fHistOut, fRecoTree);
}

BmnStatus BmnMonitor::OpenStream() {
    dataReceiver = new BmnDataReceiver();
    rawDataDecoder = new BmnRawDataDecoder();
    fDataQue = &(dataReceiver->data_queue);
    dataReceiver-> SetQueMutex(fDataMutex);
    rawDataDecoder->SetQueMutex(fDataMutex);
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
        iEv = rawDataDecoder->GetNevents();
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
    bhDCH1->FillFromDigi(fDigiArrays.dch);
    // Fill data Tree //
    fRecoTree->Fill();
    if (iEv % itersToUpdate == 0) {
        //                bhTrig->SaveHists(imgSavePath);
    }
    gSystem->ProcessEvents();
    fServer->ProcessRequests();
    cout << "Event " << iEv << " processed" << endl;
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
    // GEM histograms init //
    bhGem = new BmnHistGem("GEM");
    bhGem->Register(fServer);
    // ToF histograms init //
    bhToF400 = new BmnHistToF("ToF400");
    bhToF400->Register(fServer);

    bhToF700 = new BmnHistToF700("ToF700");
    bhToF700->Register(fServer);
    // DCH histograms init //
    bhDCH1 = new BmnHistDch("DCH");
    bhDCH1->Register(fServer);
    // Triggers histograms init //
    bhTrig = new BmnHistTrigger("Triggers");
    bhTrig->Register(fServer);
}

void BmnMonitor::FinishRun() {
    fRecoTree->Write();
    fHistOut->Write();
    
    bhGem->SetDir(NULL, fRecoTree);
    bhToF400->SetDir(NULL, fRecoTree);
    bhToF700->SetDir(NULL, fRecoTree);
    bhDCH1->SetDir(NULL, fRecoTree);
    bhTrig->SetDir(NULL, fRecoTree);
//    fHistOut->Close();
    
    bhToF400->Reset();
    bhToF700->Reset();
    bhDCH1->Reset();
    bhTrig->Reset();
    bhGem->Reset();
    
}

void BmnMonitor::threadWrapper(BmnDataReceiver* dr) {
    dr->ConnectRaw();
}

void BmnMonitor::threadDecodeWrapper(BmnRawDataDecoder* rdd) {
}

ClassImp(BmnMonitor);