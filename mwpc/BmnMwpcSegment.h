#ifndef BMNMWPCSEGMENT_H
#define BMNMWPCSEGMENT_H

#include "BmnTrack.h"

class BmnMwpcSegment : public BmnTrack {
public:

    /** Default constructor **/
    BmnMwpcSegment();

    /** Destructor **/
    virtual ~BmnMwpcSegment();

private:


    ClassDef(BmnMwpcSegment, 1);

};

#endif
