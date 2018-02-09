/* TODO:
1) Change title font to all be matching
2) FIX scales on reference and current histograms to have same scales
*/
#include "TVector3.h"
#include <TApplication.h>
#include <TGClient.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <TGStatusBar.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGraph.h>
#include <TAxis.h>

class MyMainFrame : public TGMainFrame {
   private:
      TRootEmbeddedCanvas  *fEcan;
      TGStatusBar          *fStatusBar;
   public:
      MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h);
      virtual ~MyMainFrame();
      int graph;
      int divide;
      void DoExit();
      void DoNext();
      void SetStatusText(const char *txt, Int_t pi);
      void EventInfo(Int_t event, Int_t px, Int_t py, TObject *selected);
	ClassDef(MyMainFrame, 0)
};


void MyMainFrame::DoNext(){
	if( graph == 14){
		cout << "At end, looping back to start...\n";
		graph = -1;
	}
	
	// Create canvas to draw on
	TCanvas *c1 = fEcan->GetCanvas();
	c1->Clear();
	c1->SetFillColor(0);
	c1->SetGrid();

	graph++; 
	/// Open files that contain all histograms
	//TFile *inFile_curr_TQDC = new TFile("det_histos_curr_TQDC.root");
	//TFile *inFile_curr_TDC = new TFile("det_histos_curr_TDC.root");
	//TFile *inFile_ref_TQDC = new TFile("det_histos_ref_TQDC.root");
	//TFile *inFile_ref_TDC = new TFile("det_histos_ref_TDC.root");
	
	int currTDC_lineCol, currTQDC_lineCol;
	int refTDC_lineCol, refTQDC_lineCol;
	bool doLegend;
	TString xaxisLab;
	TString yaxisLab;
	int maxLine, maxBin;

	cout << "GRAPH: " << graph << "\n";
	if ( (graph > -1) && (graph < 3) ) c1->Divide(2,1);
	if( graph == 0){
		cout << "\tLooking at Electronic Efficiencies...\n";
		TString choice = "moduleEff";
		TString title = "Efficiency Relative to Total Triggers";
		currTDC_lineCol  = 1;
		currTQDC_lineCol = 1;
		refTDC_lineCol = 1;
		refTQDC_lineCol = 1;
		doLegend = false;
		yaxisLab = "Efficiency [%]";
		xaxisLab = "Module";
		maxLine = 100;
		maxBin = 2;
	}
	if( graph == 1){
		cout << "\tLooking at Sub-Trigger Efficiencies...\n";
		TString choice = "coinScalars";
		TString title = "Scalars of Sub-Trigger Components";
		currTDC_lineCol = 46;
		currTQDC_lineCol = 9;
		refTDC_lineCol = 46;
		refTQDC_lineCol = 9;
		doLegend = true;
		yaxisLab = "Entries per second";
		xaxisLab = "Sub-Triggers";

		maxBin = 11;
	}
	if( graph == 2){
		cout << "\tLooking at Detector Scalars...\n";
		TString choice = "trigScalars";
		TString title = "Scalars of Trigger Detectors";
		currTDC_lineCol = 46;
		currTQDC_lineCol = 9;
		refTDC_lineCol = 46;
		refTQDC_lineCol = 9;
		doLegend = true;
		yaxisLab = "Entries per second";
		xaxisLab = "Detectors";

		maxBin = 12;
	}

	if( (graph >-1) && (graph<3)){

		TFile *inFile_curr_TQDC = new TFile("../output/det_histos_curr_TQDC_BCs.root");
		TFile *inFile_ref_TQDC = new TFile("../output/det_histos_ref_TQDC_BCs.root");
		TFile *inFile_curr_TDC = new TFile("../output/det_histos_curr_TDC_BCs.root");
		TFile *inFile_ref_TDC = new TFile("../output/det_histos_ref_TDC_BCs.root");
		
		TH1D * hist_TQDC = (TH1D*) inFile_curr_TQDC->Get(choice);
		TH1D * hist_TDC  = (TH1D*) inFile_curr_TDC->Get(choice);
		TH1D * ref_hist_TQDC = (TH1D*) inFile_ref_TQDC->Get(choice);
		TH1D * ref_hist_TDC  = (TH1D*) inFile_ref_TDC->Get(choice);
		
		if( (graph==2) || (graph==1)){
			TVector3 *runInfo = NULL;
			runInfo = (TVector3*) inFile_curr_TDC->Get("runInfo");
			maxLine = (*runInfo)[1];
		}		
		hist_TDC->SetLineColor(currTDC_lineCol);
		hist_TDC->SetTitle("Current Run: "+title);
		hist_TDC->GetXaxis()->SetTitle(xaxisLab);
		hist_TDC->GetYaxis()->SetTitle(yaxisLab);
		hist_TDC->GetYaxis()->SetTitleOffset(1.6);
		hist_TDC->SetLineWidth(3);
		hist_TDC->SetStats(0);
		
		hist_TQDC->SetLineColor(currTQDC_lineCol);
		hist_TQDC->SetLineWidth(3);		
		hist_TQDC->GetYaxis()->SetTitleOffset(1.6);
		
		ref_hist_TQDC->SetLineColor(refTQDC_lineCol);
		ref_hist_TQDC->GetYaxis()->SetTitleOffset(1.6);
		
		ref_hist_TDC->SetLineColor(refTDC_lineCol);
		ref_hist_TDC->SetTitle("Reference Run: "+title);
		ref_hist_TDC->GetXaxis()->SetTitle(xaxisLab);
		ref_hist_TDC->GetYaxis()->SetTitle(yaxisLab);
		ref_hist_TDC->GetYaxis()->SetTitleOffset(1.6);
		ref_hist_TDC->SetStats(0);
		
		TLine *line = new TLine(0,maxLine,maxBin,maxLine);
		line->SetLineColor(kRed);
		line->SetLineStyle(9);

		if( doLegend ){
			auto legend = new TLegend(0.75,0.1,0.9,0.2);
			legend->AddEntry(hist_TQDC,"TQDC","L");
			legend->AddEntry(hist_TDC,"TDC","L");
		}
		
		c1->cd(1);
		hist_TDC->Draw();
		hist_TQDC->Draw("same");
		line->Draw("same");
		if ( doLegend ) legend->Draw();
		if ( graph > 0 ) gPad->SetLogy();
		if ( hist_TQDC->GetMaximum() > hist_TDC->GetMaximum() ) hist_TDC->SetMaximum(hist_TQDC->GetMaximum()*1.1);
		c1->cd(2);
		ref_hist_TDC->Draw();
		ref_hist_TQDC->Draw("same");
		line->Draw("same");
		if ( doLegend ) legend->Draw();
		if ( graph > 0 ) gPad->SetLogy();
		if (ref_hist_TQDC->GetMaximum() > ref_hist_TDC->GetMaximum() ) ref_hist_TDC->SetMaximum(ref_hist_TQDC->GetMaximum()*1.1);
	}
	
	
	if( graph == 3){
		cout << "\tReading Detector BC1\n";
		TString choice = "_BC1";
		TString title = " of BC1";
	}
	else if(graph == 4){
		cout << "\tReading Detector BC2\n";
		TString choice = "_BC2";
		TString title = " of BC2";
	}
	else if( graph == 5){
		cout << "\tReading Detector BC3\n";
		TString choice = "_BC3";
		TString title = " of BC3";
	}
	else if( graph == 6){
		cout << "\tReading Detector VC\n";
		TString choice = "_VC";
		TString title = " of VC";
	}
	else if( graph == 7){
		cout << "\tReading Detector X1 left\n";
		TString choice = "_X1_left";
		TString title = " of X1 Left";
	}
	else if( graph == 8){
		cout << "\tReading Detector X1 right\n";
		TString choice = "_X1_right";
		TString title = " of X1 Right";
	}
	else if( graph == 9){
		cout << "\tReading Detector X2 left\n";
		TString choice = "_X2_left";
		TString title = " of X2 Left";
	}
	else if( graph == 10){
		cout << "\tReading Detector X2 right\n";
		TString choice = "_X2_right";
		TString title = " of X2 Right";
	}
	else if( graph == 11){
		cout << "\tReading Detector Y1 left\n";
		TString choice = "_Y1_left";
		TString title = " of Y1 Left";
	}
	else if( graph == 12){
		cout << "\tReading Detector Y1 right\n";
		TString choice = "_Y1_right";
		TString title = " of Y1 Right";
	}
	else if( graph == 13){
		cout << "\tReading Detector Y2 left\n";
		TString choice = "_Y2_left";
		TString title = " of Y2 Left";
	}
	else if( graph == 14){
		cout << "\tReading Detector Y2 right\n";
		TString choice = "_Y2_right";
		TString title = " of Y2 Right";
	}
	if( (graph < 15) && (graph>2)){
		if ( graph < 7 ){
			TFile *inFile_curr_TQDC = new TFile("../output/det_histos_curr_TQDC_BCs.root");
			TFile *inFile_ref_TQDC = new TFile("../output/det_histos_ref_TQDC_BCs.root");
			TFile *inFile_curr_TDC = new TFile("../output/det_histos_curr_TDC_BCs.root");
			TFile *inFile_ref_TDC = new TFile("../output/det_histos_ref_TDC_BCs.root");
		}
		else{
			TFile *inFile_curr_TQDC = new TFile("../output/det_histos_curr_TQDC_Arms.root");
			TFile *inFile_ref_TQDC = new TFile("../output/det_histos_ref_TQDC_Arms.root");
			TFile *inFile_curr_TDC = new TFile("../output/det_histos_curr_TDC_Arms.root");
			TFile *inFile_ref_TDC = new TFile("../output/det_histos_ref_TDC_Arms.root");
		}
		c1->Divide(3,2);
		for (int i=0;i<6;i++){
			TString graph_choice;
			TString graph_title;
			if (i==0){
				graph_choice = "hits" + choice;
				graph_title = "From TQDC: Num Hits in Window" + title;
				TH1D * hist = (TH1D*) inFile_curr_TQDC->Get(graph_choice);
				TH1D * ref_hist = (TH1D*) inFile_ref_TQDC->Get(graph_choice);
			}
			else if (i==1){
				graph_choice = "QDC" + choice;
				graph_title = "From TQDC: QDC" + title;
				TH1D * hist = (TH1D*) inFile_curr_TQDC->Get(graph_choice);
				TH1D * ref_hist = (TH1D*) inFile_ref_TQDC->Get(graph_choice);
			}
			else if(i==2){
				graph_choice = "ADC" + choice;
				graph_title = "From TQDC: ADC" + title;
				TH1D * hist = (TH1D*) inFile_curr_TQDC->Get(graph_choice);
				TH1D * ref_hist = (TH1D*) inFile_ref_TQDC->Get(graph_choice);
			}
			else if(i==3){
				graph_choice = "hits" + choice;
				graph_title = "From TDC: Num Hits in Window" + title;
				TH1D * hist = (TH1D*) inFile_curr_TDC->Get(graph_choice);
				TH1D * ref_hist = (TH1D*) inFile_ref_TDC->Get(graph_choice);
			}
			else if(i==5){
				graph_choice = "TDC" + choice;
				graph_title = "From TQDC: TDC" + title +" Relative to Trigger";
				TH1D * hist = (TH1D*) inFile_curr_TQDC->Get(graph_choice);
				TH1D * ref_hist = (TH1D*) inFile_ref_TQDC->Get(graph_choice);
			}
			else if(i==4){
				graph_choice = "TDC" + choice;
				graph_title = "From TDC: TDC" + title + " Relative to Trigger";
				TH1D * hist = (TH1D*) inFile_curr_TDC->Get(graph_choice);
				TH1D * ref_hist = (TH1D*) inFile_ref_TDC->Get(graph_choice);
			}
			
			hist->SetLineColor(46);
			hist->SetLineWidth(3);
			hist->SetTitle(graph_title);
			hist->GetXaxis()->SetTitle("Channel");
			hist->GetYaxis()->SetTitle("Entries");
			hist->GetYaxis()->SetTitleOffset(1.4);
			hist->SetStats(0);
			hist->SetLineColorAlpha(46,0.35);

			ref_hist->SetLineColor(1);
			auto legend = new TLegend(0.7,0.8,0.9,0.9);
			legend->AddEntry(hist,"Current","L");
			legend->AddEntry(ref_hist,"Reference","L");
			
			c1->cd(i+1);
			hist->Draw();
			ref_hist->Draw("same");
			legend->Draw();
			double refMax = ref_hist->GetMaximum();
			double hisMax = hist->GetMaximum();
			if (refMax > hisMax) hist->SetMaximum(refMax*1.1);
		}
	}
	
	gPad->SetLeftMargin(0.15);
	
	c1->Update();
	c1->GetFrame()->SetFillColor(21);
	c1->GetFrame()->SetBorderSize(12);
	c1->Modified();
	c1->Update();
}

