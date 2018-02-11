{
TProfile *hproft = new TProfile("hproft","Time vs chamber",60,0,60,20.5,24.5,"s");
TProfile *hprofl = new TProfile("hprofl","Length vs chamber",60,0,60,600.,700.,"s");
TCanvas *ctof = new TCanvas("ctof","MC TOF700 hists",900,700);
ctof->Divide(2,2);
ctof->cd(1);
cbmsim->Draw("(TOFPoint.fDetectorID&0xFF00)/256 >> hc(60,0,60)","(TOFPoint.fTrackID == 0)");
ctof->cd(2);
cbmsim->Draw("(TOFPoint.fDetectorID&0xFF) >> hs12(32,0,32)","(TOFPoint.fDetectorID&0xFF00)/256 == 19 && (TOFPoint.fTrackID == 0)");
//cbmsim->Draw("(TOFPoint.fDetectorID&0xFF) >> hs13(32,0,32)","(TOFPoint.fDetectorID&0xFF00)/256 == 20 && (TOFPoint.fTrackID == 0)");
ctof->cd(3);
cbmsim->Draw("TOFPoint.fLength>>hl(2000,600,700)","(TOFPoint.fDetectorID&0xFF00)/256 == 19 && (TOFPoint.fTrackID == 0)");
//cbmsim->Draw("TOFPoint.fLength>>hl(1000,0,1000)");
ctof->cd(4);
cbmsim->Draw("TOFPoint.fTime>>ht(1600,20.5,24.5)","(TOFPoint.fDetectorID&0xFF00)/256 == 19 && (TOFPoint.fTrackID == 0)");
//cbmsim->Draw("TOFPoint.fTime>>ht(1000,0,25)");
TCanvas *cpro = new TCanvas("cpro","MC TOF700 times & length vs chamber",900,700);
cpro->Divide(1,2);
cpro->cd(1);
cbmsim->Draw("TOFPoint.fTime:(TOFPoint.fDetectorID&0xFF00)/256>>hproft","(TOFPoint.fTrackID == 0)");
cpro->cd(2);
cbmsim->Draw("TOFPoint.fLength:(TOFPoint.fDetectorID&0xFF00)/256>>hprofl","(TOFPoint.fTrackID == 0)");
hproft->Print("range");
hprofl->Print("range");
}
