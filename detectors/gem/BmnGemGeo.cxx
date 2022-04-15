#include "BmnGemGeo.h"

using namespace std;

ClassImp(BmnGemGeo)

BmnGemGeo::BmnGemGeo() {
   
    fNumStations = 8;
    fX0 = -40.0;
    fY0 = -30.0;
    fZ0 = 30.0;
    fDistance = 10.0;
    
    fStripXWidth = 0.04;
    fStripYWidth = 0.04;
    
    fStripXLength = 80.;
    fStripYLength = 60.;
    
    fXstripPitch = 0.04;
    fYstripPitch = 0.04;
    
    
    

}

BmnGemGeo::~BmnGemGeo() {

}
