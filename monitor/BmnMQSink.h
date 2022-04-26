#ifndef BMNMQSINK_H_
#define BMNMQSINK_H_

#include <cxxabi.h>   // __cxa_demangle
// STL
#include <memory>
#include <string>
#include <typeinfo>
// Auxillary
#include <zmq.h>
#include <boost/program_options.hpp>
// Root
#include <TBufferFile.h>
#include <Rtypes.h>
#include <RootSerializer.h>
#include <TClonesArray.h>
// FairRoot
#include <FairRootManager.h>
#include <FairSink.h>
//#include <fairmq/Message.h>
////#include <fairmq/Device.h>
//#include <fairmq/Device.h>
//#include <fairmq/ProgOptions.h>
////#include <FairMQLogger.h>
//
//#include <fairmq/DeviceRunner.h>
// BmnRoot

#include "BmnMath.h"
#include "BmnParts.h"

namespace bpo = boost::program_options;

//struct Branch

class BmnMQSink : public FairSink
{
  public:
    BmnMQSink();
    virtual ~BmnMQSink();

    virtual Bool_t InitSink();
    virtual void Close() {}
    virtual void Reset() {}

    virtual Sink_Type GetSinkType() { printf("\t\tType asked\n");
        return kFILESINK; }

    virtual void FillEventHeader(FairEventHeader* /* feh */) {printf("\t\tfill event header?\n");}

    virtual void SetOutTree(TTree* /* fTree */) { printf("\t\tset out tree?\n");return; }

    virtual void Fill();

    virtual Int_t Write(const char* name, Int_t /* option=0 */, Int_t /* bufsize=0 */) {
        printf("\t\twrite %s?\n", name); return -1; }

    virtual void RegisterImpl(const char*, const char*, void*);
    virtual void RegisterAny(const char* brname, const std::type_info& oi, const std::type_info& pi, void* obj);

    virtual void WriteFolder() {printf("\t\t WriteFolder\n");}
    virtual bool CreatePersistentBranchesAny();

    virtual void WriteObject(TObject*  f , const char* name, Int_t /* option = 0 */) {
//        printf("\t\t WriteObject %08X %s\n", f, name);
    }
    virtual void WriteGeometry() {printf("\t\tWriteGeometry\n"); }

    virtual FairSink* CloneSink();

    bool IsPersistentBranchAny(const char* name);

    /* /// Returns a default object for a branch or looks it up when it exists already. */
    /* /// Returns nullptr when the branch does not exist or looking up with wrong type. */
    /* /// The returned default object will be filled with data by the framework. */
    template<typename T>
    T GetPersistentBranchAny(const char* name) const;

  private:
//      fair::mq::DeviceRunner fSender;
//      fair::mq::Device* fSender;// FairMQDevice* fSender;
      std::string fOutChannel;
      BmnParts fParts;
      void * fCtx;
      void * fOutSocket;
      Int_t fOutPort;
      
      
      map<const char*, TObject*> fObjMap;
      map<const char*, TClonesArray*> fArrMap;
      
    // private helper function to emit a warning
    void EmitPersistentBranchWrongTypeWarning(const char* brname, const char* typen1, const char* typen2) const;

    BmnMQSink(const BmnMQSink&);
    BmnMQSink& operator=(const BmnMQSink&);
    ClassDef(BmnMQSink, 1)
};


// try to retrieve an object address from the registered branches/names
template<typename T>
T BmnMQSink::GetPersistentBranchAny(const char* brname) const
{
    static_assert(std::is_pointer<T>::value, "Return type of GetPersistentBranchAny has to be a pointer");
    using P = typename std::remove_pointer<T>::type;
    auto iter = fPersistentBranchesMap.find(brname);
    if (iter != fPersistentBranchesMap.end()) {
        // verify type consistency
        if (typeid(P).hash_code() != iter->second->origtypeinfo.hash_code()) {
            EmitPersistentBranchWrongTypeWarning(brname, typeid(P).name(), iter->second->origtypeinfo.name());
            return nullptr;
        }
        return static_cast<T>(iter->second->ptraddr);
    }
    return nullptr;
}

#endif /* BMNMQSINK_H_ */
