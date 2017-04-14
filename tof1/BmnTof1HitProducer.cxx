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
#include "BmnTOF1Point.h"

//#include "BmnTof1GeoUtils.h"

#include "BmnTof1HitProducer.h"

using namespace std;

static Float_t workTime = 0.0;

ClassImp(BmnTof1HitProducer)
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTof1HitProducer::BmnTof1HitProducer(const char *name, Bool_t useMCdata, Int_t verbose, Bool_t test)
:  BmnTof1HitProducerIdeal(name, useMCdata, verbose, test), fTimeSigma(0.100), fErrX(1./sqrt(12.)), fErrY(0.5), pRandom(new TRandom2), h2TestStrips(nullptr) , h1TestDistance(nullptr), h2TestNeighborPair(nullptr),
	fDoINL(true), fDoSlewing(true), fSignalVelosity(0.060)
{
	pGeoUtils = new BmnTof1GeoUtils;
	
    	if(fDoTest) 
    	{	
    		fTestFlnm = "test.BmnTof1HitProducer.root";	
    		effTestEfficiencySingleHit = new TEfficiency("effSingleHit", "Efficiency single hit;R, cm;Side", 10000, -0.1, 1.); 						fList.Add(effTestEfficiencySingleHit);
		effTestEfficiencyDoubleHit = new TEfficiency("effDoubleHit", "Efficiency double hit;R, cm;Side", 10000, -0.1, 1.); 						fList.Add(effTestEfficiencyDoubleHit);
    	
		h1TestDistance = new TH1D("TestDistance", "Distance between strips;M, cm;Side", 1000, 0., 100.); 								fList.Add(h1TestDistance);
     		h2TestStrips = new TH2D("TestStrips", ";Z, cm;#phi, rads", 2000, -300., 300., 500, -3.5, 3.5);									fList.Add(h2TestStrips); 
     	
 		h2TestNeighborPair = new TH2D("TestNeighborPair", "Neighbor strip pairs test;stripID1;stripID2", 100, -0.5, 49.5, 100, -0.5, 49.5);				fList.Add(h2TestNeighborPair);		
		h2TestXYSmeared = new TH2D("TestXYSmeared", "Smeared XY (single hit) test;#DeltaX, cm;#DeltaY, cm", 1000, -1., 1., 1000, -2., 2.);				fList.Add(h2TestXYSmeared);
		h2TestXYSmeared2 = new TH2D("TestXYSmeared2", "Smeared XY (single hit) test;X, cm;Y, cm", 1000, -180., 180., 1000, -180., 180.);				fList.Add(h2TestXYSmeared2);		
		h2TestXYSmearedDouble = new TH2D("TestXYSmearedDouble", "Smeared XY (double hit) test;#DeltaX, cm;#DeltaY, cm", 1000, -2., 2., 1000, -2., 2.);			fList.Add(h2TestXYSmearedDouble);
		h2TestXYSmearedDouble2 = new TH2D("TestXYSmearedDouble2", "Smeared XY (double hit) test;X, cm;Y, cm", 1000, -180., 180., 1000, -180., 180.);			fList.Add(h2TestXYSmearedDouble2);
				
		h2TestEtaPhi = new TH2D("TestEtaPhi", ";#eta;#phi, degree", 1000, -1.6, 1.6, 1000, -181., 181.);								fList.Add(h2TestEtaPhi);
		h2TestRZ = new TH2D("TestRZ", ";X, cm;Y, cm", 1000, -300., 300., 1000, -200., 200.);										fList.Add(h2TestRZ);
		h2TdetIdStripId = new TH2D("TdetIdStripId", ";stripId;detId", 100, -0.5, 99.5, 21, -0.5, 20.5);									fList.Add(h2TdetIdStripId);		
    	}
}
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTof1HitProducer::~BmnTof1HitProducer() 
{
    	delete pRandom;
    	delete pGeoUtils;
}
//--------------------------------------------------------------------------------------------------------------------------------------
InitStatus 		BmnTof1HitProducer::Init() 
{
    	FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "Begin [BmnTof1HitProducer::Init].");
    	
    	if(fOnlyPrimary) cout<<" Only primary particles are processed!!! \n"; // FIXME NOT used now ADDD

	if(fUseMCData)
	{
    		aMcPoints = (TClonesArray*) FairRootManager::Instance()->GetObject("TOF1Point");
    		aMcTracks = (TClonesArray*) FairRootManager::Instance()->GetObject("MCTrack");
assert(aMcPoints);
assert(aMcTracks);
	}
	else
	{
    		aExpDigits = (TClonesArray*) FairRootManager::Instance()->GetObject("TOF400");
assert(aExpDigits);	
                aExpDigitsT0 = (TClonesArray*) FairRootManager::Instance()->GetObject("T0");
assert(aExpDigits);
	}
	
    	// Create and register output array
    	aTofHits = new TClonesArray("BmnTofHit");
    	FairRootManager::Instance()->Register("BmnTof1Hit", "TOF1", aTofHits, kTRUE);

	pGeoUtils->ParseTGeoManager(fUseMCData, h2TestStrips, true);
	pGeoUtils->FindNeighborStrips(h1TestDistance, h2TestNeighborPair, fDoTest);
	
    	FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "Initialization [BmnTof1HitProducer::Init] finished succesfully.");

