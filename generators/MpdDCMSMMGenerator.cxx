// -------------------------------------------------------------------------
// -----                MpdDCMSMMGenerator source file                 -----
// -----                Created 27-AUG-2019  by Igor Rufanov           -----
// -------------------------------------------------------------------------
#include "MpdDCMSMMGenerator.h"

#include "FairPrimaryGenerator.h"
#include "FairMCEventHeader.h"

#include "TRandom.h"

#include "TDatabasePDG.h"
#include "TParticlePDG.h"

#include "FairIon.h"
#include "FairParticle.h"
#include "FairRunSim.h"

using namespace std;
using namespace TMath;

// -----   Default constructor   ------------------------------------------
MpdDCMSMMGenerator::MpdDCMSMMGenerator()
: FairGenerator(),
  fInputFile(NULL),
  fFileName(NULL),
  fInputFileVersion(1)
{}
// ------------------------------------------------------------------------

// -----   Standard constructor   -----------------------------------------
MpdDCMSMMGenerator::MpdDCMSMMGenerator(const char* fileName)
: FairGenerator(),
  fInputFile(NULL),
  fFileName(fileName),
  fInputFileVersion(1),
  fSpectatorsON(kTRUE),
  fBoostType("FixedFixedInverted")
  //fBoostType("None")
{
    cout << "-I MpdDCMSMMGenerator: Opening input file " << fFileName << endl;
#ifdef GZIP_SUPPORT
    fInputFile = gzopen(fFileName, "rb");
#else
    fInputFile = fopen(fFileName, "r");
#endif
    if (!fInputFile) {
        Fatal("MpdDCMSMMGenerator", "Cannot open input file.");
        exit(1);
    }
    // read file header
    char read[200];
    Int_t A1,Z1,A2,Z2;
    Double_t T0,sqS;
    Int_t header_size = 1; // old and current formats have different header size
    
    for (Int_t i = 0; i < header_size; i++){
#ifdef GZIP_SUPPORT
        char* ch = gzgets(fInputFile, read, 200);
#else
        char* ch = fgets(read, 200, fInputFile);
#endif
        cout<<"-I MpdDCMSMMGenerator:"<<read;
        if (i == 0){
            string str0(read);
	        
            if (str0.find("Results of DCM-SMM") != string::npos) {    //new version
                fInputFileVersion = 1;
                header_size = 20;
	        }
            else if (str0.find("Results of QGSM") != string::npos) {  // old version
                fInputFileVersion = 0;
                header_size = 3;
	        }
	        else {
		        Fatal("MpdDCMSMMGenerator", "Wrong input file format.");
                exit(1);
	        }
	
            A1 = stoi( str0.substr( str0.find("A1=")+3, 3));
            Z1 = stoi( str0.substr( str0.find("Z1=")+3, 3));
            A2 = stoi( str0.substr( str0.find("A2=")+3, 3));
            Z2 = stoi( str0.substr( str0.find("Z2=")+3, 3));
            //Int_t A1= 0;
        } 
        else if (i == 1) {
	        string str0(read); // hypcoa-b1.f line 711: ss=sqrt(1.88*(T0+1.88)) => mp=0.940
            T0 = stof( str0.substr( str0.find("T0=")+3, 8));
            sqS = stof( str0.substr( str0.find("sqrt(s)=")+8, 8));

            Double_t mp = 0.940; // to obtain equivalence of read "sqrt(s)=" and calculated below mCMS
            Double_t e = T0+mp;
            Double_t p = pow( e*e-mp*mp, 0.5);
            Double_t mCMS = pow( 2.*mp*(e+mp), 0.5); cout<<"mCMS="<<mCMS<<endl;

	        if (fInputFileVersion == 0) {
		        fUseLorentzBoost = kTRUE;
                if (fBoostType == "None")
                    fUseLorentzBoost = kFALSE;
                else if (fBoostType == "CmsFixed") { // from CMS to target (2-nd particle) rest system
                    fBoostGamma = (e+mp)/mCMS;
                    fBoostBeta = pow(1.-1./(fBoostGamma*fBoostGamma), 0.5);
		        }
                else if (fBoostType == "CmsFixedInverted") { // from CMS to beam ( 1-st particle) rest system
                    fBoostGamma = -(e+mp)/mCMS;
                    fBoostBeta = -pow(1.-1./(fBoostGamma*fBoostGamma), 0.5);
		        }
                else if (fBoostType == "FixedFixedInverted") { // from target rest system to beam RS
                    fBoostGamma = -e/mp;
                    fBoostBeta = -p/e;
		        }
	        }
        }
    }
    cout<<"-I MpdDCMSMMGenerator: A1="<<A1<<" Z1="<<Z1<<" A2="<<A2<<" Z2="<<Z2<<" T0="<<T0<<" sqS="<<sqS<<endl;

    if (fSpectatorsON) Int_t n = RegisterIons();
}
// ------------------------------------------------------------------------


// -----   Destructor   ---------------------------------------------------
MpdDCMSMMGenerator::~MpdDCMSMMGenerator() {
    if (fInputFile) {
#ifdef GZIP_SUPPORT
        gzclose(fInputFile);
#else
        fclose(fInputFile);
#endif
        fInputFile = NULL;
    }
    cout<<"Leave Destructor of MpdDCMSMMGenerator"<<endl;
}
// ------------------------------------------------------------------------


