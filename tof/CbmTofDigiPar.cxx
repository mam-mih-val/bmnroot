#include "CbmTofDigiPar.h"

#include "CbmTofCell.h"

#include "FairParamList.h"
#include "FairLogger.h"

#include <iostream>

ClassImp(CbmTofDigiPar)

CbmTofDigiPar::CbmTofDigiPar(const char* name,
    const char* title,
    const char* context)
  : FairParGenericSet(name,title,context),
    fCellMap(),
    fCellMapIt(),
    fCellIdArray(),
    fCellXArray(),
    fCellYArray(),
    fCellZArray(),
    fCellDxArray(),
    fCellDyArray(),
    fNrOfCells(-1)
{
	detName="Tof";
}

CbmTofDigiPar::~CbmTofDigiPar(void)
{
	  LOG(DEBUG4)<<"Enter CbmTrdDigiPar destructor"<< FairLogger::endl;
	  for (fCellMapIt = fCellMap.begin(); fCellMapIt != fCellMap.end();
	       ++fCellMapIt) {
	    delete fCellMapIt->second;
	  }
	  fCellMap.clear();
	  clear();
	  LOG(DEBUG4)<<"Leave CbmTrdDigiPar destructor"<< FairLogger::endl;
}

void CbmTofDigiPar::clear(void)
{
	  status = kFALSE;
	  resetInputVersions();
}

void CbmTofDigiPar::putParams(FairParamList* l)
{
  if (!l) { return; }

  l->add("NrOfCells", fNrOfCells);
  l->add("CellIdArray", fCellIdArray);
  l->add("CellXArray", fCellXArray);
  l->add("CellYArray", fCellYArray);
  l->add("CellZArray", fCellZArray);
  l->add("CellDxArray", fCellDxArray);
  l->add("CellDyArray", fCellDyArray);

}

Bool_t CbmTofDigiPar::getParams(FairParamList* l)
{
  if (!l) { return kFALSE; }

  LOG(DEBUG2)<<"Get the tof digitization parameters."<<FairLogger::endl;

  if ( ! l->fill("NrOfCells", &fNrOfCells) ) return kFALSE;

  LOG(DEBUG2)<<"There are "<< fNrOfCells<<" cells to be read."<<FairLogger::endl;

  fCellIdArray.Set(fNrOfCells);
  if ( ! l->fill("CellIdArray", &fCellIdArray) ) return kFALSE;

  fCellXArray.Set(fNrOfCells);
  if ( ! l->fill("CellXArray", &fCellXArray) ) return kFALSE;

  fCellYArray.Set(fNrOfCells);
  if ( ! l->fill("CellYArray", &fCellYArray) ) return kFALSE;

  fCellZArray.Set(fNrOfCells);
  if ( ! l->fill("CellZArray", &fCellZArray) ) return kFALSE;

  fCellDxArray.Set(fNrOfCells);
  if ( ! l->fill("CellDxArray", &fCellDxArray) ) return kFALSE;

  fCellDyArray.Set(fNrOfCells);
  if ( ! l->fill("CellDyArray", &fCellDyArray) ) return kFALSE;


  for (Int_t i=0; i < fNrOfCells; i++){
    fCellMap[fCellIdArray[i]] = new CbmTofCell(fCellIdArray[i],fCellXArray[i] ,
        fCellYArray[i], fCellZArray[i],
        fCellDxArray[i], fCellDyArray[i]);

  }
  return kTRUE;
}
