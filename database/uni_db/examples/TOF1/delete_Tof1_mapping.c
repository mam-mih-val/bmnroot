//Macro for delete all map from database
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
//sP is start period
//sR is start run
//eP is end period
//eR is end run

void delete_Tof1_mapping (int sP = 5, int sR = 419, int eP = 5, int eR = 1014)
{
  char anwser;

  cout << endl << "!!!!!!!  Delete of TOF400 map from database  !!!!!!!" << endl << endl;
  cout << "Start Period = " << sP << ";  Stop Period = " << eP << endl;
  cout << "Start Run = " << sR << ";  Stop Run = " << eR << endl;
  cout << "Okay to delete the map of TOF400? (Y/N)" << endl;
  cin >> anwser;

  if (anwser == 'y' || anwser == 'Y')
    {
      Int_t t;
      t = UniDbDetectorParameter::DeleteDetectorParameter ("TOF1", "plane", sP, sR, eP, eR);
      if (t == 0) cout << endl << "TOF1 plane are deleted from database" << endl;
      else cout << "TOF1 plane are NOT deleted from database" << endl;

      t = UniDbDetectorParameter::DeleteDetectorParameter ("TOF1", "strip", sP, sR, eP, eR);
      if (t == 0) cout << endl << "TOF1 strip are deleted from database" << endl;
      else cout << "TOF1 strip are NOT deleted from database" << endl;

      t = UniDbDetectorParameter::DeleteDetectorParameter ("TOF1", "side", sP, sR, eP, eR);
      if (t == 0) cout << endl << "TOF1 side are deleted from database" << endl;
      else cout << "TOF1 side are NOT deleted from database" << endl;
      
      t = UniDbDetectorParameter::DeleteDetectorParameter ("TOF1", "placementmap", sP, sR, eP, eR);
      if (t == 0) cout << endl << "TOF1 placementmap are deleted from database" << endl;
      else cout << "TOF1 placementmap are NOT deleted from database" << endl;
    }
}
