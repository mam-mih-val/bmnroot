// utilities for ROOT macro analysis
//Author:  Dmitry Finogeev, dmitry.finogeev@cern.ch


#ifndef UTILITES_H
#define UTILITES_H
//header what include most popular root classes

//#include "CanvasShow/canvasshow.h"
//#include "CanvasShow/canvasshow.cxx"

#include <TObjArray.h>
#include <TFile.h>
#include <TChain.h>
#include <TObjString.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>

#include<TROOT.h>
#include<TSystem.h>
#include<TSystemDirectory.h>
#include<TSystemFile.h>
#include<TList.h>
#include<TCollection.h>
#include<TMath.h>
#include<TDirectory.h>
#include<TStopwatch.h>
#include<TGraph.h>
#include<TBrowser.h>
#include<TCanvas.h>
#include<TProfile.h>
#include<TStyle.h>
#include<TVectorD.h>


#include <iostream>
#include <fstream>

#define OUTINFILE 0

#if OUTINFILE
std::ofstream current_out ("CurrentLog.txt");
#else
std::ostream &current_out = std::cout;
#endif

#define uVERBOSE 1 // 1 - errors; 2 - results; 3 - details;      4 - debugging; 5 - flood
#define fit_funk_name "fitFunction"

using namespace std;

Int_t FilesListFromDir(const TString filespath, TObjArray *const collection, Int_t max_num, TString content);
Int_t SaveArrayStructInFile(TCollection *array, TFile *file, TString start_path = "");
Int_t SaveArrayOfObjectsArraysInFile(TObjArray *ObjectsArray = 0, const TString filename = "data", const TString path = "./", const Int_t deleteArrays = 0);

Int_t FitHistogramm(TH1 *histogramm, Double_t &mean, Double_t &sigma, Double_t RangeXmin = 0., Double_t RangeXmax = 0., TString fit_formula = "gaus");
Int_t FitHistogrammInFirstMaxPeak(TH1* histogramm, Double_t &Mean, Double_t &Sigma, Double_t peaksRangeMin,
                                  Double_t peaksRangeMax, Double_t peakRange, Double_t peakRMSRange = 1.5);
Int_t FitHistogrammInRMSRange(TH1 *histogramm, Double_t &mean, Double_t &sigma, Double_t RMSrange, Double_t RangeXmin = 0, Double_t RangeXmax = 0);
Int_t FitHistogrammInMaxPeak(TH1 *histogramm, Double_t &mean, Double_t &sigma, Double_t PeakWidth, Double_t RangeXmin = 0., Double_t RangeXmax = 0.);

Int_t ArrayStat(const Int_t nElements, const Double_t* Array, Double_t &maxElement, Double_t &minElement, Double_t &Amplitude, Double_t &Mean, Double_t &RMS);
void ResetArray(TObjArray **array, const TString option);


