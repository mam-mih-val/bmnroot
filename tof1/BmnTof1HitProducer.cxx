//--------------------------------------------------------------------------------------------------------------------------------------
#include<assert.h>

#include <TRandom2.h>
#include <TGeoManager.h>
#include <TGeoBBox.h>
#include <TGeoMatrix.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TEfficiency.h>

#include "FairHit.h"
#include "CbmStsPoint.h"
#include "CbmMCTrack.h"
#include "CbmTofHit.h"
#include "BmnHit.h"
#include "BmnTOF1Point.h"

#include "BmnTof1HitProducer.h"

using namespace std;

ClassImp(BmnTof1HitProducer)
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTof1HitProducer::BmnTof1HitProducer(const char *name, Int_t verbose, Bool_t test)
: FairTask(name, verbose), fTimeSigma(0.1), fErrX(1./sqrt(12.)), fErrY(0.5), fDoTest(test), fOnlyPrimary(kFALSE), pRandom(new TRandom2), aTofPoints(nullptr), aMCTracks(nullptr), aTofHits(nullptr), fTestFlnm("test.BmnTof1HitProducer.root")
{
    	if(fDoTest) 
    	{		
    		effTestEfficiencySingleHit = new TEfficiency("effSingleHit", "Efficiency single hit;R, cm;Side", 10000, -0.1, 1.); 						fList.Add(effTestEfficiencySingleHit);
		effTestEfficiencyDoubleHit = new TEfficiency("effDoubleHit", "Efficiency double hit;R, cm;Side", 10000, -0.1, 1.); 						fList.Add(effTestEfficiencyDoubleHit);
    	
		h1TestDistance = new TH1D("TestDistance", "Distance between strips;M, cm;Side", 1000, 0., 100.); 								fList.Add(h1TestDistance);
		
		h2TestMergedTimes = new TH2D("TestMergedTimes", "Merged hits on strip times test;faster hit time, ns;slower hit time, ns", 1000, 10., 50., 1000, 10., 50.);	fList.Add(h2TestMergedTimes);
		h2TestChainPID = new TH2D("TestChainPID", "Merged hits on strip pids test;pid;pid", 2000, -2250.5, 2250.5, 2000, -2250.5, 2250.5);				fList.Add(h2TestChainPID);
		h2TestXYSmeared = new TH2D("TestXYSmeared", "Smeared XY (single hit) test;#DeltaX, cm;#DeltaY, cm", 1000, -1., 1., 1000, -2., 2.);				fList.Add(h2TestXYSmeared);
		h2TestXYSmearedDouble = new TH2D("TestXYSmearedDouble", "Smeared XY (double hit) test;#DeltaX, cm;#DeltaY, cm", 1000, -2., 2., 1000, -2., 2.);			fList.Add(h2TestXYSmearedDouble);
		h2TestNeighborPair = new TH2D("TestNeighborPair", "Neighbor strip pairs test;stripID1;stripID2", 100, -0.5, 49.5, 100, -0.5, 49.5);				fList.Add(h2TestNeighborPair);
    	}
}
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTof1HitProducer::~BmnTof1HitProducer() 
{
    	delete pRandom;
}
//--------------------------------------------------------------------------------------------------------------------------------------
InitStatus 		BmnTof1HitProducer::Init() 
{
    	FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "Begin [BmnTof1HitProducer::Init].");
    	
    	if(fOnlyPrimary) cout<<" Only primary particles are processed!!! \n";

    	aTofPoints = (TClonesArray*) FairRootManager::Instance()->GetObject("TOF1Point");
    	aMCTracks = (TClonesArray*) FairRootManager::Instance()->GetObject("MCTrack");
assert(aTofPoints);
assert(aMCTracks);

    	// Create and register output array
    	aTofHits = new TClonesArray("BmnHit");
    	FairRootManager::Instance()->Register("TOF1Hit", "TOF1", aTofHits, kTRUE);

	ParseTGeoManager();
	FindNeighborStrips();

    	FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "Initialization [BmnTof1HitProducer::Init] finished succesfully.");

