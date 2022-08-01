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

template <typename T>
TCanvas* plotRatioWithTEfficiency(vector<T*> theHistos, std::vector<std::string> *theNames=nullptr, size_t theNrebin=1, std::string theTitle="plotRatioWithTEfficiency", std::string theXaxis="theXaxis", std::string theYaxis="theYaxis", float theYup=1.0, bool theWithErrors=true){

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

    //~ if (theNrebin>1){
        //~ for (auto _ : theHistos){
            //~ _->Rebin(theNrebin);
        //~ }
    //~ }

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

    for (int i=0; i<=hPass->GetNbinsX()+1; ++i){
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
        lGraph->SetMinimum(-0.02);
        lGraph->SetMaximum(theYup);
        gPad->Update();
        return c7;
    }
    return nullptr;
}


void plotEfficiencyVsR(std::vector<std::string>& theNomDenFileNames)
{
    std::string lNamePDF("plot-efficiency_r.pdf");
    std::vector<std::string> lNomDenHistoPaths{
        { "gamma-conversions/V0/beforeRecCuts/MCTrue/hConvPointRPt_MCTrue" },
        { "gamma-conversions-truth-only-mc/hGammaConvertedRPt_MCTrue" }
    };

    std::vector<TH2F*> lNomDenHistos;
    for (auto i : util::lang::indices(lNomDenHistoPaths)){
        lNomDenHistos.push_back((TH2F*)getObjectFromPathInFile(theNomDenFileNames[i],lNomDenHistoPaths[i]));
    }


    float_t lPtMin = 0.5;
    std::vector<TH1D*> lProjections;

    for (auto h : lNomDenHistos){

        // make projections on x axis to integrate over certain pT range
        int iBinPtMin = h->GetYaxis()->FindBin(lPtMin);
        lProjections.push_back(h->ProjectionX("_px", iBinPtMin));

        lProjections.back()->Rebin(8);
    }

    TCanvas *lCanvas = plotRatioWithTEfficiency(lProjections, nullptr, 1, Form("converted photons v0 reconstruction efficiency, p_T > %.2f GeV/c", lPtMin), "conversion radius (cm)", "#epsilon_{#gamma}^{reco.}", 0.5);
    if (lCanvas){
        //lCanvas->SetLogy();
        lCanvas->Update();
        lCanvas->SaveAs(lNamePDF.data());
    }
}

void plotEfficiencyVspT(std::vector<std::string>& theNomDenFileNames)
{
    std::string lNamePDF("plot-efficiency_pt.pdf");
    std::vector<std::string> lNomDenHistoPaths{
        //~ { "gamma-conversions/V0/beforeRecCuts/MCTrue/hConvPointRPt_MCTrue" },
        { "gamma-conversions/V0/beforeRecCuts/MCTrue/hRPt_MCTrue" },
        { "gamma-conversions-truth-only-mc/hGammaConvertedRPt_MCTrue" }
    };

    std::vector<TH2F*> lNomDenHistos;
    for (auto i : util::lang::indices(lNomDenHistoPaths)){
        lNomDenHistos.push_back((TH2F*)getObjectFromPathInFile(theNomDenFileNames[i],lNomDenHistoPaths[i]));
    }


    float_t lRmin = 0.;
    float_t lRmax = 180.;
    std::vector<TH1D*> lProjections;

    for (auto h : lNomDenHistos){

        // make projections onto y axis to integrate over certain pT range
        int iBinRMin = h->GetXaxis()->FindBin(lRmin);
        int iBinRMax = h->GetXaxis()->FindBin(lRmax);
        lProjections.push_back(h->ProjectionY("_py", iBinRMin, iBinRMax));

        lProjections.back()->Rebin(4);
    }

    TCanvas *lCanvas = plotRatioWithTEfficiency(lProjections, nullptr, 1, Form("converted photons v0 reconstruction efficiency, %.1f cm < r_conv < %.1f cm", lRmin, lRmax), "p_T (GeV)", "#epsilon_{#gamma}^{reco.}", 0.5);
    if (lCanvas){
        //lCanvas->SetLogy();
        lCanvas->Update();
        lCanvas->SaveAs(lNamePDF.data());
    }
}


// function to aling the edges in theXBinsWanted with the ones given by the theHisto. theXBinsGood will contain only edges that are also there in
// theHisto and that are the same or to the left of the ones in theXBinsWanted
void fillArrayWithNewEdges(TH1F* theHisto, Double_t *theXBinsWanted, int theNEdgesNew, Double_t *theXBinsGood){

    for (int iNew=0; iNew<theNEdgesNew; ++iNew){
        double_t lEdgeNew = theHisto->GetBinLowEdge(theHisto->FindBin(theXBinsWanted[iNew]));
        theXBinsGood[iNew] = lEdgeNew;
    }
}

void printArray(Double_t *theArr, int theN){

    cout << "printArray start\n";
    for (int i =0; i< theN; ++i){
        cout << i << " " << theArr[i] << endl;
    }
    cout << "printArray end\n";
}


