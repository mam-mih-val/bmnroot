// -------------------------------------------------------------------------
// -----                MpdLAQGSMGeneratorExt source file                 -----
// -------------------------------------------------------------------------

/** MpdLAQGSMGeneratorExt
 *@author Elena Litvinenko  <litvin@nf.jinr.ru>
 *@version 15.02.2016
 *
 ** The MpdLAQGSMGeneratorExt uses the ASCII input 
 ** provided by K.Gudima LAQGSM event generator.
 **/

#include "MpdLAQGSMGeneratorExt.h"
#include "MpdHypYPtGenerator.h" //AZ

#include "FairPrimaryGenerator.h"
#include "FairIon.h"
#include "FairRunSim.h"
#include "FairParticle.h"

#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TParticle.h"
#include "TSystem.h"
#include "TFile.h"

#include "FairMCEventHeader.h"

#include <iostream>

using std::cout;
using std::endl;
using std::map;


// -----   Default constructor   ------------------------------------------

MpdLAQGSMGeneratorExt::MpdLAQGSMGeneratorExt() {
    fMom = nullptr;
    fExtract_from_dir = kFALSE;
    fMomBranchName = "StartP";
    fMTree = nullptr;
    fSourceFile = nullptr;
    fNEvents = 0;
    fIEvent = 0;
}
// ------------------------------------------------------------------------



// -----   Standard constructor   -----------------------------------------

MpdLAQGSMGeneratorExt::MpdLAQGSMGeneratorExt(
        const vector<TString> &fileNames,
        const Bool_t use_collider_system,
        Int_t QGSM_format_ID,
        Int_t Max_Event_Number,
        Int_t pdg,
        Bool_t extract_from_dir)
//FairGenerator(),
//fIonMap()
{
    fExtract_from_dir = extract_from_dir;
    fMom = nullptr;
    fMomBranchName = "StartP";
    fMTree = nullptr;
    fSourceFile = nullptr;
    fNEvents = 0;
    fIEvent = 0;

    iFile = 0;
    fSelPDG = pdg;
    fFileVec = fileNames;
    fPDG = TDatabasePDG::Instance();


    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) Fatal("Init", "FairRootManager is not instantiated");

    printf("fExtract_from_dir %d\n", fExtract_from_dir);
    if (fExtract_from_dir == kTRUE) {
        ReadEventImpl = &MpdLAQGSMGeneratorExt::ExtractEventFromTxt;
        fMom = new TClonesArray(TVector3::Class());
        ioman->Register(fMomBranchName, "bmndata", fMom, kTRUE); // last arg: save to file
        TString sFileName = fFileVec[iFile];
        if (sFileName.Contains(".gz"))
            fGZ_input = 1;
        else
            fGZ_input = 0;

        fFileName = sFileName.Data();
        cout << "-I- MpdLAQGSMGeneratorExt: Opening input file " << fFileName << endl;

        fInputFile = NULL;
        fGZInputFile = NULL;


        if (fGZ_input)
            fGZInputFile = gzopen(fFileName, "rb");
        else
            fInputFile = fopen(fFileName, "r");

        if ((!fInputFile) && (!fGZInputFile))
            Fatal("MpdLAQGSMGeneratorExt", "Cannot open input file.");

        fUseColliderSystem = use_collider_system;
        fQGSM_format_ID = QGSM_format_ID;

        Init(); //AZ
        cout << "-I- MpdLAQGSMGeneratorExt: Looking for ions..." << endl;
        //AZ Int_t nIons = RegisterIons();
        //  Int_t nIons = RegisterIons1();
        //  Int_t nIons = RegisterIons();    // MG
        Int_t nIons = RegisterIons(Max_Event_Number); // EL
        cout << "-I- MpdLAQGSMGeneratorExt: " << nIons << " ions registered."
                << endl;
        iFile--;
        CloseInput();
        if (++iFile == fFileVec.size())
            iFile = 0;
        fFileName = fFileVec[iFile].Data();
        cout << "-I- MpdLAQGSMGeneratorExt: Reopening input file " << fFileName
                << endl;
        if (fGZ_input)
            fGZInputFile = gzopen(fFileName, "rb");
        else
            fInputFile = fopen(fFileName, "r");
        //    OpenNext();
    } else {
        fSourceFile = new TFile(fFileVec[0].Data(), "READ");
        if (fSourceFile->IsOpen() == false) {
            Fatal("Init", "\n!!!!\ncannot open file !\n");
            return;
        }
        ReadEventImpl = &MpdLAQGSMGeneratorExt::ReadEventFromFiltered;
        printf("ReadEventFromFiltered selected!\n");
        fMTree = (TTree *) fSourceFile->Get("bmndata");
        fMTree->SetBranchAddress(fMomBranchName.Data(), &fMom);
        //        fMom = (TClonesArray*) ioman->GetObject(fMomBranchName.Data());
        fNEvents = fMTree->GetEntries();
    }
    //AZ memset(la_tab,0,sizeof(la_tab));
    fX = 0.0;
    fY = 0.0;
    fZ = 0.0;

}
// ------------------------------------------------------------------------



