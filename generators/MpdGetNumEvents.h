#include "TROOT.h"

#ifndef __CLING__
#include <zlib.h>
#endif

class MpdLibZ
{
  public:
    MpdLibZ (const char* filename);
    ~MpdLibZ();
   
    int open(const char* mode);
    int eof();
    int close();
    #ifndef __CLING__
    int write(voidpc buf, unsigned len);
    int read(voidp buf, unsigned len);
    #endif
    char* gets(char* buf, int len);
    int puts(char* s);
    #ifndef __CLING__
    off_t tell ();
    off_t seek (off_t pos, int whence);
    #endif

  private:
    const char* fileName;
    #ifndef __CLING__
    gzFile file;
    #endif

    ClassDef(MpdLibZ, 1);
};

class MpdGetNumEvents
{
  private:
    MpdGetNumEvents(){}
    ~MpdGetNumEvents(){}

    static bool GetQGSMEventHeader(char* ss, MpdLibZ* libz, Int_t& fQGSM_format_ID, int iVerbose);

  public:
    static Int_t GetNumPHSDEvents(const char* filename, int iVerbose = 2);
    static Int_t GetNumQGSMEvents(const char* fileName, int iVerbose = 2);
    static Int_t GetNumURQMDEvents(const char* fileName, int iVerbose = 2);
    static Int_t GetNumDCMSMMEvents(const char* fileName, int iVerbose = 2);
    static Int_t GetNumROOTEvents(const char* filename, const char* treename = "", int iVerbose = 2);

    ClassDef(MpdGetNumEvents, 1);
};
