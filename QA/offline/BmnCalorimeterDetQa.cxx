#include "BmnCalorimeterDetQa.h"

BmnCalorimeterDetQa::BmnCalorimeterDetQa(TString detName) : 
fHistoManager(new BmnQaHistoManager()) {

    CommonInfo(detName);
    
}
