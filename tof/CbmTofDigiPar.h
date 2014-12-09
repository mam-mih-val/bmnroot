#ifndef CBMTOFDIGIPAR_H
#define CBMTOFDIGIPAR_H

#include "FairParGenericSet.h"

#include "TArrayI.h"
#include "TArrayD.h"

#include <map>

class CbmTofCell;
class FairParamList;

class CbmTofDigiPar : public FairParGenericSet
{
  public:

    CbmTofDigiPar(const char* name="CbmTofDigiPar",
                          const char* title="Digitization parameters for the TOF detector",
                          const char* context="TestDefaultContext");
    ~CbmTofDigiPar(void);
    void clear(void);
    void putParams(FairParamList*);
    Bool_t getParams(FairParamList*);

    void SetNrOfCells(Int_t i) { fNrOfCells = i; }
    void SetCellIdArray(TArrayI array) { fCellIdArray = array; }
    void SetCellXArray(TArrayD array) { fCellXArray = array; }
    void SetCellYArray(TArrayD array) { fCellYArray = array; }
    void SetCellZArray(TArrayD array) { fCellZArray = array; }
    void SetCellDxArray(TArrayD array) { fCellDxArray = array; }
    void SetCellDyArray(TArrayD array) { fCellDyArray = array; }

    void SetCellMap(std::map<Int_t, CbmTofCell*> map)
                      { fCellMap = map;}

    Int_t GetNrOfModules() { return fNrOfCells;}
    Int_t GetCellId(Int_t i) {return fCellIdArray[i];}

    CbmTofCell* GetCell(Int_t i) {return fCellMap[i];}


  private:

    /** Map of Unique Tof Cell Id to corresponding TofCell **/
    std::map<Int_t, CbmTofCell*> fCellMap;                  //!
    std::map<Int_t, CbmTofCell*>::iterator fCellMapIt;      //!

    TArrayI fCellIdArray; // Array to hold the unique IDs for all cells
    TArrayD fCellXArray; // Array to hold the unique IDs for all cells
    TArrayD fCellYArray; // Array to hold the unique IDs for all cells
    TArrayD fCellZArray; // Array to hold the unique IDs for all cells
    TArrayD fCellDxArray; // Array to hold the unique IDs for all cells
    TArrayD fCellDyArray; // Array to hold the unique IDs for all cells
    Int_t fNrOfCells; // Total number of cells

    CbmTofDigiPar(const CbmTofDigiPar&);
    CbmTofDigiPar& operator=(const CbmTofDigiPar&);

    ClassDef(CbmTofDigiPar,1)
};

#endif