return kSUCCESS;
}
//--------------------------------------------------------------------------------------------------------------------------------------
Bool_t 		BmnTof1HitProducer::HitExist(Double_t val) // val - distance to the pad edge [cm]
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
Bool_t 		BmnTof1HitProducer::DoubleHitExist(Double_t val) // val - distance to the pad edge  [cm]
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
void 		BmnTof1HitProducer::Exec(Option_t* opt) 
{
    clock_t tStart = clock();
    if (fVerbose) cout << endl << "======================== TOF400 exec started ====================" << endl;
	static const TVector3 XYZ_err(fErrX, fErrY, 0.); 

	aTofHits->Clear();
		 
	Int_t 		UID, trackID;	
	TVector3 	pos, XYZ_smeared; 	
    	int		nSingleHits = 0, nDoubleHits = 0;
	
	if(fUseMCData)
	{
		for(Int_t pointIndex = 0, nTofPoint = aMcPoints->GetEntriesFast(); pointIndex < nTofPoint; pointIndex++ )  // cycle by TOF points
		{
			BmnTOF1Point *pPoint = (BmnTOF1Point*) aMcPoints->UncheckedAt(pointIndex);
		
			if(fVerbose > 2) pPoint->Print(""); 		
  
			trackID = pPoint->GetTrackID();	
			UID	= pPoint->GetDetectorID();
			Double_t time = pRandom->Gaus(pPoint->GetTime(), fTimeSigma); // 100 ps		
			pPoint->Position(pos);
	
			const LStrip *pStrip = pGeoUtils->FindStrip(UID);
		
			XYZ_smeared.SetXYZ( pStrip->center.X(), pRandom->Gaus(pos.Y(), fErrY), pStrip->center.Z());

			LStrip::Side_t side;
			Double_t distance = pStrip->MinDistanceToEdge(&pos, side); // [cm]

			bool passed;
			if(passed = HitExist(distance)) // check efficiency 
			{
			 	AddHit(UID, XYZ_smeared, XYZ_err, pointIndex, trackID, time); 	
			 	nSingleHits++;

			 	if(fDoTest)
			 	{
			 		h2TestXYSmeared->Fill(pos.X() - XYZ_smeared.X(), pos.Y() - XYZ_smeared.Y());
			 		h2TestXYSmeared2->Fill(XYZ_smeared.X(), XYZ_smeared.Y());
			 		h2TestEtaPhi->Fill(pos.Eta(), pos.Phi()*TMath::RadToDeg());
			 		h2TestRZ->Fill(pos.X(), pos.Y());
			 	}
			} 
		
			if(fDoTest) effTestEfficiencySingleHit->Fill(passed, distance);
        	
        		if(passed = DoubleHitExist(distance)) // check cross hit
        		{
        			Int_t CrossUID = (side == LStrip::kRight) ? pStrip->neighboring[LStrip::kRight] : pStrip->neighboring[LStrip::kLeft];
  			
  				if(LStrip::kInvalid  == CrossUID) continue; // last strip on module
  			
  				pStrip = pGeoUtils->FindStrip(CrossUID);
        			XYZ_smeared.SetXYZ( pStrip->center.X(), pRandom->Gaus(pos.Y(), fErrY), pStrip->center.Z());
        			
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
				
		// Sorting by strip UIDs
		typedef multimap<Int_t, BmnTof1Digit*> Tmap;
		Tmap 	mDigits;
                
                Int_t nT0Digits = aExpDigitsT0->GetEntriesFast();
                if (nT0Digits == 1){                               // T0 digit should be
                    BmnTrigDigit* digT0 = (BmnTrigDigit*) aExpDigitsT0->At(0);

                    for(Int_t digitIndex = 0, nTofDigits = aExpDigits->GetEntriesFast(); digitIndex < nTofDigits; digitIndex++ )  // cycle by TOF digits
                    {
                            BmnTof1Digit *pDigit = (BmnTof1Digit*) aExpDigits->UncheckedAt(digitIndex);		
                            if(fVerbose > 2) pDigit->print(); 	
                            if (pDigit->GetStrip() == 0 || pDigit->GetStrip() == 47) continue; // skip noise strips
                            UID =  BmnTOF1Point::GetVolumeUID(0, pDigit->GetPlane() + 1, pDigit->GetStrip() + 1); // strip [0,47] -> [1, 48]
                            mDigits.insert(make_pair(UID, pDigit));				
                    }

                    // Looking for digit pairs on both sides of same strip
                    size_t counter;
                    for(Tmap::iterator it = mDigits.begin(), itEnd = mDigits.end(); it != itEnd;  it = mDigits.upper_bound(it->first))
                    {
                            UID = it->first;
                            counter = mDigits.count(UID); // hash(detId, stripId)

                            if(counter != 2) continue; // now can be ONLY ONE digit at the strip side  1 + 1 == 2
                            BmnTof1Digit *dig1, *dig2;
                            if (it->second->GetSide() == 0 ){
                                dig1 = it->second;
                                dig2 = (++it)->second;
                            }
                            else {
                                dig2 = it->second;
                                dig1 = (++it)->second;
                            }
                            Short_t side1 = dig1->GetSide(), side2 = dig2->GetSide();

                            if(side1 != side2) // digits on different sides of same strip
                            {
                                    Int_t  strip =	BmnTOF1Point::GetStrip(UID);
                                    Int_t det = BmnTOF1Point::GetModule(UID);

                                    if(fDoTest) h2TdetIdStripId->Fill(strip, det);

                                    const LStrip *pStrip = pGeoUtils->FindStrip(UID);

                                    if(GetCrossPoint(pStrip, dig1->GetTime(), dig2->GetTime(), crosspoint)) // crosspoint inside strip edges
                                    {
                                            AddHit(UID, crosspoint, XYZ_err, -1, -1, /*dig1->GetTime()*/CalculateToF(dig1, dig2, digT0)); 	
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
                }
	}
	
	MergeHitsOnStrip(); // save only the fastest hit in the strip

	int nFinally = CompressHits(); // remove blank slotes
        
        clock_t tFinish = clock();
        workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;

        if (fVerbose) cout<<"Tof400  single hits= "<<nSingleHits<<", double hits= "<<nDoubleHits<<", final hits= "<<nFinally<<endl;
        if (fVerbose) cout << "======================== TOF400 exec finished ====================" << endl;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void 			BmnTof1HitProducer::Finish() 
{
  	if(fDoTest)
    	{
      		FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "[BmnTof1HitProducer::Finish] Update  %s file. ", fTestFlnm.Data());
		TFile *ptr = gFile;
		TFile file(fTestFlnm.Data(), "RECREATE");
		fList.Write(); 
		file.Close();
		gFile = ptr;
	}
        
    cout << "Work time of the TOF-400 hit finder: " << workTime << endl;
}
//--------------------------------------------------------------------------------------------------------------------------------------
// input- strip edge position & signal times; output- strip crosspoint; return false, if crosspoint outside strip 
bool			BmnTof1HitProducer::GetCrossPoint(const TVector3& p1, double time1, const TVector3& p2, double time2, TVector3& crossPoint) 
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

bool			BmnTof1HitProducer::GetCrossPoint(const LStrip *pStrip, double time1, double time2, TVector3& crossPoint) 
{
        TVector3 s1, s2, centr;
        s1 = (pStrip->A + pStrip->B) * 0.5; // [cm] strip side1 end's position 
        s2 = (pStrip->C + pStrip->D) * 0.5; // [cm] strip side2 end's position 
        centr = pStrip->center;
	double stripLength = (s2-s1).Mag();
	double maxDelta =   (stripLength + 1.0) * fSignalVelosity; // + 10 mm on the strip edge
	if(abs(time1 - time2) > maxDelta) return false; // estimated position out the strip edge.
	double dL = (time1 - time2) * 0.5 / fSignalVelosity;
        s1(0) = 0; s1(1) = dL; s1(2) = 0; //TMP ALIGMENT CORRECTIONS
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
void 			BmnTof1HitProducer::SetSeed(UInt_t seed)
{
	pRandom->SetSeed(seed);
}
//--------------------------------------------------------------------------------------------------------------------------------------

Double_t                BmnTof1HitProducer::CalculateToF (BmnTof1Digit *d1, BmnTof1Digit *d2, BmnTrigDigit *t0){
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
