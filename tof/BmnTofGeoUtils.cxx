//------------------------------------------------------------------------------------------------------------------------
#include <assert.h>
#include <iostream>

#include <TGeoManager.h>
#include "TClonesArray.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "TGeoNode.h"

#include "FairRootManager.h"
#include "FairLogger.h"

#include "BmnTofHit.h"
#include "BmnTOFPoint.h"

#include "BmnTofGeoUtils.h"


//------------------------------------------------------------------------------------------------------------------------
BmnTofGeoUtils::BmnTofGeoUtils()
{

}
//------------------------------------------------------------------------------------------------------------------------
void BmnTofGeoUtils::FindNeighborStrips(TH1D* h1, TH2D* h2, bool doTest)
{
	size_t NR = 0, NL= 0;
	const LStrip *strip2; double  distance;
	for(MStripIT it1 = mStrips.begin(), itEnd1 = mStrips.end(); it1 != itEnd1; it1++) // cycle1 by strips
	{
		LStrip *strip1 = &(it1->second);
		
		for(MStripCIT it2 = mStrips.begin(), itEnd2 = mStrips.end(); it2 != itEnd2; it2++) // cycle2 by strips
		{
			strip2 = &(it2->second);
	
			// CATION: Ckeck  only upper and lower sides(one horizontal strips NOW) 
			distance = strip1->Distance(LStrip::kUpper, *strip2); if(doTest)  h1->Fill(distance);		
			if(distance < 0.8) // CAUTION: constant depends on the geometry layout(see h1TestDistance histo)
			{
			 	strip1->neighboring[LStrip::kUpper] = strip2->volumeUID; NR++;
			 	if(doTest) h2->Fill(strip1->stripID, strip2->stripID);
			}
			
			distance = strip1->Distance(LStrip::kLower, *strip2); if(doTest)  h1->Fill(distance);
			if(distance < 0.8) // CAUTION: constant depends on the geometry layout(see h1TestDistance histo)
			{
				strip1->neighboring[LStrip::kLower] = strip2->volumeUID; NL++;
//				if(doTest) h2->Fill( strip2->stripID, strip1->stripID);	
				if(doTest) h2->Fill( strip1->stripID, strip2->stripID);	
			}			

		}// cycle2 by strips	
	}// cycle1 by strips
	
	cout<<" [BmnTofGeoUtils::FindNeighborStrips] Neighbor strips: left = "<<NL<<", right = "<<NR<<endl;
}
//------------------------------------------------------------------------------------------------------------------------	
void BmnTofGeoUtils::ParseTGeoManager(bool useMCinput, TH2D* h1, bool forced)
{
	assert(gGeoManager);

	if( !forced &&  !mStrips.empty()) return; // already parsed and filled

	mStrips.clear();
	
//	TString stripName, pathTOF = "/cave_1/TOFB1_0";
	TString stripName, pathTOF = "/cave_1/TOF700_0";
	gGeoManager->cd(pathTOF);
	
	Double_t *X0Y0Z0 = new Double_t[3]; X0Y0Z0[0] = X0Y0Z0[1] = X0Y0Z0[2] = 0.; // center of sensetive detector
	Double_t  *local = new Double_t[3], master[3],  dX, dY, dZ;
	
	Int_t  volumeUID, detectorID, stripID; 
	size_t nDetectors = 0, nStrips = 0; 
	
	TObjArray *array = gGeoManager->GetCurrentVolume()->GetNodes();
  	TIterator *it1 = array->MakeIterator();	
  	
  	TGeoNode *detectorNode, *stripNode;
  	while( (detectorNode = (TGeoNode*) it1->Next()) ) // detectors		
    	{
    		TString PATH1 = pathTOF + "/" + detectorNode->GetName(); detectorID = detectorNode->GetNumber(); nDetectors++;
//    		cout<<"\n DETECTOR: "<<detectorNode->GetName()<<", copy# "<<detectorID<<" path= "<<PATH1.Data();
    	
    	    	TIterator *it2 = detectorNode->GetNodes()->MakeIterator(); 		
      		while( (stripNode = (TGeoNode*) it2->Next()) )		// strips
		{
			stripName = stripNode->GetName();
    			if(!stripName.Contains("StripActiveGas")) continue;
    			
    			TString PATH2 = PATH1 + "/" + stripName;  stripID = stripNode->GetNumber(); nStrips++;
//    			cout<<"\n \tSTRIP: "<<stripNode->GetName()<<", copy# "<<stripID<<" path= "<<PATH2.Data();
    	
    			gGeoManager->cd(PATH2);
    			
    			TGeoMatrix *matrix = gGeoManager->GetCurrentMatrix();	// calculate global TGeoHMatrix for current node
			matrix->LocalToMaster(X0Y0Z0, master);			// 0.0.0. --> MRS	
  
  			TGeoBBox *box = (TGeoBBox*) gGeoManager->GetCurrentNode()->GetVolume()->GetShape(); 		
			dX = box->GetDX(); dY = box->GetDY(); dZ = box->GetDZ();
//      		cout<<"\n center: ("<<master[0]<<", "<<master[1]<<", "<<master[2]<<") d("<<dX<<", "<<dY<<", "<<dZ<<")";
      			
      			volumeUID = BmnTOFPoint::GetVolumeUID(0, detectorID, stripID);   // regionID == 0 now 			
      			
      			LStrip stripData(volumeUID, 0, 0, detectorID, stripID);
			stripData.center.SetXYZ(master[0], master[1], master[2]);   	
      			
      			// edges on the front plate of the strips. perp along Z.
			local[0] = -dX;	local[1] = -dY; local[2] = -dZ;
			matrix->LocalToMaster(local, master);
			stripData.A.SetXYZ(master[0], master[1], master[2]);
					
			local[0] = +dX;	local[1] = -dY; local[2] = -dZ;
			matrix->LocalToMaster(local, master);
			stripData.B.SetXYZ(master[0], master[1], master[2]);	
					
			local[0] = +dX;	local[1] = +dY; local[2] = -dZ;
			matrix->LocalToMaster(local, master);
			stripData.C.SetXYZ(master[0], master[1], master[2]);				
									
			local[0] = -dX;	local[1] = +dY; local[2] = -dZ;
			matrix->LocalToMaster(local, master);
			stripData.D.SetXYZ(master[0], master[1], master[2]);
					
			stripData.InitCenterPerp();
   			
// stripData.Dump("\n strip:");		
			bool IsUniqueUID = mStrips.insert(make_pair(volumeUID, stripData)).second;
			assert(IsUniqueUID);	   			   	
			
    		} // strips
    	} // detectors	

    	FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "[BmnTofHitProducer::ParseTGeoManager] detectors= %d, strips= %d. ", nDetectors, nStrips);
}
//------------------------------------------------------------------------------------------------------------------------	
void BmnTofGeoUtils::ParseStripsGeometry(const char *geomFile)
{
	mStrips.clear();

	readGeom(geomFile);

	Int_t nStrips = 0;
	
  	for ( int i = 0; i < nchambers; i++) // detectors		
    	{
      		for ( int j = 0; j < nstrips[i]; j++ )		// strips
		{
 			Int_t uid = ((i+1) << 8) | (j+1); 
      			LStrip stripData(uid, 0, 0, i+1, j+1);

			stripData.center.SetXYZ(xcens[i][j], ycens[i][j], zchamb[i]);   	
      			
      			// edges on the front plate of the strips. perp along Z.
			stripData.A.SetXYZ(xmins[i][j], ymins[i][j], zchamb[i]);
					
			stripData.B.SetXYZ(xmaxs[i][j], ymins[i][j], zchamb[i]);	
					
			stripData.C.SetXYZ(xmaxs[i][j], ymaxs[i][j], zchamb[i]);				
									
			stripData.D.SetXYZ(xmins[i][j], ymaxs[i][j], zchamb[i]);
					
			stripData.InitCenterPerp();
   			
// stripData.Dump("\n strip:");		
			bool IsUniqueUID = mStrips.insert(make_pair(uid, stripData)).second;
			nStrips++;
			
    		} // strips
    	} // detectors	

    	FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "[BmnTofHitProducer::ParseStripGeometry] detectors= %d, strips= %d. ", nchambers, nStrips);
}
//------------------------------------------------------------------------------------------------------------------------
const LStrip* BmnTofGeoUtils::FindStrip(Int_t UID) 
{
	MStripCIT cit = mStrips.find(UID);
	assert(cit != mStrips.end());
	return &(cit->second);
}
//------------------------------------------------------------------------------------------------------------------------
const LStrip* BmnTofGeoUtils::FindStrip(Int_t UID, TVector3& p) 
{
	Int_t uid = UID;
	Int_t i = (UID>>8) - 1;
      	for ( int j = 0; j < nstrips[i]; j++ )		// strips
	{
	    if ((p.X() < (xmins[i][j]-0.05)) || (p.X() > (xmaxs[i][j]+0.05))) continue;
	    if ((p.Y() < (ymins[i][j]-0.05)) || (p.Y() > (ymaxs[i][j]+0.05))) continue;
	    uid |= (j+1);
	    return FindStrip(uid);
	}
	printf("Point XYZ %f %f %f\n", p.X(), p.Y(), p.Z());
	printf("Strip not found UID = %d (UID>>8-1) %d nstrips %d %f %f %f %f\n", UID, i, nstrips[i], xmins[i][0], xmaxs[i][0], ymins[i][0], ymaxs[i][0]);
	return NULL;
}
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
LRectangle::LRectangle(Int_t uid, const TVector3& a, const TVector3& b, const TVector3& c, const TVector3& d, bool check) 
: IsInvalid(false), volumeUID(uid), A(a), B(b), C(c), D(d)
{
	if(check) CheckInValid();

}
//------------------------------------------------------------------------------------------------------------------------
Double_t LRectangle::DistanceFromPointToLine(const TVector3* pos, const TVector3& P1,const TVector3& P2)const
{
    assert(P1 != P2);

    return   (  (*pos - P1).Cross(*pos - P2)   ).Mag() / (P2 - P1).Mag();
}
//------------------------------------------------------------------------------------------------------------------------
Double_t 	LRectangle::DistanceFromPointToLineSegment(const TVector3* pos, const TVector3& P1,const TVector3& P2)const
{
	assert(P1 != P2);

	TVector3 v = P2 - P1;
	TVector3 w = (*pos) - P1;
	
	double c1 = w.Dot(v);
	if( c1 <= 0 )	return w.Mag();

	double c2 = v.Dot(v);
    	if( c2 <= c1 ) return ((*pos) - P2).Mag();
    	    	
    	TVector3 Pb = P1 + (c1/c2) * v;
	return ((*pos) - Pb).Mag();
}
//------------------------------------------------------------------------------------------------------------------------
Double_t		LRectangle::MinDistanceToEdge(const TVector3* pos, Side_t& side) const
{	
	double lower 	= DistanceFromPointToLineSegment(pos, A, B);
	double upper 	= DistanceFromPointToLineSegment(pos, C, D);
	
	// sorting & return minimal value
	if( lower <= upper )
	{ 
		side =  LStrip::kLower; 
		return lower;
	}		
	 
	side =  LStrip::kUpper;  
	return upper;
}
//------------------------------------------------------------------------------------------------------------------------
void LRectangle::Print(ostream &out, const TVector3 &point, const char* comment)const
{
	if(comment) out<<comment; 
	out<<" ("<< point.X()<<","<<point.Y()<<","<<point.Z()<<") "; 
}
//------------------------------------------------------------------------------------------------------------------------
void LRectangle::Dump(const char* comment, ostream& out) const 
{ 
	if(comment) out<<comment; out<<" uid="<<volumeUID<<" IsInvalid="<<IsInvalid;
	Print(out, A, " A:"); Print(out, B, " B:"); Print(out, C, " C:"); Print(out, D, " D:");
}
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
LStrip::LStrip() 
: LRectangle(), sectorID(kInvalid), boxID(kInvalid),  detectorID(kInvalid), stripID(kInvalid) 
{ 
	neighboring[kUpper] = kInvalid; 
	neighboring[kLower] = kInvalid; 
}
//------------------------------------------------------------------------------------------------------------------------
LStrip::LStrip(Int_t uid, Int_t sector, Int_t box, Int_t detector, Int_t strip) 
 : LRectangle(), sectorID(sector), boxID(box),  detectorID(detector), stripID(strip) 
{ 
	volumeUID = uid;
	neighboring[kUpper] = kInvalid; 
	neighboring[kLower] = kInvalid; 
}	
//------------------------------------------------------------------------------------------------------------------------
void LStrip::Dump(const char* comment, ostream& out) const 
{ 	
	if(comment) out<<comment; 
	out<<"  ids: "<<sectorID<<", "<<boxID<<", "<<detectorID<<", "<<stripID; 
	
	LRectangle::Dump(nullptr, out);
}
//------------------------------------------------------------------------------------------------------------------------	
Double_t LStrip::Distance(Side_t side, const LStrip& strip)
{
	Double_t value, min1 = 1.e+10, min2 = 1.e+10; // big value
	
	if((*this) == strip) 		return min1+min2; // same strip
	if(!IsSameDetector(strip)) 	return min1+min2; // different modules
		
	TVector3 *p1, *p2;
	switch(side)
	{
		case kUpper: 	p1 = &C; p2 = &D; break;	
		case kLower: 	p1 = &A; p2 = &B; break;				
	};

	value 	= fabs((*p1 - strip.A).Mag());	min1 = (value < min1) ? value : min1; 
	value 	= fabs((*p1 - strip.B).Mag());	min1 = (value < min1) ? value : min1; 	
	value 	= fabs((*p1 - strip.C).Mag());	min1 = (value < min1) ? value : min1; 		
	value 	= fabs((*p1 - strip.D).Mag());	min1 = (value < min1) ? value : min1; 
	
	value 	= fabs((*p2 - strip.A).Mag());	min2 = (value < min2) ? value : min2;		 
	value 	= fabs((*p2 - strip.B).Mag());	min2 = (value < min2) ? value : min2; 
	value 	= fabs((*p2 - strip.C).Mag());	min2 = (value < min2) ? value : min2;	
	value 	= fabs((*p2 - strip.D).Mag());	min2 = (value < min2) ? value : min2;
	
	return min1 + min2;
}	
//------------------------------------------------------------------------------------------------------------------------
int BmnTofGeoUtils::readGeom(const char *geomfile)
{
	char fname[128];
	FILE *fg = 0;
	char ic[16] = {""};
	int n = 0;
	Double_t step, sx, sy, x, y, z, xoffs, yoffs, zoffs;
	if (strlen(geomfile) == 0)
	{
	    printf("TOF700 geometry file name not defined!\n");
	    return 0;
	}
	TString dir = getenv("VMCWORKDIR");
	sprintf(fname,"%s/geometry/%s",dir.Data(),geomfile);
	fg = fopen(fname,"r");
	if (fg == NULL)
	{
	    printf("TOF700 geometry file %s open error!\n", fname);
	    return 0;
	}
	if (fscanf(fg,"%lf %lf %lf\n", &xoffs, &yoffs, &zoffs) != 3)
	{
	    printf("Wrong first line in TOF700 geometry file %s\n", fname);
	    return 0;
	};
	printf("Zoffs = %f\n", zoffs);
	for (int i=0; i<TOF2_MAX_CHAMBERS; i++) nstrips[i] = 0;
	int c = 0;
	while(fscanf(fg,"%s %d %lf %lf %lf %lf %lf %lf\n", &ic[0], &n, &step, &sy, &sx, &x, &y, &z) == 8)
	{
		halfxwidth[c] = sx/20.;
		halfywidth[c] = sy/20.;
		zchamb[c] = z/10. + zoffs;
		nstrips[c] = n;
		if (n == 32) chtype[c] = 0;
		else	     chtype[c] = 1;
		for (int ns=n-1; ns>=0; ns--)
		{
		xcens[c][ns] = -x/10. + xoffs;
		ycens[c][ns] = y/10. + yoffs - (n-ns-1)*step/10.;
		xmins[c][ns] = xcens[c][ns] - halfxwidth[c];
		xmaxs[c][ns] = xcens[c][ns] + halfxwidth[c];
		ymins[c][ns] = ycens[c][ns] - halfywidth[c];
		ymaxs[c][ns] = ycens[c][ns] + halfywidth[c];
//		printf("C %d S %d %f %f %f %f %f\n",ic,n,zchamb[c],xmins[c][ns],xmaxs[c][ns],ymins[c][ns],ymaxs[c][ns]);
		}
//		printf("%s ns=%d step=%f sx=%f sy=%f x=%f y=%f z=%f\n",ic,n,step,sx,sy,x,y,z);
		c++;
		if (c >= TOF2_MAX_CHAMBERS) break;
	}
	nchambers = c;
	fclose(fg);
	return c;
}


