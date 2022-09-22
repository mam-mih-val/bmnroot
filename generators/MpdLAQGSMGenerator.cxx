// -------------------------------------------------------------------------
// -----                MpdLAQGSMGenerator source file                 -----
// -------------------------------------------------------------------------

/** MpdLAQGSMGenerator
 *@author Elena Litvinenko  <litvin@nf.jinr.ru>
 *@version 15.02.2016
 *
 ** The MpdLAQGSMGenerator uses the ASCII input 
 ** provided by K.Gudima LAQGSM event generator.
 **/

#include "MpdLAQGSMGenerator.h"
#include "MpdHypYPtGenerator.h" //AZ
#include "CbmStack.h" //AZ-310822

#include "FairPrimaryGenerator.h"
#include "FairIon.h"
#include "FairRunSim.h"
#include "FairParticle.h"

#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TParticle.h"
#include "TSystem.h"
#include <TVirtualMC.h> //AZ-310822 

#include "FairMCEventHeader.h"

#include <iostream>

using std::cout;
using std::endl;
using std::map;

const Double_t kProtonMass = 0.938272013;
const Double_t kNeutronMass = 0.93957;

// -----   Default constructor   ------------------------------------------

MpdLAQGSMGenerator::MpdLAQGSMGenerator() {
}
// ------------------------------------------------------------------------



// -----   Standard constructor   -----------------------------------------

MpdLAQGSMGenerator::MpdLAQGSMGenerator(const char* fileName, const Bool_t use_collider_system, Int_t QGSM_format_ID, Int_t Max_Event_Number) :
FairGenerator(),
fIonMap(),
fX(0.), fY(0.), fZ(0.) {
    //AZ memset(la_tab,0,sizeof(la_tab));

    fPDG = TDatabasePDG::Instance();

    fFileName = fileName;
    cout << "-I- MpdLAQGSMGenerator: Opening input file " << fileName << endl;

    fInputFile = NULL;
    fGZInputFile = NULL;

    TString sFileName = fileName;
    if (sFileName.Contains(".gz"))
        fGZ_input = 1;
    else
        fGZ_input = 0;

    if (fGZ_input)
        fGZInputFile = gzopen(fFileName, "rb");
    else
        fInputFile = fopen(fFileName, "r");

    if ((!fInputFile) && (!fGZInputFile))
        Fatal("MpdLAQGSMGenerator", "Cannot open input file.");

    fUseColliderSystem = use_collider_system;
    fQGSM_format_ID = QGSM_format_ID;

    Init(); //AZ
    cout << "-I- MpdLAQGSMGenerator: Looking for ions..." << endl;
    //AZ Int_t nIons = RegisterIons();
    //  Int_t nIons = RegisterIons1();
    //  Int_t nIons = RegisterIons();    // MG
    Int_t nIons = RegisterIons(Max_Event_Number); // EL
    cout << "-I- MpdLAQGSMGenerator: " << nIons << " ions registered."
            << endl;

    CloseInput();
    cout << "-I- MpdLAQGSMGenerator: Reopening input file " << fileName
            << endl;
    if (fGZ_input)
        fGZInputFile = gzopen(fFileName, "rb");
    else
        fInputFile = fopen(fFileName, "r");

}
// ------------------------------------------------------------------------



// -----   Destructor   ---------------------------------------------------

MpdLAQGSMGenerator::~MpdLAQGSMGenerator() {

    CloseInput();
    Int_t nPart = fLa_tab.size();
    for (Int_t i = 0; i < nPart; ++i) delete fLa_tab[i];
    fLa_tab.clear();
}
// ------------------------------------------------------------------------


// --------------Public method Init (fill list of known light particles)---

