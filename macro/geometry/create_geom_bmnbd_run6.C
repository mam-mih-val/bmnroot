/*************************************************************************************
*  Project:    BMN (git.jinr.ru/nica/bmnroot)
*  Author:     Elena Litvinenko (EL) 
*  Version:    17-Feb-2016
************************************************************************************/ 

#include "mpdshape.class.C"

void addnum(char *name)
{
  if (name) {
    int len = strlen(name);
    if (len) {
      name[len]='#'; 
      name[len+1]='1';
      name[len+2]='\0';
    }
  }
}

void create_geom_bmnbd (const char* filename = "", const char *detector_name="bd",const char *version_index="1_0")
{
  char mothername[30]=Form("%s01",detector_name);   // "bd"

  TString Filename=filename;
  if (Filename=="")
    Filename=Form("%s_v%s\.geo",detector_name,version_index);

  // output file
  ofstream* f;
  f = new ofstream(Filename.Data(), ios::out | ios::trunc);

  cout << "filename: " << Filename.Data() << endl;


  *f << "//*****************************************************" << endl;
  *f << "// BD (BARREL) detector for BMN project," << endl;
  *f << "// geometry version " << version_index << endl;
  *f << "// generated " << __DATE__ << " by macro create_geom_bmnbd.C" << endl;
  *f << "//*****************************************************" << endl;

Int_t
  number_of_scint_strips = 40
  ;

Double_t
  thickness_steel_tube = 0.5,    // mm
  radius_external_steel_tube = 80/2,
  //AZ size_z_steel_tube = 500 ,      // mm   // ? suggested by EL
  size_z_steel_tube = 160 ,      // mm   // ? suggested by EL

  size_xy_scint_strip = 7 ,      // mm
  //AZ size_z_scint_strip = 150,      // mm
  size_z_scint_strip = 160,      // mm

  radius_internal_active_zone = 93./2, 
  //AZ pos_z_begin_of_active_zone_from_target_center = 80,
  pos_z_begin_of_active_zone_from_target_center = -80,

  size_z_delta_between_active_zone_and_external_cover = 7 , // ? suggested by EL
  radius_delta_between_active_zone_and_external_cover = 5,  // ? suggested by EL

  radius_external_mother_volume = radius_internal_active_zone + size_xy_scint_strip + 
                                  radius_delta_between_active_zone_and_external_cover, 
  size_z_mother_volume = size_z_steel_tube,
  pos_z_mother_volume =  pos_z_begin_of_active_zone_from_target_center + size_z_scint_strip + 
                         size_z_delta_between_active_zone_and_external_cover - size_z_mother_volume/2
  ;

//pos_z_mother_volume = -219; //AZ - target position
 pos_z_mother_volume = -241; //AZ - target position
 Double_t pos_x_mother_volume = 0; //AZ - target position
 Double_t pos_y_mother_volume = 8.8; //AZ - target position

// helper streams
 std::string  points, position, rotation;
 points = "0.0 0.0 0.0" ;
 position = "0.0 0.0 0.0" ; 
 rotation = "1. 0. 0. 0. 1. 0. 0. 0. 1.";

 //__________________________ BD mother volume  _______________________________________

  char mothername[30]=Form("%s01",detector_name);               // "bd01"

  Mpdshape* mother = new Mpdshape(f, mothername, "cave", "TUBE","air", points,position,rotation,1); 
  mother->Fill_TUBE(size_z_mother_volume/2,radius_external_steel_tube-thickness_steel_tube, radius_external_mother_volume );
  //AZ mother->SetPosition(0,0,pos_z_mother_volume);
  mother->SetPosition(pos_x_mother_volume, pos_y_mother_volume, pos_z_mother_volume);
  mother->DumpWithIncrement();
  addnum(mothername);   

 //__________________________ BD steel tube   _______________________________________

  char steel_tube_name[30]=Form("%s01steel",detector_name);               // "bd01steel"

  Mpdshape* steel_tube = new Mpdshape(f, steel_tube_name, mothername, "TUBE","steel", points,position,rotation,1); 
  steel_tube->Fill_TUBE(size_z_steel_tube/2,radius_external_steel_tube-thickness_steel_tube,radius_external_steel_tube);
  steel_tube->SetPosition(0,0,0);
  steel_tube->DumpWithIncrement();
  addnum(steel_tube_name);   

 //__________________________ one scintillator strip   _______________________________________

  char scint_strip_name[30]=Form("%s01strip",detector_name);               // "bd01strip"

  TGeoRotation *rot=new TGeoRotation();
  Double_t  *rotm=rot->GetRotationMatrix();
  Double_t  coord[3];

  Mpdshape* scint_strip = new Mpdshape(f, scint_strip_name,mothername, "BOX", "FscScint", points,position,rotation,1); 
  scint_strip->Fill_BOX(size_xy_scint_strip/2, size_xy_scint_strip/2, size_z_scint_strip/2);
  coord[0] = 0;
  coord[1] = radius_internal_active_zone+size_xy_scint_strip/2;
  //AZ coord[2] = pos_z_begin_of_active_zone_from_target_center+size_z_scint_strip/2-pos_z_mother_volume;
  coord[2] = 0;
  scint_strip->SetPosition(coord[0],coord[1],coord[2]);
  scint_strip->SetRotation(rotm[0],rotm[1],rotm[2],rotm[3],rotm[4],rotm[5],rotm[6],rotm[7],rotm[8]);
  scint_strip->DumpWithIncrement();

 //__________________________ other scintillator strips   _______________________________________

  Double_t  
    phi = 90*TMath::DegToRad(),
    delta_phi = 360./number_of_scint_strips,
    dphi = delta_phi*TMath::DegToRad();

  for (int i=1;i<number_of_scint_strips; i++ ) {
    rot->RotateZ(-delta_phi);
    rotm=rot->GetRotationMatrix();
    scint_strip->SetRotation(rotm[0],rotm[1],rotm[2],rotm[3],rotm[4],rotm[5],rotm[6],rotm[7],rotm[8]);
    phi -= dphi;
    scint_strip->SetPosition(coord[1]*TMath::Cos(phi),coord[1]*TMath::Sin(phi),coord[2]);
    scint_strip->DumpWithIncrement();
  }

  f->close(); 

}
