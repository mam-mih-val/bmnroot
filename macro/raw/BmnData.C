#define _DEBUG_ 0
#include "TClonesArray.h"
#include "TFile.h"
#include "TTree.h"
#include "BmnTDCDigit.h"
#include "BmnADCDigit.h"
#include "BmnSyncDigit.h"
#include "BmnTTBDigit.h"
#include <cstdlib>

class BmnDataToRoot {

private:

  unsigned int data[1000000];

  FILE *in;
  int RUN;
  unsigned int EVENT; 
  unsigned int TRIGWORD;

  TFile *File;
  TTree *Tree;
  TClonesArray *sync;
  TClonesArray *ttb;
  TClonesArray *dch;
  TClonesArray *t0;
  TClonesArray *tof700;
  TClonesArray *tof400;
  TClonesArray *ecal;
  TClonesArray *zdc;
  TClonesArray *trigger;
  TClonesArray *counters;
  
  void process_ADC(unsigned int *d,unsigned int len,unsigned int serial){
     if(_DEBUG_>1){
        char str[100];	
        time_t t=d[1];
        struct tm * timeinfo=localtime (&t);
        strftime(str,100,"%F %T",timeinfo);
        printf("Event time: %s %u Event: %d \nChannel mask: %08X %08X\n",str,d[2]>>2,d[0]&0xFFFFFF,d[3],d[4]);
     }
     TClonesArray &ar_sync = *sync;
     long long t1 = d[1];
     long long t2 = (d[2]>>2);
     new(ar_sync[sync->GetEntriesFast()]) BmnSyncDigit(serial,EVENT,t1,t2);

     for(unsigned int ind=5;ind<len;){
	unsigned int ch=(d[ind]>>24)&0xFF; 
        unsigned int samples=(d[ind]&0xFFF)/2-4;
        unsigned short *val=(unsigned short *)&d[ind+3];
        if(serial==0x6f4bb2 || serial==0x6F4083){
          TClonesArray &ar_zdc = *zdc;
          new(ar_zdc[zdc->GetEntriesFast()]) BmnADCDigit(serial,ch,samples,val);
        }
        if(serial==0x6EFDBE){
          TClonesArray &ar_ecal = *ecal;
          new(ar_ecal[ecal->GetEntriesFast()]) BmnADCDigit(serial,ch,samples,val);
        }
        ind+=samples/2+3; ch++;
     }
   };
   void process_FVME(unsigned int *d,unsigned int len,unsigned int serial){ 
     unsigned int TYPE=0,SLOT=0,TDCID=0,CH=0,VAL=0;
     for(unsigned int i=0;i<len;i++){
        unsigned int type=(d[i]>>28)&0xFF;
        if(type==0xA){  if(_DEBUG_>1) printf("Event header: evt# %d\n",d[i]&0xFFFFF);}
        else if(type==0xB){  if(_DEBUG_>1) printf("Event trailer: readout status %X, word count %d\n",(d[i]>>24)&0xF,d[i]&0xFFFFFF);}
        else if(type==0x3){
	   if (SLOT==12 && TYPE==0x4C)
	   {
		 TRIGWORD = d[i];
	   }
	}
        else if(type==0x8){
           if(_DEBUG_>1) printf("\tModule header: slot %d, module Id %X, evt# %d\n",(d[i]>>23)&0x1F,(d[i]>>16)&0x7F,d[i]&0xFFFF);
           TYPE=(d[i]>>16)&0x7F;
           SLOT=(d[i]>>23)&0x1F;
           if(TYPE==0x13){
              if(_DEBUG_>1) printf("%d %d\t%8X %8X\n",EVENT,SLOT,d[i+1],d[i+2]);
	      TClonesArray &ar_ttb = *ttb;
              new(ar_ttb[ttb->GetEntriesFast()]) BmnTTBDigit(serial,GlobalEvent,SLOT,d[i+1],d[i+2]);

           }
	   if(TYPE==0xA){
	      get_event_time(d[i+1],d[i+2],d[i+3],d[i+4]);
	      if(_DEBUG_>1){
	         char str[100];	
                 time_t t=ts_t0_s;
                 struct tm * timeinfo=localtime (&t);
                 strftime(str,100,"%F %T",timeinfo);
	         printf("\t\tSERIAL: 0x%06X TYPE: 0x%02X SLOT: %02d %llX %s %lld\n",serial,TYPE,SLOT,GlobalEvent,str,ts_t0_ns);
	      }
	      TClonesArray &ar_sync = *sync;
              new(ar_sync[sync->GetEntriesFast()]) BmnSyncDigit(serial,GlobalEvent,ts_t0_s,ts_t0_ns);
	   }
        }
	else if(type==0x4){
           if(TYPE==0x10){
              TDCID=(d[i]>>24)&0xF;
              CH=(d[i]>>19)&0x1F;
              VAL=d[i]&0x7FFFF;
	      TClonesArray &ar_dch = *dch;
              new(ar_dch[dch->GetEntriesFast()]) BmnTDCDigit(serial,TYPE,SLOT,kTRUE,get_ch_tdc64v(TDCID,CH),0,VAL);
              if(_DEBUG_>1) printf("\t\tSERIAL: 0x%06X TYPE: 0x%02X SLOT: %d TDC: %d CH: %d VAL: %d\n",serial,TYPE,SLOT,TDCID,get_ch_tdc64v(TDCID,CH),VAL);
           }
           if(TYPE==0x11){
              TDCID=(d[i]>>24)&0xF;
              CH=(d[i]>>21)&0x7;
              VAL= ((d[i] & 0x7FFFF )<<2)|((d[i] & 0x180000)>>19); 
              if(_DEBUG_>1) printf("\t\tSERIAL: 0x%06X TYPE: 0x%02X SLOT: %d TDC: %d CH: %d fp: %d VAL: %d\n",serial,TYPE,SLOT,TDCID,CH,get_ch_tdc32vl(TDCID,CH*4),VAL);    
	      if(SLOT==3 && get_ch_tdc32vl(TDCID,CH*4)<13){
	         TClonesArray &ar_trigger = *trigger;
                 new(ar_trigger[trigger->GetEntriesFast()]) BmnTDCDigit(serial,TYPE,SLOT,kTRUE,get_ch_tdc32vl(TDCID,CH*4),0,VAL);
	      }else{     
	         TClonesArray &ar_tof700 = *tof700;
                 new(ar_tof700[tof700->GetEntriesFast()]) BmnTDCDigit(serial,TYPE,SLOT,kTRUE,get_ch_tdc32vl(TDCID,CH*4),0,VAL);
	      }     
           }
           if(TYPE==0x53){
              TDCID=(d[i]>>24)&0xF;
              CH=(d[i]>>21)&0x7;
              VAL= ((d[i] & 0x7FFFF )<<2)|((d[i] & 0x180000)>>19); 
              if(_DEBUG_>1) printf("\t\tSERIAL: 0x%06X TYPE: 0x%02X SLOT: %d TDC: %d CH: %d fp: %d VAL: %d\n",serial,TYPE,SLOT,TDCID,CH,get_ch_tdc32vl(TDCID,CH*4),VAL);    
//	      if(SLOT==3 && get_ch_tdc64vhle(TDCID,CH*4)<13){
//	         TClonesArray &ar_trigger = *trigger;
//                 new(ar_trigger[trigger->GetEntriesFast()]) BmnTDCDigit(serial,TYPE,SLOT,kTRUE,get_ch_tdc64vhle(TDCID,CH*4),VAL);
//	      }else{     
	         TClonesArray &ar_tof700 = *tof700;
                 new(ar_tof700[tof700->GetEntriesFast()]) BmnTDCDigit(serial,TYPE,SLOT,kTRUE,get_ch_tdc64vhle(TDCID,CH*4),0,VAL);
//	      }     
           }
           if(TYPE==0x12){
              TDCID=(d[i]>>24)&0xF;
              CH=(d[i]>>21)&0x7;
              VAL= ((d[i] & 0x7FFFF )<<2)|((d[i] & 0x180000)>>19); 
              if(_DEBUG_>1) printf("\t\tSERIAL: 0x%06X TYPE: 0x%02X SLOT: %d TDC: %d CH: %d fp: %d VAL: %d\n",serial,TYPE,SLOT,TDCID,CH,get_ch_tdc72vhl(TDCID,CH*4),VAL);
	      if(SLOT==21 && get_ch_tdc72vhl(TDCID,CH*4)==38){
	         TClonesArray &ar_t0 = *t0;
                 new(ar_t0[t0->GetEntriesFast()]) BmnTDCDigit(serial,TYPE,SLOT,kTRUE,get_ch_tdc72vhl(TDCID,CH*4),0,VAL);
	      }  
              ///////////////////////////////////////////////   
	      if(SLOT==21 /*&& get_ch_tdc72vhl(TDCID,CH)!=12*/){
	         TClonesArray &ar_bc = *counters;
                 new(ar_bc[counters->GetEntriesFast()]) BmnTDCDigit(serial,TYPE,SLOT,kTRUE,get_ch_tdc72vhl(TDCID,CH*4),0,VAL);
              }
              /////////////////////////////////
	      if(SLOT!=21){
	         TClonesArray &ar_tof400 = *tof400;
                 new(ar_tof400[tof400->GetEntriesFast()]) BmnTDCDigit(serial,TYPE,SLOT,kTRUE,get_ch_tdc72vhl(TDCID,CH*4),0,VAL);
	      }     
           }
        }
        else if(type==0x5){
           if(TYPE==0x10){
              TDCID=(d[i]>>24)&0xF;
              CH=(d[i]>>19)&0x1F;
              VAL=d[i]&0x7FFFF;
	      TClonesArray &ar_dch = *dch;
              new(ar_dch[dch->GetEntriesFast()]) BmnTDCDigit(serial,TYPE,SLOT,kFALSE,get_ch_tdc72vhl(TDCID,CH),0,VAL);
              if(_DEBUG_>1) printf("\t\tSERIAL: 0x%06X TYPE: 0x%02X SLOT: %d TDC: %d CH: %d VAL: %d\n",serial,TYPE,SLOT,TDCID,get_ch_tdc64v(TDCID,CH),VAL);
           }
           if(TYPE==0x11){
              TDCID=(d[i]>>24)&0xF;
              CH=(d[i]>>21)&0x7;
              VAL= ((d[i] & 0x7FFFF )<<2)|((d[i] & 0x180000)>>19); 
              if(_DEBUG_>1) printf("\t\tSERIAL: 0x%06X TYPE: 0x%02X SLOT: %d TDC: %d CH: %d fp: %d VAL: %d\n",serial,TYPE,SLOT,TDCID,CH,get_ch_tdc32vl(TDCID,CH*4),VAL);    
	      if(SLOT==3 && get_ch_tdc32vl(TDCID,CH*4)<13){
	         TClonesArray &ar_trigger = *trigger;
                 new(ar_trigger[trigger->GetEntriesFast()]) BmnTDCDigit(serial,TYPE,SLOT,kFALSE,get_ch_tdc32vl(TDCID,CH*4),0,VAL);
	      }else{     
	         TClonesArray &ar_tof700 = *tof700;
                 new(ar_tof700[tof700->GetEntriesFast()]) BmnTDCDigit(serial,TYPE,SLOT,kFALSE,get_ch_tdc32vl(TDCID,CH*4),0,VAL);
	      }     
           }
           if(TYPE==0x53){
              TDCID=(d[i]>>24)&0xF;
              CH=(d[i]>>21)&0x7;
              VAL= ((d[i] & 0x7FFFF )<<2)|((d[i] & 0x180000)>>19); 
              if(_DEBUG_>1) printf("\t\tSERIAL: 0x%06X TYPE: 0x%02X SLOT: %d TDC: %d CH: %d fp: %d VAL: %d\n",serial,TYPE,SLOT,TDCID,CH,get_ch_tdc32vl(TDCID,CH*4),VAL);    
//	      if(SLOT==3 && get_ch_tdc64vhle(TDCID,CH*4)<13){
//	         TClonesArray &ar_trigger = *trigger;
//                 new(ar_trigger[trigger->GetEntriesFast()]) BmnTDCDigit(serial,TYPE,SLOT,kFALSE,get_ch_tdc64vhle(TDCID,CH*4),VAL);
//	      }else{     
	         TClonesArray &ar_tof700 = *tof700;
                 new(ar_tof700[tof700->GetEntriesFast()]) BmnTDCDigit(serial,TYPE,SLOT,kFALSE,get_ch_tdc64vhle(TDCID,CH*4),0,VAL);
//	      }     
           }
           if(TYPE==0x12){
              TDCID=(d[i]>>24)&0xF;
              CH=(d[i]>>21)&0x7;
              VAL= ((d[i] & 0x7FFFF )<<2)|((d[i] & 0x180000)>>19); 
              if(_DEBUG_>1) printf("\t\tSERIAL: 0x%06X TYPE: 0x%02X SLOT: %d TDC: %d CH: %d fp: %d VAL: %d\n",serial,TYPE,SLOT,TDCID,CH,get_ch_tdc72vhl(TDCID,CH*4),VAL);
	      if(SLOT==21 && get_ch_tdc72vhl(TDCID,CH*4)==38){
	         TClonesArray &ar_t0 = *t0;
                 new(ar_t0[t0->GetEntriesFast()]) BmnTDCDigit(serial,TYPE,SLOT,kFALSE,get_ch_tdc72vhl(TDCID,CH*4),0,VAL);
	      }     
              ///////////////////////////////////////////////   
	      if(SLOT==21 /*&& get_ch_tdc72vhl(TDCID,CH)!=12*/){
	         TClonesArray &ar_bc = *counters;
                 new(ar_bc[counters->GetEntriesFast()]) BmnTDCDigit(serial,TYPE,SLOT,kFALSE,get_ch_tdc72vhl(TDCID,CH*4),0,VAL);
              }
              /////////////////////////////////
	      if(SLOT!=21){
	         TClonesArray &ar_tof400 = *tof400;
                 new(ar_tof400[tof400->GetEntriesFast()]) BmnTDCDigit(serial,TYPE,SLOT,kFALSE,get_ch_tdc72vhl(TDCID,CH*4),0,VAL);
	      }     
           }

        }  
        else if(type==0x9){ if(_DEBUG_>1) printf("\tModule trailer: crc %X, err %X, word count %d\n",(d[i]>>23)&0xFF,(d[i]>>16)&0xF,d[i]&0xFFFF); }
      }
   };
   long long ts_t0_ns,ts_t0_s,GlobalEvent;
   int get_ch_tdc64v(unsigned int tdc,unsigned int ch){
      const int tdc64v_tdcch2ch[2][32] ={ { 31, 15, 30, 14, 13, 29, 28, 12, 11, 27, 26, 10, 25, 9, 24, 8, 23, 7, 22, 6, 21, 5, 20, 4, 19, 3, 18, 2, 17, 1, 16, 0 },
                                               { 31, 15, 30, 14, 29, 13, 28, 12, 27, 11, 26, 10, 25, 9, 24, 8, 23, 7, 22, 6, 21, 5, 20, 4, 19, 3, 18, 2, 17, 1, 16, 0 }}; 
      int val=tdc64v_tdcch2ch[tdc-1][ch];
      if(tdc==2) val+=32;
      return val;
   };
   int get_ch_tdc32vl(unsigned int tdc,unsigned int ch){
     const  int tdc32vl_tdcid2tdcnum[16] = { -1, 0, 1, -1, 3, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, -1};
     const  int tdc32vl_tdcch2ch[32] = {7,7,7,7, 6,6,6,6, 5,5,5,5, 4,4,4,4, 3,3,3,3, 2,2,2,2, 1,1,1,1, 0,0,0,0};
     return tdc32vl_tdcid2tdcnum[tdc]*8+tdc32vl_tdcch2ch[ch];
   };
   int get_ch_tdc64vhle(unsigned int tdc,unsigned int ch){
     const  int tdc64vhle_tdcid2tdcnum[16] = { 0, 1, 2, 3, 4, 5, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1};
     const  int tdc64vhle_tdcch2ch[32] = {0,0,0,0, 1,1,1,1, 2,2,2,2, 3,3,3,3, 4,4,4,4, 5,5,5,5, 6,6,6,6, 7,7,7,7};
     return tdc64vhle_tdcid2tdcnum[tdc]*8+tdc64vhle_tdcch2ch[ch];
   };
   int get_ch_tdc72vhl(unsigned int tdc,unsigned int ch){
     const int tdc72vhl_tdcid2tdcnum[16] = {  2, 1, 0, 5, 4, 3, 8, 7, 6, -1, -1, -1, -1, -1, -1, -1 };
     const int tdc72vhl_tdcch2ch[32] = { 7,7,7,7, 6,6,6,6, 5,5,5,5, 4,4,4,4, 3,3,3,3, 2,2,2,2, 1,1,1,1, 0,0,0,0};
     return tdc72vhl_tdcid2tdcnum[tdc]*8+tdc72vhl_tdcch2ch[ch%32]; 
   }
   void get_event_time(unsigned int d1,unsigned int d2,unsigned int d3,unsigned int d4){
      ts_t0_ns=d1&0x0FFFFFFF,ts_t0_s;
      ts_t0_ns=ts_t0_ns|((d2&0x3)<<28);
      if(((d2>>2)& 0x3)!=2){
          if(_DEBUG_>1) printf("Flag invalid for time stamp (%lld) in T0 crate!\n", ts_t0_ns);
          ts_t0_ns = 0;
      } else ts_t0_s=((d2>>4)&0xFFFFFF);
      ts_t0_s=ts_t0_s|((d3&0xFFFF)<<24);
      GlobalEvent=((d4&0x0FFFFFFF)<<12)|((d3>>16)&0xFFF);
      if(RUN<189){      
         long long xts = ts_t0_s%3LL;
         long long yts = (xts*1000000000LL+ts_t0_ns)%48LL;
         ts_t0_ns += (48LL-yts);
      }
   };   
public:
  BmnDataToRoot(char *file){
     char str[200];
     EVENT=0;
     TRIGWORD=0;
     if (sscanf(&file[strlen(file)-9],"%d",&RUN) != 1)
     {
        if (sscanf(&file[strlen(file)-8],"%d",&RUN) != 1)
        {
	    printf("Can't obtain run number from file %s\n", file);
	    return;
        };
     };
     printf("Processing run: %d\n",RUN);
     sprintf(str,"bmn_run%04d.root",RUN);
     File=new TFile(str,"recreate");
     File->SetCompressionLevel(1); //try level 2 also
     Tree=new TTree("BMN_RAW","test_bmn");
  
     sync    = new TClonesArray("BmnSyncDigit");
     ttb     = new TClonesArray("BmnTTBDigit");
     dch     = new TClonesArray("BmnTDCDigit");
     t0      = new TClonesArray("BmnTDCDigit");
     tof700  = new TClonesArray("BmnTDCDigit");
     tof400  = new TClonesArray("BmnTDCDigit");
     trigger = new TClonesArray("BmnTDCDigit");
     counters= new TClonesArray("BmnTDCDigit");
     ecal    = new TClonesArray("BmnADCDigit");
     zdc     = new TClonesArray("BmnADCDigit");
     Tree->Branch("bmn_sync",        &sync);   
     Tree->Branch("bmn_ttb",         &ttb);   
     Tree->Branch("bmn_dch",         &dch);    
     Tree->Branch("bmn_t0",          &t0);     
     Tree->Branch("bmn_tof700",      &tof700); 
     Tree->Branch("bmn_tof400",      &tof400); 
     Tree->Branch("bmn_trigger",     &trigger);
     Tree->Branch("bmn_beamcounters",&counters);
     Tree->Branch("bmn_ecal",        &ecal);   
     Tree->Branch("bmn_zdc",         &zdc);    
     Tree->Branch("bmn_trigword",    &TRIGWORD,"bmn_trigword/i");   
     
     in=fopen(file,"rb");
     if (in == NULL) perror(file);
  };
  ~BmnDataToRoot(){
     Tree->Print();
     Tree->Write(); 
     File->Close();
     fclose(in); 
  };
  int process_event(){
    sync->Clear();   
    ttb->Clear();   
    dch->Clear();    
    t0->Clear();     
    tof700->Clear(); 
    tof400->Clear(); 
    ecal->Clear();  
    zdc->Clear();    
    trigger->Clear();
    counters->Clear();
    EVENT=0;
    TRIGWORD=0;
    unsigned int dat,ret;
    for(;;){ if((ret=fread(&dat,sizeof(unsigned int),1,in))!=1) return -1; if(dat==0x2A502A50 || dat==0x4A624A62 ) break;} 
    if((ret=fread(&dat,sizeof(unsigned int),1,in))!=1) return -1;
    dat=dat/4+1;
    if(dat>=100000){ printf("Wrong data size: %d\n",dat); return 0; } 
    if((ret=fread(data,sizeof(unsigned int),dat,in))!=(dat)) return -1;
    if(data[0]>0){  
      fill(data,dat);
      Tree->Fill();
    }
    return EVENT; 
  };
  int fill(unsigned int *d,unsigned int len){
     if(_DEBUG_>0)printf("Event %d (%d)\n",d[0],len);
     EVENT=d[0];
     unsigned int ind=1;
     for(;ind<len;){
       unsigned int serial=d[ind++];
       unsigned int id=(d[ind]>>24)&0xFF;
       if(RUN<189){
          if(id==0xCA) serial=(serial&0xFFFFFF);
          if(id==0xD1) serial=(serial&0xFFFFFF00)>>8;
       }else{
          serial=(serial&0xFFFFFF);
       }      
       unsigned int payload=(d[ind++]&0xFFFFFF)/4;
       if(_DEBUG_>1) printf("serial: 0x%06X id: %X payload: %d\n",serial&0xFFFFFFFF,id,payload);
       if(payload >20000){ printf("Event %d !!!!!!!!!!! serial: 0x%06X id: %X payload: %d !!!!!!!!!!!\n",EVENT,serial&0xFFFFFFFF,id,payload); EVENT=0; break;} 
       if(id==0xCA) process_ADC(&data[ind],payload,serial);       
       if(id==0xD1) process_FVME(&d[ind],payload,serial);         
       ind+=payload;
     } 
     return EVENT; 
   }; 	
};

void BmnData(char *file){
  BmnDataToRoot D(file);
  for(;;){
   int ev;
   if((ev=D.process_event())<0) break;
   if((ev%10000)==1)printf("%d\n",ev-1);
   if((ev>50000)) break;
  }
}

