// -------------------------------------------------------------------------
// -----                   CbmStsFindTracks source file                -----
// -----                  Created 02/02/05  by V. Friese               -----
// -------------------------------------------------------------------------
// Includes from STS
#include "CbmStsFindTracks.h"
#include "CbmL1StsTrackFinder.h"
#include "CbmStsHit.h"
#include "CbmStsTrack.h"
#include "CbmStsTrackFinderIdeal.h"

// Includes from base
#include "FairField.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// Includes from ROOT
#include "TClonesArray.h"

// Includes from C++
#include <iostream>
#include <iomanip>

using namespace std;


// -----   Default constructor   -------------------------------------------
CbmStsFindTracks::CbmStsFindTracks() 
: FairTask("STSFindTracks", 1),
  fUseMvd(kFALSE),
  fGeoPar(nullptr),
  fDigiPar(nullptr),
  //AZ fDigiScheme(new CbmStsDigiScheme()),
  fDigiScheme(CbmStsDigiScheme::Instance()),
  fField(nullptr),
  fFinder(nullptr),
  fMvdHits(nullptr),
  fStsHits(nullptr),
  fTracks(nullptr),
  fTimer(),
  fNEvents(0),
  fNEventsFailed(0),
  fTime(0.),
  fNTracks(0.),
  finderName("CbmStsTrackFinderIdeal")
{}

// -----   Standard constructor   ------------------------------------------
CbmStsFindTracks::CbmStsFindTracks(Int_t iVerbose,
                                   const TString& sFinderName,
                                   Bool_t useMvd, const char* name)
: FairTask(name, iVerbose),
  fUseMvd(useMvd),
  fGeoPar(nullptr),
  fDigiPar(nullptr),
  //AZ fDigiScheme(new CbmStsDigiScheme()),
  fDigiScheme(CbmStsDigiScheme::Instance()),
  fField(nullptr),
  fFinder(nullptr),
  fMvdHits(nullptr),
  fStsHits(nullptr),
  fTracks(nullptr),
  fTimer(),
  fNEvents(0),
  fNEventsFailed(0),
  fTime(0.),
  fNTracks(0.),
  finderName(sFinderName)
{}

// -----   Destructor   ----------------------------------------------------
CbmStsFindTracks::~CbmStsFindTracks() {
    //AZ if ( fDigiScheme ) delete fDigiScheme;
    fTracks->Delete();
    if ( fFinder) delete fFinder;
}


// -----   Virtual public method Exec   ------------------------------------
void CbmStsFindTracks::Exec(Option_t* opt) {

    fTimer.Start();

    fTracks->Delete();
    Int_t nTracks = fFinder->DoFind();
    for (Int_t iTrack=0; iTrack<fTracks->GetEntriesFast(); iTrack++) {
        CbmStsTrack* track = (CbmStsTrack*) fTracks->At(iTrack);
        track->SortHits();
    }

    fTimer.Stop();
    if ( fVerbose )
        cout << "+ " << setw(15) << left << fName << ": " << setprecision(4)
             << setw(8) << fixed << right << fTimer.RealTime()
             << " s, tracks found " << nTracks << endl;

    fNEvents++;
    fTime    += fTimer.RealTime();
    fNTracks += Double_t(nTracks);
}


// -----   Private virtual method SetParContainers   -----------------------
void CbmStsFindTracks::SetParContainers() {

    // Get run and runtime database
    FairRunAna* run = FairRunAna::Instance();
    if ( ! run ) Fatal("SetParContainers", "No analysis run");

    FairRuntimeDb* db = run->GetRuntimeDb();
    if ( ! db ) Fatal("SetParContainers", "No runtime database");

    // Get STS geometry parameter container
    fGeoPar = (CbmGeoStsPar*) db->getContainer("CbmGeoStsPar");

    // Get STS digitisation parameter container
    fDigiPar = (CbmStsDigiPar*) db->getContainer("CbmStsDigiPar");
}