/* get an xbins array that can be used to rebin a histogram. specify axis boarders and how many bins in the old binning should be combined into one bin in the new binning for each region between the boarders.
 * example usage:
 * const int lNBoarders = 2;
   double_t lBoarders[lNBoarders] = { 1., 2.,};
   int lFactors[lNBoarders] = { 4, 6 };
   // -> use old bins below 1., combine 4 bins into one between 1. and 2., and combine 6 bins into one bin above 2.
   double_t lXBinsNew[1000];
int lNBinsNew = computeRebinnedXbins(hNom, lBoarders, lFactors, lNBoarders, lXBinsNew);
 */

int computeRebinnedXbins(TH1* theHisto, Double_t* theBoarders, int* theFactors, int theNBoarders, Double_t* theXbinsNew){

    int i = 1;
    int iNew = 0;
    int iBoarder = 0;
    int inc = 1;
    bool  lLastBoarderReached = false;

    while (i <= theHisto->GetNbinsX()){

        theXbinsNew[iNew] = theHisto->GetBinLowEdge(i);
        ++iNew;

        if ( !lLastBoarderReached && theHisto->GetBinLowEdge(i+1) > theBoarders[iBoarder]){

            inc = theFactors[iBoarder];
            if (iBoarder+1 < theNBoarders){
                ++iBoarder;
            }
            else {
                lLastBoarderReached = true;
            }
        }
        i += inc;
    }
    return iNew-2;
}

void printHistoEdges(TH1 *theHisto){
    cout << "printHistoEdges start\n";

    int nBins = theHisto->GetNbinsX();
    for(int i=1; i<=nBins; ++i){
        cout << i << " " << theHisto->GetBinLowEdge(i) << endl;
    }
    cout << nBins << " " << theHisto->GetBinLowEdge(nBins+1) << endl;
    cout << "printHistoEdges end\n";
}


void plotConversionProbability(std::vector<std::string>& theNomDenFileNames){

    std::string lNamePDF("plot-conversion-probabilty.pdf");
    std::vector<std::string> lNomDenHistoPaths{
        { "gamma-conversions-truth-only-mc/hGammaConvertedRP_MCTrue" },
        { "gamma-conversions-truth-only-mc/hGammaProdAfterCutsP_MCTrue" },
    };

    std::vector<TObject*> lNomDenObjects;
    for (auto i : util::lang::indices(lNomDenHistoPaths)){
        lNomDenObjects.push_back(getObjectFromPathInFile(theNomDenFileNames[1],lNomDenHistoPaths[i]));
    }

    TH2F* hNomRP = (TH2F*)lNomDenObjects[0];

    float lRMinWanted = 5.0;
    float lRMaxWanted = 180.;

    TAxis* lXaxis = hNomRP->GetXaxis();

    int iBinMin = lXaxis->FindBin(lRMinWanted);
    int iBinMax = lXaxis->FindBin(lRMaxWanted) - 1;

    float lRMinIs = lXaxis->GetBinLowEdge(iBinMin);
    float lRMaxIs = lXaxis->GetBinUpEdge(iBinMax);

    cout << "will integrate in R between " << lRMinIs <<  " and " << lRMaxIs << endl;

    TH1* hNomP = (TH1D*)hNomRP->ProjectionY("hGammaConvertedP_fromproj", iBinMin, iBinMax);

    std::vector<TH1*> lNomDenHistos;

    lNomDenHistos.push_back(hNomP);
    lNomDenHistos.push_back((TH1*)lNomDenObjects[1]);


    printHistoEdges(hNomP);

    // define new binning
    const int lNBoarders = 5;
    double_t lBoarders[lNBoarders] = { 1., 2., 3., 4., 8.};
    int lFactors[lNBoarders] = { 4, 6, 8, 24, 128 };
    double_t lXBinsNew[1000];

    int lNBinsNew = computeRebinnedXbins(hNomP, lBoarders, lFactors, lNBoarders, lXBinsNew);

    printArray(lXBinsNew, lNBinsNew);

    for (auto& histo : lNomDenHistos){
        histo = dynamic_cast<TH1*>(histo->Rebin(lNBinsNew, (std::string(histo->GetName()) + "_reb").data(), lXBinsNew));
    }



    /*
    double_t xBinsWanted[4] = {0.0, 1.01, 4.0, 8.0 };
    double_t xBinsFitting[4];
    int nBinsNew = 3;

    TH1F* hNom = lNomDenHistos[0];

    printArray(xBinsWanted, 4);

    fillArrayWithNewEdges(hNom, xBinsWanted, 4, xBinsFitting);

    printArray(xBinsFitting, 4);
    */




    TCanvas *lCanvas = plotRatioWithTEfficiency(lNomDenHistos, nullptr, 1, "conversion probability run3 from prim. photons (acc. alice def.) with eta<0.8 ", "p (GeV)", "p_{conv.}", 0.6);
    if (lCanvas){
        lCanvas->SaveAs(lNamePDF.data());
    }

}

void produceAfterburnerPlots(std::string theFnameNom="", std::string theFnameDen=""){
    gROOT->Reset();

    std::vector<std::string> lNomDenFileNames{theFnameNom, theFnameDen=="" ? theFnameNom : theFnameDen};

    // convprob plot

    plotConversionProbability(lNomDenFileNames);



    //~ plotEfficiencyVsR(lNomDenFileNames);
    //plotEfficiencyVspT(lNomDenFileNames);
}
