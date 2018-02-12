#ifndef BMNDCHTRACK_H
#define BMNDCHTRACK_H

#include "BmnTrack.h"

class BmnDchTrack : public BmnTrack {
public:

    /** Default constructor **/
    BmnDchTrack();

    /** Destructor **/
    virtual ~BmnDchTrack();
    //AM 7.08.2017
    vector<Double_t> xozParameters;
    vector<Double_t> yozParameters;
    double covXOZ[3][3]; // XOZ covariance matrix
    double covYOZ[2][2]; // YOZ covariance matrix
    Double_t xrms;
    Double_t yrms;

private:


    ClassDef(BmnDchTrack, 1);

};

#endif
