#include "BmnMQSink.h"

using namespace std;

BmnMQSink::BmnMQSink() :
//fSender(nullptr)
 fCtx(nullptr)
, fOutSocket(nullptr)
, fOutPort(0) {
}

BmnMQSink::BmnMQSink(const BmnMQSink&) {
}

BmnMQSink::~BmnMQSink() {
//    if (fSender)
//        delete fSender;
    if (fCtx)
        zmq_ctx_destroy(fCtx);
}

Bool_t BmnMQSink::InitSink() {
    printf("BmnMQSink::InitSink\n");
    fOutChannel = "data-out";
//    FairMQProgOptions *config = new FairMQProgOptions();
//    string DstChanName = "chans." + fOutChannel + ".0.address";
//    ////    config->SetValue<string>("out-channel", DstChanName);
//    ////    boost::program_options::options_description options;
//    ////    options.add_options()
//    ////            ("in-channel", bpo::value<std::string>()->default_value("digi"), "Name of the input channel")
//    ////            ("out-channel", bpo::value<std::string>()->default_value("dst"), "Name of the output channel");
//    ////    //    config.AddToCmdLineOptions()
//    //       config->SetValue<string>(DstChanName, "tcp://127.0.0.1:6666");
//    //       config->SetValue<string>("transport", "zeromq");
//    string arg = "--channel-config 'name=data-out,type=push,method=connect,rateLogging=1,address=tcp://localhost:5220'";
//    vector<string> args;
//    //       args.push_back(arg);
//    args.push_back("");
//    args.push_back("--mq-config");
//    args.push_back("../monitor/mq-reco.json");
//    args.push_back("--id");
//    args.push_back("mq-sink");
//    args.push_back("--channel-config");
//    args.push_back("name=data-out,type=pub,method=connect,rateLogging=1,address=tcp://localhost:5220");
////    config->ParseAll(args, kFALSE);
////    config->SetProperty<string>("data.0.address", "tcp://localhost:4321");
//    config->SetProperty<string>(fOutChannel, DstChanName);
//    config->PrintOptions();
//    config->PrintOptionsRaw();
//    fSender = new FairMQDevice(*config,{0, 0, 0});
//    //    fSender = new FairMQDevice();
//    //    fSender->SetId("mq-sink");
//    fSender->SetConfig(*config);
//    //    fSender->fChannels = config->GetVarMap();
//    //    fSender->RunStateMachine();
//    //            fSender->ChangeState("INIT_DEVICE");
//    //    fSender->RegisterChannelEndpoints();
//    //    FairMQProgOptions* config =  fSender->GetConfig();
//    //    fSender->RegisterChannelEndpoint(fOutChannel);
//    //    if (fConfig.Count("print-channels")) {
//    fSender->PrintRegisteredChannels();
//    printf("BmnMQSink::InitSink PrintRegisteredChannels\n");
//    //    fSender->RunStateMachine();
//    //    printf("BmnMQSink::InitSink RunStateMachine\n");
//    //        fSender.ChangeState(fair::mq::Transition::End);
//    //        return 0;
//    //    }
//    //    fSender.SetConfig(fConfig);
//    //    fSender.RunStateMachine();
//    //    printf("BmnMQSink::InitSink RunStateMachine\n");

    fOutPort = 6666;
    fCtx = zmq_ctx_new();
    fOutSocket = zmq_socket(fCtx, ZMQ_PUB);
    Int_t sndBuf = 0;
    size_t vl = sizeof (sndBuf);
    if (zmq_getsockopt(fOutSocket, ZMQ_SNDBUF, &sndBuf, &vl) == -1)
        DBG("zmq_getsockopt of ZMQ_RCVBUF")
        printf("sndbuf = %d\n", sndBuf);
    sndBuf = 8192; //MAX_BUF_LEN;
    if (zmq_setsockopt(fOutSocket, ZMQ_SNDBUF, &sndBuf, sizeof (sndBuf)) == -1)
        DBGERR("zmq_setsockopt of ZMQ_SNDBUF");
    sndBuf = 0;
    if (zmq_getsockopt(fOutSocket, ZMQ_RCVBUF, &sndBuf, &vl) == -1)
        DBGERR("zmq_getsockopt of ZMQ_RCVBUF")
        printf("sndbuf = %d\n", sndBuf);
    TString localDecoStr = Form("tcp://*:%d", fOutPort);
    if (zmq_bind(fOutSocket, localDecoStr.Data()) != 0) {
        DBGERR("zmq bind")
        return kFALSE;
    }

    return kTRUE;
}