// -----   Destructor   ---------------------------------------------------

MpdLAQGSMGeneratorExt::~MpdLAQGSMGeneratorExt() {
    if (fSourceFile) {
        fSourceFile->Close();
        delete fSourceFile;
    }
}
// ------------------------------------------------------------------------

Bool_t MpdLAQGSMGeneratorExt::OpenNext() {
    if (++iFile == fFileVec.size())
        if (fExtract_from_dir) {
            printf("\nLast file finished!\n");
            //            fRunSimInst->GetMCApp()->StopRun();
            //            printf("run stopped \n");
            //            return;
            iFile = 0;
            //            return kFALSE;
        } else
            iFile = 0;
    fFileName = fFileVec[iFile].Data();
    cout << "-I- MpdLAQGSMGeneratorExt: Reopening input file " << fFileName
            << endl;
    if (fGZ_input)
        fGZInputFile = gzopen(fFileName, "rb");
    else
        fInputFile = fopen(fFileName, "r");

    return kTRUE;
}



// --------------Public method Init (fill list of known light particles)---

// ------------------------------------------------------------------------



// ------------------------------------------------------------------------

//AZ -----   Public method SkipEvents   --------------------------------------

Bool_t MpdLAQGSMGeneratorExt::SkipEvents(Int_t count) {

    if (count <= 0) return kTRUE;

    // Check for input file
    if (fGZ_input) {
        if (!fGZInputFile) {
            cout << "-E- MpdLAQGSMGeneratorExt: Input file not open!" << endl;
            return kFALSE;
        }
    } else {
        if (!fInputFile) {
            cout << "-E- MpdLAQGSMGeneratorExt: Input file not open!" << endl;
            return kFALSE;
        }
    }

    Int_t eventId = 0, nTracks = 0;

    for (Int_t ii = 0; ii < count; ii++) {
        char ss[252];

        if (!GetEventHeader(ss)) {
            OpenNext();
            if (!GetEventHeader(ss))
                return kFALSE;
        }

        sscanf(ss, " %d %d", &eventId, &nTracks);
        //cout << ii << " " << eventId << endl;

        for (Int_t itrack = 0; itrack < nTracks; itrack++) {
            general_fgets(ss, 250, fInputFile);
        }
    }
    return kTRUE;
}
// ------------------------------------------------------------------------



// -----   Public method ReadEvent   --------------------------------------

Bool_t MpdLAQGSMGeneratorExt::ReadEvent(FairPrimaryGenerator* primGen) {
    (this->*ReadEventImpl)(primGen);
    return kTRUE;
}

