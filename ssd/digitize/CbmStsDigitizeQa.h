#ifndef CBMSTSDIGITIZEQA_H_
#define CBMSTSDIGITIZEQA_H_

#include "FairTask.h"
#include "CbmStsDigi.h"
#include "CbmHistManager.h"
#include "CbmMCDataArray.h"
#include <iostream>
#include <fstream>
#include <vector>
class TClonesArray;
class CbmStsDigitizeParameters;
class CbmStsDigitize;
class CbmStsSetup;

class CbmStsDigitizeQa : public FairTask
{
    public:
	CbmStsDigitizeQa(CbmStsDigitize * digitizer=NULL);

	virtual ~CbmStsDigitizeQa();

	virtual InitStatus Init();

	virtual void Exec(Option_t* opt);

        virtual void SetParContainers();
        
	virtual void Finish();

	void SetOutputDir(const std::string& outputDir) { fOutputDir = outputDir; }

	void CreateHistograms();

	void CreateNofObjectsHistograms();

	void CreateDigiHistograms();

	void ProcessDigisAndPoints(const TClonesArray* digis, const TClonesArray * points);
	void ProcessAngles();

    private:
	void ReadDataBranches();

        CbmStsDigitizeParameters* fDigiPar;
	CbmHistManager* fHM;
	std::string fOutputDir;
	TClonesArray* fStsDigis;
	TClonesArray* fStsPoints;
	CbmStsSetup * fSetup;
	Int_t fNofStation;

	Int_t fMaxScale;
	std::ofstream fOutFile;
	std::vector < std::vector <std::vector <std::vector < std::vector <Int_t>>>>> fnOfDigisChip;

    CbmStsDigitizeQa(const CbmStsDigitizeQa&) = delete;
    CbmStsDigitizeQa& operator=(const CbmStsDigitizeQa&) = delete;

	ClassDef(CbmStsDigitizeQa, 1);
};

#endif