void MyMainFrame::DoExit()
{
   printf("Exit application...\n");
   gApplication->Terminate(0);
}

void MyMainFrame::SetStatusText(const char *txt, Int_t pi)
{
   // Set text in status bar.
   fStatusBar->SetText(txt,pi);
}

void MyMainFrame::EventInfo(Int_t event, Int_t px, Int_t py, TObject *selected)
{
//  Writes the event status in the status bar parts
   const char *text0, *text1, *text3;
   char text2[50];
   text0 = selected->GetTitle();
   SetStatusText(text0,0);
   text1 = selected->GetName();
   SetStatusText(text1,1);
   if (event == kKeyPress)
      sprintf(text2, "%c", (char) px);
   else
      sprintf(text2, "%d,%d", px, py);
   SetStatusText(text2,2);
   text3 = selected->GetObjectInfo(px,py);
   SetStatusText(text3,3);
}
MyMainFrame::MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h) :
   TGMainFrame(p, w, h)
{
   graph = -1;
   divide = -1;
	// Create the embedded canvas
   fEcan = new TRootEmbeddedCanvas(0,this,1250,1000);
   Int_t wid = fEcan->GetCanvasWindowId();
   TCanvas *myc = new TCanvas("MyCanvas", 10,10,wid);
   fEcan->AdoptCanvas(myc);
   myc->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)","MyMainFrame",this,
               "EventInfo(Int_t,Int_t,Int_t,TObject*)");
   AddFrame(fEcan, new TGLayoutHints(kLHintsTop | kLHintsLeft |
                                     kLHintsExpandX  | kLHintsExpandY,0,0,1,1));
   // status bar
   Int_t parts[] = {45, 15, 10, 30};
   fStatusBar = new TGStatusBar(this, 50, 10, kVerticalFrame);
   fStatusBar->SetParts(parts, 4);
   fStatusBar->Draw3DCorner(kFALSE);
   AddFrame(fStatusBar, new TGLayoutHints(kLHintsExpandX, 0, 0, 10, 0));

   // Create a horizontal frame containing two buttons
   TGHorizontalFrame *hframe = new TGHorizontalFrame(this, 200, 40);
  
      // First button
   TGTextButton *draw = new TGTextButton(hframe, "&Next Detector");
   draw->Connect("Clicked()", "MyMainFrame", this, "DoNext()");
   hframe->AddFrame(draw, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
      // Second button
   TGTextButton *exit = new TGTextButton(hframe, "&Exit GUI");
   exit->Connect("Pressed()", "MyMainFrame", this, "DoExit()");
   hframe->AddFrame(exit, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

   AddFrame(hframe, new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));


   // Set a name to the main frame
   SetWindowName("Embedded Canvas Status Info");
   MapSubwindows();
   // Initialize the layout algorithm via Resize()
   Resize(GetDefaultSize());
   // Map main frame
   MapWindow();
}
MyMainFrame::~MyMainFrame()
{
   // Clean up main frame...
   Cleanup();
   delete fEcan;
}
void monGUI()
{
   // Popup the GUI...
   new MyMainFrame(gClient->GetRoot(), 200, 200);
}
