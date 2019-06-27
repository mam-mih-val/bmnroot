#ifndef BMNMWPCTRACK_H
#define BMNMWPCTRACK_H

#include "BmnTrack.h"

using namespace std;

class BmnMwpcTrack : public BmnTrack {
public:

    /** Default constructor **/
    BmnMwpcTrack();

    /** Destructor **/
    virtual ~BmnMwpcTrack();


    vector<Double_t> GetCoord(){
      return fCoord;
    }

    vector<Double_t> GetClust(){
      return fClust;
    }

    void SetCoord(vector<Double_t> Coord){
      fCoord = Coord;
    }

    void SetClust(vector<Double_t> Clust){
      fClust = Clust;
    }
    

private:

    vector<Double_t> fCoord;
    vector<Double_t> fClust;
    
    ClassDef(BmnMwpcTrack, 1);

};

#endif
