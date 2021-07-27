// Configuration macro for Geant3 VirtualMC
void Config()
{
    FairRunSim* fRun = FairRunSim::Instance();

    TString* gModel = fRun->GetGeoModel();
    TGeant3* geant3 = NULL;
    if (strncmp(gModel->Data(), "TGeo", 4) == 0)
    {
        geant3 = new  TGeant3TGeo("C++ Interface to Geant3");
        cout << "-I- G3Config: Geant3 with TGeo has been created." << endl;
    }
    else
    {
        geant3 = new  TGeant3("C++ Interface to Geant3");
        cout << "-I- G3Config: Geant3 native has been created." << endl;
    }

    // create Cbm Specific Stack
    CbmStack* st = new CbmStack();
    // Set minimum number of points to store the track
    // The default value is one, which means each track needs at least 1 point in any detector
    st->SetMinPoints(1);
    geant3->SetStack(st);

    // ******* GEANT3  specific configuration for simulated Runs  *******
    geant3->SetTRIG(1);         //Number of events to be processed
    geant3->SetSWIT(4, 100);
    geant3->SetDEBU(0, 0, 1);
  
    geant3->SetRAYL(1);
    geant3->SetSTRA(0); //1);
    geant3->SetAUTO(0);         //Select automatic STMIN etc... calc. (AUTO 1) or manual (AUTO 0)
    geant3->SetABAN(0);         //Restore 3.16 behaviour for abandoned tracks
    geant3->SetOPTI(2);         //Select optimisation level for GEANT geometry searches (0,1,2)
    geant3->SetERAN(5.e-7);
    geant3->SetCKOV(1);     // cerenkov photons 
}