void CbmStsFindTracks::InitFinder()
{
    if(finderName == "CbmStsTrackFinderIdeal")
        fFinder = new CbmStsTrackFinderIdeal(fVerbose);
    else if(finderName == "CbmL1StsTrackFinder")
        fFinder = new CbmL1StsTrackFinder;
    else
        LOG(error) << "Cannot initialize finder. Invalid finder name: " << finderName;
}

// -----   Private virtual method Init  ------------------------------------
InitStatus CbmStsFindTracks::Init()
{
    cout << endl;
    cout << "---------------------------------------------" << endl;
    cout << "-I- Initialising " << GetName() << " ...." << endl;

    InitFinder();

    // Get input hit arrays
    FairRootManager* ioman = FairRootManager::Instance();
    if ( ! ioman ) Fatal("Init", "No FairRootManager");
    fStsHits = (TClonesArray*) ioman->GetObject("StsHit");
    if ( ! fStsHits ) {
        cout << "-E- "<< GetName() << "::Init: No StsHit array!" << endl;
        return kERROR;
    }
    if ( fUseMvd ) {
        fMvdHits = (TClonesArray*) ioman->GetObject("MvdHit");
        if ( ! fMvdHits ) {
            cout << "-W- " << GetName() << "::Init: No MVD hits available!"
                 << endl;
            cout << "     Running track finding without MVD hits" << endl;
        }
    }

    // Create and register output array STSTrack
    fTracks = new TClonesArray("CbmStsTrack",100);
    ioman->Register("StsTrack", "STS", fTracks, kTRUE);

    // Build digitisation scheme
    if ( fDigiScheme->Init(fGeoPar, fDigiPar) ) {
        if      (fVerbose == 1 || fVerbose == 2) fDigiScheme->Print(kFALSE);
        else if (fVerbose >  2) fDigiScheme->Print(kTRUE);
        cout << "-I- "
             << "STS digitisation scheme succesfully initialised" << endl;
        cout << "    Stations: " << fDigiScheme->GetNStations()
             << ", Sectors: " << fDigiScheme->GetNSectors() << ", Channels: "
             << fDigiScheme->GetNChannels() << endl;
    }

    // Check for Track finder
    if (! fFinder) {
        cout << "-E- " << GetName() << "::Init: No track finder selected!"
             << endl;
        return kERROR;
    }
    cout << "-I- Track finder engine " << fFinder->GetName()
         << " selected" << endl;


    // Set members of track finder and verbosity and initialise track finder
    fFinder->SetDigiScheme(fDigiScheme);
    fFinder->SetField(fField);
    fFinder->SetMvdHitArray(fMvdHits);
    fFinder->SetStsHitArray(fStsHits);
    fFinder->SetTrackArray(fTracks);
    fFinder->SetVerbose(fVerbose);
    fFinder->Init();

    // Screen output
    cout << "-I- " << GetName() << " intialised " << endl;
    cout << "---------------------------------------------" << endl;

    return kSUCCESS;
}


// -----   Virtual private method Finish   ---------------------------------
void CbmStsFindTracks::Finish() {

    fFinder->Finish();

    if (fVerbose > 0) {
        cout << endl;
        cout << "============================================================"
             << endl;
        cout << "=====   " << GetName() << ": Run summary " << endl;
        cout << "===== " << endl;
        cout << "===== Good events   : " << setw(6) << fNEvents << endl;
        cout << "===== Failed events : " << setw(6) << fNEventsFailed << endl;
        cout << "===== Average time  : " << setprecision(4) << setw(8) << right
             << fTime / Double_t(fNEvents) << " s" << endl;
        cout << "===== " << endl;
        cout << "===== Found tracks per event  : " << fixed << setprecision(0)
             << fNTracks / Double_t(fNEvents) << endl;
        cout << "============================================================"
             << endl;
    }
    printf("Work time of CbmStsFindTracks: %4.2f sec.\n", fTime);

}
// -------------------------------------------------------------------------




ClassImp(CbmStsFindTracks)
