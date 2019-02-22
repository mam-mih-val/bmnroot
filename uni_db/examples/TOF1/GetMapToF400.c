void GetMapToF400(Int_t Periud=6, Int_t Run=1985)
{
     BmnTof1Raw2Digit* tof = new BmnTof1Raw2Digit(Periud, Run);
     tof->saveMapToFile("TOF400_PlaseMap_period6_fromDB.txt","TOF400_StripMap_period6_fromDB.txt");
     tof->~BmnTof1Raw2Digit();

}

