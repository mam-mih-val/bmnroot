// ------------------------------------------------------------------
// -----                      CbmTofMerger                      -----
// -----              Created 24-01-2006 by D.Kresan            -----
// ------------------------------------------------------------------
#ifndef _CBMTOFMERGER_
#define _CBMTOFMERGER_

#include "TObject.h"

class TClonesArray;


class CbmTofMerger : public TObject {

protected:
    Int_t fVerbose;    // Verbosity level

public:
 CbmTofMerger() : TObject(), fVerbose(0) {};
 
   virtual ~CbmTofMerger() {};
    
    virtual void Init() {};
    virtual Int_t DoMerge(TClonesArray* glbTracks,
			  TClonesArray* tofHits) = 0;

    inline void SetVerbose(Int_t verbose) {  fVerbose = verbose; }

    ClassDef(CbmTofMerger, 1);
};


#endif


