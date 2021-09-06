//Macro for delete all map from database
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
//sP is start period
//sR is start run
//eP is end period
//eR is end run

void delete_Tof1_mapping (int sP = 5, int sR = 419, int eP = 5, int eR = 1014)
{
   //clear TDC map from DB
    UniDbDetectorParameter::DeleteDetectorParameter("TOF1","placementmap",sP, sR, eP, eR);
    //delete parameter
    UniDbParameter::DeleteParameter("placementmap");
    
    //clear strip map from DB
    UniDbDetectorParameter::DeleteDetectorParameter("TOF1","stripmap",sP, sR, eP, eR);
    //delete parameter
    UniDbParameter::DeleteParameter("stripmap");
            
            
}
