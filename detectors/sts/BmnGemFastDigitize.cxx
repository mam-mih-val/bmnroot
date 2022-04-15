// -------------------------------------------------------------------------
// -----                  BmnGemFastDigitize source file               -----
// -----                 Created 4/07/19  by A. Zinchenko              -----
// -------------------------------------------------------------------------

#include "BmnGemFastDigitize.h"
#include "CbmMCTrack.h"
#include "CbmStsPoint.h"
#include "CbmStsSensor.h"

#include "FairRootManager.h"

#include <TClonesArray.h>
#include <TCut.h>
#include <TDatabasePDG.h>
#include <TEntryList.h>
#include <TGeoManager.h>
#include <TH1I.h>
#include <TH2I.h>
#include <TParticlePDG.h>
#include <TRandom.h>
#include <TROOT.h>
#include "TFile.h"
#include <TSystem.h>
#include <TTree.h>
//#include <TH1F.h>
//#include <TH3F.h>

#include <iostream>
#include <fstream>
#include <queue>

using std::cout;
using std::endl;
using std::ifstream;
using std::iterator;
using std::map;
using std::multimap;
using std::pair;
using std::set;
using std::vector;

// -----   Default constructor   -------------------------------------------
BmnGemFastDigitize::BmnGemFastDigitize() 
  : FairTask("GemFastDigitize"),
    fhInd(NULL)
{ 
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
BmnGemFastDigitize::~BmnGemFastDigitize() { }
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus BmnGemFastDigitize::Init() {

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- BmnGemFastDigitize::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input arrays
  fPoints = (TClonesArray*) ioman->GetObject("StsPoint");
  if ( ! fPoints ) {
    cout << "-W- BmnGetFastDigitize::Init: "
	 << "No STSPoint array!" << endl;
    return kERROR;
  }
  fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");

  // Get list of files with digit trees
  if (gSystem->Exec("du -s clusLib")) {
    LOG(fatal) << " !!! No clusLib directory found - exit !!!";
  }
  //gSystem->Exec("ls -1 clusLib/thx*histo.root > aaa");
  //gSystem->Exec("ls -1 thxp10thyp00.histo.root > aaa");

  //ifstream fin("aaa");
  ifstream fin("clusLib/file_list.txt");
  TString fileName;
  //const Int_t nbinsx = 15, nbinsy = 5;
  //Double_t xlim[nbinsx] = {-75, 75}; // theta_x (deg)
  const Int_t nbinsx = 29, nbinsy = 5;
  Double_t xlim[nbinsx] = {-72.5, 72.5}; // theta_x (deg)
  Double_t ylim[nbinsy] = {-25, 25}; // theta_y
  fhInd = new TH2I("hInd","",nbinsx,xlim[0],xlim[1],nbinsy,ylim[0],ylim[1]);
  fhInd->StatOverflows(kTRUE); // correct numbering of underflow / overflow bins
  
  while(1) {
    fin >> fileName;
    if (fin.eof()) break;

    // Decode file name to get theta_X and theta_Y
    //Int_t ithx = TString(fileName(4,2)).Atoi(), ithy = TString(fileName(10,2)).Atoi();
    //if (fileName(4) == 'm') ithx = -ithx;
    //if (fileName(10) == 'm') ithy = -ithy;
    Int_t ithx = TString(fileName(12,2)).Atoi(), ithy = TString(fileName(18,2)).Atoi();
    if (fileName(11) == 'm') ithx = -ithx;
    if (fileName(17) == 'm') ithy = -ithy;
    TString hname = "h";
    hname += ithx;
    hname += "_";
    hname += ithy;
    /*
    //Double_t xmin[2] = {238.0, 252.5}, xmax[2] = {242.5, 257.0};
    Double_t xmin[2] = {236.8, 251.4}, xmax[2] = {238.3, 252.8};
    Int_t nbins = 100;
    //TH2I *hXY = new TH2I("hXY","Y vs X",nbins,xmin[0],xmax[0],nbins,xmin[1],xmax[1]); // Y vs X (cog[1] vs cog[0])
    //clus->Project(hXY->GetName(),"cog[1]:cog[0]",cut);
    */
    //TFile *file = TFile::Open("checkDigi_st5_new.histo.root");
    TFile *file = TFile::Open(fileName);
    TTree *clus = (TTree*) file->Get("clus");
 
    // Get stored histogram
    TH2I *hXY = (TH2I*) file->Get("hXY");
    hXY = (TH2I*) hXY->Clone(hname);
    Int_t indXY = fhInd->Fill(ithx,ithy);
    fHistMap[indXY] = hXY;
    fTreeMap[indXY] = clus;

    clus->SetBranchStatus("*",0); // disable branches
    clus->SetBranchStatus("qtot*",1); // enable branches
    clus->SetBranchStatus("ibeg*",1); // enable branches
    clus->SetBranchStatus("strin*",1); // enable branches
    clus->SetBranchStatus("dpitin*",1); // enable branches
    clus->SetBranchStatus("strout*",1); // enable branches
    clus->SetBranchStatus("cog*",1); // enable branches
    clus->SetBranchStatus("digis*",1); // enable branches
  
    //TList *list = file.GetList();
    //Int_t ntot = list->GetEntries();

    //TCut cut0("abs(imax[0]-240)<5&&strout[0]==244"); // side 0
    //TCut cut1("abs(imax[1]-254)<5&&strout[1]==259&&imax[1]>252"); // side 1
    TCut cut0("abs(ibeg[0]-237)<5&&strout[0]==244"); // side 0
    TCut cut1("abs(ibeg[1]-252)<5&&strout[1]==259"); // side 1
    TCut cut = ""; //cut0 && cut1;
    if (file->Get("CUT")) cut = *((TCut*)file->Get("CUT"));
  
    TEntryList *entryList = NULL;
    clus->Draw(">>entryList",cut,"entrylist",9000000000);
    entryList = (TEntryList*) gDirectory->Get("entryList");
    clus->SetEntryList(entryList);
    cout << " xxxxxx " << fileName << " " << entryList->GetN() << " " << ithx << " " << ithy << " " << indXY << endl;
    
    fvDig0 = &fvDigis[0];
    fvDig1 = &fvDigis[1];
    
    clus->SetBranchAddress("qtot[2]",&fQtot);
    clus->SetBranchAddress("ibeg[2]",&fIbeg);
    clus->SetBranchAddress("strin[2]",&fStrin);
    clus->SetBranchAddress("dpitin[2]",&fDpitin);
    clus->SetBranchAddress("strout[2]",&fStrout);
    clus->SetBranchAddress("cog[2]",&fCog);
    clus->SetBranchAddress("digis0", &fvDig0);
    clus->SetBranchAddress("digis1", &fvDig1);
    
    Int_t entries = entryList->GetN();
    entries = TMath::Min(10000,entries);
    map<Int_t,Int_t> strin[2]; // entrance strips
    std::multimap<Float_t,Int_t> mmm;
    fXYmap[0][indXY] = mmm;
    fXYmap[1][indXY] = mmm;
    
    for (Int_t j = 0; j < entries; ++j) {
      Int_t treeEn = entryList->GetEntry(j);
      clus->GetEntry(treeEn);
      //cout << j << " " << entryList->GetEntry(j) << " " << cog[0] << " " << cog[1] << endl;
      
      for (Int_t side = 0; side < 2; ++side) {
	fXYmap[side][indXY].insert(pair<Float_t,Int_t>(fCog[side],treeEn));
	if (strin[side].find(fStrin[side]) == strin[side].end()) strin[side][fStrin[side]] = 0;
	++strin[side][fStrin[side]];
      }
    }
    
    // Find reference strip
    for (Int_t side = 0; side < 2; ++side) {
      map<Int_t,Int_t>::iterator it = strin[side].begin();
      fStrRef[side][indXY] = it->first;
      Int_t maxStrip = it->second;
      
      for ( ; it != strin[side].end(); ++it) {
	if (it->second > maxStrip) {
	  maxStrip = it->second;
	  fStrRef[side][indXY] = it->first;
	}
      }
    }
    
    //file.Close();
  }
  
  cout << "-I- BmnGemFastDigitize: Initialisation successful" << endl;
  return kSUCCESS;

}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void BmnGemFastDigitize::Exec(Option_t* opt) {

}
// -------------------------------------------------------------------------

// -----   Public method Finish   ------------------------------------------
void BmnGemFastDigitize::Finish()
{
}
// -------------------------------------------------------------------------

// -----   Public method ProduceHitResponse   ------------------------------
void BmnGemFastDigitize::ProduceHitResponseFast(CbmStsSensor* sensor, std::set<Int_t> &pSet, 
						Double_t *stripSignalF,  Double_t *stripSignalB,
						map<Int_t, set<Int_t> > &chanPointMapF, 
						map<Int_t, set<Int_t> > &chanPointMapB) 
{
  // Produce sensor response 

  Int_t       iPoint = -1, stationNr = Int_t(sensor->GetStationNr());
  CbmStsPoint* point = NULL;
  set<Int_t>::iterator it1;
  Double_t dPitch = 0;

  for (it1 = pSet.begin(); it1 != pSet.end(); it1++) {

    //AZ Apply overall efficiency 
    //const Double_t eff0 = 0.65; 
    //if (gRandom->Rndm() > eff0) continue;
  
    iPoint = (*it1);
    point  = (CbmStsPoint*)fPoints->UncheckedAt(iPoint);
    TVector3 mom;
    point->Momentum(mom);

    CbmMCTrack *mcTr = (CbmMCTrack*) fMCTracks->UncheckedAt(point->GetTrackID()); //ES
    TParticlePDG *part = TDatabasePDG::Instance()->GetParticle(mcTr->GetPdgCode());
    Double_t mnoc = 1.0, mass = 3.7283999; // He4
    if (part) mass = part->Mass();
    if (mass > 0.0001) {
      TLorentzVector lorv;
      lorv.SetVectM(mom, mass);
      Double_t beta = lorv.Beta();
      Double_t gamma = lorv.Gamma();
      Double_t charge = (part) ? part->Charge()/3 : 1;
      mnoc = GetNumberOfClusters(beta, gamma, charge, 1.787, 12.33);
      if (mnoc < 0.5) continue; //AZ
      //cout << " ***clus*** " << stationNr << " " << mass << " " << beta << " " << gamma << " " << mnoc << endl;
    }
    Double_t coeff = mnoc / 29.2; // 29.2 - mnoc for test particles (pion with p = 1.6 GeV/c) 
    //AZ 31.07.20 coeff *= 2; // to correct ADC conversion factor
    coeff *= 1.; // to correct ADC conversion factor

    //Double_t  moduleThickness = 0.3 + 0.25 +  0.2 + 0.15 ; // DriftGapThickness + FirstTransferGapThickness + SecondTransferGapThickness + InductionGapThickness;

    TVector3 posIn, posOut;
    point->PositionIn(posIn);
    point->PositionOut(posOut);
    TVector3 dpos = posOut;
    dpos -= posIn;
    dpos *= 0.001;
    posIn += dpos;
    posOut -= dpos;
    
    Double_t xrand, yrand;

    // Get theX, theY data set
    if (mom.Z() < 0) mom *= -1; // particle going backward
    Double_t theX = TMath::ATan2 (mom.X(),mom.Z()) * TMath::RadToDeg();
    Double_t theY = TMath::ATan2 (mom.Y(),mom.Z()) * TMath::RadToDeg();
    if (stationNr % 2 == 0) theY = -theY; // 28-09-19 - OK
    Int_t indXY = FindDataSet(theX, theY);
    //cout << " ------------------ Data set: " << indXY << " " << posIn.Z() << " " << theX << " " << theY << endl;
    TH2I *hxy = fHistMap[indXY];

    //TTree *clus = (TTree*) gROOT->FindObjectAny("clus");
    //((TH2I*)gROOT->FindObjectAny("hXY"))->GetRandom2(xrand,yrand);
    hxy->GetRandom2(xrand,yrand);
    Float_t xy[2];
    xy[0] = xrand;
    xy[1] = yrand;

    Int_t istrip0[2] = {0};
    multimap<Float_t,Int_t>::iterator mit, mit1;

    for (Int_t ixy = 0; ixy < 2; ++ixy) {
      map<Int_t,Float_t> charges[2]; // 2 clusters
      //for (Int_t iclus = 0; iclus < 2; ++iclus) charges[iclus].clear();
      mit = fXYmap[ixy][indXY].lower_bound(xy[ixy]);
      if (mit == fXYmap[ixy][indXY].end()) --mit;
      // First cluster
      Int_t entry = mit->second;
      fTreeMap[indXY]->GetEntry(entry); 
      Double_t delt = TMath::Abs (fCog[ixy] - xy[ixy]);
      Int_t ndig = fvDigis[ixy].size();
      Double_t qTot0 = fQtot[ixy];
      Double_t dpitch0 = fDpitin[ixy];
      for (Int_t j = 0; j < ndig; ++j) {
	charges[0][j+fIbeg[ixy]] = fvDigis[ixy][j];
      }
      //cout << cog[ixy] << " " << xy[ixy] << endl;
      // Second cluster
      mit1 = mit;
      if (mit != fXYmap[ixy][indXY].begin()) --mit1;
      else ++mit1;
      entry = mit1->second;
      fTreeMap[indXY]->GetEntry(entry);
      //cout << cog[ixy] << " " << xy[ixy] << endl;
      Double_t delt1 = TMath::Abs (fCog[ixy] - xy[ixy]);
      Double_t dist = delt + delt1;
      //cout << dist << " " << delt << " " << delt1 << endl;
      Double_t w = (dist - delt) / dist;
      Double_t w1 = (dist - delt1) / dist;
      //w1 = 0; w = 1; // !!!!! test
      //AZ if (gRandom->Rndm() >= 0.5) { w1 = 0; w = 1; }
      //AZ else { w1 = 1; w = 0; }
      //cout << w << " " << w1 << " " << w+w1 << " " << (mit==xyMap[ixy].begin()) << endl;
      ndig = fvDigis[ixy].size();
      Double_t qTot1 = fQtot[ixy];
      Double_t dpitch1 = fDpitin[ixy];
      for (Int_t j = 0; j < ndig; ++j) {
	charges[1][j+fIbeg[ixy]] = fvDigis[ixy][j];
      }
      Int_t i0 = TMath::Min(charges[0].begin()->first,charges[1].begin()->first);
      //i0 = TMath::Max (i0,0);
      Int_t i1 = TMath::Max(charges[0].rbegin()->first,charges[1].rbegin()->first);
      //if (ixy == 0) i1 = TMath::Min (i1,sensor->GetNChannelsFront());
      //else i1 = TMath::Min (i1,sensor->GetNChannelsBack());
      /*
      Double_t cognew[3] = {0}, qqq[3] = {0};
      for (Int_t iclus = 0; iclus < 2; ++iclus) {
	for (map<Int_t,Float_t>::iterator it = charges[iclus].begin(); it != charges[iclus].end(); ++it) {
	  cognew[iclus] += it->first * it->second;
	  qqq[iclus] += it->second;
	}
	cognew[iclus] /= qqq[iclus];
	h1[ixy][iclus+1]->Fill(cognew[iclus]);
      }
      */
      Int_t istr;
      if (ixy == 0) {
	if (stationNr % 2) istrip0[ixy] = sensor->GetFrontChannel(posIn.X(),posIn.Y(),posIn.Z(),dPitch);
	else istrip0[ixy] = sensor->GetFrontChannel(posOut.X(),posOut.Y(),posOut.Z(),dPitch);
      } else {
	if (stationNr % 2) istrip0[ixy] = sensor->GetBackChannel(posIn.X(),posIn.Y(),posIn.Z(),dPitch);
	else istrip0[ixy] = sensor->GetBackChannel(posOut.X(),posOut.Y(),posOut.Z(),dPitch);
      }

      Double_t pitch = (ixy == 0) ? sensor->GetDx() : sensor->GetDy();
      dPitch /= pitch;
      if (dPitch < 0) { --istrip0[ixy]; dPitch += 1.0; } // new
      //if (stationNr % 2 == 0) { istrip0[ixy]-=1; dPitch = 1 - dPitch; }
      if (stationNr % 2 == 0) { dPitch = 1 - dPitch; }
      //AZ if (stationNr % 2 == 0) { dPitch = 1 - TMath::Abs(dPitch); }
      //AZ if (stationNr % 2 == 0) { dPitch = - dPitch; }
      if (stationNr % 2 == 0) ++istrip0[ixy]; // AZ - how to explain this???

      // Weighted average of 2 clusters
      Double_t qtot12 = qTot0 * w + qTot1 * w1;
      Double_t dpitch12 = dpitch0 * w + dpitch1 * w1;
      //if (stationNr % 2 == 0) { dpitch12 = 1 - dpitch12; } // AZ
      Int_t ntot = i1 - i0 + 1;
      Double_t *q12array = new Double_t[ntot];
      Double_t q12sum0 = 0, q12sum1 = 0;
      
      for (Int_t j = i0; j <= i1; ++j) {
	if (charges[0].find(j) == charges[0].end()) charges[0][j] = 0;
	if (charges[1].find(j) == charges[1].end()) charges[1][j] = 0;
	//Double_t q12 = (charges[0][j] * w + charges[1][j] * w1) / (w + w1) * fQtot[ixy];
	q12array[j-i0] = (charges[0][j] * w + charges[1][j] * w1) * qtot12; // w + w1 = 1
	q12sum0 += q12array[j-i0];
      }
      Double_t shift = dPitch - dpitch12, q12 = 0;
      //if (stationNr % 2 == 0) shift = -shift; //AZ
      map<Int_t,Double_t> buffer;

      for (Int_t j = i0; j <= i1; ++j) {
	if (stationNr % 2) q12 = Interp (q12array, j-i0, ntot, -shift);
	//else q12 = Interp (q12array, j-i0, ntot, -shift);
	else q12 = Interp (q12array, j-i0, ntot, -shift);

	//Int_t dj = (ixy == 0) ? j - 241 : j - 256;
	//Int_t dj = (ixy == 0) ? j - 240 : j - 254;
	Int_t dj = j - fStrRef[ixy][indXY];
	buffer[dj] = q12;
	q12sum1 += q12;
	//cognew[2] += q12 * j;
	//qqq[2] += q12;
      }
      Double_t scale = q12sum0 / q12sum1;
      scale *= coeff;
      
      for (map<Int_t,Double_t>::iterator it = buffer.begin(); it != buffer.end(); ++it) {
	Int_t jj = it->first;
	if (stationNr % 2 > 0) jj += istrip0[ixy];
	else jj = istrip0[ixy] - it->first + 0; // even stations (rotated)

	if (ixy == 0) {
	  if (jj < 0 || jj >= sensor->GetNChannelsFront()) continue;
	  stripSignalF[jj] += it->second * scale;
	  chanPointMapF[jj].insert(iPoint);
	} else {
	  if (jj < 0 || jj >= sensor->GetNChannelsBack()) continue;
	  stripSignalB[jj] += it->second * scale;
	  chanPointMapB[jj].insert(iPoint);
	}
      }
      delete [] q12array;
      //cognew[2] /= qqq[2];
      //h1[ixy][3]->Fill(cognew[2]);
      //cogGen[ixy] = cognew[2];
    } // for (Int_t ixy = 0; 
  } // for (it1 = pSet.begin(); 
}

// -------------------------------------------------------------------------

Double_t BmnGemFastDigitize::Interp(Double_t *yy, Int_t indx, Int_t ntot, Double_t dx)
{
  static Double_t y0 = 0, y1 = 0;
  static Int_t indx0 = 0;

  if (indx == 0) {
    if (dx >= 0) {
      indx0 = 1;
      y0 = yy[0];
    } else {
      indx0 = 0;
      y0 = 0.0;
    }
    y1 = yy[indx0];
  } else {
    y0 = y1;
    if (indx0 >= ntot) y1 = 0.0;
    else y1 = yy[indx0];
  }

  ++indx0;
  if (dx < 0) dx += 1;
  //cout << indx << " aaa " << y0 + (y1 - y0) / (x1 - x0) * (x - x0) << " " << yy[indx] << " " << indx0 << endl;
  //cout << " Interp: " << indx0 << " " << dx << " " << y0 << " " << y0 + (y1 - y0) * dx << endl;
  //AZ return y0 + (y1 - y0) * dx;
  return TMath::Max (y0 + (y1 - y0) * dx, 0.0);
}

// -------------------------------------------------------------------------

Int_t BmnGemFastDigitize::FindDataSet(Double_t theX, Double_t theY)
{
  // Find appropriate data set according to theX, theY
  // (contribution from R.Zinchenko)

  static Int_t first = 1;

  if (first) {
    first = 0;
    // Find nearest non-zero bin
    Int_t nx = fhInd->GetNbinsX(), ny = fhInd->GetNbinsY();
    vector<Int_t> ia(ny);
    vector<vector<Int_t> > matri(nx,ia);
    vector<Float_t> fa(ny);
    vector<vector<Float_t> > matrf(nx,fa);
    Int_t ix, iy, iz, ix0, iy0, iz0, ixOK, iyOK;
    std::queue<Int_t> que;

    for (Int_t i = 1; i <= nx; ++i) {
      for (Int_t j = 1; j <= ny; ++j) {
	if (fhInd->GetBinContent(i,j)) {
	  matri[i-1][j-1] = fhInd->GetBin(i,j);
	  que.push(fhInd->GetBin(i,j));
	}
      }
    }

    while (que.size()) {
      Int_t bin0 = que.front(); // current cell
      fhInd->GetBinXYZ(bin0,ix0,iy0,iz0);
      --ix0;
      --iy0;
      Int_t binOK = matri[ix0][iy0]; // active cell
      fhInd->GetBinXYZ(binOK,ixOK,iyOK,iz0);
      --ixOK;
      --iyOK;
      que.pop();
      
      // Loop over cell neighbours
      for (Int_t i = -1; i < 2; ++i) {
	ix = ix0 + i;
	if (ix < 0 || ix > nx-1) continue;
	
	for (Int_t j = -1; j < 2; ++j) {
	  iy = iy0 + j;
	  if (iy < 0 || iy > ny-1) continue;
	  if (i == 0 && j == 0) continue;
	  Float_t r20 = matrf[ix][iy];
	  if (matri[ix][iy] && r20 < 0.1) continue; // active cell
	  Float_t r2 = (ix-ixOK)*(ix-ixOK) + (iy-iyOK)*(iy-iyOK);
	  if (r20 < 0.1 || r2 < r20) {
	    matrf[ix][iy] = r2;
	    matri[ix][iy] = matri[ix0][iy0];
	    if (r20 < 0.1) que.push(fhInd->GetBin(ix+1,iy+1));
	  }
	}
      }
    }

    for (Int_t i = 1; i <= nx; ++i) {
      for (Int_t j = 1; j <= ny; ++j) {
	fhInd->SetBinContent(i,j,matri[i-1][j-1]);
      }
    }
  } // if (first)

  Int_t ix = fhInd->GetXaxis()->FindBin(theX);
  if (ix < 1) ix = 1;
  else if (ix > fhInd->GetNbinsX()) ix = fhInd->GetNbinsX();
  Int_t iy = fhInd->GetYaxis()->FindBin(theY);
  if (iy < 1) iy = 1;
  else if (iy > fhInd->GetNbinsY()) iy = fhInd->GetNbinsY();

  return fhInd->GetBinContent(ix, iy);

}

// -------------------------------------------------------------------------

Double_t BmnGemFastDigitize::GetNumberOfClusters(Double_t beta, Double_t gamma, Double_t charge, Double_t p0, Double_t p1)
{
  Double_t beta2 = beta * beta;
  Double_t gamma2 = gamma * gamma;
  Double_t val = p0/beta2*(p1 + TMath::Log(beta2*gamma2) - beta2);
  return TMath::Min(val*charge*charge,20000.);
}

// ------------------------------------------------------------------------- 
ClassImp(BmnGemFastDigitize)