Bool_t MpdLAQGSMGeneratorExt::ReadEventFromTxt(FairPrimaryGenerator* primGen) {
    printf("Read TextEvent\n");

    // Check for input file
    if (fGZ_input) {
        if (!fGZInputFile) {
            cout << "-E- MpdLAQGSMGeneratorExt: Input file not open!" << endl;
            return kFALSE;
        }
    } else {
        if (!fInputFile) {
            cout << "-E- MpdLAQGSMGeneratorExt: Input file not open!" << endl;
            return kFALSE;
        }
    }

    // ---> Check for primary generator
    if (!primGen) {
        cout << "-E- MpdLAQGSMGeneratorExt::ReadEvent: "
                << "No PrimaryGenerator!" << endl;
        return kFALSE;
    }

    //AZ if (la_tab[0].pdg<=0)
    if (fLa_tab[0]->pdg <= 0)
        Init();


    // Define event variable to be read from file
    Int_t eventId = 0;
    Int_t nTracks = 0;
    Float_t b = 0., bx = 0, by = 0;

    // Define track variables to be read from file
    Int_t
    iCharge = 0,
            iLeptonic = 0,
            iStrange = 0,
            iBarionic = 0, iCode = 0, io1 = 0, io2 = 0, io3 = 0 //,
            //A=0,
            //Z=0
            ;
    Float_t
    px = 0.,
            py = 0.,
            pz = 0.,
            pz1 = 0,
            pza1 = 0,
            //    Ekin = 0,
            //    Ekin1 = 0,
            mass = 0;

    Int_t PDG = 0;
    char ss[252];

    char ionName[30];

    if (!GetEventHeader(ss)) {
        OpenNext();
        if (!GetEventHeader(ss))
            return kFALSE;
    }

    //Double_t projA,projZ,projEex, targA, targZ, targEex;
    Float_t projA, projZ, projEex, targA, targZ, targEex;

    int bpos;
    sscanf(ss, " %d %d %n", &eventId, &nTracks, &bpos);

    if (!fQGSM_format_ID)
        sscanf(&(ss[bpos]), "%g %g %g %g %g %g %g %g %g", &b, &bx, &by, &projA, &projZ, &projEex, &targA, &targZ, &targEex);
    else
        sscanf(&(ss[bpos]), "%g %g %g", &b, &bx, &by);

    cout << "-I- MpdLAQGSMGeneratorExt::ReadEvent: Event " << eventId
            << ",  b = " << b << " fm, multiplicity " << nTracks << endl;

    TVector2 bb; // MG
    bb.Set(bx, by); // MG

    // Set event id and impact parameter in MCEvent if not yet done
    FairMCEventHeader* event = primGen->GetEvent();
    if (event && (!event->IsSet())) {
        event->SetEventID(eventId - 1);
        event->SetB(b);
        //    event->SetPhi(bb.Phi());    // MG
        event->MarkSet(kTRUE);
    }
    // Loop over tracks in the current event
    for (Int_t itrack = 0; itrack < nTracks; itrack++) {

        general_fgets(ss, 250, fInputFile);

        if (fQGSM_format_ID < 3)
            sscanf(ss, " %d %d %d %d %d", &iCharge, &iLeptonic, &iStrange, &iBarionic, &iCode);
        else
            sscanf(ss, " %d %d %d %d %d %d %d", &iCharge, &iLeptonic, &iStrange, &iBarionic, &io1, &io2, &io3);

        Int_t p_num = 21;
        if (fQGSM_format_ID == 3)
            p_num = 36;

        if (!fQGSM_format_ID)
            sscanf(&(ss[p_num]), "%g%g%g%g%g", &px, &py, &pz, &pz1, &mass);
        else
            sscanf(&(ss[p_num]), "%g%g%g%g%g%g", &px, &py, &pz, &pz1, &pza1, &mass);


        if (FindParticle(iCharge, iStrange, iLeptonic, iBarionic, mass, PDG, ionName)) {

            if (PDG != fSelPDG)
                continue;

            if (!fPDG->GetParticle(PDG)) {
                cout << "-W- MpdLAQGSMGeneratorExt::ReadEvent: Cannot find " << PDG << " "
                        << ionName << " in database!" << endl;
                continue;
            }

            // Give track to PrimaryGenerator
            if (fUseColliderSystem)
                primGen->AddTrack(PDG, px, py, pz, 0., 0., 0.);
            else
                primGen->AddTrack(PDG, px, py, pz1, fX, fY, fZ);
        }
    }

    return kTRUE;
}
// ------------------------------------------------------------------------

