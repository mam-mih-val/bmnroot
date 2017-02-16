void checkBmnTrackExtrapolator()
{
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load libraries
    
    Double_t fieldScale = 1.;
    BmnFieldMap* magField = new BmnNewFieldMap("field_sp41v3_ascii_Extrap.dat");
    magField->SetScale(fieldScale);
    magField->Init();
    magField->Print();
    
    FairRunAna *fRun = new FairRunAna();
    fRun->SetField(magField);
    
    BmnTrackExtrapolator* extrapolator = new BmnTrackExtrapolator();
    const FairTrackParam fairTrackParam = FairTrackParam();
    fairTrackParam.SetX(0);
    fairTrackParam.SetY(0);
    fairTrackParam.SetZ(0);
    fairTrackParam.SetTx(0);
    fairTrackParam.SetTy(0);
    fairTrackParam.SetQp((Double_t)(1.0/9.78));
    
    FairTrackParam fairTrackParamRKTrans = FairTrackParam(fairTrackParam);
    extrapolator->RKTrans(&fairTrackParamRKTrans, 542.0);
    cout << "RKTrans"   << endl;
    cout << "x_out = "  << fairTrackParamRKTrans.GetX() << ", y_out = " << fairTrackParamRKTrans.GetY() << endl;
    cout << "tx_out = " << fairTrackParamRKTrans.GetTx() << ", ty_out = " << fairTrackParamRKTrans.GetTy() << endl;
    cout << "1/Qp = "   << 1/fairTrackParamRKTrans.GetQp() << endl;
    
    FairTrackParam fairTrackParamRK5Order = FairTrackParam(fairTrackParam);
    extrapolator->RK5Order(&fairTrackParamRK5Order, 542.0);
    cout << "RK5Order"  << endl;
    cout << "x_out = "  << fairTrackParamRK5Order.GetX() << ", y_out = " << fairTrackParamRK5Order.GetY() << endl;
    cout << "tx_out = " << fairTrackParamRK5Order.GetTx() << ", ty_out = " << fairTrackParamRK5Order.GetTy() << endl;
    cout << "1/Qp = "   << 1/fairTrackParamRK5Order.GetQp() << endl;
    
    FairTrackParam fairTrackParamRK5Clip = FairTrackParam(fairTrackParam);
    extrapolator->RK5Clip(&fairTrackParamRK5Clip, 542.0);
    cout << "RK5Clip"   << endl;
    cout << "x_out = "  << fairTrackParamRK5Clip.GetX() << ", y_out = " << fairTrackParamRK5Clip.GetY() << endl;
    cout << "tx_out = " << fairTrackParamRK5Clip.GetTx() << ", ty_out = " << fairTrackParamRK5Clip.GetTy() << endl;
    cout << "1/Qp = "   << 1/fairTrackParamRK5Clip.GetQp() << endl;
    
    delete magField;
    delete extrapolator;
}