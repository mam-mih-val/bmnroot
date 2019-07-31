//--------------------------------------------------------------------------------------------------------------------------------------
#include<assert.h>
#include<map>

#include <TRandom2.h>
#include <TGeoManager.h>
#include <TGeoBBox.h>
#include <TGeoMatrix.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TEfficiency.h>
#include <TVector3.h>

#include "FairLogger.h"

#include "CbmMCTrack.h"
#include "BmnTofHit.h"
#include "BmnTOFPoint.h"

//#include "BmnTofGeoUtils.h"

#include "BmnTofHitProducer.h"

#define MAKE_CLUSTERS 1

using namespace std;

static Float_t workTime = 0.0;

ClassImp(BmnTofHitProducer)
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTofHitProducer::BmnTofHitProducer(const char *name, const char *geomFile, Bool_t useMCdata, Int_t verbose, Bool_t test)
:  BmnTofHitProducerIdeal(name, useMCdata, verbose, test), fTimeSigma(0.100), fErrX(0.5), fErrY(1./sqrt(12.)), pRandom(new TRandom2), h2TestStrips(nullptr) , h1TestDistance(nullptr), h2TestNeighborPair(nullptr),
	fDoINL(true), fDoSlewing(true), fSignalVelosity(0.050)
{
	pGeoUtils = new BmnTofGeoUtils(useMCdata);
	pGeoUtils->SetVerbosity(fVerbose);
	fgeomFile = geomFile;
	fMCTimeFile = NULL;
	
    	if(fDoTest) 
    	{	
    		fTestFlnm = "test.BmnTofHitProducer.root";	
    		effTestEfficiencySingleHit = new TEfficiency("TOF700_effSingleHit", "Efficiency single hit;R, cm;Side", 10000, -0.1, 2.); 				fList.Add(effTestEfficiencySingleHit);
		effTestEfficiencyDoubleHit = new TEfficiency("TOF700_effDoubleHit", "Efficiency double hit;R, cm;Side", 10000, -0.1, 2.); 				fList.Add(effTestEfficiencyDoubleHit);
		h1TestDistance = new TH1D("TOF700_TestDistance", "Distance between strips;M, cm;Side", 1000, 0., 100.); 						fList.Add(h1TestDistance);
//     		h2TestStrips = new TH2D("TOF700_TestStrips", ";Z, cm;#phi, rads", 2000, -300., 300., 500, -3.5, 3.5);							fList.Add(h1TestStrips);
     	
 		h2TestNeighborPair = new TH2D("TOF700_TestNeighborPair", "Neighbor strip pairs test;stripID1;stripID2", 100, -0.5, 49.5, 100, -0.5, 49.5);		fList.Add(h2TestNeighborPair);		
		h2TestXYSmeared = new TH2D("TOF700_TestXYSmeared", "Smeared XY (single hit) test;#DeltaX, cm;#DeltaY, cm", 1000, -1., 1., 1000, -2., 2.);		fList.Add(h2TestXYSmeared);
		h2TestXYSmeared2 = new TH2D("TOF700_TestXYSmeared2", "Smeared XY (single hit) test;X, cm;Y, cm", 1000, -180., 180., 1000, -180., 180.);			fList.Add(h2TestXYSmeared2);		
		h2TestXYSmearedDouble = new TH2D("TOF700_TestXYSmearedDouble", "Smeared XY (double hit) test;#DeltaX, cm;#DeltaY, cm", 1000, -2., 2., 1000, -2., 2.);	fList.Add(h2TestXYSmearedDouble);
		h2TestXYSmearedDouble2 = new TH2D("TOF700_TestXYSmearedDouble2", "Smeared XY (double hit) test;X, cm;Y, cm", 1000, -180., 180., 1000, -180., 180.);	fList.Add(h2TestXYSmearedDouble2);
				
		h2TestEtaPhi = new TH2D("TOF700_TestEtaPhi", ";#eta;#phi, degree", 1000, -5., +5., 1000, -181., 181.);							fList.Add(h2TestEtaPhi);
		h2TestRZ = new TH2D("TOF700_TestRZ", ";X, cm;Y, cm", 1000, -300., 300., 1000, -200., 200.);								fList.Add(h2TestRZ);
		h2TdetIdStripId = new TH2D("TOF700_TdetIdStripId", ";stripId;detId", 100, -0.5, 99.5, 26, -0.5, 25.5);							fList.Add(h2TdetIdStripId);		
		h1TestMass = new TH1D("TOF700_TestMass", "Mass", 500, 0., 100.); 											fList.Add(h1TestMass);
		h1TestMassLong = new TH1D("TOF700_TestMassLong", "Mass - long tracks", 500, 0., 100.); 									fList.Add(h1TestMassLong);
    	}
	if (!useMCdata)
	{
	    fTOF2 = new BmnTof2Raw2DigitNew("TOF700_map_period_7.txt","bmn_run3332_raw.root");
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTofHitProducer::~BmnTofHitProducer() 
{
    	delete pRandom;
    	delete pGeoUtils;
}
//--------------------------------------------------------------------------------------------------------------------------------------
InitStatus BmnTofHitProducer::Init() 
{
    	LOG(INFO) << "Begin [BmnTof700HitProducer::Init].";
    	
    	if(fOnlyPrimary && fVerbose > 1) LOG(INFO)<<" Only primary particles are processed!!! ";

	if(fUseMCData)
	{
    		aMcPoints = (TClonesArray*) FairRootManager::Instance()->GetObject("TOFPoint");
                if (!aMcPoints)
                {
                  cout<<"BmnTof700HitProducer::Init(): branch TOFPoint not found! Task will be deactivated"<<endl;
                  SetActive(kFALSE);
                  return kERROR;
                }
    		aMcTracks = (TClonesArray*) FairRootManager::Instance()->GetObject("MCTrack");
                if (!aMcTracks)
                {
                  cout<<"BmnTof700HitProducer::Init(): branch MCTrack not found! Task will be deactivated"<<endl;
                  SetActive(kFALSE);
                  return kERROR;
                }
	}
	else
	{
    		aExpDigits = (TClonesArray*) FairRootManager::Instance()->GetObject("TOF700");
                if (!aExpDigits)
                {
                  cout<<"BmnTof700HitProducer::Init(): branch TOF700 not found! Task will be deactivated"<<endl;
                  SetActive(kFALSE);
                  return kERROR;
                }
		if (fMCTimeFile == NULL)
		{
                    cout<<"BmnTof700HitProducer::Init(): MC times file not defined! Use default!"<<endl;
		    for (int c = 0; c < TOF2_MAX_CHAMBERS; c++) fMCTime[c] = 21.f;
		}
		else
		{
		    readMCTimeFile(fMCTimeFile);
		}
	}
	
    	// Create and register output array
    	aTofHits = new TClonesArray("BmnTofHit");
    	FairRootManager::Instance()->Register("BmnTofHit", "TOF", aTofHits, kTRUE);

//	readGeom(geomFile);
//	pGeoUtils->ParseTGeoManager(fUseMCData, h2TestStrips, true);
	pGeoUtils->ParseStripsGeometry(fgeomFile);
	pGeoUtils->FindNeighborStrips(h1TestDistance, h2TestNeighborPair, fDoTest);
	
    	LOG(INFO) << "Initialization [BmnTof700HitProducer::Init] finished succesfully.";

	return kSUCCESS;
}
//--------------------------------------------------------------------------------------------------------------------------------------
Bool_t BmnTofHitProducer::HitExist(Double_t val) // val - distance to the pad edge [cm]
{
    const static Double_t slope = (0.98 - 0.95)/0.2;
    Double_t efficiency = (val > 0.2) ? 0.98 : ( 0.95 + slope*val);
    //-------------------------------------
    // 99% ---------
    //              \
    //               \
    //                \
    // 95%             \ 
    //  <-----------|--|
    //            0.2  0.
    //-------------------------------------
    if(pRandom->Rndm() < efficiency) return true;
    return false;	
}
//------------------------------------------------------------------------------------------------------------------------
Bool_t BmnTofHitProducer::DoubleHitExist(Double_t val) // val - distance to the pad edge  [cm]
{
  const static Double_t slope = (0.3 - 0.0)/0.5;
  Double_t efficiency = (val > 0.5) ? 0. : (0.3 - slope*val);
	
  //-------------------------------------
  // 30%               /
  //                  /
  //                 / 
  //                /
  // 0%            /  
  //  <-----------|----|
  //            0.5    0.
  //-------------------------------------
	
  if(efficiency == 0.) return false;
	
  if(pRandom->Rndm() < efficiency) return HitExist(val);
  return false;	
}
//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTofHitProducer::Exec(Option_t* opt) 
{
    if (!IsActive())
        return;
    
    float idchambers[59] = { 27.1,28.1,3.1,1.1,29.1,4.1,33.1,30.1,5.1,19.3,31.1,6.1,2.1,32.1,15.2,16.2,17.2,
    18.2,19.2,20.2,7.1,115.2,113.1,117.1,35.1,9.1,37.1,11.1,39.1,13.1,34.1,8.1,36.1,10.1,38.1,12.1,21.2,
    23.2,25.2,22.2,24.2,26.2,107.2,108.2,109.2,110.2,111.2,112.2,114.1,116.2,118.1,14.1,40.1,119.2,120.2,
    121.2,122.2,123.2,124.2 };
    clock_t tStart = clock();
    if (fVerbose > 1) cout << endl << "======================== TOF700 exec started ====================" << endl;
	static const TVector3 XYZ_err(fErrX, fErrY, 0.); 

	aTofHits->Clear();
		 
	Int_t 		UID, trackID;	
	TVector3 	pos, XYZ_smeared, mom; 	
    	int		nSingleHits = 0, nDoubleHits = 0;
	
	if(fUseMCData)
	{
		for(Int_t pointIndex = 0, nTofPoint = aMcPoints->GetEntriesFast(); pointIndex < nTofPoint; pointIndex++ )  // cycle by TOF points
		{
			BmnTOFPoint *pPoint = (BmnTOFPoint*) aMcPoints->UncheckedAt(pointIndex);
		
			if(fVerbose > 2) pPoint->Print(""); 		
  
			trackID = pPoint->GetTrackID();	
			UID	= pPoint->GetDetectorID();
			Double_t time = pRandom->Gaus(pPoint->GetTime(), fTimeSigma); // time rsolution in ps		
			Double_t length = pRandom->Gaus(pPoint->GetLength(), 1.); // 1 cm		
			pPoint->Position(pos);
			pPoint->Momentum(mom);
			Double_t p = mom.Mag();
			p = p*(1.+pRandom->Gaus(0.04*p)); // momentum resolution 4%
			Double_t cvel = 29.97925;
			Double_t ct = cvel*time;
			Double_t ctl = ct/length;
			Double_t sqr = TMath::Sqrt(ctl*ctl - 1.);
			Double_t mass = p*sqr;
			Double_t elcut = 0.; // 0.02e-3;
			if (fDoTest) if (pPoint->GetEnergyLoss() > elcut) h1TestMass->Fill(mass);
			if (fDoTest) if (pPoint->GetEnergyLoss() > elcut && length > 600.) h1TestMassLong->Fill(mass);
			const LStrip *pStrip = pGeoUtils->FindStrip(UID, pos);
//			const LStrip *pStrip = pGeoUtils->FindStrip(UID);
			if (pStrip == NULL) continue;
		
			XYZ_smeared.SetXYZ( pRandom->Gaus(pos.X(), fErrX), pStrip->center.Y(), pStrip->center.Z());

			LStrip::Side_t side;
			Double_t distance = pStrip->MinDistanceToEdge(&pos, side); // [cm]

			bool passed;
			if(passed = HitExist(distance)) // check efficiency 
			{
			 	AddHit(UID, XYZ_smeared, XYZ_err, pointIndex, trackID, time); 	
			 	nSingleHits++;

			 	if(fDoTest)
			 	{
                        		Int_t strip = BmnTOFPoint::GetStrip(UID);
                        		Int_t chamber = BmnTOFPoint::GetChamber(UID);
			 		h2TestXYSmeared->Fill(pos.X() - XYZ_smeared.X(), pos.Y() - XYZ_smeared.Y());
			 		h2TestXYSmeared2->Fill(XYZ_smeared.X(), XYZ_smeared.Y());
			 		h2TestEtaPhi->Fill(pos.Eta(), pos.Phi()*TMath::RadToDeg());
			 		h2TestRZ->Fill(pos.X(), pos.Y());
                        		h2TdetIdStripId->Fill(strip, chamber);
			 	}
			} 
		
			if(fDoTest) effTestEfficiencySingleHit->Fill(passed, distance);
        	
        		if((passed = DoubleHitExist(distance))) // check cross hit
        		{
        			Int_t CrossUID = (side == LStrip::kUpper) ? pStrip->neighboring[LStrip::kUpper] : pStrip->neighboring[LStrip::kLower];
  			
  				if(LStrip::kInvalid  == CrossUID) continue; // last strip on module
  			
  				pStrip = pGeoUtils->FindStrip(CrossUID);
        			XYZ_smeared.SetXYZ( pRandom->Gaus(pos.X(), fErrX), pStrip->center.Y(), pStrip->center.Z());
        			
        			AddHit(CrossUID, XYZ_smeared, XYZ_err, pointIndex, trackID, time); 
        			nDoubleHits++;
  			
        			if(fDoTest)
        			{
        				h2TestXYSmearedDouble->Fill((pos - XYZ_smeared).Mag(), pos.Z() - XYZ_smeared.Z());
        				h2TestXYSmearedDouble2->Fill(XYZ_smeared.X(), XYZ_smeared.Y());
        			}
        		}
        	
        		if(fDoTest) effTestEfficiencyDoubleHit->Fill(passed, distance);

		}	// cycle by the TOF points
	}
	else
	{
		TVector3 crosspoint;
		Int_t tofWidths[TOF2_MAX_CHAMBERS][32] = {{0}};
		Float_t tof[TOF2_MAX_CHAMBERS][32] = {{0}};
		Float_t lrdiff[TOF2_MAX_CHAMBERS][32] = {{0}};
		if (MAKE_CLUSTERS)
		{
            	    for(Int_t digitIndex = 0, nTof2Digits = aExpDigits->GetEntriesFast(); digitIndex < nTof2Digits; digitIndex++ )  // cycle by TOF digits
            	    {
                            BmnTof2Digit *pDigit = (BmnTof2Digit*) aExpDigits->UncheckedAt(digitIndex);		
                            UID = ((pDigit->GetPlane() + 1)<<8) | (pDigit->GetStrip() + 1);
                            Int_t strip = pDigit->GetStrip();
                            Int_t chamber = pDigit->GetPlane();
			    tofWidths[chamber][strip] = pDigit->GetAmplitude();
			    tof[chamber][strip] = pDigit->GetTime();
			    lrdiff[chamber][strip] = pDigit->GetDiff();
            	    }
		    int ncl0 = 0, ncl[TOF2_MAX_CHAMBERS] = {0};
		    int clchamb[TOF2_MAX_CHAMBERS] = {0};
		    int clstrip[TOF2_MAX_CHAMBERS] = {0};
		    for (int i = 0; i<TOF2_MAX_CHAMBERS; i++)
		    {
			    ncl[i] = 0;
			    int clstart = -1, clwidth = -1, cstr = -1;
			    float samps = 0., timemin = 1000000000.;
			    int nstr = 32;
			    if (idchambers[i] > 100.f) nstr = 16;
			    for (int j = 0; j<nstr; j++)
			    {
				if (tofWidths[i][j] > 0.)
				{
				    if (clstart < 0)
				    {
					clstart = j;
					timemin = tof[i][j];
				    }
				    else
				    {
					if (tof[i][j] < timemin) {timemin = tof[i][j]; cstr = j; }
				    }
				    samps += tofWidths[i][j];
				    if (j == (nstr-1))
				    {
					clchamb[ncl0] = i;
					clstrip[ncl0] = cstr;
					clwidth = (j+1-clstart);
					ncl0++;
					ncl[i]++;
					samps = 0.;
					timemin =  1000000000.;
                        		UID = ((i + 1)<<8) | (cstr + 1);
                        		Float_t xcl, ycl, zcl;
                        		fTOF2->get_hit_xyz(i,j,lrdiff[i][cstr],&xcl,&ycl,&zcl);
                        		crosspoint.SetXYZ(xcl,ycl,zcl);
                        		AddHit(UID, crosspoint, XYZ_err, -1, -1, timemin+fMCTime[i]);
                        		nSingleHits++;

                        		if(fDoTest)
                        		{
                            		    h2TestXYSmeared2->Fill(crosspoint.X(), crosspoint.Y());
                        		    Float_t xc, yc, zc;
                        		    fTOF2->get_strip_xyz(i,j,&xc,&yc,&zc);
                            		    h2TestRZ->Fill(xc, yc);
                        		}			
				    }
				}
				else if (clstart >= 0)
				{
				    clchamb[ncl0] = i;
				    clstrip[ncl0] = cstr;
				    clwidth = (j-clstart);
				    ncl0++;
				    ncl[i]++;
				    samps = 0.;
				    timemin =  1000000000.;
				    clstart = -1;
                        	    UID = ((i + 1)<<8) | (cstr + 1);
                        	    Float_t xcl, ycl, zcl;
                        	    fTOF2->get_hit_xyz(i,j,lrdiff[i][cstr],&xcl,&ycl,&zcl);
                        	    crosspoint.SetXYZ(xcl,ycl,zcl);
                        	    AddHit(UID, crosspoint, XYZ_err, -1, -1, timemin+fMCTime[i]);
                        	    nSingleHits++;

                        	    if(fDoTest)
                        	    {
                            		h2TestXYSmeared2->Fill(crosspoint.X(), crosspoint.Y());
                        		Float_t xc, yc, zc;
                        		fTOF2->get_strip_xyz(i,j,&xc,&yc,&zc);
                            		h2TestRZ->Fill(xc, yc);
                        	    }			
				}
			    }
		    }
		}
		else
		{
            	    for(Int_t digitIndex = 0, nTof2Digits = aExpDigits->GetEntriesFast(); digitIndex < nTof2Digits; digitIndex++ )  // cycle by TOF digits
            	    {
                            BmnTof2Digit *pDigit = (BmnTof2Digit*) aExpDigits->UncheckedAt(digitIndex);		
                            UID = ((pDigit->GetPlane() + 1)<<8) | (pDigit->GetStrip() + 1);
                            Int_t strip = pDigit->GetStrip();
                            Int_t chamber = pDigit->GetPlane();
//			    if (chamber > 40 && strip > 15) continue;
//			    printf("C %d s %d\n",pDigit->GetPlane(), pDigit->GetStrip());
//			    pos.SetXYZ(xcens[chamber][strip],ycens[chamber][strip],zchamb[chamber]);
                            const LStrip *pStrip = pGeoUtils->FindStrip(UID);
                    	    Float_t xcl, ycl, zcl;
                    	    fTOF2->get_hit_xyz(chamber,strip,pDigit->GetDiff(),&xcl,&ycl,&zcl);
                    	    crosspoint.SetXYZ(xcl,ycl,zcl);
                    	    AddHit(UID, crosspoint, XYZ_err, -1, -1, pDigit->GetTime()+fMCTime[chamber]); 	
                    	    nSingleHits++;

                    	    if(fDoTest)
                    	    {
                            	    h2TestXYSmeared2->Fill(crosspoint.X(), crosspoint.Y());
                            	    TVector3 stripCenter(pStrip->center);
                            	    h2TestRZ->Fill(stripCenter.X(), stripCenter.Y());
                    	    }			
            	    }
            	}
	}
	
	MergeHitsOnStrip(); // save only the fastest hit in the strip

	int nFinally = CompressHits(); // remove blank slotes
        
        clock_t tFinish = clock();
        workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;

        if (fVerbose > 1) cout<<"BmnTof700HitProducer: single hits= "<<nSingleHits<<", double hits= "<<nDoubleHits<<", final hits= "<<nFinally<<endl;
        if (fVerbose == 1) cout << "BmnTof700HitProducer: " << nFinally << " hits" << endl;
        if (fVerbose > 1) cout << "======================== TOF700 exec finished ===================" << endl;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTofHitProducer::Finish() 
{
  	if(fDoTest)
    	{
      		LOG(INFO) << "[BmnTof700HitProducer::Finish] Update " << fTestFlnm.Data() << " file.";
		TFile *ptr = gFile;
		TFile file(fTestFlnm.Data(), "RECREATE");
		fList.Write(); 
		file.Close();
		gFile = ptr;
	}
        
    if (fVerbose > 0) cout << "Work time of the TOF-700 hit finder: " << workTime << endl;
}
//--------------------------------------------------------------------------------------------------------------------------------------
// input- strip edge position & signal times; output- strip crosspoint; return false, if crosspoint outside strip 
bool BmnTofHitProducer::GetCrossPoint(const TVector3& p1, double time1, const TVector3& p2, double time2, TVector3& crossPoint) 
{
	double stripLength = (p2-p1).Mag();
	double maxDelta =   (stripLength + 0.5) * fSignalVelosity; // + 5 mm on the strip edge
	if(abs(time1 - time2) > maxDelta) return false; // estimated position out the strip edge.
	double dL =  abs(time1 - time2) / fSignalVelosity;
	double a2 = (stripLength - dL) / 2.;
	if(time1 > time2)	crossPoint = p2 + (p1-p2) * (a2 / stripLength);
	else			crossPoint = p1 + (p2-p1) * (a2 / stripLength);
	return true;	
}

bool BmnTofHitProducer::GetCrossPoint(const LStrip *pStrip, double dT, TVector3& crossPoint) 
{
        TVector3 s1, s2, centr;
        s1 = (pStrip->A + pStrip->D) * 0.5; // [cm] strip side1 end's position 
        s2 = (pStrip->B + pStrip->C) * 0.5; // [cm] strip side2 end's position 
        centr = pStrip->center;
	double stripLength = (s2-s1).Mag();
	double maxDelta =   (stripLength + 1.0) * fSignalVelosity; // + 10 mm on the strip edge
	if(abs(dT) > maxDelta) return false; // estimated position out the strip edge.
	double dL = dT / fSignalVelosity;
        s1(0) = dL; s1(1) = 0; s1(2) = 0; //TMP ALIGMENT CORRECTIONS
	crossPoint = centr + s1;
        /*if (time1 > time2)
        {
            cout << "stripLength = " << stripLength << "  dL = " << dL << endl;
            cout << "centr.x = " << centr(0) << "; centr.y = " << centr(1) << "; centr.z = " << centr(2) << endl;
            cout << "crossPoint.x = " << crossPoint(0) << "; crossPoint.y = " << crossPoint(1) << "; crossPoint.z = " << crossPoint(2) << endl;
            getchar();
        }//*/
	return true;	
}
//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTofHitProducer::SetSeed(UInt_t seed)
{
	pRandom->SetSeed(seed);
}
//--------------------------------------------------------------------------------------------------------------------------------------

Double_t BmnTofHitProducer::CalculateToF (BmnTof2Digit *d1, BmnTof2Digit *d2, BmnTrigDigit *t0){
    Int_t CorrRPC_It1, CorrT0_It1;
    Double_t CorrPlane7Coeff_It1[5][4] = {
        {12.24, -3.919, 0.2624, -0.005153},
        {-122.9, 14.84, -0.6158, 0.008724},
        {-12.34, 0.482, 0., 0.},
        {-12.28, 0.479, 0., 0.},
        {-13.14, 0.5034, 0., 0.}
    };
    Double_t CorrT0Coeff_It1[3][4] = {
        {-5.486, 1.087, -0.07936, 0.00208},
        {10.77, -2.635, 0.1984, -0.004631},
        {0., 0., 0., 0.}
    };
    Double_t timeRPC = (d1->GetTime() + d2->GetTime()) * 0.5;
    Double_t ampRPC = d1->GetAmplitude() + d2->GetAmplitude();
    Double_t timeT0 = t0->GetTime();
    Double_t ampT0 = t0->GetAmp();
    Double_t dt = timeRPC - timeT0;
    if (ampRPC < 20.46) CorrRPC_It1 = 0;
    else if (ampRPC >= 20.46 && ampRPC < 27.25) CorrRPC_It1 = 1;
    else if (ampRPC >= 27.25 && ampRPC < 34.81) CorrRPC_It1 = 2;
    else if (ampRPC >= 34.81 && ampRPC < 37.11) CorrRPC_It1 = 3;
    else if (ampRPC >= 37.11) CorrRPC_It1 = 4;
    dt = dt - (CorrPlane7Coeff_It1[CorrRPC_It1][0] + CorrPlane7Coeff_It1[CorrRPC_It1][1] * ampRPC +
            CorrPlane7Coeff_It1[CorrRPC_It1][2] * ampRPC * ampRPC +
            CorrPlane7Coeff_It1[CorrRPC_It1][3] * ampRPC * ampRPC * ampRPC);
    if (ampT0 >= 6.15 && ampT0 < 12.3) CorrT0_It1 = 0;
    else if (ampT0 >= 12.3 && ampT0 < 17.14) CorrT0_It1 = 1;
    else  CorrT0_It1 = 2;
    dt = dt - (CorrT0Coeff_It1[CorrT0_It1][0] + CorrT0Coeff_It1[CorrT0_It1][1] * ampT0 +
            CorrT0Coeff_It1[CorrT0_It1][2] * ampT0 * ampT0 +
            CorrT0Coeff_It1[CorrT0_It1][3] * ampT0 * ampT0 * ampT0); 
    return dt + 14.; // 14 ns
}

//------------------------------------------------------------------------------------------------------------------------
int BmnTofHitProducer::readMCTimeFile(const char *MCTimeFile)
{
//	cout << "readMCTimeFile: " << MCTimeFile << endl;
	float idchambers[59] = { 27.1,28.1,3.1,1.1,29.1,4.1,33.1,30.1,5.1,19.3,31.1,6.1,2.1,32.1,15.2,16.2,17.2,
	18.2,19.2,20.2,7.1,115.2,113.1,117.1,35.1,9.1,37.1,11.1,39.1,13.1,34.1,8.1,36.1,10.1,38.1,12.1,21.2,
	23.2,25.2,22.2,24.2,26.2,107.2,108.2,109.2,110.2,111.2,112.2,114.1,116.2,118.1,14.1,40.1,119.2,120.2,
	121.2,122.2,123.2,124.2 };
	bool notused[59] = {true};
	int order[59] = {-1};
	int c = 0, cmin = -1, c0 = 0;
	float idmin = 200.f;
	for (c0 = 0; c0 < 59; c0++) { notused[c0] = true; order[c0] = -1; }
	for (c0 = 0; c0 < 59; c0++)
	{
	    cmin = -1;
	    idmin = 200.f;
	    for (c=0; c<59; c++)
	    {
		if (notused[c] && (idchambers[c]<idmin))
		{
		    idmin = idchambers[c];
		    cmin = c;
		}
	    }
//	    printf("c0 %d cmin %d notused %d\n",c0,cmin,(int)notused[cmin]);
	    if (cmin >= 0)
	    {
		notused[cmin] = false;
		order[c0] = cmin;
	    }
	}
	char fname[128];
	FILE *ft = 0;
	float time = 0.f, timesigma = 0.f;
	if (MCTimeFile == NULL)
	{
	    printf("TOF700 MC time-of-flight file name not defined!\n");
	    return 0;
	}
	if (strlen(MCTimeFile) == 0)
	{
	    printf("TOF700 MC time-of-flight file name not defined!\n");
	    return 0;
	}
	TString dir = getenv("VMCWORKDIR");
	sprintf(fname,"%s/geometry/%s",dir.Data(),MCTimeFile);
	ft = fopen(fname,"r");
	if (ft == NULL)
	{
	    printf("TOF700 MC time-of-flight file %s open error!\n", fname);
	    return 0;
	}
	for (int i=0; i<TOF2_MAX_CHAMBERS; i++) fMCTime[i] = 21.f;
	for (int i=0; i<TOF2_MAX_CHAMBERS; i++) fMCTimeSigma[i] = 0.f;
	int cexp = 0, ic = -1;
	while(fscanf(ft,"%d %f %f\n", &ic, &time, &timesigma) == 3)
	{
		if (ic > 0 && ic <= 59)
		{
		    cexp = order[ic-1];
		    fMCTime[cexp] = time;
		    fMCTimeSigma[cexp] = timesigma;
		    if (fVerbose == 1) printf("Chamber %.1f (MC %d, EXP %d) average time-of-flight %f with sigma %f\n",idchambers[cexp], ic, cexp, time, timesigma);
		}
	}
	fclose(ft);
	return 1;
}