void MpdLAQGSMGenerator::Init(const char *light_particles_filename) {
    TString fname, dd;
    if (!light_particles_filename) {
        dd = getenv("VMCWORKDIR");
        if (gSystem->OpenDirectory(dd + "/parameters"))
            dd += "/parameters";
        else {
            if (gSystem->OpenDirectory(dd + "/generators"))
                dd += "/generators";
            else
                dd = gSystem->WorkingDirectory();
        }

        fname = dd + "/MpdLAQGSM_light_particles.dat";
    } else
        fname = light_particles_filename;

    Int_t i = 0, i_val, i_max = 84;
    Float_t a_val;

    char ss[300];

    FILE *fInputFile_light = fopen(fname.Data(), "r");
    //AZ while (!(feof(fInputFile_light))&&(i<i_max)) {
    while (!(feof(fInputFile_light))) {
        la_tab_t *lt = new la_tab_t;
        /*AZ
        fscanf(fInputFile_light, "%d", &i_val); la_tab[i].pdg=i_val;
        fscanf(fInputFile_light, "%d", &i_val); la_tab[i].Z=i_val;
        fscanf(fInputFile_light, "%d", &i_val); la_tab[i].lepton=i_val;
        fscanf(fInputFile_light, "%d", &i_val); la_tab[i].strange=i_val;
        fscanf(fInputFile_light, "%d", &i_val); la_tab[i].A=i_val;
        fscanf(fInputFile_light, "%f", &a_val); la_tab[i].mass=a_val;
        fscanf(fInputFile_light, "%s", ss);     strncpy(la_tab[i].name,ss,10);
        i++;   
         */
        fscanf(fInputFile_light, "%d", &i_val);
        lt->pdg = i_val;
        fscanf(fInputFile_light, "%d", &i_val);
        lt->Z = i_val;
        fscanf(fInputFile_light, "%d", &i_val);
        lt->lepton = i_val;
        fscanf(fInputFile_light, "%d", &i_val);
        lt->strange = i_val;
        fscanf(fInputFile_light, "%d", &i_val);
        lt->A = i_val;
        fscanf(fInputFile_light, "%f", &a_val);
        lt->mass = a_val;
        fscanf(fInputFile_light, "%s", ss);
        strncpy(lt->name, ss, 10);
        fLa_tab.push_back(lt);
    }
    fclose(fInputFile_light);
}
// ------------------------------------------------------------------------

Bool_t MpdLAQGSMGenerator::general_fgets(char *ss, Int_t nn, FILE* p) {
    Bool_t finished = 0;

    if (fGZ_input) {
        gzgets(fGZInputFile, ss, nn);
        finished = (gzeof(fGZInputFile));
    } else {
        fgets(ss, nn, fInputFile);
        finished = (feof(fInputFile));
    }
    return finished;
}

Bool_t MpdLAQGSMGenerator::GetEventHeader(char *ss) {
    Bool_t finished = 0;

    // if (fGZ_input) {
    //   gzgets(fGZInputFile,ss,250); 
    //   finished = (gzeof(fGZInputFile));
    // }
    // else {
    //   fgets(ss,250,fInputFile); 
    //   finished = ( feof(fInputFile) );
    // }
    finished = general_fgets(ss, 250, fInputFile);

    // If end of input file is reached : close it and abort run
    if (finished) {
        cout << "-I- MpdLAQGSMGenerator: End of input file reached " << endl;
        CloseInput();
        return kFALSE;
    }

    Bool_t wrong_file = kFALSE;

    TString tss(ss);
    if (tss.Contains("QGSM")) { // 0

        general_fgets(ss, 250, fInputFile);
        tss = ss;
        Int_t lines = 0;
        while (!(tss.Contains("particles, b,bx,by"))) {
            if (tss.Contains("QGSM format ID")) {
                sscanf(ss, "QGSM format ID=%d", &fQGSM_format_ID);
                cout << "QGSM format ID " << fQGSM_format_ID << endl;

                if (fQGSM_format_ID == 2) { // correction of incorrect format_ID in some data files
                    fpos_t file_loc;
                    z_off_t gz_file_loc;
                    if (fGZ_input)
                        gz_file_loc = gztell(fGZInputFile);
                    else
                        fgetpos(fInputFile, &file_loc);
                    for (int k = 0; k < 5; k++) general_fgets(ss, 250, fInputFile);
                    if (strlen(ss) > 90)
                        fQGSM_format_ID = 3;
                    cout << "QGSM format ID now " << fQGSM_format_ID << endl;
                    if (fGZ_input)
                        gzseek(fGZInputFile, gz_file_loc, SEEK_SET);
                    else
                        fsetpos(fInputFile, &file_loc);
                }

            }
            general_fgets(ss, 250, fInputFile);
            tss = ss;
            lines++;
            if ((fQGSM_format_ID >= 2)&&(lines >= 4))
                return kTRUE;
            if (lines > 25) {
                wrong_file = kTRUE;
                break;
            }
        }
    } else {
        if (fQGSM_format_ID >= 2)
            return kTRUE;
        if (!tss.Contains("particles, b,bx,by")) {
            general_fgets(ss, 250, fInputFile);
            tss = ss;
        }
    }

    if (wrong_file) {
        cout << "-E- MpdLAQGSMGenerator: Wrong file header! " << endl;
        CloseInput();
        return kFALSE;
    }

    if (!tss.Contains("event")) { // 3
        if (fQGSM_format_ID >= 2)
            return kTRUE;
        else {
            cout << "-E- MpdLAQGSMGenerator: Wrong event header!" << endl;
            CloseInput();
            return kFALSE;
        }
    }

    char tmp[250];

    switch (fQGSM_format_ID) {
        case 0:
        case 1:
            general_fgets(tmp, 250, fInputFile);
            break;
        case 2:
        case 3:
            general_fgets(ss, 250, fInputFile);
            general_fgets(ss, 250, fInputFile);
            break;
        case 4: //AZ
            general_fgets(ss, 250, fInputFile);
            general_fgets(ss, 250, fInputFile);
            break;
        default:
            break;
    }

    return kTRUE;
}