void BmnMQSink::RegisterImpl(const char* name, const char* folderName, void* ob) {
    printf("RegisterImpl name %s  foldername %s  ob %08lX\n",
            name, folderName, reinterpret_cast<uintptr_t>(ob));
    if (std::strstr(name, ".")) {
        TNamed * obn = static_cast<TNamed*> (ob);
        printf("casted: name %s class name %s classname %s class_name %s\n",
                obn->GetName(), obn->Class()->GetName(), obn->ClassName(), obn->Class_Name());
        printf("draw \n");
        TClass *cl = TClass::GetClass(obn->ClassName());
        printf("clname %s\n", cl->GetName());
        fObjMap.insert(pair<const char *, TObject *> (name, static_cast<TObject *> (ob)));
        fParts.AddObject(obn);
    } else {
        TClonesArray * tca = static_cast<TClonesArray*> (ob);
        printf("ar cl %s  name %s title %s  len %5d\n", tca->GetClass()->GetName(), tca->GetName(), tca->GetTitle(), tca->GetEntriesFast());
        fArrMap.insert(pair<const char *, TClonesArray *> (name, static_cast<TClonesArray *> (ob))); //new TClonesArray(tca->GetClass())));
        fParts.AddArray(tca);
    }
    //    TFolder* folder = 0;
    //    TFolder* f = 0;
    //    f = static_cast<TFolder*>(fOutFolder->FindObjectAny(folderName));
    //    if (f == 0) {
    //        folder = fOutFolder->AddFolder(folderName, folderName);
    //    } else {
    //        folder = f;
    //    }
    //    //  ((TNamed*)obj)->SetName(name);
    //    folder->Add((TNamed*)obj);

    //        auto& ot = typeid(T*);
    //        auto& pt = typeid(T);
    //        RegisterAny(name, ot, pt, &ob);

    return;
}

void BmnMQSink::RegisterAny(const char* brname, const std::type_info& oi, const std::type_info& pi, void* obj) {
    fPersistentBranchesMap[brname] = std::unique_ptr<TypeAddressPair const>(new TypeAddressPair(oi, pi, obj));
    printf("\nRegisterAny brname %s  %08lX\n", brname, reinterpret_cast<uintptr_t>(fPersistentBranchesMap[brname].get()));
}

bool BmnMQSink::IsPersistentBranchAny(const char* name) {
    printf("IsPersistentBranchAny\n");
    if (fPersistentBranchesMap.find(name) == fPersistentBranchesMap.end()) {
        return false;
    }
    return true;
}

namespace impl {
    // a helper function to demangle a type_name

    inline std::string demangle(const char* name) {
        int status = -4; // some arbitrary value to eliminate the compiler warning
        std::unique_ptr<char, void (*)(void*) > res{abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free};
        return (status == 0) ? res.get() : name;
    }
} // namespace impl

bool BmnMQSink::CreatePersistentBranchesAny() {
    printf("CreatePersistentBranchesAny\n");
    //    for (auto& iter : fPersistentBranchesMap) {
    //        LOG(info) << "CREATING BRANCH " << iter.first;
    //        auto& tinfo = iter.second->persistenttypeinfo;
    //        auto tname = impl::demangle(tinfo.name());
    //
    //        // for the branch creation we need a TClass describing the object
    //        // get it from ROOT via the type name
    //        auto cl = TClass::GetClass(tinfo);
    //        if (!cl) {
    //            LOG(fatal) << "No TClass found for " << tname << "\n";
    //            return false;
    //        }
    //
    //        if (!cl->HasDictionary()) {
    //            LOG(fatal) << "No dictionary found for " << tname << "\n";
    //            return false;
    //        }
    //
    //        // create the branch
    //        auto obj = iter.second->ptraddr;
    //
    //        LOG(info) << "Creating branch for " << iter.first.c_str() << " with address " << obj;
    //        
    ////        fOutTree->Branch(iter.first.c_str(), tname.c_str(), obj);
    //    }
    return true;
}

