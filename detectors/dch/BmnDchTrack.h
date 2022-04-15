#ifndef BMNDCHTRACK_H
#define BMNDCHTRACK_H

#include "BmnTrack.h"

class BmnDchTrack : public BmnTrack {
public:

    /** Default constructor **/
    BmnDchTrack();

    /** Destructor **/
    virtual ~BmnDchTrack();

    Int_t GetEventID() const { return fEventID; }
    Int_t GetTrackId() const { return fTrackID; }

    void SetEventID(Int_t iev) { fEventID = iev; }
    void SetTrackId(Int_t itr) { fTrackID = itr; }

private:

    Int_t fEventID; // identifier of event
    Int_t fTrackID; // identifier of track 

    ClassDef(BmnDchTrack, 1);

};

#endif
