#ifndef BMNMWPCSEGMENT_H
#define BMNMWPCSEGMENT_H

#include "BmnMwpcTrack.h"

class BmnMwpcSegment : public BmnMwpcTrack {
public:

    /** Default constructor **/
    BmnMwpcSegment();

    /** Destructor **/
    virtual ~BmnMwpcSegment();

private:


    ClassDef(BmnMwpcSegment, 1);

};

#endif