Bool_t MpdLAQGSMGeneratorExt::ExtractEventFromTxt(FairPrimaryGenerator* primGen) {
    fMom->Clear("C");
    Bool_t isFound = kFALSE;
    while (!isFound) {
        // Check for input file
        if (fGZ_input) {
            if (!fGZInputFile) {
                cout << "-E- MpdLAQGSMGeneratorExt: Input file not open!" << endl;
                return kFALSE;
            }
        } else {
            if (!fInputFile) {
                cout << "-E- MpdLAQGSMGeneratorExt: Input file not open!" << endl;
                return kFALSE;
            }
        }
        //AZ if (la_tab[0].pdg<=0)
        if (fLa_tab[0]->pdg <= 0)
            Init();


        // Define event variable to be read from file
        Int_t eventId = 0;
        Int_t nTracks = 0;
        Float_t b = 0., bx = 0, by = 0;

        // Define track variables to be read from file
        Int_t
        iCharge = 0,
                iLeptonic = 0,
                iStrange = 0,
                iBarionic = 0, iCode = 0, io1 = 0, io2 = 0, io3 = 0 //,
                //A=0,
                //Z=0
                ;
        Float_t
        px = 0.,
                py = 0.,
                pz = 0.,
                pz1 = 0,
                pza1 = 0,
                //    Ekin = 0,
                //    Ekin1 = 0,
                mass = 0;

        Int_t PDG = 0;
        char ss[252];

        char ionName[30];

        if (!GetEventHeader(ss)) {
            if (OpenNext() == kFALSE)
                return kFALSE;
            if (!GetEventHeader(ss))
                return kFALSE;
        }

        //Double_t projA,projZ,projEex, targA, targZ, targEex;
        Float_t projA, projZ, projEex, targA, targZ, targEex;

        int bpos;
        sscanf(ss, " %d %d %n", &eventId, &nTracks, &bpos);

        if (!fQGSM_format_ID)
            sscanf(&(ss[bpos]), "%g %g %g %g %g %g %g %g %g", &b, &bx, &by, &projA, &projZ, &projEex, &targA, &targZ, &targEex);
        else
            sscanf(&(ss[bpos]), "%g %g %g", &b, &bx, &by);

        //        cout << "-I- MpdLAQGSMGeneratorExt::ReadEvent: Event " << eventId
        //                << ",  b = " << b << " fm, multiplicity " << nTracks << endl;

        TVector2 bb; // MG
        bb.Set(bx, by); // MG

        //    // Set event id and impact parameter in MCEvent if not yet done
        //    FairMCEventHeader* event = primGen->GetEvent();
        //    if (event && (!event->IsSet())) {
        //        event->SetEventID(eventId - 1);
        //        event->SetB(b);
        //        //    event->SetPhi(bb.Phi());    // MG
        //        event->MarkSet(kTRUE);
        //    }
        // Loop over tracks in the current event
        for (Int_t itrack = 0; itrack < nTracks; itrack++) {

            general_fgets(ss, 250, fInputFile);

            if (fQGSM_format_ID < 3)
                sscanf(ss, " %d %d %d %d %d", &iCharge, &iLeptonic, &iStrange, &iBarionic, &iCode);
            else
                sscanf(ss, " %d %d %d %d %d %d %d", &iCharge, &iLeptonic, &iStrange, &iBarionic, &io1, &io2, &io3);

            Int_t p_num = 21;
            if (fQGSM_format_ID == 3)
                p_num = 36;

            if (!fQGSM_format_ID)
                sscanf(&(ss[p_num]), "%g%g%g%g%g", &px, &py, &pz, &pz1, &mass);
            else
                sscanf(&(ss[p_num]), "%g%g%g%g%g%g", &px, &py, &pz, &pz1, &pza1, &mass);

            if (FindParticle(iCharge, iStrange, iLeptonic, iBarionic, mass, PDG, ionName)) {
                if (PDG != fSelPDG)
                    continue;
                isFound = kTRUE;
                if (fUseColliderSystem)
                    new ((*fMom)[fMom->GetEntriesFast()])TVector3(px, py, pz);
                else
                    new ((*fMom)[fMom->GetEntriesFast()])TVector3(px, py, pz1);
                break; // if we need only 1 particle
            }
        }
    }

    return kTRUE;
}

Bool_t MpdLAQGSMGeneratorExt::ReadEventFromFiltered(FairPrimaryGenerator* primGen) {
    // ---> Check for primary generator
    if (!primGen) {
        cout << "-E- MpdLAQGSMGeneratorExt::ReadEvent: "
                << "No PrimaryGenerator!" << endl;
        return kFALSE;
    }
    if (++fIEvent == fNEvents)
        fIEvent = 0;
    fMTree->GetEntry(fIEvent);

    for (Int_t i = 0; i < fMom->GetEntriesFast(); i++) {
        TVector3* v = (TVector3*) fMom->UncheckedAt(i);
        // Give track to PrimaryGenerator
        primGen->AddTrack(fSelPDG, v->X(), v->Y(), v->Z(), 0., 0., 0.);
    }

    return kTRUE;
}

ClassImp(MpdLAQGSMGeneratorExt)