Int_t ConstructFilesListFromDirectory(const TString &Directory, TCollection *const collection)
{
    TString filespath = Directory;
    if(!filespath.EndsWith("/")) filespath += "/";

    if(!collection)
    {
	if(uVERBOSE >= 1) current_out << Form("utilites.h/ConstructFilesListInDirectory: collection ptr is NULL")<<std::endl;
	return 0;
    }

    TSystemFile *currFile;
    TString currFileName;
    TSystemDirectory SDirectory(filespath, filespath);
    TIter nextfile(SDirectory.GetListOfFiles());

    Int_t nfiles = 0;
    while ((currFile=(TSystemFile*)nextfile()))
    {
	currFileName = filespath + (currFile->GetName());
	if (!currFile->IsDirectory() && currFileName.EndsWith("root"))
	{
	    collection->Add( new TObjString( currFileName.Data() ) );

	    if(uVERBOSE >= 3) current_out << Form("utilites.h/ConstructFilesListInDirectory: found root file \"%s\"",
	                                          currFileName.Data() )<<std::endl;
	    nfiles++;
	}
    }

    return nfiles;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Int_t RUNnumberInString(const TString &str)
{
    const Int_t lenght = 6;
    Int_t value;
    TString sub_str;
    for(Int_t start=0; start <= str.Sizeof() - lenght; start++)
    {
	sub_str = str(start, lenght);
	value = sub_str.Atoi();
	//printf("debug:start = %i; strpath = %s; value = %i\n",start, sub_str.Data(), value);
	if( value < 999999 && 100000 < value) return value;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
TObject* CreateXprofile(const TH1 *sourcehist)
{
    TProfile *profile = ((TH2*)sourcehist)->ProfileX( Form("%s_prof",sourcehist->GetName()) , 1, -1, "o");
    profile->SetTitle(  sourcehist->GetTitle() );
    if(uVERBOSE >= 2) current_out << Form("utilites.h/CreateXprofile: profile \"%s\" was created",sourcehist->GetName())<<std::endl;

    return (TObject*)profile;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Int_t CollectRAWfilesInChain(TChain* rawTree, const TString dirname, const Int_t maxNfiles = 50)
{

    TString filespath = dirname;
    if(!filespath.EndsWith("/")) filespath += "/";

    if(uVERBOSE >= 2) current_out << Form("utilites.h/CollectRAWfilesInChain: collecting files from \"%s\"",filespath.Data())<<std::endl;

    TSystemFile *currFile;
    TString currFileName;
    TSystemDirectory SDirectory(filespath, filespath);
    TIter nextfile(SDirectory.GetListOfFiles());

    Int_t NfilesLoaded = 0;
    while ((currFile=(TSystemFile*)nextfile()))
    {
	currFileName = filespath + (currFile->GetName());
	if (!currFile->IsDirectory() && currFileName.EndsWith("root"))
	{
	    rawTree->AddFile( currFileName.Data(), 0, "t0tree" );
	    if(uVERBOSE >= 4) current_out << Form("utilites.h/CollectRAWfilesInChain: %i file \"%s\" was added in chain",
	                                          NfilesLoaded,currFileName.Data())<<std::endl;
	    NfilesLoaded++;
	}

	if(maxNfiles <= NfilesLoaded) break;
    }

    if(uVERBOSE >= 2) current_out << Form("utilites.h/CollectRAWfilesInChain: collected %d files, total events: %lld",NfilesLoaded, rawTree->GetEntries())<<std::endl;

    return NfilesLoaded;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
Int_t FilesListFromDir(const TString Directory, TObjArray *const collection, Int_t max_num, TString content)
{
    TString filespath = Directory;

    if(!filespath.EndsWith("/")) filespath += "/";

    if(!collection)
    {
	current_out << Form("ERROR: Collection ptr is NULL") << std::endl;
	return 0;
    }

    TSystemFile *currFile;
    TString currFileName;
    TSystemDirectory SDirectory(filespath, filespath);
    TIter nextfile(SDirectory.GetListOfFiles());

    Int_t nfiles = 0;
    while ((currFile=(TSystemFile*)nextfile()))
    {
	currFileName = currFile->GetName();
	if (!currFile->IsDirectory() && currFileName.Contains(content))
	{
	    collection->Add( new TObjString( currFileName.Data() ) );
	    nfiles++;
	    if(max_num > 0)
		if(nfiles == max_num) break;
	}
    }

    return nfiles;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
Int_t SaveArrayStructInFile(TCollection *array, TFile *file, TString start_path)
{
    //if(start_path = "") start_path = "/";
    //file->cd(start_path.Data());

    //file->mkdir(array->GetName());

	if(array == 0) return 0;

    if(start_path == "")
    {
	start_path = (TString)array->GetName();
    }
    else
    {
	start_path += "/" + (TString)array->GetName();
    }

    file->mkdir(start_path.Data());
    file->cd(start_path.Data());

    TIter nx_iter((TCollection*)(array));
    TObject* obj_ptr;
    while ( (obj_ptr=(TObjArray*)nx_iter()) )
    {
	if(obj_ptr == 0) continue; 

	if(TCollection *new_coll = dynamic_cast<TCollection*>(obj_ptr))
	{
	    SaveArrayStructInFile(new_coll, file, start_path);
	    file->cd(start_path.Data());
	}
	else
	{
	   if(uVERBOSE >= 2) current_out << Form("Writing %s object", obj_ptr->GetName()) << std::endl;
	    obj_ptr->Write();
	}
    }

    return 1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
Int_t SaveArrayOfObjectsArraysInFile(TObjArray *ObjectsArray,const TString filename,  const TString path, const Int_t deleteArrays)
{
    //    TList *ObjectsArray = (TList*)ObjectsArray_;
    if( !ObjectsArray )
    {
	if(uVERBOSE >= 1) current_out <<Form("utilites.h/SaveArrayObjectsArrayInFile::ERROR: Array does NOT exist")<<std::endl;
	return 0;
    }

    if(ObjectsArray->GetLast() < 0)
    {
	if(uVERBOSE >= 1) current_out <<Form("utilites.h/SaveArrayObjectsArrayInFile: WARING: Objects array is empty")<<std::endl;
	return 0;
    }

    TString resultfilename = filename;
    if(!resultfilename.EndsWith(".root")) resultfilename += ".root";

    TObject* pobjCurrObject = NULL;
    TString objectname;

    TFile *file = new TFile(resultfilename.Data(), "UPDATE");

    if(file->IsOpen())
    {
	if(uVERBOSE >= 2) current_out <<Form("utilites.h/SaveArrayObjectsArrayInFile: Writing arrays in file \"%s\" ... ", resultfilename.Data())<<std::endl;
    }
    else
    {
	if(uVERBOSE >= 1) current_out <<Form("utilites.h/SaveArrayObjectsArrayInFile::ERROR: File \"%s\" was NOT opened for UPDATE",resultfilename.Data())<<std::endl;
	return 0;
    }


    for(Int_t n=0; n<ObjectsArray->GetEntries(); n++)
    {
	pobjCurrObject = ObjectsArray->At(n);
	if(pobjCurrObject == NULL) continue;

	objectname = pobjCurrObject->GetName();
	if(uVERBOSE >= 3) current_out <<Form("utilites.h/SaveArrayObjectsArrayInFile::Saving \"%s\" ...",objectname.Data())<<std::endl;
	file->rmdir(objectname.Data()); //delete old data to rewrite
	TDirectory *CurrDir = file->mkdir(objectname.Data()); CurrDir->cd();
	pobjCurrObject->Write();

	if(uVERBOSE >= 2) current_out <<Form("utilites.h/SaveArrayObjectsArrayInFile::Object \"%s\" was successfully writed",objectname.Data())<<std::endl;
    }

    file->Close();
    delete file;

    gDirectory->cd("Rint:/");

    TObjArray *currArray = NULL;
    if(deleteArrays) //
    {
	for(Int_t n=0; n<ObjectsArray->GetEntries(); n++)
	{
	    currArray = (TObjArray*)ObjectsArray->At(n);
	    if(currArray == NULL) continue;
	    objectname = currArray->GetName();
	    if(uVERBOSE >= 2) current_out <<Form("utilites.h/SaveArrayObjectsArrayInFile::Deleting \"%s\"",objectname.Data())<<std::endl;
	    ResetArray(&currArray, "delete");
	}
	ObjectsArray->Clear();
    }

    return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Int_t FitHistogrammInFirstMaxPeak(TH1* histogramm, Double_t &Mean, Double_t &Sigma, Double_t peaksRangeMin,
                                  Double_t peaksRangeMax, Double_t peakRange, Double_t peakRMSRange)
{
    const Double_t peakLeftMinValueRatio = 0.5;

    histogramm->SetAxisRange(peaksRangeMin, peaksRangeMax); //cutting saturation peak
    Double_t PeakMax = histogramm->GetBinCenter( histogramm->GetMaximumBin() );
    Double_t PeakMaxValue = histogramm->GetBinContent(histogramm->GetMaximumBin());
    Double_t PeakLeftEdge = histogramm->GetBinCenter(histogramm->FindFirstBinAbove(PeakMaxValue*peakLeftMinValueRatio)-2);
    histogramm->SetAxisRange(PeakLeftEdge, PeakMax+peakRange); //cutting saturation peak
    Double_t PeakRMS = histogramm->GetRMS();

    FitHistogramm(histogramm, Mean, Sigma, PeakLeftEdge-PeakRMS*0.5, PeakMax+PeakRMS*peakRMSRange);

    histogramm->SetAxisRange(peaksRangeMin, peaksRangeMax); //cutting saturation peak

    return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Int_t FitHistogrammInRMSRange(TH1 *histogramm, Double_t &mean, Double_t &sigma, Double_t RMSrange, Double_t RangeXmin, Double_t RangeXmax)
{
    if( !histogramm ){if(uVERBOSE >= 1)
	    current_out <<Form("utilites.h/FitHistogrammInRMSRange::ERROR: histogramm pointer is NULL")<<std::endl; return 0;}

    if(RangeXmax <= RangeXmin)
    {
	RangeXmin = histogramm->GetXaxis()->GetXmin();
	RangeXmax = histogramm->GetXaxis()->GetXmax();
    }

    Double_t hist_start_Xmin = histogramm->GetXaxis()->GetXmin();
    Double_t hist_start_Xmax = histogramm->GetXaxis()->GetXmax();

    histogramm->SetAxisRange(RangeXmin, RangeXmax);
    Double_t histMean = histogramm->GetMean();
    Double_t histRMS = histogramm->GetRMS();
    histogramm->SetAxisRange(hist_start_Xmin, hist_start_Xmax);

    Double_t fit_range_min = histMean-RMSrange*histRMS;
    Double_t fit_range_max = histMean+RMSrange*histRMS;

    if(fit_range_min < RangeXmin) fit_range_min = RangeXmin;
    if(fit_range_max > RangeXmax) fit_range_max = RangeXmax;

    return FitHistogramm(histogramm, mean, sigma, fit_range_min, fit_range_max);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
Int_t FitHistogrammInMaxPeak(TH1 *histogramm, Double_t &mean, Double_t &sigma, Double_t PeakWidth, Double_t RangeXmin, Double_t RangeXmax)
{
    if( !histogramm ){if(uVERBOSE >= 1)
	    current_out <<Form("utilites.h/FitHistogrammInMaxPeak::ERROR: histogramm pointer is NULL")<<std::endl; return 0;}


    if(RangeXmax <= RangeXmin)
    {
	RangeXmin = histogramm->GetXaxis()->GetXmin();
	RangeXmax = histogramm->GetXaxis()->GetXmax();
    }

    Double_t hist_start_Xmin = histogramm->GetXaxis()->GetXmin();
    Double_t hist_start_Xmax = histogramm->GetXaxis()->GetXmax();

    histogramm->SetAxisRange(RangeXmin, RangeXmax);
    Double_t PeakMax = histogramm->GetBinCenter( histogramm->GetMaximumBin() );
    histogramm->SetAxisRange(hist_start_Xmin, hist_start_Xmax);

    Double_t FitRangeMin = PeakMax-PeakWidth*0.5;
    Double_t FitRangeMax = PeakMax+PeakWidth*0.5;

    FitRangeMin = (FitRangeMin < RangeXmin)? RangeXmin : FitRangeMin;
    FitRangeMax = (FitRangeMax > RangeXmax)? RangeXmax : FitRangeMax;


    if(uVERBOSE >= 4)current_out<<Form("utilites.h/FitHistogrammInMaxPeak: Hsit: \"%s\" Maximum: %.2f, Peak Width: %.2f, Range:[%.2f, %.2f]",
                                       histogramm->GetName(), PeakMax, PeakWidth, FitRangeMin, FitRangeMax)<<std::endl;


    return FitHistogramm(histogramm, mean, sigma,FitRangeMin,FitRangeMax);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Int_t FitHistogrammSmartInMaxPeak(TH1 *histogramm, Double_t &mean, Double_t &sigma,Double_t nRMSfit = 2., Double_t RangeXmin = 0., Double_t RangeXmax = 0.)
{
    static const Int_t nSmooth = 1;
    static const Double_t nSmoothRMScut = 1.;
    //static const Double_t nRMSfit = 2;

    if( !histogramm ){if(uVERBOSE >= 1)
	    current_out <<Form("utilites.h/FitHistogrammSmartInMaxPeak::ERROR: histogramm pointer is NULL")<<std::endl; return 0;}

    if(RangeXmax <= RangeXmin)
    {
	RangeXmin = histogramm->GetXaxis()->GetXmin();
	RangeXmax = histogramm->GetXaxis()->GetXmax();
    }

    histogramm->SetAxisRange(RangeXmin, RangeXmax);

    //searching fit ranges
    TH1 *tempSmoothProjection = (TH1*)histogramm->Clone("tempSmoothFitHist");
    tempSmoothProjection->Smooth(nSmooth);
    tempSmoothProjection->SetAxisRange(RangeXmin, RangeXmax);
    Double_t SmoothPeakMax = tempSmoothProjection->GetBinCenter( tempSmoothProjection->GetMaximumBin() );
    Double_t SmoothRMS = tempSmoothProjection->GetRMS();

    //maximum on hist edge:
    if( TMath::Abs(SmoothPeakMax - RangeXmax) < SmoothRMS*0.5 )
    {
	RangeXmax += SmoothRMS*0.5;
	tempSmoothProjection->SetAxisRange(RangeXmin, RangeXmax);
	SmoothPeakMax = tempSmoothProjection->GetBinCenter( tempSmoothProjection->GetMaximumBin() );
	SmoothRMS = tempSmoothProjection->GetRMS();
    } else if( TMath::Abs(SmoothPeakMax - RangeXmin) < SmoothRMS*0.5 )
    {
	RangeXmin -= SmoothRMS*0.5;
	tempSmoothProjection->SetAxisRange(RangeXmin, RangeXmax);
	SmoothPeakMax = tempSmoothProjection->GetBinCenter( tempSmoothProjection->GetMaximumBin() );
	SmoothRMS = tempSmoothProjection->GetRMS();
    }

    tempSmoothProjection->SetAxisRange(SmoothPeakMax-SmoothRMS*nSmoothRMScut, SmoothPeakMax+SmoothRMS*nSmoothRMScut);
    Double_t SmoothRMSreal = tempSmoothProjection->GetRMS();
    delete tempSmoothProjection;

    Double_t FitRangeMin = SmoothPeakMax-SmoothRMSreal*nRMSfit;
    Double_t FitRangeMax = SmoothPeakMax+SmoothRMSreal*nRMSfit;

    FitRangeMin = (FitRangeMin < RangeXmin)? RangeXmin : FitRangeMin;
    FitRangeMax = (FitRangeMax > RangeXmax)? RangeXmax : FitRangeMax;

    if(uVERBOSE >= 3)current_out<<Form("utilites.h/FitHistogrammSmartInMaxPeak: Hsit: \"%s\" Maximum: %.2f, SmoothRMS: %.2f, SmoothRMSreal: %.2f, Range:[%.2f, %.2f]",
                                       histogramm->GetName(), SmoothPeakMax, SmoothRMS, SmoothRMSreal, FitRangeMin, FitRangeMax)<<std::endl;
    //return NULL;
    return FitHistogramm(histogramm, mean, sigma, FitRangeMin, FitRangeMax);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Int_t FitHistogramm(TH1 *histogramm, Double_t &mean, Double_t &sigma, Double_t RangeXmin, Double_t RangeXmax, TString fit_formula)
{
    static const Int_t minFitEvents = 1;       //minimum events in range for fitting
    static const Int_t EventsNoFit = 15;

    TString fitoptions[] = {"QR", "QRL", "QRI", "END"};

    if( !histogramm ){if(uVERBOSE >= 1)
	    current_out <<Form("utilites.h/FitHistogramm::ERROR: histogramm pointer is NULL")<<std::endl; return 0;}


    if(RangeXmax <= RangeXmin)
    {
	RangeXmin = histogramm->GetXaxis()->GetXmin();
	RangeXmax = histogramm->GetXaxis()->GetXmax();
    }


    //if too low events for fitting returning 0
    Int_t intBinMin = histogramm->FindBin(RangeXmin);
    Int_t intBinMax = histogramm->FindBin(RangeXmax);
    Int_t integral =  histogramm->Integral(intBinMin, intBinMax);

    if( integral < minFitEvents){if(uVERBOSE >= 1)
	    current_out <<Form("utilites.h/FitHistogramm::ERROR: Too low events in \"%s\" histogram: %i",histogramm->GetName(), integral)<<std::endl; return 0;}

    if(uVERBOSE >= 2)
	current_out <<Form("utilites.h/FitHistogramm::Fiting histogramm \"%s\"",histogramm->GetName())<<std::endl;


    Double_t hist_start_Xmin = histogramm->GetXaxis()->GetXmin();
    Double_t hist_start_Xmax = histogramm->GetXaxis()->GetXmax();

    histogramm->SetAxisRange(RangeXmin, RangeXmax);
    Double_t histMean = histogramm->GetMean();
    Double_t histRMS = histogramm->GetRMS();
    histogramm->SetAxisRange(hist_start_Xmin, hist_start_Xmax);


    //if low events, only RMS and mean;
    if( integral <= EventsNoFit )
    {
	mean = histMean;
	sigma = histRMS;
	return 2;
    }

    TF1 *currfitfunction = NULL;
    TF1 *bestfitfunction = NULL;
    Int_t bestOption = -111;

    Double_t bestChisquare = -111., currChisquare = 0;

    for(Int_t opt = 0; !fitoptions[opt].Contains("END"); opt++){

	currfitfunction = new TF1(fit_funk_name, fit_formula.Data(), RangeXmin, RangeXmax);

	histogramm->Fit(currfitfunction, fitoptions[opt], "", RangeXmin, RangeXmax);
	currChisquare = currfitfunction->GetChisquare();

	if(uVERBOSE >= 3)current_out<<Form("utilites.h/FitHistogramm: current fiting option: \"%s\", chi-square: %.1f",fitoptions[opt].Data(), currChisquare)<<std::endl;
	if( ( (currChisquare < bestChisquare)&&(currChisquare > 0.0001) )||( (bestChisquare < 0.)&&(currChisquare > 0.0001)) )
	{
	    bestChisquare = currChisquare;
	    if(bestfitfunction) delete bestfitfunction;
	    bestfitfunction = currfitfunction;
	    bestOption = opt;
	}
	else
	{
	    if(currfitfunction) delete currfitfunction;
	}
    }


    if( 0. < bestChisquare)
    {
	histogramm->Fit(bestfitfunction, fitoptions[bestOption], "", RangeXmin, RangeXmax);
	if(uVERBOSE >= 2)current_out<<Form("utilites.h/FitHistogramm: best fit [%.2f, %.2f]: %.2f, option %i",
	                                   RangeXmin, RangeXmax, bestChisquare, bestOption)<<std::endl;
	histogramm->GetFunction(fit_funk_name)->SetRange(RangeXmin, RangeXmax); //to see selected range on func at hist


	mean = bestfitfunction->GetParameter(1);
	sigma = bestfitfunction->GetParameter(2);
    }
    else
    {
	if(uVERBOSE >= 1)current_out<<Form("WARING: fitting failure for all options")<<std::endl;
	mean = histMean;
	sigma = histRMS;
    }

    if(bestfitfunction) delete bestfitfunction;
    return 1;

}
///////////////////////////////////////////////////////////////////////////////////////////////////
void ResetArray(TObjArray **array, TString option)
{
    Int_t ioption;
    TObject *currObject = NULL;

    //if option is incorrect
    if(option.Contains("delete")) ioption = 0;
    else if(option.Contains("recreate")) ioption = 1;
    else {if(uVERBOSE >= 1) current_out <<Form("utilites.h/ResetArray::ERROR: array \"%s\" was NOT correctly deleted, wrong option \"%s\"",(*array)->GetName(), option.Data())<<std::endl; return;}

    //if array is empty
    if(!(*array))
    {
	if(ioption == 1){*array = new TObjArray(); return;}
	if(ioption == 0)return;
    }

    for(Int_t element = 0; element <= (*array)->GetLast(); element++)
    {
	currObject = (*array)->At(element);
	if(!currObject)continue;
	if(uVERBOSE >= 4) current_out <<Form("utilites.h/ResetArray::Deleting object \"%s\"",currObject->GetName())<<std::endl;
	delete currObject;
    }

    (*array)->Clear();

    delete *array;

    if(ioption == 1){*array = new TObjArray(); return;}
    if(ioption == 0){*array = NULL; return;}

}
///////////////////////////////////////////////////////////////////////////////////////////////////
Int_t ArrayStat(Int_t nElements, Double_t* Array, Double_t &maxElement, Double_t &minElement, Double_t &Amplitude, Double_t &Mean, Double_t &RMS)
{
    maxElement = Array[0];
    minElement = Array[0];
    Amplitude = Mean = RMS = 0;
    Double_t currElement = 0, SqSumm = 0;

    for(Int_t i = 0; i < nElements; i++)
    {
	currElement = Array[i];
	if(maxElement < currElement)maxElement = currElement;
	if(currElement < minElement)minElement = currElement;
	Mean += currElement;
    }

    Mean /= (Double_t)nElements;
    Amplitude = maxElement - minElement;

    for(Int_t i = 0; i < nElements; i++)
    {
	currElement = Array[i];
	SqSumm += (currElement-Mean)*(currElement-Mean);
    }

    RMS = TMath::Sqrt( SqSumm / (Double_t)nElements );

    return 1;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
TH1 *CreateSmartPojection(TH2 *sourcehist, TString option = "n", const Float_t minbinevX = 1,
                          const Float_t minbinevedge = 0.001, const Int_t rebinX = 1, const Int_t rebinY = 1, Int_t ProjectionRebin = 5, Float_t projWidth = 1.5, TObjArray *monitor_slices_array = NULL)
{
    //options: "SIGMA" - draw sigma (RMS) else mean; "FIT" - fit projection, else mean and RMS;   "s" - smooth; "n" - no rebin ; "r" - rebin

    if(!sourcehist)
    {
	if(uVERBOSE >= 1)cout <<Form("utilites.h/CreateSmartPojection::ERROR: hist pointer is NULL !!!\n")<<endl;
	return NULL;
    }

    static const Int_t minBinEvOnEdge = 5;
    //static const Int_t ProjectionRebin = 5;

    const Bool_t IsDrawSIGMA = option.Contains("SIGMA");
    const Bool_t IsFitProjection = option.Contains("FIT");

    if(uVERBOSE >= 2)cout <<Form("utilites.h/CreateSmartPojection: Creating %s Projection%s for \"%s\" ... ",
                                 (IsDrawSIGMA)? "sigma":"mean", (IsFitProjection)?" by fit":"", sourcehist->GetName())<<endl;

    Int_t NumTotalEv = sourcehist->GetEntries();
    if(NumTotalEv < 1000)
    {
	cout <<Form("utilites.h/CreateSmartPojection::ERROR: Too low events in \"%s\" hist: %i",sourcehist->GetName(), NumTotalEv)<<endl;
	return NULL;
    }

    TH2 *rebinedhist = sourcehist->Rebin2D(rebinX, rebinY, Form("%s_rebin",sourcehist->GetName()) );

    if(option.Contains("s"))rebinedhist->Smooth(3);

    Int_t NumBinsY = rebinedhist->GetNbinsY(); //number of bin along Y
    Int_t NumBinsX = rebinedhist->GetNbinsX(); //number of bin along X

    //average events in bin, does not calculated empty bins
    Int_t AverageEvInBinX = 0, bincalculated = 0, integral; //events in bin (X axis) on the averages
    for(Int_t xbin = 1; xbin <= NumBinsX; xbin++)
    {
	integral = rebinedhist->Integral(xbin,xbin,1,NumBinsY);
	if(0 < integral){ AverageEvInBinX += integral; bincalculated++;}
    }
    AverageEvInBinX = ceil((Float_t)AverageEvInBinX / (Float_t)bincalculated);


    if(uVERBOSE >= 4)cout <<Form("utilites.h/CreateSmartPojection: Average events in bin: %i", AverageEvInBinX)<<endl;

    //fist and last bin with minbinevedge*AverageEvInBinX
    Int_t firstBinX = -1, lastBinX = -1;
    for(Int_t xbin = 1; xbin <= NumBinsX; xbin++)
    {
	if(firstBinX < 0)
	    if( (ceil(minbinevedge*AverageEvInBinX) <= rebinedhist->Integral(xbin,xbin,1,NumBinsY)) &&
	            ( minBinEvOnEdge <= rebinedhist->Integral(xbin,xbin,1,NumBinsY)) ) firstBinX = xbin;

	if(lastBinX < 0)
	    if( (ceil(minbinevedge*AverageEvInBinX) <= rebinedhist->Integral(NumBinsX-xbin+1,NumBinsX-xbin+1,1,NumBinsY)) &&
	            ( minBinEvOnEdge <= rebinedhist->Integral(NumBinsX-xbin+1,NumBinsX-xbin+1,1,NumBinsY))) lastBinX = NumBinsX-xbin+1;

	if( (lastBinX > 0) && (firstBinX > 0) ) break;
    }

    if(uVERBOSE >= 4)cout <<Form("utilites.h/CreateSmartPojection: FIRST BIN: %i, %.1f (%.1f ev); LAST BIN: %i, %.1f (%.1f ev)",
                                 firstBinX, rebinedhist->GetXaxis()->GetBinCenter(firstBinX), rebinedhist->Integral(firstBinX,firstBinX,1,NumBinsY),
                                 lastBinX,  rebinedhist->GetXaxis()->GetBinCenter( lastBinX), rebinedhist->Integral(lastBinX, lastBinX, 1,NumBinsY))<<endl;
    if(option.Contains("r"))if(uVERBOSE >= 4)cout <<Form("Bin before rebinning: %i", lastBinX-firstBinX)<<endl;

    //finding bins: in each bin minimum minQTCCFDbinEv*QTCCFDnumEvInBinXAverage events
    Float_t *NewBinning = new Float_t[lastBinX-firstBinX+2];
    Int_t eventsINbin = 0, numberOfBins = 0;

    if(uVERBOSE >= 5)cout <<Form("BINNING: ");
    NewBinning[0] = rebinedhist->GetXaxis()->GetBinLowEdge(firstBinX);
    for(Int_t xbin = firstBinX+1; xbin <= lastBinX; xbin++)
    {
	eventsINbin += rebinedhist->Integral(xbin,xbin,1,NumBinsY);

	if( (eventsINbin >= AverageEvInBinX*minbinevX) || (xbin == lastBinX) || (!option.Contains("r")))
	{
	    numberOfBins++;
	    NewBinning[numberOfBins] = rebinedhist->GetXaxis()->GetBinUpEdge(xbin);
	    if(uVERBOSE >= 5)cout <<Form("%.1f ", NewBinning[numberOfBins]);
	    eventsINbin = 0;
	}
    } //for sbin
    if(uVERBOSE >= 4)cout <<""<<endl;
    if(option.Contains("r"))if(uVERBOSE >= 4)cout <<Form("Bin after rebining: %i", numberOfBins)<<endl;

    if(numberOfBins <= 0)
    {
	if(uVERBOSE >= 1)cout <<Form("utilites.h/CreateSmartPojection: Binning failure, number of bins: %i", numberOfBins)<<endl;
	return NULL;
    }

    //filling 1D histograms with mean and errors by gauss fit
    TH1 *ptrProjection = new TH1F(Form("%s_smProj_%s", sourcehist->GetName(),(IsDrawSIGMA)?"sigma":"mean"),"title",numberOfBins,0,1);
    ptrProjection->GetXaxis()->Set(numberOfBins, NewBinning);
    ptrProjection->SetOption("E1");
    ptrProjection->SetTitle(Form("[%s projection%s] %s",
                                 (IsDrawSIGMA)? "SIGMA":"MEAN", (IsFitProjection)?" by fit":"", sourcehist->GetTitle()));
    if(IsFitProjection && IsDrawSIGMA)
    {
	ptrProjection->SetMarkerStyle(22);

    }

    Int_t firstbin, lastbin;
    Double_t mean = 0., sigma = 0., sigmaERROR = 0., meanERROR = 0.;
    TH1 *ptrProjectionY;
    static Int_t monitor_slices_number = 0;

    for(Int_t bin = 0; bin < numberOfBins; bin++)
    {
	ptrProjectionY = NULL;
	mean = sigma = sigmaERROR = 0.0001;
	firstbin = rebinedhist->GetXaxis()->FindBin( NewBinning[bin] );
	lastbin = rebinedhist->GetXaxis()->FindBin( NewBinning[bin+1] );

	ptrProjectionY = rebinedhist->ProjectionY(Form("%i_%s_slice_bin%i_binmin%i_binmax%i",
	                                               monitor_slices_number++,(IsDrawSIGMA)? "SIGMA":"MEAN",bin, firstbin,lastbin),firstbin,lastbin);
	ptrProjectionY->SetTitle( Form("[proj slice: minX %.2f, maxX %.2f] %s", NewBinning[bin], NewBinning[bin+1], ptrProjection->GetTitle()));

	if(ptrProjectionY)
	{
	    ptrProjectionY->Rebin(ProjectionRebin);

	    if(IsFitProjection)
	    {
		//FitHistogrammInMaxPeak(ptrProjectionY,mean,sigma, projWidth); //width = 1.5
		FitHistogrammSmartInMaxPeak(ptrProjectionY,mean,sigma); //width = 1.5

		if(ptrProjectionY->GetFunction(fit_funk_name))
		{
		    meanERROR = ptrProjectionY->GetFunction(fit_funk_name)->GetParError(1);
		    sigmaERROR = ptrProjectionY->GetFunction(fit_funk_name)->GetParError(2);
		}
	    }
	    else
	    {
		mean = ptrProjectionY->GetMean();
		sigma = ptrProjectionY->GetRMS();
		meanERROR = ptrProjectionY->GetMeanError();
		sigmaERROR = ptrProjectionY->GetRMSError();
	    }

	    if(monitor_slices_array) monitor_slices_array->Add(ptrProjectionY);
	    else delete ptrProjectionY;

	} else {  cout <<Form("utilites.h/CreateSmartPojection: WARING: ProjectionY \"%s\" was NOT created", Form("QTCCFDslice_bin_%i_%i",firstbin,lastbin) )<<endl; }

	ptrProjection->SetBinContent(bin+1,(IsDrawSIGMA) ? sigma      : mean);
	ptrProjection->SetBinError(bin+1,  (IsDrawSIGMA) ? sigmaERROR*0.5 :  meanERROR*0.5);

    }

    if(monitor_slices_array) monitor_slices_array->Add(rebinedhist);
    else delete rebinedhist;

    delete[] NewBinning;
    return ptrProjection;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
TObjArray* HDraw(const TObjArray *Histogramms_arr, TString name = "",
                 int Hr = 4, int Vt = 4, const int Pair = 1,
                 int min = 0, int max = 100, int Log = 0, int Leg = 0, const TString param = "", int Norm = 0)
{
    gStyle->SetPalette(1);

    //name, HxV,number paired hits,  min, max,log (1 - y, 2 - z), legend (0-no, 1-all, 2-first pad)
    int H_Last = Histogramms_arr->GetLast();
    if(H_Last <= 0) return NULL;

    if(min < 0) min = 0;
    if(min > H_Last) min = H_Last;
    if(max < 0) max = 0;
    if(max > H_Last) max = H_Last;
    if(Hr < 1) Hr = 1;
    if(Vt < 1) Vt = 1;
    if((Leg == 2)&&(Hr*Vt == 1))Leg = 0;

    if(name == "") name = Histogramms_arr->GetName();

    TObjArray *Canvas_arr = new TObjArray();
    Canvas_arr->SetOwner();

    int flagD = Hr*Vt;

    for(int t=min; t <= max; t++){
	//((TH1F*)Histogramms_arr->At(t)) -> SetLineWidth(3);
	// ((TH1F*)Histogramms_arr->At(t)) -> SetLineColor(t%Pair +1);
	//((TH1F*)Histogramms_arr->At(t)) -> SetFillColor(t%Pair +1);
	//((TH1F*)Histogramms_arr->At(t)) -> SetFillStyle(3001);
	//((TH1F*)Histogramms_arr->At(t)) -> SetMarkerColor(t%Pair +1);
    }

    for(int t=min; t <= max; t++){

	if(++flagD > Hr*Vt){
	    char n_Canv[30]; sprintf(n_Canv, "%s#%d",name.Data(),Canvas_arr->GetLast()+1);
	    Canvas_arr->Add( new TCanvas(n_Canv, n_Canv, 2000, 2000) );
	    ((TCanvas*)Canvas_arr->Last())->Divide(Hr,Vt,0.0001,0.0001);
	    flagD = 1;}

	int t_0 = t;
	((TCanvas*)Canvas_arr->Last())->cd(flagD);
	//if(Histogramms_arr->At(t) != NULL)
	    if(!Norm){ ((TH1F*)Histogramms_arr->At(t))->Draw(param.Data()); }
	    else
		//if(Histogramms_arr->At(t) != NULL)
		    ((TH1F*)Histogramms_arr->At(t))->DrawNormalized(param.Data(), 1);
	for(int c_Pair = 1; c_Pair < Pair; c_Pair++){
	    t++;
	    //if(Histogramms_arr->At(t) != NULL)
	        if(!Norm){ ((TH1F*)Histogramms_arr->At(t))->Draw("sames");}
		else
		    //if(Histogramms_arr->At(t) != NULL)
		        ((TH1F*)Histogramms_arr->At(t))->DrawNormalized("sames", 1);
	}

	//to build big legend on 1st pad drawing hist set twice
	if((Leg == 2)&&(flagD == 1)){
	    gPad -> BuildLegend(0,0,1,1);
	    flagD++;
	    t = t_0;
	    ((TCanvas*)Canvas_arr->Last())->cd(flagD);
	    //if(Histogramms_arr->At(t) != NULL)
	        if(!Norm) ((TH1F*)Histogramms_arr->At(t))->Draw(param.Data());
		else
		    //if(Histogramms_arr->At(t) != NULL)
		        ((TH1F*)Histogramms_arr->At(t))->DrawNormalized(param.Data(),1);
	    for(int c_Pair = 1; c_Pair < Pair; c_Pair++){
		t++;
		//if(Histogramms_arr->At(t) != NULL)
		    if(!Norm) ((TH1F*)Histogramms_arr->At(t))->Draw("sames");
		    else
			//if(Histogramms_arr->At(t) != NULL)
			    ((TH1F*)Histogramms_arr->At(t))->DrawNormalized("sames", 1);
	    }
	}


	if(Log == 1) gPad -> SetLogy(1);
	if(Log == 2) gPad -> SetLogz(1);

	//if(Leg == 1) ;//gPad -> BuildLegend();

    }

    return Canvas_arr;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // UTILITES_H
