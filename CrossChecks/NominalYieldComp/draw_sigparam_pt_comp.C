#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <TString.h>
#include <string>
#include <cstring>
#include <math.h>

#include <TROOT.h>
#include "TSystem.h"
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TNtuple.h>
#include <TMath.h>
#include <math.h>
#include <TH1.h>
#include <TH2.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include "TClonesArray.h"
#include <TAxis.h>
#include <cmath>
#include <TLorentzRotation.h>

#include <TCanvas.h>
#include <TStyle.h>
#include <TPaveStats.h>
#include <TGraphErrors.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TRandom.h>

#include <RooFit.h>
#include <RooGlobalFunc.h>
#include <RooCategory.h>
#include <RooGenericPdf.h>
#include <RooFFTConvPdf.h>
#include <RooWorkspace.h>
#include <RooBinning.h>
#include <RooHistPdf.h>
#include <RooProdPdf.h>
#include <RooAddPdf.h>
#include <RooRealVar.h>
#include <RooDataSet.h>
#include <RooHist.h>
#include <RooFitResult.h>
#include <RooPlot.h>
#include <RooConstVar.h>
#include "../../SONGKYO.h"
#include "../../commonUtility.h"

using namespace std;
using namespace RooFit;

int draw_sigparam_pt_comp(TString szAA = "PA", int states =2, int DrawOpt = 0) 
{
  /////////////////////////////////////////////////////////
  //// set style
  /////////////////////////////////////////////////////////
  
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1);
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  gStyle->SetTitleFillColor(0);
  gStyle->SetStatColor(0);

  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameFillColor(0);
  gStyle->SetFrameLineColor(kBlack);
  gStyle->SetCanvasColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasBorderSize(0);
  gStyle->SetPadColor(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);

  gStyle->SetTextSize(0.04);
  gStyle->SetTextFont(42);
  gStyle->SetLabelFont(42,"xyz");
  gStyle->SetTitleFont(42,"xyz");
  gStyle->SetTitleSize(0.048,"xyz");
  gStyle->SetTitleOffset(1.6,"y"); // KYO for yield

  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.12) ; 
  gStyle->SetPadLeftMargin(0.16) ; // KYO for yield
  
  /////////////////////////////////////////////////////////
  //// binning setting
  /////////////////////////////////////////////////////////
  double tmpArr1s[7] = {0.0, 2.0, 4.0, 6.0, 9.0, 12.0, 30.0};
  double tmpArr2s[4] = {0.0, 4.0, 9.0, 30.0};
  double tmpArr3s[3] = {0.0, 6.0, 30.0};
  
  int tmpBin;
  if ( states ==1) {
    cout << " ***** 1S *****" << endl; tmpBin = 6;  
  }else if (states ==2){
    cout << " ***** 2S *****" << endl; tmpBin = 3;  
  }else if (states ==3){
    cout << " ***** 3S *****" << endl; tmpBin = 2;  
  }else {
    cout << " Error ::: Select among 1S, 2S, and 3S" << endl; return 0;
  }
 
  const int nStates = 3; 
  const int nBin = tmpBin; // number of bin 
  const int nArrNum = nBin+1; // number of array
  double binArr[nArrNum]; // array

  

  cout << "nBin = " << nBin << endl;
  for (int ib =0; ib < nArrNum; ib ++ ) {
    if (states ==1) { binArr[ib] = tmpArr1s[ib]; }
    else if (states ==2) { binArr[ib] = tmpArr2s[ib]; }
    else if (states ==3) { binArr[ib] = tmpArr3s[ib]; }
    cout << ib <<"th bin = " << binArr[ib] << endl;
  }

  /////////////////////////////////////////////////////////
  //// Open RooDataFile
  /////////////////////////////////////////////////////////
 
  int nFit = 2;
  if(DrawOpt!=0) nFit = 1;

  //file and ws
  TFile *fileIn[nFit][nBin];
  RooWorkspace* ws[nFit][nBin];
  // parameters 
  double alpha[nFit][nBin];
  double alphaErr[nFit][nBin];
  double n1s[nFit][nBin];
  double n1sErr[nFit][nBin];
  double sigma[nFit][nBin];
  double sigmaErr[nFit][nBin];
  double f1s[nFit][nBin];
  double f1sErr[nFit][nBin];
  double x1s[nFit][nBin];
  double x1sErr[nFit][nBin];
 
  char *Fit_loc[2] = {" ../../NominalFitResult/jaebeomFit/", "../../NominalFitResult/jaredFit/"};
  char *Name_Fit[2] = {"JaeBeom", "Jared"};
  TString fileLoc[nFit];
  TString fitName[nFit]; 
  if(DrawOpt!=0){fileLoc[0] = Fit_loc[DrawOpt-1]; fitName[0] = Name_Fit[DrawOpt-1]; }
  else if(DrawOpt==0){ 
    for(int i=0; i<nFit; i++)
    {
      fileLoc[i] = Fit_loc[i];
      fitName[i] = Name_Fit[i];
    }
  }

  Int_t fitColorArr[] = { kGray+3, kRed+2, kBlue+1, kOrange+7, kGreen+3, kAzure+9, kViolet-1, kGreen+1,kBlack };

  for (int ib =0; ib < nBin; ib ++ ) {
    for(int ifit = 0; ifit < nFit; ifit++){
      //// read files
      if (szAA == "PP" ) { 
        fileIn[ifit][ib]= new TFile(Form("%sAllParmFree_fitresults_upsilon_DoubleCB_5TeV_%s_DATA_pt%.1f-%.1f_y-1.93-1.93_muPt4.0.root",fileLoc[ifit].Data(),szAA.Data(),binArr[ib],binArr[ib+1]));}
      else if (szAA == "PA" ) { fileIn[ifit][ib]= new TFile(Form("%sAllParmFree_fitresults_upsilon_DoubleCB_5TeV_%s_DATA_pt%.1f-%.1f_y-1.93-1.93_muPt4.0.root",fileLoc[ifit].Data(),szAA.Data(),binArr[ib],binArr[ib+1])); }
      else { cout << " Error ::: Select among PP and AA" << endl; return 0; }
      cout << ib << "th file = " << fileIn[ifit][ib]->GetName() << endl;
      if (fileIn[ifit][ib]->IsZombie()) { cout << "CANNOT open data root file\n"; return 1; }
      fileIn[ifit][ib]->cd();
      ws[ifit][ib]= (RooWorkspace*)fileIn[ifit][ib]->Get("workspace");
      //ws[ifit][ib]->Print();

      //// get parameters
      alpha[ifit][ib]=ws[ifit][ib]->var("alpha1s_1")->getVal();
      alphaErr[ifit][ib]=ws[ifit][ib]->var("alpha1s_1")->getError();
      n1s[ifit][ib]=ws[ifit][ib]->var("n1s_1")->getVal();
      n1sErr[ifit][ib]=ws[ifit][ib]->var("n1s_1")->getError();
      sigma[ifit][ib]=ws[ifit][ib]->var("sigma1s_1")->getVal();
      sigmaErr[ifit][ib]=ws[ifit][ib]->var("sigma1s_1")->getError();
      f1s[ifit][ib]=ws[ifit][ib]->var("f1s")->getVal();
      f1sErr[ifit][ib]=ws[ifit][ib]->var("f1s")->getError();
      x1s[ifit][ib]=ws[ifit][ib]->var("x1s")->getVal();
      x1sErr[ifit][ib]=ws[ifit][ib]->var("x1s")->getError();
      //cout << ib << "th nSig1s = " << nSig1s[ifit][ib] << endl;
      //cout << ib << "th nSig2s = " << nSig2s[ifit][ib] << endl;
      //cout << ib << "th nSig3s = " << nSig3s[ifit][ib] << endl;
      //cout << ib << "th nBkg = " << nBkg[ifit][ib] << endl;
    }
  }

  //// histogram
  TH1D* h1_alpha[nFit]; 
  TH1D* h1_n[nFit]; 
  TH1D* h1_sigma[nFit]; 
  TH1D* h1_f[nFit]; 
  TH1D* h1_x[nFit]; 
  
  for(int ifit=0; ifit<nFit; ifit++){
    h1_alpha[ifit] = new TH1D(Form("h1_alpha%ds_%d",states,ifit+1),Form("h1_alpha%ds;p_{T} (GeV/c);#alpha",states),nBin,binArr); 
    h1_n[ifit] = new TH1D(Form("h1_n%ds_%d",states,ifit+1),Form("h1_n%ds;p_{T} (GeV/c);n",states),nBin,binArr); 
    h1_sigma[ifit] = new TH1D(Form("h1_sigma%ds_%d",states,ifit+1),Form("h1_sigma%ds;p_{T} (GeV/c);#sigma",states),nBin,binArr); 
    h1_f[ifit] = new TH1D(Form("h1_f%ds_%d",states,ifit+1),Form("h1_f%ds;p_{T} (GeV/c);f",states),nBin,binArr); 
    h1_x[ifit] = new TH1D(Form("h1_x%ds_%d",states,ifit+1),Form("h1_x%ds;p_{T} (GeV/c);x",states),nBin,binArr); 
    h1_n[ifit]->GetYaxis()->SetTitleFont(32);
    h1_f[ifit]->GetYaxis()->SetTitleFont(32);
    h1_x[ifit]->GetYaxis()->SetTitleFont(32);
    for (int ib =0; ib < nBin; ib ++ ) {
      h1_alpha[ifit]->SetBinContent(ib+1,alpha[ifit][ib]);   
      h1_alpha[ifit]->SetBinError(ib+1,alphaErr[ifit][ib]);   
      h1_n[ifit]->SetBinContent(ib+1,n1s[ifit][ib]);   
      h1_n[ifit]->SetBinError(ib+1,n1sErr[ifit][ib]);   
      h1_sigma[ifit]->SetBinContent(ib+1,sigma[ifit][ib]);   
      h1_sigma[ifit]->SetBinError(ib+1,sigmaErr[ifit][ib]);   
      h1_f[ifit]->SetBinContent(ib+1,f1s[ifit][ib]);   
      h1_f[ifit]->SetBinError(ib+1,f1sErr[ifit][ib]);   
      h1_x[ifit]->SetBinContent(ib+1,x1s[ifit][ib]);   
      h1_x[ifit]->SetBinError(ib+1,x1sErr[ifit][ib]);   
    }
  }

  //// normalization
  for(int ifit=0; ifit<nFit; ifit++){
    SetHistStyle(h1_alpha[ifit],ifit, ifit);
    SetHistStyle(h1_n[ifit],ifit, ifit);
    SetHistStyle(h1_sigma[ifit],ifit, ifit);
    SetHistStyle(h1_f[ifit],ifit, ifit);
    SetHistStyle(h1_x[ifit],ifit, ifit);
  }

  int binmax[5] = { h1_alpha[0]->GetMaximumBin(),h1_n[0]->GetMaximumBin(), h1_sigma[0]->GetMaximumBin(), h1_f[0]->GetMaximumBin(), h1_x[0]->GetMaximumBin()};
  double valmax[5] = { h1_alpha[0]->GetBinContent(binmax[0]), h1_n[0]->GetBinContent(binmax[1]), h1_sigma[0]->GetBinContent(binmax[2]), h1_f[0]->GetBinContent(binmax[3]), h1_x[0]->GetBinContent(binmax[4])};
  h1_alpha[0]->GetYaxis()->SetRangeUser(0,valmax[0]*3);
  h1_n[0]->GetYaxis()->SetRangeUser(0,valmax[1]*3);
  h1_sigma[0]->GetYaxis()->SetRangeUser(0,valmax[2]*3);
  h1_f[0]->GetYaxis()->SetRangeUser(0,valmax[3]*3);
  h1_x[0]->GetYaxis()->SetRangeUser(0,valmax[4]*3);
  //// actual draw
  TLatex* latex = new TLatex();
  latex->SetNDC();
  latex->SetTextFont(32);
  latex->SetTextAlign(12);
  latex->SetTextSize(0.035);

  double pos_y_diff = 0.05; 

  TLegend* fitleg = new TLegend(0.56,0.67,0.71,0.77); fitleg->SetTextSize(19);
  fitleg->SetTextFont(43);
  fitleg->SetBorderSize(0);

  TCanvas* c_alpha = new TCanvas("c_alpha","c_alpha",600,600);
  c_alpha->cd();
  for(int ifit=0; ifit<nFit; ifit++){ 
    h1_alpha[0]->Draw("pe"); if(ifit>0) h1_alpha[ifit]->Draw("pe same");
  }
  latex->SetTextColor(kBlack);
  latex->DrawLatex(0.55,0.86,Form("%s #Upsilon(%dS)",szAA.Data(),states));
  latex->DrawLatex(0.55,0.86-pos_y_diff*0.8,"param : #alpha");
  for(int ifit=0;ifit<nFit; ifit++){
    fitleg->AddEntry(h1_alpha[ifit],Form("%s fit",fitName[ifit].Data()),"pe");
  }
  fitleg->Draw("same");
  c_alpha->SaveAs(Form("sigparam/pt_alpha_Upsilon%ds_%s_DrawOpt%d.pdf",states,szAA.Data(),DrawOpt));
  
  TCanvas* c_n = new TCanvas("c_n","c_n",600,600);
  c_n->cd();
  for(int ifit=0; ifit<nFit; ifit++){ 
    h1_n[0]->Draw("pe"); if(ifit>0) h1_n[ifit]->Draw("pe same");
  }
  latex->SetTextColor(kBlack);
  latex->DrawLatex(0.55,0.86,Form("%s #Upsilon(%dS)",szAA.Data(),states));
  latex->DrawLatex(0.55,0.86-pos_y_diff*0.8,"param : n");
  fitleg->Draw("same");
  c_n->SaveAs(Form("sigparam/pt_n_Upsilon%ds_%s_DrawOpt%d.pdf",states,szAA.Data(),DrawOpt));
  
  TCanvas* c_sigma = new TCanvas("c_sigma","c_sigma",600,600);
  c_sigma->cd();
  for(int ifit=0; ifit<nFit; ifit++){ 
    h1_sigma[0]->Draw("pe"); if(ifit>0) h1_sigma[ifit]->Draw("pe same");
  }
  latex->SetTextColor(kBlack);
  latex->DrawLatex(0.55,0.86,Form("%s #Upsilon(%dS)",szAA.Data(),states));
  latex->DrawLatex(0.55,0.86-pos_y_diff*0.8,"param : #sigma");
  fitleg->Draw("same");
  c_sigma->SaveAs(Form("sigparam/pt_sigma_Upsilon%ds_%s_DrawOpt%d.pdf",states,szAA.Data(),DrawOpt));
  
  TCanvas* c_f = new TCanvas("c_f","c_f",600,600);
  c_f->cd();
  for(int ifit=0; ifit<nFit; ifit++){ 
    h1_f[0]->Draw("pe"); if(ifit>0) h1_f[ifit]->Draw("pe same");
  }
  latex->SetTextColor(kBlack);
  latex->DrawLatex(0.55,0.86,Form("%s #Upsilon(%dS)",szAA.Data(),states));
  latex->DrawLatex(0.55,0.86-pos_y_diff*0.8,"param : f");
  fitleg->Draw("same");
  c_f->SaveAs(Form("sigparam/pt_f_Upsilon%ds_%s_DrawOpt%d.pdf",states,szAA.Data(),DrawOpt));
  
  TCanvas* c_x = new TCanvas("c_x","c_x",600,600);
  c_x->cd();
  for(int ifit=0; ifit<nFit; ifit++){ 
    h1_x[0]->Draw("pe"); if(ifit>0) h1_x[ifit]->Draw("pe same");
  }
  latex->SetTextColor(kBlack);
  latex->DrawLatex(0.55,0.86,Form("%s #Upsilon(%dS)",szAA.Data(),states));
  latex->DrawLatex(0.55,0.86-pos_y_diff*0.8,"param : x");
  fitleg->Draw("same");
  c_x->SaveAs(Form("sigparam/pt_x_Upsilon%ds_%s_DrawOpt%d.pdf",states,szAA.Data(),DrawOpt));
  

  
  return 0;
}

