#include <iostream>

#include "TSystem.h"
#include "TStyle.h"
#include "TRandom3.h"
#include "TH2F.h"
#include "TCanvas.h"

std::string itoa(int i) 
{
  char res[10];
  sprintf(res, "%d", i);
  std::string ret(res);
  return ret;
}

int getBinContent(TH2F *h, int i, int j)
{
  int xMax=h->GetNbinsX();
  int yMax=h->GetNbinsY();
  
  if (i==0) i=xMax;
  else if (i==xMax+1) i=0;
  if (j==0) j=yMax;
  else if (j==yMax+1) j=0;
  
  return h->GetBinContent(i, j);
}

void evolve(TH2F *h)
{
  TH2F *h_temp=(TH2F*)h->Clone();
  
  for (int i=1; i<=h->GetNbinsX(); ++i)
  {
    for (int j=1; j<=h->GetNbinsY(); ++j)
    {
      unsigned int state=getBinContent(h, i, j);
      unsigned int nLiveNeighbors=getBinContent(h, i-1, j-1)
                                 +getBinContent(h, i, j-1)
                                 +getBinContent(h, i+1, j-1)
                                 +getBinContent(h, i-1, j)
                                 +getBinContent(h, i+1, j)
                                 +getBinContent(h, i-1, j+1)
                                 +getBinContent(h, i, j+1)
                                 +getBinContent(h, i+1, j+1);
      
      if (state==1)
      {
        // std::cout<<i<<" "<<j<<" is alive, nLiveNeighbors = "<<nLiveNeighbors<<std::endl;
        if (nLiveNeighbors<2 || nLiveNeighbors>3) h_temp->SetBinContent(i, j, 0);
        else h_temp->SetBinContent(i, j, 1);
      }
      else if (state==0)
      {
        // std::cout<<i<<" "<<j<<" is dead, nLiveNeighbors = "<<nLiveNeighbors<<std::endl;
        if (nLiveNeighbors==3) h_temp->SetBinContent(i, j, 1);
        else h_temp->SetBinContent(i, j, 0);
      }
    }
  }
  
  h->Reset();
  h->Add(h_temp);
  
  delete h_temp;
}

void GameOfLife()
{
  unsigned int xMax=100, yMax=100;
  unsigned int maxIterations=200;

  TH2F *h_Life=new TH2F("h_Life", "", xMax, 0, xMax, yMax, 0, yMax);
  
  TRandom3 *r3=new TRandom3();
  for (int i=1; i<=h_Life->GetNbinsX(); ++i)
  {
    for (int j=1; j<=h_Life->GetNbinsY(); ++j)
    {
      if (r3->Uniform()>=0.9) h_Life->SetBinContent(i, j, 1);
      else h_Life->SetBinContent(i, j, 0);
    }
  }
  
  gSystem->Unlink("Movie/GameOfLife.gif");
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1);
  TCanvas *c=new TCanvas("c", "c", 700, 700);
  for (unsigned int i=0; i<maxIterations; ++i)
  {
    evolve(h_Life);
    if (i%1==0)
    {
      h_Life->Draw("col");
      c->SaveAs("Movie/GameOfLife.gif+");
    }
    if (i%(maxIterations/10)==0) std::cout<<"Completed "<<i*100./maxIterations<<"% "<<std::endl;
  }
  
  delete r3;
  delete h_Life;
  delete c;
  
}
