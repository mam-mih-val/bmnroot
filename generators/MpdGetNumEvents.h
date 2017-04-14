#include "TROOT.h"

#ifndef __CINT__
#include <zlib.h>
#endif

class MpdLibZ
{
  public:
    MpdLibZ (char* filename);
    ~MpdLibZ();
   
    int open(const char* mode);
    int eof();
    int close();
    #ifndef __CINT__
    int write(voidpc buf, unsigned len);
    int read(voidp buf, unsigned len);
    #endif
    char* gets(char* buf, int len);
    int puts(char* s);
    #ifndef __CINT__
    off_t tell ();
    off_t seek (off_t pos, int whence);
    #endif

  private:
    char* fileName;
    #ifndef __CINT__
    gzFile file;
    #endif

    ClassDef(MpdLibZ, 1);
};

class MpdGetNumEvents
{
  private:
    MpdGetNumEvents(){}
    ~MpdGetNumEvents(){}

    static bool GetQGSMEventHeader(char* ss, MpdLibZ* libz, Int_t& fQGSM_format_ID);

  public:
    static Int_t GetNumROOTEvents(char* filename);
    static Int_t GetNumPHSDEvents(char* filename);
    static Int_t GetNumQGSMEvents(char* fileName);
    static Int_t GetNumURQMDEvents(char* fileName);

    ClassDef(MpdGetNumEvents, 1);
};
