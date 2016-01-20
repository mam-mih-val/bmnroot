#include "BmnZDCRaw2Digit.h"

BmnZDCRaw2Digit::BmnZDCRaw2Digit(){
  n_rec=0;
}
BmnZDCRaw2Digit::BmnZDCRaw2Digit(TString mappingFile) {
    n_rec=0;
    TString dummy;
    ifstream in;

    TString dir = getenv("VMCWORKDIR");
    TString path = dir + "/input/";
    in.open((path + mappingFile).Data());
    in >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy>> dummy >> dummy >> dummy >> dummy;
    while (!in.eof()) {
        int id,chan,front_chan,size,ix,iy,used;
	float x,y;
        in >>std::hex >> id >>std::dec >> chan >> front_chan>>size>>ix>>iy>>x>>y>>used;
        if (!in.good()) break;
        zdc_map_element[n_rec].id=id;
        zdc_map_element[n_rec].chan=chan;
        zdc_map_element[n_rec].front_chan=front_chan;
        zdc_map_element[n_rec].size=size;
        zdc_map_element[n_rec].ix=ix;
        zdc_map_element[n_rec].iy=iy;
        zdc_map_element[n_rec].x=x;
        zdc_map_element[n_rec].y=y;
        zdc_map_element[n_rec++].used=used;
    }
    in.close();
}


void BmnZDCRaw2Digit::print() {
     printf("id#\tchan\t\tf_chan\tsize\tix\tiy\\tx\ty\tused\n");
     for(int i=0;i<n_rec;i++)
     printf("0x%06lX\t%d\t%d\t%d\t%d\t%d\t%d\t%g\t%g\n",
         zdc_map_element[i].id,zdc_map_element[i].chan,zdc_map_element[i].front_chan,zdc_map_element[i].size,
         zdc_map_element[i].ix,zdc_map_element[i].iy,zdc_map_element[i].used,zdc_map_element[i].x,zdc_map_element[i].y); 
   
}


void BmnZDCRaw2Digit::fillEvent(TClonesArray *data, TClonesArray *zdcdigit) {
    for (int i = 0; i < data->GetEntriesFast(); i++) {
       BmnADCDigit *digit = (BmnADCDigit*) data->At(i);
       int ind; 
       for(ind=0;ind<n_rec;ind++) if(digit->GetSerial()==zdc_map_element[ind].id && digit->GetChannel()==(zdc_map_element[ind].chan-1)) break;
       if(ind==n_rec) continue; 
       if(zdc_map_element[ind].used==0) continue;
       TClonesArray &ar_zdc = *zdcdigit;
       new(ar_zdc[zdcdigit->GetEntriesFast()]) BmnZDCDigit(zdc_map_element[ind].ix,zdc_map_element[ind].iy,zdc_map_element[ind].x,zdc_map_element[ind].y,zdc_map_element[ind].size,
           zdc_map_element[ind].front_chan,digit->GetSamples(),digit->GetValue());  
    }
}

ClassImp(BmnZDCRaw2Digit)

