/* todos:
 */

#include <iostream>
#include <string.h>
#include <vector>


#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TString.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TCanvas.h"

// todo: understand why dynamic_cast works without std::
using std::cout;
using std::endl;

Bool_t sameName(TObject* obj1, TObject* obj2){
    return strncmp(obj1->GetName(), obj2->GetName(), 100)==0;
}

Bool_t sameClassName(TObject* obj1, TObject* obj2){
    return strncmp(obj1->ClassName(), obj2->ClassName(), 100)==0;
}


void normalizeTH1ByBinWidth(TH1 *theHisto, TString &theMotherListName){

    cout << "plotting " << theHisto->GetName() << " from " << theMotherListName.Data() << endl;

    TH1 *lHistoNorm = (TH1*)theHisto->Clone("normalized");
    for (Int_t iBin = 1; iBin <= theHisto->GetNbinsX(); ++iBin){
        Double_t lWidth =  theHisto->GetBinWidth(iBin);
        lHistoNorm->SetBinContent(iBin, lWidth ?  theHisto->GetBinContent(iBin)/lWidth : 0.);

        //~ lHistoNorm->SetBinError(iBin, sqrt(lHistoNorm->GetBinContent(iBin)));
        lHistoNorm->SetBinError(iBin, lHistoNorm->GetBinError(iBin)/lWidth);
    }

    TCanvas* lCanvas = new TCanvas("lCanvas","",200,10,1350,900);
    lHistoNorm->SetTitle(Form("%s_%s", theHisto->GetName(), theMotherListName.ReplaceAll(" ","_").Data()));


    lHistoNorm->Draw();

    lCanvas->SaveAs(Form("%s_%s.pdf", theHisto->GetName(), theMotherListName.ReplaceAll(" ","_").Data()));
    delete lCanvas;
}

void plotTH1(TH1 *theHisto, TString &theMotherListName){

    cout << "plotting " << theHisto->GetName() << " from " << theMotherListName.Data() << endl;

    //~ TH1 *lHistoNorm = (TH1*)theHisto->Clone("normalized");


    TCanvas* lCanvas = new TCanvas("lCanvas","",200,10,1350,900);
    theHisto->SetTitle(Form("%s_%s", theHisto->GetTitle(), theMotherListName.ReplaceAll(" ","_").Data()));


    theHisto->Draw("COLZ");

    lCanvas->SaveAs(Form("%s_%s.pdf", theHisto->GetName(), theMotherListName.ReplaceAll(" ","_").Data()));
    delete lCanvas;
}

void plotGamma_dEdxSig_after(TH2F *theHisto, TString &theMotherListName)
{
    cout << "plotting " << theHisto->GetName() << " from " << theMotherListName.Data() << endl;

    TCanvas* lCanvas = new TCanvas("lCanvas","",200,10,1350,900);
    theHisto->SetTitle(Form("%s_%s", theHisto->GetName(), theMotherListName.ReplaceAll(" ","_").Data()));

    theHisto->GetYaxis()->SetRangeUser(-6., 6.);
    if (theMotherListName.Contains("GammaConvV1_"))
    {
        theHisto->GetYaxis()->SetRangeUser(-8., 8.);
    }


    theHisto->Draw("COLZ");

    lCanvas->SaveAs(Form("%s_%s.pdf", theHisto->GetName(), theMotherListName.ReplaceAll(" ","_").Data()));
    delete lCanvas;
}

void plotGamma_dEdx_after(TH2F *theHisto, TString &theMotherListName)
{
    cout << "plotting " << theHisto->GetName() << " from " << theMotherListName.Data() << endl;

    TCanvas* lCanvas = new TCanvas("lCanvas","",200,10,1350,900);
    theHisto->SetTitle(Form("%s_%s", theHisto->GetName(), theMotherListName.ReplaceAll(" ","_").Data()));

    theHisto->Draw("COLZ");

    lCanvas->SaveAs(Form("%s_%s.pdf", theHisto->GetName(), theMotherListName.ReplaceAll(" ","_").Data()));
    delete lCanvas;
}


// this will be the dispatch function to decide what to do with the found histogram
void doSth(TObject *theObject, TString theMotherListName){

    // add here what should be done..
    cout << "found object " << theObject->GetName() << " in " << theMotherListName.Data() << endl;
}


void   searchInList(TList*         theList1,
                    TString        theNameOfMotherList,
                    TString        theHierachy,
                    TFile*         theFile1=nullptr,
                    TDirectoryFile* theDirectoryFile=nullptr)
{
    if (!theList1) cout << theHierachy.Data() << "theList1 is null\n";
    
    cout << "searchInList with: " << theList1->GetName() << " " << theList1->GetTitle() << endl;
    
    TString lNewHierarchy = theHierachy  + theList1->GetName() + "/";

    TIter lIter1(theList1);
    while (TObject *lObj1 = lIter1.Next()){
        
        // lObj1 is a TKey if theList1 was obtained from TFile::GetListOfKeys() call. This will bring its natural type
        if (theFile1) {
            lObj1 = theFile1->Get(lObj1->GetName());
            if (!lObj1) cout << "nullp\n";
        }
        if (theDirectoryFile) {
            lObj1 = theDirectoryFile->Get(lObj1->GetName());
            if (!lObj1) cout << "nullp\n";
        }
        
        
        TString lFullName = TString() + lNewHierarchy.Data() + lObj1->GetName() + " (" + lObj1->ClassName() + ")";
        
        if (lObj1->InheritsFrom("TH1"))
        {
            plotTH1(dynamic_cast<TH1*>(lObj1), theNameOfMotherList);
        }
        
        if (lObj1->InheritsFrom("TList"))
        {
            searchInList(dynamic_cast<TList*>(lObj1),
                         theList1->GetName(),
                         lNewHierarchy);
        }
        if (lObj1->InheritsFrom("TDirectoryFile"))
        {
            TDirectoryFile *lDirectoryFile = dynamic_cast<TDirectoryFile*>(lObj1);
            searchInList(lDirectoryFile->GetListOfKeys(),
                         lObj1->GetName(),
                         lNewHierarchy,
                         nullptr,
                         lDirectoryFile);
        }
    }
}


void plotAll(TString fname1=""){

    gROOT->Reset();

    TFile* lFile1 = new TFile(fname1);

    cout << "===============================================================\n";
    cout << "Current pwd is: " << gSystem->pwd() << endl;
    cout << "Plotting all from the following file:\n" << lFile1->GetName() << endl;
    cout << "===============================================================\n";


    searchInList(lFile1->GetListOfKeys(),
                 "",
                 "",
                 lFile1);
}
