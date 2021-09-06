/* 
 * File:   delete_Tof1_INL.c
 * Author: mikhail
 *
 * Created on January 16, 2017, 2:48 PM
 */



void delete_Tof1_INL(int sP = 5, int sR = 419, int eP = 5, int eR = 1014) {
   //clear data from DB
    UniDbDetectorParameter::DeleteDetectorParameter("TOF1","inl",sP, sR, eP, eR);
    //delete parameter
    UniDbParameter::DeleteParameter("inl");

}
