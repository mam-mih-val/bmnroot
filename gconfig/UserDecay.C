void SetMode1_for_PDG(Int_t pdg, Float_t b0,Int_t m00,Int_t m01)
{
  Int_t mode[6][3];                  
  Float_t bratio[6];

  for (Int_t kz = 0; kz < 6; kz++) {
    bratio[kz] = 0.;
    mode[kz][0] = 0;
    mode[kz][1] = 0;
    mode[kz][2] = 0;
  }

  bratio[0] = b0; mode[0][0] = m00; mode[0][1] = m01;

  gMC->SetDecayMode(pdg,bratio,mode);
}

void SetMode2_for_PDG(Int_t pdg, Float_t b0,Int_t m00,Int_t m01,Int_t m02,Float_t b1,Int_t m10,Int_t m11,Int_t m12)
{
  Int_t mode[6][3];                  
  Float_t bratio[6];

  for (Int_t kz = 0; kz < 6; kz++) {
    bratio[kz] = 0.;
    mode[kz][0] = 0;
    mode[kz][1] = 0;
    mode[kz][2] = 0;
  }

  bratio[0] = b0; mode[0][0] = m00; mode[0][1] = m01; mode[0][2] = m02;
  bratio[0] = b1; mode[1][0] = m10; mode[1][1] = m11; mode[1][2] = m12;

  gMC->SetDecayMode(pdg,bratio,mode);
}

void UserDecayConfig() {
  cout << "Loading User Decay Config from macro"<< endl;  
  TDatabasePDG *db= TDatabasePDG::Instance();
  TParticlePDG *p=0;

  Int_t mode[6][3];                  
  Float_t bratio[6];
  Int_t AlphaPDG, He5PDG, He3, H3L, H4L;

  p= db->GetParticle("Alpha");
  if (!p) {
    p = db->GetParticle("alpha");
    if (!p) {
      p = db->GetParticle("He4");
    }
  }
  if(p)
    AlphaPDG=p->PdgCode();

  cout << "!!!! 0-0 !!!!!!! " <<  (db->ParticleList()->GetEntries()) << " " << AlphaPDG << endl;
    
  p = db->GetParticle("He3");
  if (!p) 
    p = db->GetParticle("HE3");
  if (p) 
    He3 = p->PdgCode();
  else {
    if (!He3) {
      He3=1000020030;
      db->AddParticle("HE3","HE3", 2.80923,kFALSE,0,6,"Ion",He3);
    }
  }
  cout << "!!!! 0-1 !!!!!!! " <<  (db->ParticleList()->GetEntries()) << " " << He3 << endl;

  if (He3) {
    p = db->GetParticle("H3L");
    if (p)  {
      H3L = p->PdgCode();
    }
    else {
      H3L = 1010010030;
      gMC->DefineParticle(H3L, "H3L", kPTHadron, 2.99131 , 1.0, 2.632e-10,"Ion", 0.0, 0, 1, 0, 0, 0, 0, 0, 3, kFALSE);
    }
    SetMode1_for_PDG(H3L,100,-211,He3);

    p = db->GetParticle("H4L");
    if (p)  {
      H4L = p->PdgCode();
    }
    else {
      H4L = 1010010040;
      gMC->DefineParticle(H3L, "H4L", kPTHadron, 3.92503 , 1.0, 2.632e-10,"Ion", 0.0, 0, 1, 0, 0, 0, 0, 0, 4, kFALSE);
    }
    SetMode1_for_PDG(H4L,100,-211,AlphaPDG);

  }


  p = db->GetParticle("LN");
  if (!p) {
    p = db->GetParticle("LambdaNeutron");
    if (!p) {
      gMC->DefineParticle(1010000020, "LN", kPTNeutron, 2.054 , 0.0, 2.632e-10,"Ion", 0.0, 0, 1, 0, 0, 0, 0, 0, 2, kFALSE);
      p = db->GetParticle("LN");
    }
  }
  if (p)
    SetMode1_for_PDG(p->PdgCode(),100,1000010020,-211);


  cout << "!!!! 1-0 !!!!!!! " <<  (db->ParticleList()->GetEntries()) << " " << H3L<< " " << H4L << endl;
   
}

