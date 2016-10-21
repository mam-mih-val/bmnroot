#ifndef BMNMONITOR_H
#define BMNMONITOR_H 1

#include <iostream>

#include <TNamed.h>
#include "BmnDataReceiver.h"

using namespace std;
using namespace TMath;

class BmnMonitor : public TNamed {
public:
    
    BmnMonitor() {};
    virtual ~BmnMonitor();
    // Getters

    // Setters
   
private:
    Int_t fTest;
  
     
    ClassDef(BmnMonitor, 1)
};

#endif