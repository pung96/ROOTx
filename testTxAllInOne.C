#include "src/TxAllInOne.h"
using namespace ROOTX;
void testTxAllInOne(){

  TFile * out = TFile::Open("out.root","RECREATE");
  auto binJetMass    = AxisFix( "JetMass","", 10, 0, 500 );
  auto binInvM       = AxisVar( "InvM", "",{40, 50, 60, 70, 80, 100, 120, 140, 200, 5000});
  auto binCent       = AxisFix( "Cent","", 1, 0, 100 );
  auto binJetPtCut   = AxisVar( "JetPtCut","",{0, 5, 10, 15, 20, 30, 50, 1000}  );
  auto binDiJetSel   = AxisStr( "DiJetSel","", { "LS","LS-PI2","L-PI2-S" } );
  auto binType       = AxisStr( "Type","", { "DATA","Mixing-Simple" } );
  auto binLog1k     = AxisLog("Log1k","",500,0.1,1000,0);

  TxHnSparseHelper h = TxHnSparseHelper::Create( "hDiJetPtPair","",{
      binType, binDiJetSel, binCent, binJetPtCut, binInvM, binLog1k  } ,"s");

  for( auto i : range( 100000 )){
    auto t = gRandom->Gaus( 2,2 );
    auto d = gRandom->Gaus( 2,2 );
    auto c = gRandom->Gaus(50,50);
    auto j = gRandom->Gaus(150,50);
    auto m = gRandom->Gaus(200,100);
    auto p = gRandom->Gaus(300,100);
    h.Fill( { t,d,c,j,m,p });
    auto pre = { t,d,c,j};
    h.Fill( { pre,{m,p} });

  }

  h->Write();
  out->Write();


}
