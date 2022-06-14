/* todos:
 */

#include "/Users/stephanstiefelmaier/work/repos/projects/a_helper/getObjectFromPathInFile.C"
#include "/Users/stephanstiefelmaier/work/repos/projects/a_helper/range.hpp"

#include <iostream>
#include <string.h>
#include <vector>


#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TString.h"
#include "TH1.h"
#include "TH2.h"
#include "TLegend.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TCanvas.h"
#include "TEfficiency.h"
#include "TGraphAsymmErrors.h"

// todo: understand why dynamic_cast works without std::
using std::cout;
using std::endl;

void plotTH1(TH1 *theHisto, TString &theMotherListName){
    cout << "plotting " << theHisto->GetName() << " from " << theMotherListName.Data() << endl;

    TCanvas* lCanvas = new TCanvas("lCanvas","",200,10,1350,900);
    theHisto->SetTitle(Form("%s_%s", theHisto->GetTitle(), theMotherListName.ReplaceAll(" ","_").Data()));

    theHisto->Draw();

    lCanvas->SaveAs(Form("%s_%s.pdf", theHisto->GetName(), theMotherListName.ReplaceAll(" ","_").Data()));
    delete lCanvas;
}

TCanvas* plotRatioWithTEfficiency(vector<TH1F*> theHistos, std::vector<std::string> *theNames=nullptr, size_t theNrebin=1, std::string theTitle="plotRatioWithTEfficiency", std::string theXaxis="theXaxis", std::string theYaxis="theYaxis", float theYup=1.0, bool theWithErrors=true){

    if (!theNames){
        theNames = new std::vector<std::string>{};
        for (auto _ : theHistos){
            theNames->push_back(_->GetName());
        }
    }

    if (theNrebin>1){
        for (auto _ : theHistos){
            _->Rebin(theNrebin);
        }
    }

    if (theNrebin>1){
        for (auto _ : theHistos){
            _->Rebin(theNrebin);
        }
    }

    TH1 *hPass = theHistos[0];
    TH1 *hTotal = theHistos[1];

    TEfficiency *lEff = nullptr;
    TFile* pFile = new TFile("myfile.root","recreate");

    hPass->Write();
    hTotal->Write();

    TCanvas* c5 = new TCanvas("c5","c5",800,600);
    hPass->Draw();

    TCanvas* c6 = new TCanvas("c6", "c6",800,600);
    hTotal->Draw();

    for (int i=0; i<=801; ++i){
        int lPassed = hPass->GetBinContent(i);
        int lTotal = hTotal->GetBinContent(i);
        printf("bin: %i centered at: %f passed: %i total: %i\n", i, hPass->GetXaxis()->GetBinCenter(i), lPassed, lTotal);

        if (lPassed > lTotal){printf("SFS bigger!\n");}
    }


    //h_pass and h_total are valid and consistent histograms
    if(TEfficiency::CheckConsistency(*hPass,*hTotal))
    {
        lEff = new TEfficiency(*hPass,*hTotal);
        lEff->SetTitle((theTitle + ";p_T;" + theTitle).data());
        lEff->SetTitle((theTitle + ";" + theXaxis + ";" + theYaxis).data());

        // this will write the TEfficiency object to "myfile.root"
        // AND lEff will be attached to the current directory
        lEff->Write();
        TCanvas* c7 = new TCanvas(theTitle.data(),theTitle.data(),800,600);
        gPad->SetGridy();
        lEff->Draw("AP");
        gPad->Update();
        auto lGraph = lEff->GetPaintedGraph();
        lGraph->SetMinimum(0.);
        lGraph->SetMaximum(theYup);
        gPad->Update();
        return c7;
    }
    return nullptr;
}


void produceAfterburnerPlots(std::string theFnameNom="", std::string theFnameDen=""){
    gROOT->Reset();

    std::vector<std::string> lNomDenFileNames{theFnameNom, theFnameDen=="" ? theFnameNom : theFnameDen};

    // convprob plot
    //~ {
        //~ std::string lNamePDF("plot-conversion-probabilty.pdf");
        //~ std::vector<std::string> lNomDenHistoPaths{
            //~ { "gamma-conversions-truth-only-mc/hGammaConvertedP_Rsel_MCTrue" },
            //~ { "gamma-conversions-truth-only-mc/hGammaProdAfterCutsP_MCTrue" },
        //~ };

        //~ std::vector<TH1F*> lNomDenHistos;
        //~ for (auto i : util::lang::indices(lNomDenHistoPaths)){
            //~ lNomDenHistos.push_back((TH1F*)getObjectFromPathInFile(lNomDenFileNames[1],lNomDenHistoPaths[i]));
        //~ }

        //~ TCanvas *lCanvas = plotRatioWithTEfficiency(lNomDenHistos, nullptr, 1, "conversion probability run3 from prim. photons (acc. alice def.) with eta<0.8 ", "p (GeV)", "p_{conv.}", 0.6);
        //~ if (lCanvas){
            //~ lCanvas->SaveAs(lNamePDF.data());
        //~ }
    //~ }


    // efficiency plot
    {
        std::string lNamePDF("plot-efficiency.pdf");
        std::vector<std::string> lNomDenHistoPaths{
            { "gamma-conversions/V0/beforeRecCuts/MCTrue/hPt_MCTrue" },
            { "gamma-conversions-truth-only-mc/hGammaConvertedPt_Rsel_MCTrue" }
        };

        std::vector<TH1F*> lNomDenHistos;
        for (auto i : util::lang::indices(lNomDenHistoPaths)){
            lNomDenHistos.push_back((TH1F*)getObjectFromPathInFile(lNomDenFileNames[i],lNomDenHistoPaths[i]));
        }

        TCanvas *lCanvas = plotRatioWithTEfficiency(lNomDenHistos, nullptr, 4, "converted photons v0 reconstruction efficiency", "p_T (GeV)", "#epsilon_{#gamma}^{reco.}", 1.0);
        if (lCanvas){
            //lCanvas->SetLogy();
            lCanvas->Update();
            lCanvas->SaveAs(lNamePDF.data());
        }
    }
}
