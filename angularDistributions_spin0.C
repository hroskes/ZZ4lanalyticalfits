#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include <iostream>
#include <cmath>
#include <string>
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
#include "TChain.h"
#include "ScalarPdfFactory_withFepspr.cc"
#include "AngularPdfFactory.cc"


using namespace RooFit;

using namespace std;

void angularDistributions_spin0(TString INPUT_NAME, int nbins=80){
  RooRealVar* mzz = new RooRealVar("ZZMass", "M_{ZZ} (GeV)", 400, 100, 1000);
  RooRealVar* z1mass = new RooRealVar("Z1Mass", "m_{Z1} (GeV)", 4, 120);
  RooRealVar* z2mass = new RooRealVar("Z2Mass", "m_{Z2} (GeV)", 4, 120);
  RooRealVar* hs = new RooRealVar("costhetastar", "cos#theta^{*}", -1, 1);
  RooRealVar* h1 = new RooRealVar("costheta1", "cos#theta_{Z1}", -1, 1);
  RooRealVar* h2 = new RooRealVar("costheta2", "cos#theta_{Z2}", -1, 1);
  RooRealVar* Phi = new RooRealVar("Phi", "#Phi", -TMath::Pi(), TMath::Pi());
  RooRealVar* Phi1 = new RooRealVar("Phi1", "#Phi_{Z1}", -TMath::Pi(), TMath::Pi());

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

  string cinput_common = "./";
  string coutput_common = "./Plots/";
  string cinput = cinput_common + INPUT_NAME.Data();
  cinput = cinput + ".root";

  TChain* tree = new TChain("SelectedTree");
  tree->Add(cinput.c_str());
  RooDataSet* dataSM = new RooDataSet("data", "data", tree, RooArgSet(*z1mass, *z2mass, *hs, *h1, *h2, *Phi, *Phi1));
  for (int plotIndex=3/*0*/; plotIndex<7; plotIndex++){
    cout << plotIndex << endl;

    RooPlot* plot = measurables[plotIndex]->frame(nbins);
    plot->GetXaxis()->CenterTitle();
    plot->GetYaxis()->SetTitleOffset(1.2);
    plot->GetYaxis()->CenterTitle();
    plot->GetYaxis()->SetTitle("Number of Events");
    plot->GetXaxis()->SetNdivisions(-505);

    char ctitle[200]="";
    string m_name = measurables[plotIndex]->GetName();
    cout << ctitle << endl;

    plot->SetTitle(ctitle);

    dataSM->plotOn(plot, MarkerColor(kRed), MarkerStyle(3), MarkerSize(1.2), LineWidth(0), XErrorSize(0), DataError(RooAbsData::Poisson));
    someHiggs->PDF->plotOn(plot, LineColor(kRed), LineWidth(2));

    TGaxis::SetMaxDigits(3);

    TCanvas* can = new TCanvas("can", "can", 600, 600);

    plot->Draw();

    char cname[200];
    sprintf(cname, "%s_%s", INPUT_NAME.Data(), m_name.c_str());
    string cname_pdf=cname;
    string cname_eps=cname;
    string cname_png=cname;
    cname_pdf = (coutput_common + cname_pdf) + ".pdf";
    cname_eps = (coutput_common + cname_eps) + ".eps";
    cname_png = (coutput_common + cname_png) + ".png";

    can->SaveAs(cname_pdf.c_str());
    can->SaveAs(cname_eps.c_str());
    can->SaveAs(cname_png.c_str());
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
