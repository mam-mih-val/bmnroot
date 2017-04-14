/*************************************************************************************
*  Project:    BMN (git.jinr.ru/nica/bmnroot)
*  Author:     Elena Litvinenko (EL) 
*  Adopted for BMN using the code written by Boiana Marinova (BM)
*  Version:    16-Feb-2016
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

void create_geom_bmnecal (const char* filename = "", const char *detector_name="ecal",const char *version_index="1_0")
{
  char mothername[30]=Form("%s01",detector_name);   // "ecal"

  TString Filename=filename;
  if (Filename=="")
    Filename=Form("%s_v%s\.geo",detector_name,version_index);

  // output file
  ofstream* f;
  f = new ofstream(Filename.Data(), ios::out | ios::trunc);

  cout << "filename: " << Filename.Data() << endl;


  *f << "//*****************************************************" << endl;
  *f << "// ECAL detector for BMN project," << endl;
  *f << "// geometry version " << version_index << endl;
  *f << "// generated " << __DATE__ << " by macro create_geom_bmnecal.C" << endl;
  *f << "//*****************************************************" << endl;

Int_t
  number_of_modules_in_one_segment_along_X = 8,
  number_of_modules_in_one_segment_along_Y = 10,
  number_of_layers = 220;

Double_t

  thickness_lead = 0.3,    // mm
  thickness_scint = 1.5,   // mm
  half_thickness_layer = (thickness_lead+thickness_scint)/2,
  thickness_module_front_cover = 2,

  size_xy_scint = 40,      // 4x4 cm^2
  size_xy_module = 120,    // == size_xy_lead
  size_z_module =  400, 
  size_z_module_delta = 150,  // 150, // 15 cm reserved for PMT and FEE   // EL
  size_x_segment = size_xy_module * number_of_modules_in_one_segment_along_X,
  size_y_segment = size_xy_module * number_of_modules_in_one_segment_along_Y,
  size_z_segment = size_z_module+size_z_module_delta,

  pos_x_segment_center = -300-size_x_segment/2,
  pos_y_segment_center = 0,
  pos_z_segment_center = 7200,    // 720 cm
  pos_x_beam_in_ecal_place = 300  //  30 cm
  ;

// helper streams
 std::string  points, position, rotation;
 points = "0.0 0.0 0.0" ;
 position = "0.0 0.0 0.0" ; 
 rotation = "1. 0. 0. 0. 1. 0. 0. 0. 1.";

  //__________________________One ECAL segment (chamber) _______________________________________

  char mothername[30]=Form("%s01",detector_name);               // "ecal01"

  Mpdshape* mother = new Mpdshape(f, mothername, "cave", "BOX","air", points,position,rotation,1); 
  mother->Fill_BOX(size_x_segment/2, size_y_segment/2, size_z_segment/2 );
  mother->SetPosition(pos_x_beam_in_ecal_place+pos_x_segment_center, pos_y_segment_center, pos_z_segment_center);
  mother->DumpWithIncrement();
  addnum(mothername);   
         
  //__________________________One ECAL module (120x120) _______________________________________

  char modulename[30]     = Form("%s01mod",detector_name);      // "ecal01mod"

 Mpdshape* module = new Mpdshape(f, modulename, mothername, "BOX", "air", points,position,rotation,1); 
 module -> Fill_BOX(size_xy_module/2, size_xy_module/2, size_z_module/2);
 module -> SetPosition(size_x_segment/2-size_xy_module/2, size_y_segment/2-size_xy_module/2, -size_z_module_delta/2);
 module->DumpWithIncrement();
 addnum (modulename);
         
  //_____________ Front cover in one module  _____________

  Mpdshape* front_cover = new Mpdshape(f, "ecal01front",modulename, "BOX", "steel", points,position,rotation,1); 
  front_cover->Fill_BOX(size_xy_module/2, size_xy_module/2, thickness_module_front_cover/2);
  front_cover->SetPosition(0, 0, -size_z_module/2+thickness_module_front_cover/2);
  front_cover->DumpWithIncrement();

  //_____________ One ECAL layer (120x120) _____________

  char layername[30]     = Form("%s01lay",detector_name);      // "ecal01lay"

  Mpdshape* layer = new Mpdshape(f, layername,modulename, "BOX", "air", points,position,rotation,1); 
  layer->Fill_BOX(size_xy_module/2, size_xy_module/2, half_thickness_layer);
  layer->SetPosition(0, 0, -size_z_module/2+thickness_module_front_cover+half_thickness_layer);
  layer->DumpWithIncrement();
  addnum (layername);

  Mpdshape* lead = new Mpdshape(f, "ecal01lead",layername, "BOX", "lead",points,position,rotation,1); 
  lead-> Fill_BOX(size_xy_module/2, size_xy_module/2, thickness_lead/2); 
  lead->SetPosition(0, 0,-thickness_scint/2); 
  lead->DumpWithIncrement();

  Mpdshape* scint = new Mpdshape(f, "ecal01scint",layername, "BOX", "FscScint",points,position,rotation,1); 
  scint-> Fill_BOX(size_xy_scint/2, size_xy_scint/2, thickness_scint/2); 
  scint->SetPosition(size_xy_module/2-size_xy_scint/2, size_xy_module/2-size_xy_scint/2,thickness_lead/2); 
  scint->DumpWithIncrement();

  for (int i=0;i<3;i++) {
    for (int j=0;j<3;j++) {
      if (i||j) {
	scint->SetPosition(size_xy_module/2-size_xy_scint/2-size_xy_scint*i, size_xy_module/2-size_xy_scint/2-size_xy_scint*j,thickness_lead/2); 
	scint->DumpWithIncrement();
      }}
  }

  //_________Other ECAL layers (120x120) __________________

  for (int i=1;i<number_of_layers;i++) {
    layer->SetPosition(0, 0, -size_z_module/2+thickness_module_front_cover+half_thickness_layer+i*2*half_thickness_layer);
    layer->DumpWithIncrement();
  }


  //__________________________Other ECAL modules (120x120) _______________________________________

  for (int i=0;i<number_of_modules_in_one_segment_along_X;i++) {
   for (int j=0;j<number_of_modules_in_one_segment_along_Y;j++) {
     if (i||j) {
       module -> SetPosition(size_x_segment/2-size_xy_module/2-size_xy_module*i, size_y_segment/2-size_xy_module/2-size_xy_module*j, -size_z_module_delta/2);
       module->DumpWithIncrement();
     }
   }
 }   

  //__________________________Second ECAL segment (chamber) _______________________________________

  mother->SetPosition(pos_x_beam_in_ecal_place-pos_x_segment_center, pos_y_segment_center, pos_z_segment_center);
  //  mother->SetRotation("-1. 0. 0. 0. 1. 0. 0. 0. 1.");
  mother->DumpWithIncrement();

  f->close(); 

}

