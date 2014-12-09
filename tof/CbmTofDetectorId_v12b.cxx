// -------------------------------------------------------------------------
// -----                   CbmTofDetectorId source file                -----
// -----                  Created 20/11/12  by F. Uhlig                -----
// -------------------------------------------------------------------------


#include "CbmTofDetectorId_v12b.h"
#include "CbmDetectorList.h"

const Int_t CbmTofDetectorId_v12b::shiftarray[] = {0,5,9,17,20,24};
const Int_t CbmTofDetectorId_v12b::bitarray[] = {5,4,8,3,4,8};


CbmTofDetectorId_v12b::CbmTofDetectorId_v12b()
  : CbmTofDetectorId(),
    result_array(),
    maskarray(),
    modulemask(0)
{
  for (Int_t i=0; i < array_length; i++) {
	maskarray[i]=(1 << bitarray[i]) - 1;
  }

  modulemask =( (maskarray[0] << shiftarray[0]) |
		  (maskarray[1] << shiftarray[1]) |
		  (maskarray[2] << shiftarray[2]) |
		  (maskarray[3] << shiftarray[3])  |
		  (0 << shiftarray[4]) |
		  (maskarray[5] << shiftarray[5])
  );
}

CbmTofDetectorInfo CbmTofDetectorId_v12b::GetDetectorInfo(const Int_t detectorId)
{
  for (Int_t i=0; i < array_length; i++) {
   result_array[i] = (( detectorId >> shiftarray[i] ) & maskarray[i] );
  }

  return CbmTofDetectorInfo(result_array[0], result_array[1], result_array[2], 
			    result_array[3], result_array[4], result_array[5]);

}

Int_t  CbmTofDetectorId_v12b::GetSystemId(Int_t detectorId) 
{
  return (detectorId & maskarray[0]);
}

//-----------------------------------------------------------

Int_t CbmTofDetectorId_v12b::GetSMType(const Int_t detectorId)
{
  return (( detectorId >> shiftarray[1] ) & maskarray[1] );
}

//-----------------------------------------------------------

Int_t CbmTofDetectorId_v12b::GetSModule(const Int_t detectorId)
{
  return (( detectorId >> shiftarray[2] ) & maskarray[2] );
}

//-----------------------------------------------------------

Int_t CbmTofDetectorId_v12b::GetCounter(const Int_t detectorId)
{
   return (( detectorId >> shiftarray[3] ) & maskarray[3] );
}

//-----------------------------------------------------------

Int_t CbmTofDetectorId_v12b::GetGap(const Int_t detectorId)
{
   return (( detectorId >> shiftarray[4] ) & maskarray[4] );
}

//-----------------------------------------------------------

Int_t CbmTofDetectorId_v12b::GetCell(const Int_t detectorId)
{
   return (( detectorId >> shiftarray[5] ) & maskarray[5] );
}

//-----------------------------------------------------------

Int_t CbmTofDetectorId_v12b::GetRegion(const Int_t detectorId)
{
  return -1;
}

Int_t CbmTofDetectorId_v12b::GetCellId(const Int_t detectorId)
{
  return (detectorId & modulemask);
}

//-----------------------------------------------------------

Int_t CbmTofDetectorId_v12b::SetDetectorInfo(const CbmTofDetectorInfo detInfo)
{
  return ( (detInfo.fDetectorSystem << shiftarray[0]) | 
           (detInfo.fSMtype << shiftarray[1]) | 
           (detInfo.fSModule << shiftarray[2]) | 
           (detInfo.fCounter << shiftarray[3])  | 
           (detInfo.fGap << shiftarray[4])  |
           (detInfo.fCell << shiftarray[5])  
         ); 
}