return kSUCCESS;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void 		BmnTof1HitProducer::Exec(Option_t* opt) 
{
	static const TVector3 XYZ_err(fErrX, fErrY, 0.);
	
	aTofHits->Clear();

   	if(!aTofPoints) 
    	{
        	Error("BmnTof1HitProducer::Init()", " !!! Unknown branch name !!! ");
        	return;
    	}

	const LStrip	*pStrip;
	Int_t 		UID, trackID;
	Double_t 	time;
	BmnTOF1Point 	*pPoint;
    	TVector3 	pos, XYZ_smeared;
    	int		nSingleHits = 0, nDoubleHits = 0;
    	
    	Int_t nTofPoint = aTofPoints->GetEntriesFast();
    	for(Int_t pointIndex = 0; pointIndex < nTofPoint; pointIndex++) // cycle by TOF points
    	{
        	pPoint = (BmnTOF1Point*) aTofPoints->At(pointIndex);
        	if(fVerbose > 2) pPoint->Print("");

        	if(fOnlyPrimary) 
		{
assert(pPoint->GetTrackID() >= 0);
            		CbmMCTrack* track = (CbmMCTrack*) aMCTracks->At(pPoint->GetTrackID());
assert(track != nullptr);
            		if(track->GetMotherId() != -1) continue;
        	}
        	
        	trackID = pPoint->GetTrackID();
        	UID = pPoint->GetVolumeUID();
        	time = pRandom->Gaus(pPoint->GetTime(), fTimeSigma); // default 100 ps
		pPoint->Position(pos);
		
		pStrip = FindStrip(UID);		
		XYZ_smeared.SetXYZ(pStrip->center.X(), pRandom->Gaus(pos.Y(), fErrY),  pos.Z());

		Double_t distance;
		LStrip::Side_t side;
		MinDistanceToEdge(&pos, pStrip, distance, side);

		bool passed;
		if(passed = HitExist(distance)) // check efficiency 
		{
		 	AddHit(UID, XYZ_smeared, XYZ_err, pointIndex, time);   	
		 	nSingleHits++;

		 	if(fDoTest) h2TestXYSmeared->Fill(pos.X() - XYZ_smeared.X(), pos.Y() - XYZ_smeared.Y());
		} 
		
		if(fDoTest) effTestEfficiencySingleHit->Fill(passed, distance);
        	
        	if(LStrip::kUp == side || LStrip::kDown == side) continue; // CAUTION: NOW MUSTBE ONLY left&right cross hits. 
        	
        	if(passed = DoubleHitExist(distance)) // check cross hit
        	{
        		Int_t CrossUID = (side == LStrip::kRight) ? pStrip->neighboring[LStrip::kRight] : pStrip->neighboring[LStrip::kLeft];
  			
  			if(LStrip::kInvalid  == CrossUID) continue; // last strip on module
  			
  			pStrip = FindStrip(CrossUID);
        		XYZ_smeared.SetXYZ(pStrip->center.X(), pRandom->Gaus(pos.Y(), fErrY),  pos.Z());
        			
        		AddHit(CrossUID, XYZ_smeared, XYZ_err, pointIndex, time); 
        		nDoubleHits++;
  		
        		if(fDoTest) h2TestXYSmearedDouble->Fill(pos.X() - XYZ_smeared.X(), pos.Y() - XYZ_smeared.Y());
        	}
        	
        	if(fDoTest) effTestEfficiencyDoubleHit->Fill(passed, distance);
        	
    	} // cycle by TOF points

	MergeHitsOnStrip(); // leave only the fastest hit in the strip 
	
	int nFinally = CompressHits(); // remove blank slotes, update indexes

    	cout<<" -I- [BmnTof1HitProducer::Exec] MCpoints= "<<nTofPoint<<", single hits= "<<nSingleHits<<", double hits= "<<nDoubleHits<<", final hits= "<<nFinally<<endl;
    	
    	///Dump();
}
//--------------------------------------------------------------------------------------------------------------------------------------
void		BmnTof1HitProducer::FindNeighborStrips()
{
	const LStrip *strip2; double  distance;
	for(MStripIT it1 = mStrips.begin(), itEnd1 = mStrips.end(); it1 != itEnd1 ; it1++) // cycle1 by strips
	{
		LStrip *strip1 = &(it1->second);
		
		for(MStripCIT it2 = mStrips.begin(), itEnd2 = mStrips.end(); it2 != itEnd2 ; it2++) // cycle2 by strips
		{
			strip2 = &(it2->second);
			
			// CATION: Ckeck  only left and right sides(one row strips NOW) 
			distance = strip1->Distance(LStrip::kRight, *strip2); if(fDoTest)  h1TestDistance->Fill(distance);
			if(distance < 0.65) // CAUTION: constant depends on the geometry layout(see h1TestDistance histo)
			{
			 	strip1->neighboring[LStrip::kRight] = strip2->detectorUID;
			 	if(fDoTest) h2TestNeighborPair->Fill(strip1->stripID, strip2->stripID);
			}
			
			///cout<<"\n RIGHT "<<distance; strip1->Dump("\t1");	strip2->Dump("\t2");	
		    		    	
			distance = strip1->Distance(LStrip::kLeft, *strip2); if(fDoTest)  h1TestDistance->Fill(distance);
			if(distance < 0.65) // CAUTION: constant depends on the geometry layout(see h1TestDistance histo)
			{
				strip1->neighboring[LStrip::kLeft] = strip2->detectorUID;
				if(fDoTest) h2TestNeighborPair->Fill(strip1->stripID, strip2->stripID);	
			}
			
			///cout<<"\n LEFT "<<distance;strip1->Dump("\t1");	strip2->Dump("\t2");			
		
		}// cycle2 by strips	
	}// cycle1 by strips
}
//--------------------------------------------------------------------------------------------------------------------------------------
void		BmnTof1HitProducer::ParseTGeoManager()
{
	mStrips.clear();
	
	TString stripName, PATH1, PATH2, PATH3, pathTOF = "/cave_1/TOFB1_0";
	gGeoManager->cd(pathTOF);
	
	TGeoNode *reg, *mod, *strip;
	TGeoMatrix *matrix;
	TGeoBBox *box;
	Int_t volumeUID, regID, modID, stripID;// reg[1,2], mod[1,6], strip[1,24]
	
	Double_t *X0Y0Z0 = new Double_t[3]; X0Y0Z0[0] = X0Y0Z0[1] = X0Y0Z0[2] = 0.; // center of sensetive detector
	Double_t  *local = new Double_t[3], master[3],  dX, dY, dZ;
	LStrip stripData;
	
assert(gGeoManager);	

  	TObjArray *array = gGeoManager->GetCurrentVolume()->GetNodes();
  	TIterator *it1 = array->MakeIterator(); int nRegions = 0, nModules = 0, nStrips = 0;	
  	while( (reg = (TGeoNode*) it1->Next()) )			// REGIONS
    	{
    	      	PATH1 = pathTOF + "/" + reg->GetName(); regID = reg->GetNumber(); nRegions++;
      		///cout<<"\n REGION: "<<reg->GetName()<<", copy# "<<regID<<" path= "<<PATH1.Data()<<endl;
    	
    		TIterator *it2 = reg->GetNodes()->MakeIterator(); 		
      		while( (mod = (TGeoNode*) it2->Next()) )		// MODULES
		{
	 		PATH2 = PATH1 + "/" + mod->GetName(); modID = mod->GetNumber(); nModules++;
	  		///cout<<"\n MODULE: "<<mod->GetName()<<", copy# "<<modID<<" path= "<<PATH2.Data()<<endl;
	  			
	  		TIterator *it3 = mod->GetNodes()->MakeIterator();
	  		while( (strip = (TGeoNode*) it3->Next()) )	// STRIPS
	    		{	
	    			stripName = strip->GetName(); nStrips++;
	    			stripID = strip->GetNumber();
	    			
	    			strip = strip->GetDaughter(1); // select TOFB1ActiveGasV_1 subvolume
	    			    		
		 		PATH3 = PATH2 + "/" + stripName.Data() + "/" + strip->GetName(); 
	  			///cout<<"\n STRIP: "<<strip->GetName()<<", copy# "<<stripID<<" path= "<<PATH3.Data()<<endl;    		
	    		
	    			gGeoManager->cd(PATH3);				// cd to strip node	
	    			matrix = gGeoManager->GetCurrentMatrix();	// calculate global TGeoHMatrix for current node
				matrix->LocalToMaster(X0Y0Z0, master);		// 0.0.0. --> MRS			
				///cout<<"\n center= "<<master[0]<<", "<<master[1]<<", "<<master[2];
			
				volumeUID = BmnTOF1Point::GetVolumeUID(regID, modID, stripID);
			
				box = (TGeoBBox*) strip->GetVolume()->GetShape(); 		// cell<->node (sensitive gas volume)
				dX = box->GetDX(); dY = box->GetDY(); dZ = box->GetDZ();			
				///cout<<"\n dXYZ= "<<dX<<", "<<dY<<", "<<dZ;			
	
				stripData.center.SetXYZ(master[0], master[1], master[2]);
				
				// edges on the front plate of the strips. CAUTION: current implementation for unrotated strips
				stripData.point[0].SetXYZ(master[0] - dX, master[1] + dY, master[2] - dZ);
				stripData.point[1].SetXYZ(master[0] + dX, master[1] + dY, master[2] - dZ);				
				stripData.point[2].SetXYZ(master[0] + dX, master[1] - dY, master[2] - dZ);
				stripData.point[3].SetXYZ(master[0] - dX, master[1] - dY, master[2] - dZ);	
						
				stripData.SetIDs(volumeUID, regID, modID, stripID);
				
				bool IsUniqueUID = mStrips.insert(make_pair(volumeUID, stripData)).second;
assert(IsUniqueUID);			
	    		} // STRIPS  		
		} // MODULES
    	} // REGIONS   	
    	
    	FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "[BmnTof1HitProducer::ParseTGeoManager] Regions= %d, modules= %d, strips= %d. ", nRegions, nModules, nStrips);
}
//------------------------------------------------------------------------------------------------------------------------
Int_t 		BmnTof1HitProducer::CompressHits() 
{
	aTofHits->Compress();
	
	// Update BmnHit::fIndex member data, naxrena eto komuto nado?
	for(Int_t hitIndex = 0, nHits = aTofHits->GetEntriesFast(); hitIndex < nHits; hitIndex++) 
		((BmnHit*) aTofHits->UncheckedAt(hitIndex))->SetIndex(hitIndex);
		
return 	aTofHits->GetEntriesFast();	
}	
//------------------------------------------------------------------------------------------------------------------------
Int_t 		BmnTof1HitProducer::MergeHitsOnStrip() // leave only the fastest hit in the strip 
{
typedef map<Int_t, BmnHit*> hitsMapType;

	hitsMapType 		fastestHits; // pair<detectorUID, BmnHit*>
	hitsMapType::iterator 	it;	
	BmnHit *pHit;
	Int_t UID, nHits = aTofHits->GetEntriesFast(), reducedNmb = 0;   
	double time1, time2;
	
	for(Int_t hitIndex = 0; hitIndex < nHits; hitIndex++ ) // cycle by hits
	{	
		pHit = (BmnHit*) aTofHits->UncheckedAt(hitIndex); 		
assert(nullptr != pHit);//	if(!pHit)continue; // blank slot
		
		UID = pHit->GetDetectorID();
		it = fastestHits.find(UID);
		if(it != fastestHits.end()) // exist hit for this UID
		{
			reducedNmb++; 
			time1 = pHit->GetTimeStamp();
			time2 = it->second->GetTimeStamp();
			
			if(time1 < time2) // founded more faster hit
			{
				if(fDoTest)
				{
					h2TestMergedTimes->Fill(time1, time2);
					
					int mcTrackId = ( (BmnTOF1Point*) aTofPoints->UncheckedAt(pHit->GetRefIndex()) )->GetTrackID();
					int pid1 = ( (CbmMCTrack*) aMCTracks->At(mcTrackId) )->GetPdgCode();
					mcTrackId = ( (BmnTOF1Point*) aTofPoints->UncheckedAt(it->second->GetRefIndex()) )->GetTrackID();
					int pid2 = ( (CbmMCTrack*) aMCTracks->At(mcTrackId) )->GetPdgCode();
					
					h2TestChainPID->Fill(pid1, pid2);
				}
				
				//pHit->AddLinks(it->second->GetLinks());		// copy links
				aTofHits->Remove(it->second); 				// remove old hit   --> make blank slote !!
				//pHit->SetFlag(pHit->GetFlag() | MpdTofUtils::HaveTail);	// Set "HaveTail" flag					
				fastestHits[UID]= pHit;					// change pair value to current UID
			}
			else  	aTofHits->Remove(pHit);					// remove current hit --> make blank slote !!
		}
		else fastestHits.insert(make_pair(UID, pHit)); 				// insert new pair
	} // cycle by hits

return 	reducedNmb;	
}
//------------------------------------------------------------------------------------------------------------------------
Bool_t 		BmnTof1HitProducer::HitExist(Double_t distance) const //  distance to the edge of the pad
{
  const static Double_t slope = (0.98 - 0.95)/0.2;
  Double_t efficiency = (distance > 0.2) ? 0.98 : ( 0.95 + slope*distance);
	
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
Bool_t 		BmnTof1HitProducer::DoubleHitExist(Double_t distance) const // distance to the edge of the pad
{
  const static Double_t slope = (0.3 - 0.0)/0.5;
  Double_t efficiency = (distance > 0.5) ? 0. : (0.3 - slope*distance);
	
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
	
  if(pRandom->Rndm() < efficiency) return HitExist(distance);
  
return false;	
}
//------------------------------------------------------------------------------------------------------------------------
const LStrip*		BmnTof1HitProducer::FindStrip(Int_t UID) const
{
	MStripCIT cit = mStrips.find(UID);

assert(cit != mStrips.end());

return &(cit->second);
}
//--------------------------------------------------------------------------------------------------------------------------------------
void			BmnTof1HitProducer::MinDistanceToEdge(const TVector3* pos,const LStrip *strip, Double_t& distance, LStrip::Side_t& side) const
{	
	double up 	= DistanceFromPointToLineSegment(pos, strip->point[0], strip->point[1]);
	double right 	= DistanceFromPointToLineSegment(pos, strip->point[1], strip->point[2]);
	double down 	= DistanceFromPointToLineSegment(pos, strip->point[2], strip->point[3]);	
	double left 	= DistanceFromPointToLineSegment(pos, strip->point[3], strip->point[0]);
	
	// sorting
	if(up <= right && up <= down && up <= left)		{ distance  = up; 	side =  LStrip::kUp; 	return; }
	if(right <= down && right <= left && right <= up)	{ distance  = right; 	side =  LStrip::kRight; return; }	
	if(down <= left && down <= up && down <= right)		{ distance  = down; 	side =  LStrip::kDown;  return; }	
	if(left <= up && left <= right && left <= down)		{ distance  = left; 	side =  LStrip::kLeft;  return; }	
	
assert(false);		
}
//--------------------------------------------------------------------------------------------------------------------------------------
Double_t 		BmnTof1HitProducer::DistanceFromPointToLineSegment(const TVector3* pos, const TVector3& P1,const TVector3& P2)const
{
	const double dX = P2.X() - P1.X();
	const double dY = P2.Y() - P1.Y();	

return ( dY * pos->X() - dX * pos->Y() + P2.X() * P1.Y() - P2.Y() * P1.X() ) / sqrt(dY*dY + dX*dX);
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
}
//--------------------------------------------------------------------------------------------------------------------------------------
void			BmnTof1HitProducer::AddHit(Int_t UID, TVector3 XYZ_smeared, TVector3 XYZ_err, Int_t pointIndex, Double_t time)
{
	int hitIndex = aTofHits->GetEntriesFast();
        BmnHit* hit = new ((*aTofHits)[hitIndex]) BmnHit(UID, XYZ_smeared, XYZ_err, pointIndex);
        hit->SetIndex(hitIndex);
        hit->SetDetId(kTOF1);
        hit->SetTimeStamp(time);
}
//--------------------------------------------------------------------------------------------------------------------------------------
void			BmnTof1HitProducer::Dump(TClonesArray *array, const char* comment, ostream& out)const
{
	out<<"\n [BmnTof1HitProducer::Dump]   "; if(comment) out<<comment;  out<<", size= "<<array->GetEntriesFast();

	TIterator *iter = array->MakeIterator(); BmnTOF1Point *point; BmnHit *pHit; TVector3 hitPos, pointPos;		
      	while( (pHit = (BmnHit*) iter->Next()) )   						
	{
		pHit->Position(hitPos);
		out<<"\n    hit detUID = "<<pHit->GetDetectorID()<<", hit pos("<<hitPos.X()<<","<<hitPos.Y()<<","<<hitPos.Z()<<"), flag ="<<pHit->GetFlag();
		
		if(aTofPoints)
		{
			point = (BmnTOF1Point*) aTofPoints->UncheckedAt(pHit->GetRefIndex());
			point->Position(pointPos);
			out<<"\n point detUID = "<<point->GetDetectorID()<<", point pos("<<pointPos.X()<<","<<pointPos.Y()<<","<<pointPos.Z()<<"), dev="<<(hitPos-pointPos).Mag();
		}
	}

	delete iter;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void 			BmnTof1HitProducer::SetSeed(UInt_t seed)
{
	pRandom->SetSeed(seed);
}
//--------------------------------------------------------------------------------------------------------------------------------------