// ------------------------------------------------------------------------

//AZ -----   Public method SkipEvents   --------------------------------------

Bool_t MpdLAQGSMGenerator::SkipEvents(Int_t count) {

    if (count <= 0) return kTRUE;

    // Check for input file
    if (fGZ_input) {
        if (!fGZInputFile) {
            cout << "-E- MpdLAQGSMGenerator: Input file not open!" << endl;
            return kFALSE;
        }
    } else {
        if (!fInputFile) {
            cout << "-E- MpdLAQGSMGenerator: Input file not open!" << endl;
            return kFALSE;
        }
    }

    Int_t eventId = 0, nTracks = 0;

    for (Int_t ii = 0; ii < count; ii++) {
        char ss[252];

        if (!GetEventHeader(ss))
            return kFALSE;

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

Bool_t MpdLAQGSMGenerator::ReadEvent(FairPrimaryGenerator* primGen) {

    // Check for input file
    if (fGZ_input) {
        if (!fGZInputFile) {
            cout << "-E- MpdLAQGSMGenerator: Input file not open!" << endl;
            return kFALSE;
        }
    } else {
        if (!fInputFile) {
            cout << "-E- MpdLAQGSMGenerator: Input file not open!" << endl;
            return kFALSE;
        }
    }

    // ---> Check for primary generator
    if (!primGen) {
        cout << "-E- MpdLAQGSMGenerator::ReadEvent: "
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

    if (!GetEventHeader(ss))
        return kFALSE;

    //Double_t projA,projZ,projEex, targA, targZ, targEex;
    Float_t projA, projZ, projEex, targA, targZ, targEex;

    int bpos;
    sscanf(ss, " %d %d %n", &eventId, &nTracks, &bpos);

    if (!fQGSM_format_ID)
        sscanf(&(ss[bpos]), "%g %g %g %g %g %g %g %g %g", &b, &bx, &by, &projA, &projZ, &projEex, &targA, &targZ, &targEex);
    else
        sscanf(&(ss[bpos]), "%g %g %g", &b, &bx, &by);

    cout << "-I- MpdLAQGSMGenerator::ReadEvent: Event " << eventId
            << ",  b = " << b << " fm, multiplicity " << nTracks << endl;

    TVector2 bb; // MG
    bb.Set(bx, by); // MG

    // Set event id and impact parameter in MCEvent if not yet done
    FairMCEventHeader* event = primGen->GetEvent();
    if (event && (!event->IsSet())) {
        event->SetEventID(eventId - 1);
        event->SetB(b);
        event->SetRotZ(bb.Phi());    // MG
        event->MarkSet(kTRUE);
    }

    /*
    //AZ - Loop over tracks to find hypernuclei
    fpos_t position;
    fgetpos (fInputFile, &position); // save position
    Int_t iHyperOk = 0;

    for (Int_t itrack = 0; itrack < nTracks; itrack++) {

      fgets(ss,250,fInputFile);
      sscanf(ss," %d %d %d %d %d", &iCharge,&iLeptonic,&iStrange,&iBarionic, &iCode);

      if (iStrange == -1 && iCharge == 1 && iBarionic == 3) {
        // select only H3L
        iHyperOk = 1;
        break;
      }
      // Select H4LL
      //if (iStrange == -2 && iCharge == 1 && iBarionic == 4) {
      //iHyperOk = 1;
      //break;
      //}
    }
    if (iHyperOk) fsetpos (fInputFile, &position);
    //else nTracks = 0; // do not store any track
    else {
      // Add particle manually
      TParticlePDG* part = fPDG->GetParticle("H3L");
      Int_t pdgType = part->PdgCode(); 
      static MpdHypYPtGenerator *hypGen = 0x0;
      if (hypGen == 0x0) {
        hypGen = new MpdHypYPtGenerator(pdgType,1);
        hypGen->SetDistributionPt(0.163); // temperature - H3L: NICA @ 5 GeV
        hypGen->SetDistributionY(0.0, 0.373); // y0, Sigma_y - H3L: NICA @ 5 GeV
        hypGen->Init();
      }
      hypGen->ReadEvent(primGen);
      fsetpos (fInputFile, &position);
    }
    // AZ
     */

    // Loop over tracks in the current event
    for (Int_t itrack = 0; itrack < nTracks; itrack++) {

        general_fgets(ss, 250, fInputFile);

        //AZ if (fQGSM_format_ID < 3)
        if (fQGSM_format_ID < 3 || fQGSM_format_ID == 4)
            sscanf(ss, " %d %d %d %d %d", &iCharge, &iLeptonic, &iStrange, &iBarionic, &iCode);
        else
            sscanf(ss, " %d %d %d %d %d %d %d", &iCharge, &iLeptonic, &iStrange, &iBarionic, &io1, &io2, &io3);

        Int_t p_num = 21;
        if (fQGSM_format_ID == 3)
	  p_num = 36;
        else if (fQGSM_format_ID == 4) p_num = 30;

        if (!fQGSM_format_ID)
            sscanf(&(ss[p_num]), "%g%g%g%g%g", &px, &py, &pz, &pz1, &mass);
        else
            sscanf(&(ss[p_num]), "%g%g%g%g%g%g", &px, &py, &pz, &pz1, &pza1, &mass);

        if (FindParticle(iCharge, iStrange, iLeptonic, iBarionic, mass, PDG, ionName)) {

            if (!fPDG->GetParticle(PDG)) {
                cout << "-W- MpdLAQGSMGenerator::ReadEvent: Cannot find " << PDG << " "
                        << ionName << " in database!" << endl;
                continue;
            }

            // Give track to PrimaryGenerator
            if (fUseColliderSystem)
                primGen->AddTrack(PDG, px, py, pz, 0., 0., 0.);
            else
                primGen->AddTrack(PDG, px, py, pz1, fX, fY, fZ);
	    //AZ-310822
	    if (fQGSM_format_ID == 3 && TMath::Abs(PDG) == 3122) {
	       // Store lambda polarization 
	       Int_t nTr = gMC->GetStack()->GetNtrack();
	       TParticle *part = ((CbmStack*)gMC->GetStack())->GetParticle(nTr-1);
	       Double_t polx = pza1; // pza1 - polarization after rescattering 
	       if (polx > 0) polx = 0;
	       //polx = -0.5; // pza1 - polarization after rescattering 
	       Double_t poly = TMath::Sqrt (1.0 - polx * polx);
	       Double_t polz = pz1; // pz1 - initial polarization
	       part->SetPolarisation(polx, poly, polz);
	    }
        }
    }

    return kTRUE;
}
// ------------------------------------------------------------------------



// -----   Private method CloseInput   ------------------------------------

void MpdLAQGSMGenerator::CloseInput() {
    if (fInputFile) {
        if (!fGZ_input)
            fclose(fInputFile);
        fInputFile = NULL;
    }
    if (fGZInputFile) {
        if (fGZ_input)
            gzclose(fGZInputFile);
        fGZInputFile = NULL;
    }
}
// ------------------------------------------------------------------------

Int_t MpdLAQGSMGenerator::CreatePdgCode(Int_t Z, Int_t A, Int_t Strange, Int_t user) {
    // 10LZZZAAAI 

    const Int_t kion = 1000000000;
    const Int_t kion_strange = kion + 10000000;
    const Int_t kspecial = 50000000;

    if (Strange) {
        Int_t ss = abs(Strange);
        if (Z) {
            if (ss == 1)
                return (kion_strange + Z * 10000 + A * 10);
            else
                return (kion + 10000000 * ss + Z * 10000 + A * 10);
        } else
            return (kspecial + A * 10);
    } else {
        return (kion + Z * 10000 + A * 10 + user);
    }
}

Bool_t MpdLAQGSMGenerator::general_feof(void *p) {
    Bool_t finished = 0;
    if (fGZ_input)
        finished = (gzeof(fGZInputFile));
    else
        finished = (feof(fInputFile));
    return finished;
}

// -----   Private method RegisterIons   ----------------------------------

Int_t MpdLAQGSMGenerator::RegisterIons(Int_t Max_Event_Number) {

    Int_t nIons = 0;
    //  Int_t eventId, nTracks, iPid, iMass, iCharge;
    //  Double_t pBeam, b, px, py, pz;
    Int_t eventId = 0;
    Int_t nTracks = 0;
    //  Float_t b       = 0., bx=0, by=0;

    // Define track variables to be read from file
    Int_t
    iCharge = 0,
            iLeptonic = 0,
            iStrange = 0,
            iBarionic = 0,
            iCode = 0;
    Float_t
    px = 0.,
            py = 0.,
            pz = 0.,
            pz1 = 0,
            pza1 = 0,
            //    Ekin = 0,
            //    Ekin1 = 0,
            mass = 0,
            excEnergy = 0;
    char
    ss[252],
            buf_ionName[30];

    Int_t
    A = 0, Z = 0, Q = 0;

    fIonMap.clear();


    if (!GetEventHeader(ss))
        return 0;

    Bool_t geant4_bug_case = 1;

    Int_t PDG;
    TString ionName;

    while ((!general_feof(fInputFile)) && ((!Max_Event_Number) || (eventId < Max_Event_Number))) {

        sscanf(ss, " %d %d", &eventId, &nTracks);

        if (general_feof(fInputFile)) continue;

        for (Int_t iTrack = 0; iTrack < nTracks; iTrack++) {

            general_fgets(ss, 250, fInputFile);

            sscanf(ss, " %d %d %d %d %d", &iCharge, &iLeptonic, &iStrange, &iBarionic, &iCode);

            //       if (!fQGSM_format_ID)
            // 	sscanf(&(ss[21]),"%g%g%g%g%g",&px,&py,&pz,&pz1,&mass);
            //       else 
            // 	sscanf(&(ss[21]),"%g%g%g%g%g%g",&px,&py,&pz,&pz1,&pza1,&mass);

            Int_t p_num = 21;
            if (fQGSM_format_ID == 3)
                p_num = 36;
	    else if (fQGSM_format_ID == 4) p_num = 30;

            if (!fQGSM_format_ID)
                sscanf(&(ss[p_num]), "%g%g%g%g%g", &px, &py, &pz, &pz1, &mass);
            else
                sscanf(&(ss[p_num]), "%g%g%g%g%g%g", &px, &py, &pz, &pz1, &pza1, &mass);


            if (iBarionic > 1) { // ion

                A = iBarionic;
                Z = iCharge;
                Q = iCharge;

                if (A > 9)
                    geant4_bug_case = 0;


                if (FindParticle(iCharge, iStrange, iLeptonic, iBarionic, mass, PDG, buf_ionName)) {

                    if (fPDG->GetParticle(PDG))
                        continue;

                    ionName = buf_ionName;

                    if (!iStrange) { // normal ion
                        if (Z > 2 || (Z == 2 && A > 4)) { // MG

                            if (fIonMap.find(ionName) == fIonMap.end()) { // new ion
                                //	      excEnergy = fabs(mass - kProtonMass*Z -kNeutronMass*(iBarionic-Z)); 
                                FairIon* ion = new FairIon(ionName, Z, A, Q); // , excEnergy,mass);  // from MG
                                fIonMap[ionName] = ion;
                                nIons++;
                            }
                        }
                    }

                    else { // hyper
		      //AZ if(false)
                        if (CreateNucleus(Z, mass, PDG, buf_ionName)) {
                            //	      FairParticle *pa=new FairParticle(buf_ionName,Z, iBarionic, abs(iStrange), mass,Z,0,2.6e-10);
                            /*
                            TParticle *tpa= new TParticle();
                            if (tpa) {
                              tpa->SetPdgCode(PDG);
                              FairParticle *pa=new FairParticle(PDG,tpa);
                              if (pa)
                                (FairRunSim::Instance())->AddNewParticle(pa);
                            }
                             */
                            if (Z) { //AZ
                                FairParticle *pa = new FairParticle(PDG, buf_ionName, kPTHadron, mass, Z, 2.6e-10, "hadron");
                                if (pa) FairRunSim::Instance()->AddNewParticle(pa);
                            } else {
                                FairParticle *pa = new FairParticle(PDG, buf_ionName, kPTNeutron, mass, Z, 2.6e-10, "special");
                                if (pa) FairRunSim::Instance()->AddNewParticle(pa);
                            } //AZ
                        }
                    }
                }

            } // ion

        } // track loop

        if (!GetEventHeader(ss))
            break;

    } // event loop

    if (geant4_bug_case) { // artificial heavy ion
        mass = 9.98;
        excEnergy = fabs(mass - (kProtonMass + kNeutronMass)*5);
        TString ionName_dummy = "Ion_10_5";
        if (fIonMap.find(ionName_dummy) == fIonMap.end()) {
            FairIon* ion = new FairIon(ionName_dummy, 5, 10, 5, excEnergy, mass);
            fIonMap[ionName_dummy] = ion;
            nIons++;
        }
    }

    FairRunSim* run = FairRunSim::Instance();
    map<TString, FairIon*>::iterator mapIt;
    for (mapIt = fIonMap.begin(); mapIt != fIonMap.end(); ++mapIt) {
        FairIon* ion = (*mapIt).second;
        run->AddNewIon(ion);
    }

    return nIons;
}

// -----   Private method RegisterIons1   ----------------------------------

Int_t MpdLAQGSMGenerator::RegisterIons1() {
    // Read file with particle data

    Int_t nIons = 0, nPart = fLa_tab.size();

    fIonMap.clear();
    Bool_t geant4_bug_case = 1;
    Int_t PDG;
    TString ionName;
    Float_t mass = 0.0, excEnergy = 0.0;
    char buf_ionName[30];

    for (Int_t i = 0; i < nPart; ++i) {
        Int_t Z = fLa_tab[i]->Z, iCharge = Z, Q = Z;
        Int_t iLeptonic = fLa_tab[i]->lepton;
        Int_t iStrange = fLa_tab[i]->strange;
        Int_t A = fLa_tab[i]->A, iBarionic = A;
        mass = fLa_tab[i]->mass;

        if (iBarionic > 1) {
            // ion

            //if (A > 9) geant4_bug_case = 0;

            if (FindParticle(iCharge, iStrange, iLeptonic, iBarionic, mass, PDG, buf_ionName)) {

                if (fPDG->GetParticle(PDG)) continue;

                ionName = buf_ionName;

                if (!iStrange) {
                    // normal ion

                    if (fIonMap.find(ionName) == fIonMap.end()) { // new ion
                        excEnergy = fabs(mass - kProtonMass * Z - kNeutronMass * (iBarionic - Z));
                        FairIon* ion = new FairIon(ionName, Z, A, Q, excEnergy, mass);
                        fIonMap[ionName] = ion;
                        nIons++;
                    }
                } else {
                    // hyper
if(false)
                    if (CreateNucleus(Z, mass, PDG, buf_ionName)) {
                        //	      FairParticle *pa=new FairParticle(buf_ionName,Z, iBarionic, abs(iStrange), mass,Z,0,2.6e-10);
                        if (Z) {
                            FairParticle *pa = new FairParticle(PDG, buf_ionName, kPTHadron, mass, Z, 2.6e-10, "hadron");
                            if (pa) FairRunSim::Instance()->AddNewParticle(pa);
                        } else {
                            FairParticle *pa = new FairParticle(PDG, buf_ionName, kPTNeutron, mass, Z, 2.6e-10, "special");
                            if (pa) FairRunSim::Instance()->AddNewParticle(pa);
                        }
                    }
                }
            }

        } // if (iBarionic > 1)

    } // for (Int_t i = 0; i < nPart;

    if (geant4_bug_case) { // artificial heavy ion
        mass = 9.98;
        excEnergy = fabs(mass - (kProtonMass + kNeutronMass)*5);
        TString ionName_dummy = "Ion_10_5";
        if (fIonMap.find(ionName_dummy) == fIonMap.end()) {
            FairIon* ion = new FairIon(ionName_dummy, 5, 10, 5, excEnergy, mass);
            fIonMap[ionName_dummy] = ion;
            nIons++;
        }
    }

    FairRunSim* run = FairRunSim::Instance();
    map<TString, FairIon*>::iterator mapIt;
    for (mapIt = fIonMap.begin(); mapIt != fIonMap.end(); ++mapIt) {
        FairIon* ion = (*mapIt).second;
        run->AddNewIon(ion);
    }

    return nIons;
}

// ------------------------------------------------------------------------

Bool_t MpdLAQGSMGenerator::FindParticle(Int_t Z, Int_t strange, Int_t lepton, Int_t A, Float_t mass, Int_t &PDG, char name[11]) {
    Int_t
    n_mass,
            result = 0;

    //AZ if (mass < 6) { // use table
    if (strange || Z <= 2) { // use table

        Int_t
        l_min = -1, // line number first
                l_max = -1; // line number last

        n_mass = (Int_t) (mass / 0.1);

        switch (n_mass) {
            case 1: l_min = 8;
                l_max = 12; // MU+,MU-,PI0,PI+,PI-
                break;
            case 9: l_min = 28;
                l_max = 32; // P,AP,AN,N,ETAP
                break;
            case 4: l_min = 13;
                l_max = 18; // K+,K-,AK0,KL,KS,K0
                break;
            case 18: l_min = 66;
                l_max = 66; // d 
                break;
            case 5: l_min = 19;
                l_max = 19; // ETA
                break;
            case 11: l_min = 34;
                l_max = 41; // AL,L,S+,AS+,AS0,S0,AS-,S-
                break;
            case 28: l_min = 68;
                l_max = 69; // He3,t
                break;
            case 37: l_min = 71;
                l_max = 71; // He4
                break;
            case 13: l_min = 50;
                l_max = 59; // AXI0,XI0,AXI-,XI-,AS*0,S*0,S*+,AS*+,S*-,AS*-
                break;
            case 29: l_min = 70;
                l_max = 70; // H3L
                break;

            case 22:
                if (!Z) {
                    l_min = 67;
                    l_max = 67;
                }// LL 
                else {
                    l_min = 81;
                    l_max = 81;
                } // KsiNP
                break;

            case 39: l_min = 72;
                l_max = 73; // He4L,H4L
                break;
            case 48: l_min = 75;
                l_max = 75; // He5L
                break;
            case 16: l_min = 64;
                l_max = 65; // AOM-,OM-
                break;
            case 41: l_min = 74;
                l_max = 74; // H4LL
                break;
            case 50: l_min = 76;
                l_max = 77; // He5LL,H5LL
                break;
            case 59: l_min = 78;
                l_max = 78; // He6LL
                break;
            case 0: l_min = 1;
                l_max = 7; // GM,ANUM,ANUE,NUE,NUM,E+,E-
                break;
            case 7: l_min = 20;
                l_max = 23; // RHO+,RHO-,RHO0,OMEG
                break;
            case 8: l_min = 24;
                l_max = 27; // K*+,K*-,AK*0,K*0
                break;
            case 10: l_min = 33;
                l_max = 33; // PHI
                break;
            case 12: l_min = 42;
                l_max = 49; // DL++,ADL++,DL+,DL-,ADL0,DL0,ADL-,ADL+
                break;
            case 15: l_min = 60;
                l_max = 63; // AXI*0,XI*0,AXI*-,XI*-
                break;
            case 20: l_min = 79;
                l_max = 80; // LN, LP
                break;
            case 24: l_min = 82;
                l_max = 83; // KsiL0, KsiL1
                break;
            case 26: l_min = 84;
                l_max = 84; // KsiKsi
                break;
            default: // unknown particle
                break;
        }

        if (l_min > 0) {
            for (int i = l_min - 1; i < l_max; i++) {
                /*AZ
                if (Z!=la_tab[i].Z) continue;
                if (strange!=la_tab[i].strange) continue;
                if (lepton!=la_tab[i].lepton) continue;
                if (A!=la_tab[i].A) continue;
                if (mass<(la_tab[i].mass*0.99)) continue;
                if (mass>(la_tab[i].mass*1.01)) continue;
                PDG=la_tab[i].pdg;
                strncpy(name,la_tab[i].name,10);
                 */
                if (Z != fLa_tab[i]->Z) continue;
                if (strange != fLa_tab[i]->strange) continue;
                if (lepton != fLa_tab[i]->lepton) continue;
                if (A != fLa_tab[i]->A) continue;
                if (mass < (fLa_tab[i]->mass * 0.99)) continue;
                if (mass > (fLa_tab[i]->mass * 1.01)) continue;
                PDG = fLa_tab[i]->pdg;
                strncpy(name, fLa_tab[i]->name, 10);
                result = 1;
                break;
            }
        }

    } else { // must be ion

        if (A > 1) {
            PDG = CreatePdgCode(Z, A, strange, 0); //  1);   // MG 
            sprintf(name, "Ion_%d_%d", A, Z);
            result = 1;
        }

    }
    return (result);
}

// ------------------------------------------------------------------------

Bool_t MpdLAQGSMGenerator::CreateNucleus(Int_t Z, Float_t mass, Int_t pdgCode, char pdgName[11]) {

    //  const Double_t kAu2Gev=0.9314943228;       // constants from TGeant3::AddParticlesToPdgDataBase code
    const Double_t khSlash = 1.0545726663e-27; // constants from TGeant3::AddParticlesToPdgDataBase code
    const Double_t kErg2Gev = 1 / 1.6021773349e-3;
    const Double_t khShGev = khSlash*kErg2Gev;
    const Double_t kYear2Sec = 3600 * 24 * 365.25;

    Double_t width = khShGev / (12.33 * kYear2Sec); // ?   EL

    cout << "-I- MpdLAQGSMGenerator::CreateNucleus " << pdgName << " " << pdgCode << endl;

    TParticlePDG* p = 0;

    if (Z)
        p = fPDG->AddParticle(pdgName, pdgName, mass, kFALSE,
            width, Z * 3, "nucleus", pdgCode, 0, kPTHadron);
    else
        p = fPDG->AddParticle(pdgName, pdgName, mass, kFALSE,
            6.58211889e-25 / 2.6e-10, 0,
            //			  "special",pdgCode,0,kPTHadron);
            "special", pdgCode, 0, kPTNeutron); //AZ!!!

    if (p)
        p->Print();

    return (p);
}
// ------------------------------------------------------------------------


ClassImp(MpdLAQGSMGenerator)

