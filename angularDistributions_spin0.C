#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include <iostream>
#include <cmath>
#include <algorithm>
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussModel.h"
#include "RooDecay.h"
#include "RooBMixDecay.h"
#include "RooCategory.h"
#include "RooBinning.h"
#include "RooPlot.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TH1.h"
#include "TGaxis.h"
#include "TString.h"
#include "TSystem.h"
#include "TChain.h"
#include "ScalarPdfFactory_withFepspr.cc"
#include "AngularPdfFactory.cc"


using namespace RooFit;

using namespace std;

void angularDistributions_spin0(int nfiles, TString *files, TString plotdir, int nbins=80){
  gSystem->mkdir(plotdir);
  RooRealVar* mzz = new RooRealVar("mH", "M_{ZZ} (GeV)", 400, 100, 1000);
  RooRealVar* z1mass = new RooRealVar("mZ1", "m_{Z1} (GeV)", 4, 120);
  RooRealVar* z2mass = new RooRealVar("mZ2", "m_{Z2} (GeV)", 4, 120);
  RooRealVar* hs = new RooRealVar("costhetastar_ZZ4l", "cos#theta^{*}_{ZZ4l}", -1, 1);
  RooRealVar* h1 = new RooRealVar("costheta1_ZZ4l", "cos#(theta_{1})_{ZZ4l}", -1, 1);
  RooRealVar* h2 = new RooRealVar("costheta2_ZZ4l", "cos#(theta_{Z2})_{ZZ4l}", -1, 1);
  RooRealVar* Phi = new RooRealVar("Phi_ZZ4l", "#Phi_{ZZ4l}", -TMath::Pi(), TMath::Pi());
  RooRealVar* Phi1 = new RooRealVar("Phi1_ZZ4l", "#(Phi_{1})_{ZZ4l}", -TMath::Pi(), TMath::Pi());

  RooRealVar* measurables[8]={ z1mass, z2mass, h1, h2, hs, Phi, Phi1, mzz };

  ScalarPdfFactory_withFepspr* someHiggs = new ScalarPdfFactory_withFepspr(z1mass, z2mass, hs, h1, h2, Phi, Phi1, mzz, 1, false, true);
  someHiggs->_modelParams.fepspr->setVal(0);
  someHiggs->_modelParams.g1Val->setVal(1);
  someHiggs->_modelParams.g2Val->setVal(0);
  someHiggs->_modelParams.g3Val->setVal(0);
  someHiggs->_modelParams.g4Val->setVal(0);
  someHiggs->_modelParams.g2ValIm->setVal(0);
  someHiggs->_modelParams.g4ValIm->setVal(0);
  someHiggs->makeParamsConst(true);

  TChain* tree = new TChain("tree");
  for (int i = 0; i < nfiles; i++)
    tree->Add(files[i]);
  RooDataSet* dataSM = new RooDataSet("data", "data", tree, RooArgSet(*z1mass, *z2mass, *hs, *h1, *h2, *Phi, *Phi1));
  for (int plotIndex=0; plotIndex<7; plotIndex++){
    cout << plotIndex << endl;

    RooPlot* plot = measurables[plotIndex]->frame(nbins);
    plot->GetXaxis()->CenterTitle();
    plot->GetYaxis()->SetTitleOffset(1.2);
    plot->GetYaxis()->CenterTitle();
    plot->GetYaxis()->SetTitle("Number of Events");
    plot->GetXaxis()->SetNdivisions(-505);

    char ctitle[200]="";
    TString m_name = measurables[plotIndex]->GetName();
    cout << ctitle << endl;

    plot->SetTitle(ctitle);

    dataSM->plotOn(plot, MarkerColor(kRed), MarkerStyle(3), MarkerSize(1.2), LineWidth(0), XErrorSize(0), DataError(RooAbsData::Poisson));
    someHiggs->PDF->plotOn(plot, LineColor(kRed), LineWidth(2));

    TGaxis::SetMaxDigits(3);

    TCanvas* can = new TCanvas("can", "can", 600, 600);

    plot->Draw();

    TString cname = m_name;
    TString cname_pdf=cname;
    TString cname_eps=cname;
    TString cname_png=cname;
    cname_pdf = TString(plotdir) += TString("/") += TString(m_name) += ".pdf";
    cname_eps = TString(plotdir) += TString("/") += TString(m_name) += ".eps";
    cname_png = TString(plotdir) += TString("/") += TString(m_name) += ".png";

    can->SaveAs(cname_pdf);
    can->SaveAs(cname_eps);
    can->SaveAs(cname_png);
    can->Close();
  }

  delete mzz;
  delete z1mass;
  delete z2mass;
  delete hs;
  delete h1;
  delete h2;
  delete Phi;
  delete Phi1;
  delete someHiggs;
  delete tree;
}