// -----   Public method ReadEvent   --------------------------------------
Bool_t MpdDCMSMMGenerator::ReadEvent(FairPrimaryGenerator* primGen) {
    // ---> Check for input file
    // cout<<"MpdDCMSMMGenerator::ReadEvent -------------------------"<<endl;
    if (!fInputFile) {
        cout << "-E MpdDCMSMMGenerator: Input file not open! " << endl;
        return kFALSE;
    }

    // ---> Check for primary generator
    if (!primGen) {
        cout << "-E- MpdDCMSMMGenerator::ReadEvent: "
                << "No PrimaryGenerator!" << endl;
        return kFALSE;
    }
    
    char read[128]; // pnaleks: It was 80, but the line length of 98 chars has been seen
#ifdef GZIP_SUPPORT
    char* ch = gzgets(fInputFile, read, 128);
#else
    char* ch = fgets(read, 128, fInputFile);
#endif
    Int_t evnr = 0; Float_t b, bimpX, bimpY;
    Int_t npp = 0; // pnaleks: number of produced particles after cascade and light clusters after coalescence stages (for fInputFileVersion == 1)
    
    if (fInputFileVersion == 0) //old version
        sscanf(read, "%d %f %f %f", &evnr, &b, &bimpX, &bimpY);
    else                        //new version
        sscanf(read, "%d %d %f %f %f", &evnr, &npp, &b, &bimpX, &bimpY);

#ifdef GZIP_SUPPORT
    if ( gzeof(fInputFile) ) {
#else
    if ( feof(fInputFile) ) {
#endif
        cout << "-I MpdDCMSMMGenerator : End of input file reached." << endl;
        const Bool_t ZeroSizeEvents = kFALSE;
        if (ZeroSizeEvents)
            return kTRUE; // gives zero multiplicity events after EOF
        else { // gives geant crash after EOF and one empty event in .root file
#ifdef GZIP_SUPPORT
	        gzclose(fInputFile);
#else
	        fclose(fInputFile);
#endif
	        fInputFile = NULL;
	        return kFALSE;
        }
    }
    
    Float_t phi = atan2( bimpY, bimpX);

    // Set event id and impact parameter in MCEvent if not yet done
    FairMCEventHeader* event = primGen->GetEvent();
    if (event && (!event->IsSet())) {
        event->SetEventID(evnr);
        event->SetB(b);
        event->MarkSet(kTRUE);
        event->SetRotZ(phi);
    }
    
    TDatabasePDG* dbPDG = TDatabasePDG::Instance();

    Float_t px,py,pz;
    for (Int_t ibeam = 0; ibeam < 3; ibeam++) { // spectators pz+, spectators pz-, participants.
        Int_t np = 0;
#ifdef GZIP_SUPPORT
        ch = gzgets(fInputFile, read, 128);
#else
        ch = fgets(read, 128, fInputFile);
#endif
        sscanf(read, "%d", &np); //cout<<np<<" "<<endl;
        // Information for fInputFileVersion == 1:
        // pnaleks: In this line is also an additional information for ibeam < 2, 
        // which is ignored because next lines contains this datta by fragments
      
        for (Int_t i = 0; i < np; i++) {
#ifdef GZIP_SUPPORT
            ch = gzgets(fInputFile, read, 128);
#else
            ch = fgets(read, 128, fInputFile);
#endif	
            Int_t iN, iQ, iS = 0;
            Float_t xxx = 0., mass;
            Int_t pdgID_in = 0, iL;
            Float_t pLabZ, pALabZ = 0;
	        if (fInputFileVersion == 0) {
                if (ibeam < 2) sscanf(read, "%d %d %f %f %f %f", &iN, &iQ, &xxx, &px,&py,&pz);
                else sscanf(read, "%d %d %d %f %f %f %f", &iN, &iQ, &iS, &px,&py,&pz, &mass);
	        }
	        else { // fInputFileVersion == 1
		        Float_t var10, var11;
                Int_t res = sscanf(read, "%d%d%d%d%d%f%f%f%f%f%f", &iQ, &iL, &iS, &iN, &pdgID_in, &px, &py, &pz, &pLabZ, &var10, &var11);
                switch (res) {
                    case 10:
                        mass = var10;
                        break;
                    case 11:
                        pALabZ = var10;
                        mass = var11;
                        break;
                    default:
                        Fatal(__func__, ": data format mismatch, event %d\n", evnr);
                }
	        }

	        // Int_t pdgID=0;
	        Int_t pdgID = pdgID_in;
	        if (fInputFileVersion == 0) {
                if (ibeam == 2) { // participants
                    Double_t massFactor = 1.;
                    pdgID = FindPDGCodeParticipant(iN, iS, iQ, mass, massFactor);
                    if (massFactor != 1.) { px*=massFactor; py*=massFactor; pz*=massFactor; }
                    //if (pdgID > 1000030000) cout<<pdgID<<" "<<iN<<" "<<iS<<" "<<iQ<<" "<<mass<<endl;
                }
                else { // spectators
                    if (fSpectatorsON) {
                        Int_t dN = -999; // difference of number of nucleons iN-NTab between DCM-DCMSMM and registered ions
                        pdgID = FindPDGCodeSpectator(iN, iQ, dN);
                    }
		        }
	        }

	        TParticlePDG* particle = pdgID ? dbPDG->GetParticle(pdgID) : 0;
            if (particle) {
		        if (fInputFileVersion == 0) { 
                    if (fUseLorentzBoost) {
			            // TParticlePDG* particle= dbPDG->GetParticle(pdgID);
                        Double_t m = particle->Mass();
                        Double_t e = pow( px*px+ py*py+ pz*pz+ m*m, 0.5 );
			            Double_t pzF = fBoostGamma * ( pz + fBoostBeta * e);
                        Double_t eF = pow( px*px+ py*py+ pzF*pzF+ m*m, 0.5 );
			            //cout<<fBoostGamma<<" "<<fBoostBeta<<" "<<pdgID<<" "<<m<<" "<<pz<<" "<<pzF<<" "<<eF-m<<endl;
                        pz = pzF;
		          }
	            }
                else // fInputFileVersion == 1
	          	    pz = pLabZ;
	            
	            // Int_t Geant3ID= dbPDG->ConvertPdgToGeant3(pdgID);
                if (fabs(pz)>50.) cout<<"pz="<<pz<<" N="<<iN<<" Q="<<iQ<<" pdg="<<pdgID<<"\n";
                primGen->AddTrack(pdgID, px, py, pz, 0., 0., 0.);
	        }
	        else {
	            // if( ibeam==2 || (ibeam==2&&fSpectatorsON))
	            if (fInputFileVersion == 1 || ibeam==2 || fSpectatorsON) {
	                // cout<<"-I MpdDCMSMMGenerator : unknown particle N="<<iN<<" Q="<<iQ<<endl;
	                cout<<"-I MpdDCMSMMGenerator : unknown particle N="<<iN<<" Q="<<iQ;
	                if (fInputFileVersion == 1) cout << " PDG_in="<< pdgID_in << " ibeam=" << ibeam << endl;
	                else cout << endl;
	            }
            }
        }
    }
    return kTRUE;
}   
// ------------------------------------------------------------------------
    
Bool_t MpdDCMSMMGenerator::SkipEvents(Int_t count) {
  if (count <= 0) return kTRUE;
  for (Int_t ii = 0; ii < count; ii++) {
    // ---> Check for input file
    if (!fInputFile) {
      cout << "-E MpdDCMSMMGenerator: Input file not open! " << endl;
      return kFALSE;
    }
    char read[128];
#ifdef GZIP_SUPPORT
    char* ch = gzgets(fInputFile, read, 128);
#else
    char* ch = fgets(read, 128, fInputFile);
#endif
    // Int_t evnr=0; Float_t b, bimpX, bimpY;
    // sscanf(read, "%d %f %f %f", &evnr, &b, &bimpX, &bimpY);
    for (Int_t ibeam = 0; ibeam < 3; ibeam++) {
      Int_t np = 0;
#ifdef GZIP_SUPPORT
      ch = gzgets(fInputFile, read, 128);
#else
      ch = fgets(read, 128, fInputFile);
#endif
      sscanf(read, "%d", &np);
      for (Int_t i = 0; i < np; i++) {
#ifdef GZIP_SUPPORT
        ch = gzgets(fInputFile, read, 128);
#else
        ch = fgets(read, 128, fInputFile);
#endif
        // Int_t iN, iQ, iS=0;
        // Float_t xxx=0., mass, px,py,pz;
        // if( ibeam < 2) sscanf(read, "%d %d %f %f %f %f", &iN, &iQ, &xxx, &px,&py,&pz); //cout<<np<<" "<<endl;
        // else sscanf(read, "%d %d %d %f %f %f %f", &iN, &iQ, &iS, &px,&py,&pz, &mass);
      }
    }
  }

  return kTRUE;
}
// ------------------------------------------------------------------------

 
Int_t MpdDCMSMMGenerator::FindPDGCodeParticipant( Int_t A, Int_t S, Int_t Z, Float_t mass, Double_t &massFactor) {
  Int_t k=0;
  Int_t aA= abs(A), aS= abs(S), aZ= abs(Z);

  if( aA == 0) { // mesons and gamma =====================================
    if( S == 0) { // not strange
      if( aZ == 0) { // neutral
	const Int_t n000=7; // gamma, pi0,    eta,   rho, omega, etaPri,  phi
	Float_t M000M[n000]={ 0.0,0.13497,0.54745,0.7690,0.78265,0.95778,1.019455};
	Float_t M000L[n000]={ 0.0,0.13400,0.54700,0.7680,0.78200,0.95700,1.019390};
	Float_t M000U[n000]={ 0.0,0.13510,0.54810,0.7711,0.78310,0.95810,1.019610};
	Int_t   C000[n000]= {  22,    111,    211,   113,    223,    331,     333};
	for( Int_t i=0; i<n000; i++) {
	  if( mass < M000L[i] || mass > M000U[i]) continue;
	  k= C000[i];
	  break;
	}
	
	if( k==0 && mass>0.4979 && mass<0.4981 ) k=22; // rare particle in pairs in begining of list
	// invariant mass of these pairs (considering them as gamma) is close to mass of pion
      }
      else { // non-stranges charged mesons
	if( mass > 0.1389 && mass < 0.1401) k=211*Z; // pi+/-
	else if( mass > 0.134 && mass < 0.136) k=211*Z; // also pi+/-
	else if( mass == 0.776) k=213*Z; // rho+/-
	else if( mass > 0.0004 && mass < 0.0011) k=-11*Z; // e+/e-
	else if( mass > 0.1055 && mass < 0.1065) k=-13*Z; // mu+/mu-
      }
    }
    else { // A==0 && S!=0 strange mesons
      if( aZ == 0) { // neutral K0, K*0
	if( mass > 0.491 && mass < 0.4981) { // K0 S=1 311, AK0 S=-1 -311
	  Double_t KSL= gRandom->Uniform(0.,1.);
	  if( KSL<0.5) k= 130; // KL
	  else k= 310; // KS
	} else if( mass == 0.8922) k=313*S; // K*0
      }
      if( aZ == 1) {
	if( mass > 0.491 && mass < 0.4951) k=321*Z; // K+
	else if( mass > 0.8815 && mass > 0.8895 ) k=323*Z; // K*+ (not occured)
      }
    }
  }
  else if ( aA==1 ) { // barions ============================================
    if( S == 0) { // Light baryons: n,p,Delta
      if( Z==0) {
	if( ( mass > 0.9389 && mass < 0.940) || mass < -1. ) k=2112*A; // n
	else if( mass == 1.232) k=2114*A; // Delta0 (not observed)
      }
      else if( aZ==1) {
	if( ( mass > 0.9379 && mass < 0.9401) || mass < -1. ) k=2212*A; // p
	else if( mass == 1.232) {
	  if( A*Z == 1) k=2214*A; // Delta+ (not observed)
	  else if( A*Z == -1) k=1114*A; // Delta- (not observed)
	}
      }
      else if( aZ==2) {
	if( mass == 1.232) k=2224*A; // Delta++ (not observed)
      }
    }
    else if( aS==1) { // Lambda, Sigma, Sigma*
      if( Z == 0) {
	if( mass > 1.1149 && mass < 1.1161) k= 3122*A; // Lambda
	else if( mass > 1.189 && mass < 1.1921 ) k= 3212*A; // Sigma0
	else if( mass == 1.382 ) k= 3214*A; // Sigma*0  (not observed)
      }
      else if( aZ == 1) { // Sigma-, Sigma+, Sigma*-, Sigma*+
	if( mass > 1.1889 && mass < 1.1921) k= 3222*A; // Sigma+
	else if( mass > 1.196 && mass < 1.1971) k= 3112*A; // Sigma-
	else if( mass == 1.3823) k= 3224*A; // Sigma*+ (not observed)
	else if( mass == 1.3875) k= 3114*A; // Sigma*- (not observed)
      }
    }
    else if( aS==2) { // Xi, Xi*
      if( Z == 0) { // Xi0, Xi*0
	if( mass > 1.3139 && mass < 1.3161 ) k= 3322*A; // Xi0
	else if( mass == 1.5318) k= 3324*A; // Xi*0
      }
      else if( aZ == 1) { // Xi-, Xi*-
	if( mass > 1.3139 && mass < 1.3221 ) k= 3312*A; // Xi-
	else if( mass == 1.535) k= 3314*A; // Xi*-
      }
    }
    else if( aS==3) { // Omega-
      if( aZ == 1) { 
	if( mass > 1.6 && mass < 1.801 ) k= 3334*A; // Omega- 1.67245
      }
    }
  }
  else if ( A==2 ) { // d is only valid ion (particle). All over are unstable. ==
    
    if( S==0) {
      if( Z==1 && ( mass<-1 || ( mass>1.875 && mass<1.877))) k=1000010020; // d
    }
    else if( S==-1) { // unstable strange pairs => take the most heavy nucleon
      if( Z==-1) { k= 3112; massFactor=1.197436/mass;} // Sigma- n 2.13699 GeV (not occured)
      else if( Z==0 && mass>2.0545 &&mass<2.0555 ) { k= 3122; massFactor=1.11568/mass;} // Lambda n
      else if( Z==1 && mass>2.0535 &&mass<2.0545 ) { k= 3122; massFactor=1.11568/mass;} // Lambda p
      else if( Z==2 ) { k= 3222; massFactor=1.18937/mass;} // Sigma+ p 2.12764 GeV (not occured)
    } 
    else if( S==-2) {
      if( Z==-1 && mass>2.2545 && mass<2.2615) { k= 3312; massFactor=1.32132/mass;} // Xi- n M=2.26088 ( also ghost at 2.255 GeV)
      else if( Z==0) {
	if( mass>2.2305 && mass<2.2315) { k= 3122; massFactor=1.11568/mass;} // Lambda Lambda M=2.23136
	else if( mass>2.2525 && mass<2.2555) { k= 3322; massFactor=1.19255/mass;} // Xi0 n M=2.25446
	else if( mass>2.2595 && mass<2.2605) { k= 3312; massFactor=1.197436/mass;} // Xi- p M=2.25959
      }
      else if( Z==1 && mass>2.2525 && mass<2.2535) { k= 3322; massFactor=1.3149/mass;} // Xi0 p M=2.25317 (132)
    }
    else if( S==-3) {
      if( Z==-1 && mass>2.4305 && mass<2.4375) { k= 3312; massFactor=1.197436/mass;} // Xi- Lambda M=2.43700 ( also ghost at 2.431 GeV) (37)
      else if( Z==0 && mass>2.4301 && mass<2.4315) { k=3322; massFactor=1.3149/mass;} // Xi0 Lambda M=2.43058 (36)
    }
    else if( S==-4 && Z==-1 && mass>2.6295 && mass<2.6365) { k=3312; massFactor=1.32132/mass;} // Xi- Xi0 M=2.63622 (1)
  }
  else if( A==3) {
    //
    if( S==0) {
      if( Z==1 && mass>2.8085 && mass<2.8095)  k=1000010030; // t
      else if( Z==2 && mass>2.8085 && mass<2.8095)  k=1000020030; // He3
    }
    else if( S==-1) {
      if( Z==1 && mass>2.9915 && mass<2.9925)  k=1010010030; // H3L
    }
  }
  else if( A==4) {
    if( S==0) {
      if( Z==2 && mass>3.7275 && mass<3.7285) k=1000020040; // Alpha
      //else if( Z==1 && mass>3.7275 && mass<3.7285) k=1000020040; // H4
    }
    else if( S==-1) {
      if( Z==1 && mass>3.9245 && mass<3.9255) k=1010010040; // H4L
      if( Z==2 && mass>3.9245 && mass<3.9255) k=1010020040; // He4L
    }
    else if( S==-2) {
      //if( Z==1 && mass>4.1065 && mass<4.1075 ) k=1020010040; // H4LL (6 per 100k MPD events)
    }
  }
  else if( A==5) {
    //if( S==-2 && mass>5.0405 && mass<5.0415) k= 1020020050; // He5LL (0 per 100k MPD events)
  }
  
  //if( aA<=2 && k==0) {
  if( mass>-10 && k==0) {
    //if( A==0 && S==0 && Z==0 && fabs( mass)-0.498 < 0.001) {}
    //else
    cout<<"A="<<A<<" S="<<S<<" Z="<<Z<<"  mass= "<<mass<<"   Pdg="<<k<<endl;
  }
  //k= 1000030060;
  return k;
}
// ------------------------------------------------------------------------


Int_t MpdDCMSMMGenerator::FindPDGCodeSpectator( Int_t N, Int_t Z, Int_t &dN) {
  Int_t k=0;
  dN=0;
  if( Z==0 && N==1) k=2112; // n
  else if( Z==1 && N==1 ) k=2212; // p
  //else if( Z>=1 && Z<=fZMax ) { // find registeried ion with a same Z
  //  Int_t NTab;
  //  if( N<fN1[Z]) { NTab=fN1[Z]; dN=N-fN1[Z];}
  //  else if( N<=fN2[Z]) { NTab=N; dN=0;} // same B
  //  else { NTab=fN2[Z]; dN=N-fN2[Z];}
  //  k=1000000000+ Z*10000+ NTab*10;
  //}
  else if( N>=2 && N<=fBMax ) { // find registeried ion with a same baryon number
    Int_t ZTab;
    if( Z<fZ1[N]) { ZTab=fZ1[N]; dN=Z-fZ1[N];}
    else if( Z<=fZ2[N]) { ZTab=Z; dN=0;} // same Z
    else { ZTab=fZ2[N]; dN=Z-fZ2[N];}
    k=1000000000+ ZTab*10000+ N*10;
  }
  return k;
}
// ------------------------------------------------------------------------


Int_t MpdDCMSMMGenerator::RegisterIons( void) {
  // see ./fairsoft/transport/geant3/TGeant3/TGeant3.cxx::AddParticlesToPdgDataBase()
  // or ./fairsoft/transport/geant4_vmc/source/physics/src/TG4ParticlesManager::DefineParticles()
  // Deuteron, Triton, Alpha, HE3, HyperTriton and their ANTI-particle are added there
  // also Special particles: "Cherenkov", "FeedbackPhoton".
  // He4L, H4L are added in mpdroot/gconfig/UserDecay.C

  struct gpions { Int_t N,Z; Char_t Name[6]; Double_t Mass; };
  const Int_t NSI=236;
  const struct gpions ions[NSI] = {
    { 2, 1, "d", 0.0}, { 3, 1, "t", 0.0}, { 3, 2, "He3", 0.0},
    { 4, 2, "He4", 0.0}, { 5, 2, "He5q", 4.6688534}, { 6, 2, "He6", 5.6065596},
    { 7, 2, "He7", 6.5465601}, { 6, 3, "Li6", 5.60305},    { 7, 3, "Li7", 6.53536},
    { 8, 3, "Li8", 7.4728996}, { 9, 3, "Li9", 8.40840118}, { 7, 4, "Be7", 6.53622},
    { 8, 4, "Be8q", 7.4568945}, { 9, 4, "Be9", 8.39479}, { 10, 4, "Be10", 9.32754},
    { 11, 4, "Be11", 10.2666}, { 12, 4, "Be12", 11.203006}, { 8, 5, "B8", 7.4748744},
    {  9, 5,  "B9", 8.3958634}, { 10, 5, "B10", 9.32699}, { 11, 5, "B11", 10.25510},
    { 12, 5, "B12", 11.1913}, { 9, 6, "C9", 8.4123574}, { 10, 6, "C10", 9.3306397},
    { 11, 6, "C11", 10.257085}, { 12, 6, "C12", 11.17793}, { 13, 6, "C13", 12.112548},
    { 14, 6, "C14", 13.043937}, { 15, 6, "C15", 13.98228}, { 16, 6, "C16", 14.917600},
    { 11, 7, "N11", 10.257085},
    { 12, 7, "N12", 11.195267}, { 13, 7, "N13", 12.11477}, { 14, 7, "N14", 13.04378},
    { 15, 7, "N15", 13.972513}, { 16, 7, "N16", 14.90959}, { 17, 7, "N17", 15.843271},
    // N11, o11-o13 are only unstable isotopes in this list (add for SRC analysis)
    { 11, 8, "O11", 10.257085}, { 12, 8, "O12", 11.195267}, { 13, 8, "O13", 12.132536},
    { 14, 8, "O14", 13.048925}, { 15, 8, "O15", 13.97527}, { 16, 8, "O16", 14.89917},
    { 17, 8, "O17", 15.834591}, { 18, 8, "O18", 16.76611}, { 19, 8, "O19", 17.701723},
    { 19, 9, "F19", 17.69690},  { 20, 10, "Ne20", 18.62284}, { 21, 10, "Ne21", 19.555644},
    { 22, 10, "Ne22", 20.484846}, { 23, 11, "Na23", 21.41483 }, { 24, 12, "Mg24", 22.34193},
    { 25, 12, "Mg25", 23.274160}, { 26, 12, "Mg26", 24.202632}, { 27, 13, "Al27", 25.13314},
    { 28, 14, "Si28", 26.06034 }, { 29, 14, "Si29", 26.991434}, { 30, 14, "Si30", 27.920390}, 
    { 31, 15,  "P31", 28.85188 }, { 32, 16,  "S32", 29.78180},  { 33, 16,  "S33", 30.712719},
    { 34, 16,  "S34", 31.640868}, { 35, 17, "Cl35", 32.57328},
    { 36, 18, "Ar36", 33.50356},  { 37, 18, "Ar37", 34.434334}, { 38, 18, "Ar38", 35.362061}, 
    { 39, 19,  "K39", 36.29447},  { 40, 20, "Ca40", 37.22492},  { 41, 20, "Ca41", 38.156120},
    { 42, 20, "Ca42", 39.084205}, { 43, 20, "Ca43", 40.015838}, { 44, 20, "Ca44", 40.944272},
    { 45, 21, "Sc45", 41.87617},  { 46, 22, "Ti46", 42.804605}, { 47, 22, "Ti47", 43.735290},
    /*
    { 48, 22, "Ti48", 44.66324},  { 49, 22, "Ti49", 45.594652}, { 50, 22, "Ti50", 46.523278}, 
    { 51, 23,  "V51", 47.45401},  { 52, 24, "Cr52", 48.38228},  { 53, 24, "Cr53", 49.313903},
    { 54, 24, "Cr54", 50.243749}, { 55, 25, "Mn55", 51.17447},  { 56, 26, "Fe56", 52.10307},
    { 57, 26, "Fe57", 53.034984}, { 58, 26, "Fe58", 53.964505}, 
    { 59, 27, "Co59", 54.89593},  { 60, 28, "Ni60", 55.825174}, { 61, 28, "Ni61", 56.756919}, 
    { 62, 28, "Ni62", 57.685888}, { 63, 29, "Cu63", 58.61856},  { 64, 29, "Cu64", 59.550198},
    { 65, 29, "Cu65", 60.479853}, { 66, 30, "Zn66", 61.409711}, { 67, 30, "Zn67", 62.342224},
    { 68, 30, "Zn68", 63.271592}, { 69, 31, "Ga69", 64.203765}, { 70, 31, "Ga70", 65.135677},
    { 71, 31, "Ga71", 66.065941}, { 72, 32, "Ge72", 66.994989}, { 73, 32, "Ge73", 68.857141},
    { 74, 32, "Ge74", 68.85715},  { 75, 33, "As75", 69.789025}, { 76, 34, "Se76", 70.718300},
    { 77, 34, "Se77", 71.650446}, { 78, 34, "Se78", 72.579514}, { 79, 34, "Se79", 73.512116},
    { 80, 34, "Se80", 74.44178},  { 81, 35, "Br81", 75.373047}, { 82, 36, "Kr82", 76.301927},
    { 83, 36, "Kr83", 77.234029}, { 84, 36, "Kr84", 78.16309},  { 85, 37, "Rb85", 79.09483},
    { 86, 38, "Sr86", 80.023969}, { 87, 38, "Sr87", 80.955106}, { 88, 38, "Sr88", 81.88358},
    { 89, 39, "Y89", 82.81527},   { 90, 40, "Zr90", 83.74571},  { 91, 40, "Zr91", 84.678073},
    { 92, 40, "Zr92", 85.609003}, { 93, 41, "Nb93", 86.541743}, { 94, 41, "Nb94", 87.474081},
    { 95, 42, "Mo95", 88.404232}, { 96, 42, "Mo96", 89.334643}, { 97, 42, "Mo97", 90.267388}, 
    { 98, 42, "Mo98", 91.19832},  { 99, 43, "Tc99", 92.13059}, { 100, 44, "Ru100", 93.060191},
    { 101, 44, "Ru101", 93.992955}, { 102, 44, "Ru102", 94.923300}, { 103, 45, "Rh103", 95.855870},
    { 104, 46, "Pd104", 96.785997}, { 105, 46, "Pd105", 97.718468}, { 106, 46, "Pd106", 98.64997},
    { 107, 47, "Ag107", 99.581467}, { 108, 47, "Ag108", 100.51376}, { 109, 47, "Ag109", 101.444134},
    { 110, 48, "Cd110", 102.37400}, { 111, 48, "Cd111", 103.30659}, { 112, 48, "Cd112", 104.23676},
    { 113, 48, "Cd113", 105.16978}, { 114, 48, "Cd114", 106.10997}, { 115, 49, "In115", 107.03228},
    { 116, 49, "In116", 107.96507}, { 117, 49, "In117", 108.89586}, { 118, 50, "Sn118", 109.82465},
    { 119, 50, "Sn119", 110.75773}, { 120, 50, "Sn120", 111.68821},
    { 121, 51, "Sb121", 112.62119}, { 122, 51, "Sb122", 113.55395}, { 123, 51, "Sb123", 114.48455},
    { 124, 52, "Te124", 115.41474}, { 125, 52, "Te125", 116.3477},  { 126, 52, "Te126", 117.27819},
    { 127, 53,  "I127", 118.21077}, { 128, 53,  "I128", 119.14351}, { 129, 53,  "I129", 120.07424 }, 
    { 130, 54, "Xe130", 121.00435}, { 131, 54, "Xe131", 121.93731}, { 132, 54, "Xe132", 122.86796},
    { 133, 55, "Cs133", 123.80064}, { 134, 55, "Cs134", 124.73332}, { 135, 55, "Cs135", 125.66412195},
    { 136, 56, "Ba136", 126.59431}, { 137, 56, "Ba137", 127.52697}, { 138, 56, "Ba138", 128.45793},
    { 139, 57, "La139", 129.39045}, { 140, 58, "Ce140", 130.32111}, { 141, 59, "Pr141", 131.2546475},
    { 142, 60, "Nd142", 132.18621}, { 143, 60, "Nd143", 133.11965}, { 144, 60, "Nd144", 134.05140},
    { 145, 60, "Nd145", 134.9852},  { 146, 60, "Nd146", 135.91721}, { 147, 60, "Nd147", 136.85148},
    { 148, 61, "Pm148", 137.78426}, { 149, 61, "Pm149", 138.71655},
    { 150, 62, "Sm150", 139.64706}, { 151, 62, "Sm151", 140.58103}, { 152, 62, "Sm152", 141.51236},
    { 153, 63, "Eu153", 142.44522}, { 154, 64, "Gd154", 143.37638}, { 155, 64, "Gd155", 144.30951},
    { 156, 64, "Gd156", 145.24054}, { 157, 64, "Gd157", 146.17374}, { 158, 65, "Tb158", 147.10659},
    { 159, 65, "Tb159", 148.03802}, { 160, 65, "Tb160", 148.97121}, { 161, 65, "Tb161", 149.90308}, 
    { 162, 66, "Dy162", 150.83386}, { 163, 66, "Dy163", 151.76715}, { 164, 66, "Dy164", 152.69909},
    { 165, 67, "Ho165", 153.63162}, { 166, 68, "Er166", 154.56309}, { 167, 68, "Er167", 155.49622},
    { 168, 68, "Er168", 156.42801}, { 169, 69, "Tm169", 157.36122}, { 170, 69, "Tm170", 158.29420},
    { 171, 69, "Tm171", 159.22628}, { 172, 70, "Yb172", 160.15773}, { 173, 70, "Yb173", 161.09092},
    { 174, 70, "Yb174", 162.02245}, { 175, 71, "Lu175", 162.95630}, { 176, 72, "Hf176", 163.88838},
    { 177, 72, "Hf177", 164.82157}, { 178, 72, "Hf178", 165.75351}, { 179, 72, "Hf179", 166.68697},
    { 180, 73, "Ta180", 167.62000}, { 181, 73, "Ta181", 168.55199}, { 182, 74,  "W182", 169.48368},
    { 183, 74,  "W183", 170.41705}, { 184, 74,  "W184", 171.34924}, { 185, 75, "Re185", 172.28258},
    { 186, 76, "Os186", 173.21490}, { 187, 76, "Os187", 174.14818}, { 188, 76, "Os188", 175.079754},
    { 189, 76, "Os189", 176.01340}, { 190, 76, "Os190", 176.94517}, { 191, 77, "Ir191", 177.878667}, 
    { 192, 78, "Pt192", 178.81057}, { 193, 78, "Pt193", 179.74388}, { 194, 78, "Pt194", 180.67513},
    { 195, 78, "Pt195", 181.60855}, { 196, 78, "Pt196", 182.54020}, { 197, 79, "Au197", 183.47324},
    { 198, 80, "Hg198", 184.40488}, { 199, 80, "Hg199", 185.33778}, { 200, 80, "Hg200", 186.26932},
    { 201, 80, "Hg201", 187.20265}, { 202, 80, "Hg202", 188.13451}, { 203, 81, "Tl203", 189.06754},
    { 204, 81, "Tl204", 190.00045}, { 205, 81, "Tl205", 190.93247}, { 206, 82, "Pb206", 191.86400},
    { 207, 82, "Pb207", 192.79683}, { 208, 82, "Pb208", 193.72907}};
    */
    { 48, 23,  "V48", 44.66324},  { 49, 23,  "V49", 45.594652}, { 50, 24, "Cr50", 46.523278}, 
    { 51, 24, "Cr51", 47.45401},  { 52, 25, "Mn52", 48.38228},  { 53, 25, "Mn53", 49.313903},
    { 54, 26, "Fe54", 50.243749}, { 55, 26, "Fe55", 51.17447},  { 56, 27, "Co56", 52.10307},
    { 57, 27, "Co57", 53.034984}, { 58, 27, "Co58", 53.964505}, 
    { 59, 28, "Ni59", 54.89593},  { 60, 28, "Ni60", 55.825174}, { 61, 28, "Ni61", 56.756919}, 
    { 62, 29, "Cu62", 57.685888}, { 63, 29, "Cu63", 58.61856},  { 64, 30, "Zn64", 59.550198},
    { 65, 30, "Zn65", 60.479853}, { 66, 31, "Ga66", 61.409711}, { 67, 31, "Ga67", 62.342224},
    { 68, 32, "Ge68", 63.271592}, { 69, 32, "Ge69", 64.203765}, { 70, 33, "As70", 65.135677},
    { 71, 33, "As71", 66.065941}, { 72, 34, "Se72", 66.994989}, { 73, 34, "Se73", 68.857141},
    { 74, 34, "Se74", 68.85715},  { 75, 35, "Br75", 69.789025}, { 76, 35, "Br76", 70.718300},
    { 77, 36, "Kr77", 71.650446}, { 78, 36, "Kr78", 72.579514}, { 79, 36, "Kr79", 73.512116},
    { 80, 37, "Rb80", 74.44178},  { 81, 37, "Rb81", 75.373047}, { 82, 38, "Sr82", 76.301927},
    { 83, 38, "Sr83", 77.234029}, { 84, 39,  "Y84", 78.16309},  { 85, 39,  "Y85", 79.09483},
    { 86, 40, "Zr86", 80.023969}, { 87, 40, "Zr87", 80.955106}, { 88, 40, "Zr88", 81.88358},
    { 89, 41, "Nb89", 82.81527},  { 90, 42, "Mo90", 83.74571},  { 91, 42, "Mo91", 84.678073},
    { 92, 43, "Tc92", 85.609003}, { 93, 43, "Tc93", 86.541743}, { 94, 43, "Tc94", 87.474081},
    { 95, 44, "Ru95", 88.404232}, { 96, 44, "Ru96", 89.334643}, { 97, 45, "Rh97", 90.267388}, 
    { 98, 45, "Rh98", 91.19832},  { 99, 45, "Rh99", 92.13059}, { 100, 46, "Pd100", 93.060191},
    { 101, 46, "Pd101", 93.992955}, { 102, 47, "Ag102", 94.923300}, { 103, 47, "Ag103", 95.855870},
    { 104, 47, "Ag104", 96.785997}, { 105, 48, "Cd105", 97.718468}, { 106, 48, "Cd106", 98.64997},
    { 107, 49, "In107", 99.581467}, { 108, 49, "In108", 100.51376}, { 109, 50, "Sn109", 101.444134},
    { 110, 50, "Sn110", 102.37400}, { 111, 51, "Sb111", 103.30659}, { 112, 51, "Sb112", 104.23676},
    { 113, 52, "Te113", 105.16978}, { 114, 52, "Te114", 106.10997}, { 115, 52, "Te115", 107.03228},
    { 116, 53,  "I116", 107.96507}, { 117, 53,  "I117", 108.89586}, { 118, 54, "Xe118", 109.82465},
    { 119, 54, "Xe119", 110.75773}, { 120, 55, "Cs120", 111.68821},
    { 121, 55, "Cs121", 112.62119}, { 122, 56, "Ba122", 113.55395}, { 123, 56, "Ba123", 114.48455},
    { 124, 56, "Ba124", 115.41474}, { 125, 57, "La125", 116.3477},  { 126, 57, "La126", 117.27819},
    { 127, 58, "Ce127", 118.21077}, { 128, 58, "Ce128", 119.14351}, { 129, 58, "Ce129", 120.07424 }, 
    { 130, 59, "Pr130", 121.00435}, { 131, 59, "Pr131", 121.93731}, { 132, 60, "Nd132", 122.86796},
    { 133, 60, "Nd133", 123.80064}, { 134, 60, "Nd134", 124.73332}, { 135, 61, "Pm135", 125.66412195},
    { 136, 62, "Sm136", 126.59431}, { 137, 62, "Sm137", 127.52697}, { 138, 62, "Sm138", 128.45793},
    { 139, 63, "Eu139", 129.39045}, { 140, 63, "Eu140", 130.32111}, { 141, 64, "Gd141", 131.2546475},
    { 142, 64, "Gd142", 132.18621}, { 143, 64, "Gd143", 133.11965}, { 144, 65, "Tb144", 134.05140},
    { 145, 65, "Tb145", 134.9852},  { 146, 66, "Dy146", 135.91721}, { 147, 66, "Dy147", 136.85148},
    { 148, 66, "Dy148", 137.78426}, { 149, 67, "Ho149", 138.71655},
    { 150, 67, "Ho150", 139.64706}, { 151, 67, "Ho151", 140.58103}, { 152, 68, "Er152", 141.51236},
    { 153, 68, "Er153", 142.44522}, { 154, 69, "Tm154", 143.37638}, { 155, 69, "Tm155", 144.30951},
    { 156, 69, "Tm156", 145.24054}, { 157, 70, "Yb157", 146.17374}, { 158, 70, "Yb158", 147.10659},
    { 159, 71, "Lu159", 148.03802}, { 160, 71, "Lu160", 148.97121}, { 161, 71, "Lu161", 149.90308}, 
    { 162, 72, "Hf162", 150.83386}, { 163, 72, "Hf163", 151.76715}, { 164, 72, "Hf164", 152.69909},
    { 165, 72, "Hf165", 153.63162}, { 166, 73, "Ta166", 154.56309}, { 167, 73, "Ta167", 155.49622},
    { 168, 73, "Ta168", 156.42801}, { 169, 74,  "W169", 157.36122}, { 170, 74,  "W170", 158.29420},
    { 171, 75, "Re171", 159.22628}, { 172, 75, "Re172", 160.15773}, { 173, 75, "Re173", 161.09092},
    { 174, 75, "Re174", 162.02245}, { 175, 75, "Re175", 162.95630}, { 176, 75, "Re176", 163.88838},
    { 177, 76, "Os177", 164.82157}, { 178, 76, "Os178", 165.75351}, { 179, 76, "Os179", 166.68697},
    { 180, 77, "Ir180", 167.62000}, { 181, 77, "Ir181", 168.55199}, { 182, 77, "Ir182", 169.48368},
    { 183, 77, "Ir183", 170.41705}, { 184, 77, "Ir184", 171.34924}, { 185, 78, "Pt185", 172.28258},
    { 186, 78, "Pt186", 173.21490}, { 187, 78, "Pt187", 174.14818}, { 188, 78, "Pt188", 175.079754},
    { 189, 78, "Pt189", 176.01340}, { 190, 78, "Pt190", 176.94517}, { 191, 78, "Pt191", 177.878667}, 
    { 192, 79, "Au192", 178.81057}, { 193, 79, "Au193", 179.74388}, { 194, 79, "Au194", 180.67513},
    { 195, 79, "Au195", 181.60855}, { 196, 79, "Au196", 182.54020}, { 197, 79, "Au197", 183.47324},
    { 198, 80, "Hg198", 184.40488}, { 199, 80, "Hg199", 185.33778}, { 200, 80, "Hg200", 186.26932},
    { 201, 80, "Hg201", 187.20265}, { 202, 80, "Hg202", 188.13451}, { 203, 81, "Tl203", 189.06754},
    { 204, 81, "Tl204", 190.00045}, { 205, 81, "Tl205", 190.93247}, { 206, 82, "Pb206", 191.86400},
    { 207, 82, "Pb207", 192.79683}, { 208, 82, "Pb208", 193.72907}};
  TDatabasePDG* dbPDG = TDatabasePDG::Instance();
  for( Int_t i=0; i<NSI; i++) {
    if( ions[i].Mass < 0.1) continue; // expected that it is already registered
    Int_t ID= 1000000000+ ions[i].Z*10000+ ions[i].N*10;
    if ( dbPDG->GetParticle(ID) ) continue;
    FairIon* ion = new FairIon( ions[i].Name, ions[i].Z, ions[i].N, ions[i].Z) ;
    FairRunSim::Instance()->AddNewIon(ion);
  }
  // Min and max baryon charge for a given Z
  for( Int_t i=0; i<NSI; i++) {
    Int_t z= ions[i].Z; if( z<0 && z>fZMax) continue;
    fN2[z]=ions[i].N;
  }
  for( Int_t i=NSI-1; i>=0; i--) {
    Int_t z= ions[i].Z; if( z<0 && z>fZMax) continue;
    fN1[z]=ions[i].N;
  }

  // Min and max Z for a given baryon charge
  for( Int_t i=0; i<NSI; i++) {
    Int_t B= ions[i].N; if( B<0 && B>fBMax) continue;
    fZ2[B]= ions[i].Z;
  }
  for( Int_t i=NSI-1; i>=0; i--) {
    Int_t B= ions[i].N; if( B<0 && B>fBMax) continue;
    fZ1[B]=ions[i].Z;
  }
  for( int i=2; i<=fBMax; i++) cout <<i<<": "<<fZ1[i]<<"-"<<fZ2[i]<<"  "; cout <<"\n";
  return NSI;
}

ClassImp(MpdDCMSMMGenerator);
