#include "BmnTof2Raw2Digit.h"

BmnTof2Raw2Digit::BmnTof2Raw2Digit(){
    n_rec=0;
}

BmnTof2Raw2Digit::BmnTof2Raw2Digit(TString mappingFile) {
static const char *tmp[4]= {"UB", "DB", "US", "DS"};
   for(int i=0;i<4;i++) sprintf(Bmn_Tof2_names[i],"%s",tmp[i]);
    n_rec=0;
    TString dummy;
    ifstream in;

    TString dir = getenv("VMCWORKDIR");
    TString path = dir + "/input/";
    in.open((path + mappingFile).Data());
    in >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy;
    while (!in.eof()) {
        TString plane,side;
        int id,slot,chan,strip; 
        in >>std::hex >> id >>std::dec >> slot >> chan >> plane >> strip >> side;
        if (!in.good()) break;
        map[n_rec].id=id;
        map[n_rec].slot=slot;
        map[n_rec].chan=chan;
        map[n_rec].strip=strip;
        if(plane.Contains(Bmn_Tof2_names[0])) map[n_rec].plane=0;
        if(plane.Contains(Bmn_Tof2_names[1])) map[n_rec].plane=1;
        if(plane.Contains(Bmn_Tof2_names[2])) map[n_rec].plane=2;
        if(plane.Contains(Bmn_Tof2_names[3])) map[n_rec].plane=3;
        map[n_rec++].side=side.Data()[0]; 
    }
    in.close();
}

char *BmnTof2Raw2Digit::getPlaneName(int i) {
    return (char *)Bmn_Tof2_names[i & 0x3];
}
void BmnTof2Raw2Digit::getEventInfo(unsigned long *ev,unsigned long *t1,unsigned long *t2){
    *ev=EVENT;
    *t1=TIME_SEC;
    *t2=TIME_NS;
}

void BmnTof2Raw2Digit::print(){
     printf("crate\tslot\tchannel\tplane\tstrip\tside\n=================================================\n");
     for(int i=0;i<n_rec;i++){
       printf("0x%X\t%d\t%d\t%s\t%d\t%c\n",map[i].id,map[i].slot,map[i].chan ,Bmn_Tof2_names[map[i].plane],map[i].strip,map[i].side);
     }   
}

void BmnTof2Raw2Digit::fillEvent(TClonesArray *data, TClonesArray *sync, TClonesArray *t0, TClonesArray *tof2digit) {
    unsigned int t0id = 0;
    unsigned int t0time = 0, digittime = 0;
  
    for (int i = 0; i < t0->GetEntriesFast(); i++) {
        BmnTDCDigit *digit = (BmnTDCDigit*) t0->At(i);
        if (digit->GetLeading()) {
            T0 = digit->GetValue() / 42.6666;
            t0id = digit->GetSerial();
            break;
        }
    }
    float val[TOF2_MAX_CHANNEL][2];
    memset(val,0,TOF2_MAX_CHANNEL*sizeof(float)*2);
    for (int i = 0; i < data->GetEntriesFast(); i++) {
       BmnTDCDigit *digit = (BmnTDCDigit*) data->At(i);
       for (int j = 0; j < sync->GetEntriesFast(); j++) {
            BmnSyncDigit *rec = (BmnSyncDigit*) sync->At(j);
            if (rec->GetSerial() == t0id){
               t0time   = rec->GetTime_ns();
               EVENT    = rec->GetEvent();
               TIME_SEC = rec->GetTime_sec();
               TIME_NS  = rec->GetTime_ns();
            }
            if (rec->GetSerial() == digit->GetSerial()) digittime = rec->GetTime_ns();
       }
       float tm = digit->GetValue() / 42.6666 - (T0 + (t0time - digittime));
       int ind; 
       for(ind=0;ind<n_rec;ind++) 
         if(digit->GetSerial()==map[ind].id && digit->GetSlot()==map[ind].slot && digit->GetChannel()==map[ind].chan) break;
       if(ind==n_rec) continue; 
       if(digit->GetLeading()) val[ind][0]=tm; else val[ind][1]=tm; 
    }
    for(int ind=0;ind<n_rec;ind++){ 
       if(val[ind][0]!=0 || val[ind][1]!=0){
          TClonesArray &ar = *tof2digit;
          new(ar[tof2digit->GetEntriesFast()]) BmnTof2Digit(map[ind].plane,map[ind].strip,map[ind].side,val[ind][0],val[ind][1]);  
       }
    }
}

ClassImp(BmnTof2Raw2Digit)

