#include "TROOT.h"

#include <zlib.h>

class MpdLibZ
{
  public:
    MpdLibZ (char* filename);
    ~MpdLibZ();
   
    int open(const char* mode);
    int eof();
    int close();
    int write(voidpc buf, unsigned len);
    int read(voidp buf, unsigned len);
    char* gets(char* buf, int len);
    int puts(char* s);
    off_t tell ();
    off_t seek (off_t pos, int whence);

  private:
    char* fileName;
    gzFile file;

    ClassDef(MpdLibZ, 1);
};

class MpdGetNumEvents
{
  private:
    MpdGetNumEvents(){}
    ~MpdGetNumEvents(){}

    static bool GetQGSMEventHeader(char* ss, MpdLibZ* libz, Int_t* fQGSM_format_ID);

  public:
    static Int_t GetNumROOTEvents(char* filename);
    static Int_t GetNumPHSDEvents(char* filename);
    static Int_t GetNumQGSMEvents(char* fileName);
    static Int_t GetNumURQMDEvents(char* fileName);

    ClassDef(MpdGetNumEvents, 1);
};