void BmnMQSink::EmitPersistentBranchWrongTypeWarning(const char* brname,
        const char* type1,
        const char* type2) const {
    //LOG(warn) << "Trying to read from persistent branch " << brname << " with wrong type " << type1
    //        << " (expexted: " << type2 << " )";
}

void BmnMQSink::Fill() {
//    printf("BmnMQSink::Fill\n");
//    FairMQParts parts;
    TBufferFile t(TBuffer::kWrite);

    //    for (auto &brEl : fPersistentBranchesMap) {
    //        TClonesArray** tcArray =
    //                GetPersistentBranchAny<TClonesArray**>(brEl.first.c_str());
    //
    //        if (tcArray) {
    //            LOG(debug) << FairRootManager::Instance()->GetInstanceId() << "]  "
    //                    << " /// *Array->GetName() " << (*tcArray)->GetName();
    //            //            TObject* objClone = (*tcArray)->Clone();
    //            //                        FairMQMessagePtr mess =  unique_ptr<FairMQMessage>(new FairMQMessage());//make_unique<FairMQMessage>();
    //            FairMQMessagePtr mess(fSender->NewMessage());
    //            fSender->Serialize<RootSerializer>(*mess, tcArray);
    //            //                        Serializer().Serialize<>
    //            //            buf.WriteObject(tcArray);
    //            parts.AddPart(std::move(tcArray));
    //        }
    //
    //    }
    //    for (auto & it : fObjMap) {
    //        TObject* ob = it.second;
    //        FairMQMessagePtr mess(fSender->NewMessage());
    ////        FairMQMessagePtr mess = new FairMQMessage(fSender->NewMessage());
    //        fSender->Serialize<RootSerializer>(*mess, ob);
    //        parts.AddPart(std::move(mess));
    //
    //    }
    //    for (auto & it : fArrMap) {
    //        TClonesArray* ar = it.second;
    //        FairMQMessagePtr mess = new FairMQMessage();//(fSender->NewMessage());
    //        fSender->Serialize<RootSerializer>(*mess, ar);
    //        parts.AddPart(std::move(mess));
    //
    //    }
//    for (auto & ar : fParts.GetArrays()) {
//        printf("ar %s counts %d\n", ar->GetClass()->GetName(), ar->GetEntriesFast());
//    }

    t.WriteObject(&fParts);
    Int_t sendRes = zmq_send(fOutSocket, t.Buffer(), t.Length(), ZMQ_NOBLOCK);
    printf("sendRes %d\n", sendRes);
    t.Reset();
    if (sendRes == -1) {
        fprintf(stderr, "Send error # %d #%s\n", errno, zmq_strerror(errno));
    }
//    int64_t sent = 0;
//    printf("fChannels size %lu\n", fSender->fChannels.size());
//    for (const pair<string, vector < FairMQChannel>> &gr : fSender->fChannels) {
//        printf("gr: %s \n", gr.first.c_str());
//        for (const FairMQChannel & ch : gr.second) {
//            printf("\taddr:%s\n", ch.GetAddress().c_str());
//        }
//    }
//    //    fSender->Send(parts, "data-out");
//    //    printf("sent %ld\n", sent);
//    //                sent = fSender->Send(parts, "dst:1:1");
//    //                printf("sent %ld\n", sent);
//    //    //    //    sent = fSender->Send(parts, "dst");
//    //    //    //    printf("sent %ld\n", sent);
//    //    //    sent = fSender->Send(parts, "dst:1");
//    //    //    printf("sent %ld\n", sent);
//    //    //    sent = fSender->Send(parts, "dst:0:0");
//    //    //    printf("sent %ld\n", sent);
//    //    sent = fSender->Send(parts, "out-channel");
//    //    printf("sent %ld\n", sent);
}

FairSink * BmnMQSink::CloneSink() {
    FairRootManager* tempMan = FairRootManager::Instance();
    BmnMQSink* newSink = new BmnMQSink(*this);
    //    newSink->SetMQRunDevice(this->GetMQRunDevice());
    //    LOG(info) << "[" << tempMan->GetInstanceId() << "] BmnMQSink::CloneSink() setting MQRunDevice to "
    //            << this->GetMQRunDevice();

    return newSink;
}

ClassImp(BmnMQSink)
