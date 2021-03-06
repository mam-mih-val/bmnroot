/**
 * \file BmnStudyReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "BmnStudyReport.h"
#include "BmnReportElement.h"
#include "BmnHistManager.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "TFile.h"

using std::ofstream;
using std::vector;
using std::stringstream;

BmnStudyReport::BmnStudyReport()
 : BmnReport(),
   fHM(),
   fStudyNames()
{

}

BmnStudyReport::~BmnStudyReport()
{

}

void BmnStudyReport::Create(
      const vector<BmnHistManager*>& histManagers,
      const vector<string>& studyNames,
      const string& outputDir)
{
   assert(histManagers.size() == studyNames.size());
   fHM = histManagers;
   fStudyNames = studyNames;
   SetOutputDir(outputDir);
   CreateReports();
}

void BmnStudyReport::Create(
      const vector<string>& fileNames,
      const vector<string>& studyNames,
      const string& outputDir)
{
	assert(fileNames.size() == studyNames.size());
	Int_t nofStudies = fileNames.size();
	vector<TFile*> files(nofStudies);
	fHM.resize(nofStudies);
	for (Int_t i = 0; i < nofStudies; i++) {
		fHM[i] = new BmnHistManager();
		files[i] = new TFile(fileNames[i].c_str());
		fHM[i]->ReadFromFile(files[i]);
	}
   fStudyNames = studyNames;
	SetOutputDir(outputDir);

	CreateReports();

	// Do not delete histogram managers and files.
	// Otherwise histograms disappear from canvases
	// and are not saved to file.
//	for (Int_t i = 0; i < nofStudies; i++) {
//		delete fHM[i];
//		delete files[i];
//	}
//	fHM.clear();
//	files.clear();
}

ClassImp(BmnStudyReport)
