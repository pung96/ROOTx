#include "TxHnSparse.h" // #NotToMerge
#include <TPRegexp.h>
#include <TDirectory.h>
#include <TH1D.h>

namespace ROOTX {

  //____________________________________________________
  TAxis AxisFix(const char* name, const char * title, 
      Int_t nbin, Double_t xmin, Double_t xmax){
    TAxis ax(nbin,xmin,xmax);ax.SetNameTitle(name,title);
    return ax;
  }

  //____________________________________________________
  TAxis AxisStr(const char* name, const char * title, 
      String1D bin ){
    TAxis ax = AxisFix(name,title,bin.size(),0.5,bin.size()+0.5);
    for( auto i : bin_range(bin) )
      ax.SetBinLabel( i, bin[i] );
    return ax;
  }

  //____________________________________________________
  TAxis AxisVar( const char* name, const char *title,
      Double1D bin ){
    TAxis ax( bin.size()-1, &bin.front() ) ;ax.SetNameTitle(name,title);
    return ax;
  }

  //____________________________________________________
  TAxis AxisLog(const char* name, const char * title, 
      Int_t nbin, Double_t xmin, Double_t xmax, Double_t xbegin){
    int binoffset = ( xbegin<0 || (xmin-xbegin)<1e-9) ? 0 : 1;
    Double1D bin(nbin+1+binoffset,0);
    double logBW3 = (log(xmax)-log(xmin))/nbin;
    for(auto i : range(bin)+1)  bin[i+binoffset]=xmin*exp(i*logBW3);
    TAxis ax( nbin, &bin.front() ) ;
    ax.SetNameTitle(name,title);
    return ax;
  }

  //____________________________________________________
  int TxHnSparseHelperBase::GetAxisIndex(const char* name){
    for( auto i : range( fH->GetNdimensions() ) )
      if( TString(name) == fH->GetAxis(i)->GetName() )
        return i;
    fH->Print();PrintAxis();
    ROOTX_ERROR(-1,1,"Axis "+name+" doesn't exists");
    return -1;
  }

  //____________________________________________________
  TAxis* TxHnSparseHelperBase::GetAxisSafe(int iaxis){
    if( iaxis<0 || iaxis>fH->GetNdimensions() )  
      ROOTX_ERROR(-1,1,"Exeed of Axis "+iaxis+fH->GetName());
    auto ax = fH->GetAxis(iaxis);
    if( !ax ) ROOTX_ERROR(-1,1,"Unknown error by THnSparse "+fH->GetName());
    return ax;
  }

  //____________________________________________________
  void  TxHnSparseHelperBase::ResetUserAxis(Int_t iaxis){
    auto ax = GetAxisSafe(iaxis);
    fUserAxisMap[iaxis] = {0};
    fUserAxis[iaxis].clear();
    for( auto i : bin_range(ax->GetNbins())+1 ){
      fUserAxis[iaxis].push_back( ax->GetBinLowEdge(i) );
      fUserAxisMap[iaxis].push_back( i );
    }
  }

  //____________________________________________________
  void  TxHnSparseHelperBase::ResetUserAxisAll(){
    fUserAxis.resize(fH->GetNdimensions());
    fUserAxisMap.resize(fH->GetNdimensions());
    for( auto i : range(fH->GetNdimensions()) )
      ResetUserAxis(i);
  }

  //____________________________________________________
  const Double1D & TxHnSparseHelperBase::SetUserAxis( int iaxis, Double1D bin ){
    auto ax = GetAxisSafe(iaxis);
    if( bin.size()==0 ) ResetUserAxis(iaxis);
    else{
      fUserAxis[iaxis] = bin;
      fUserAxisMap[iaxis] = { 0 };
      for( auto i : range(bin) ){
        int ob = ax->FindBin( bin[i] );
        //TODO if( ib>0 && bin <= fCBinsB[i][ib-1] ) ErrorExit("Wrong bin");
        fUserAxisMap[iaxis].push_back( ax->FindBin( bin[ob] ) );
      }
    }
    return fUserAxis[iaxis];
  }

  //____________________________________________________
  const Double2D & TxHnSparseHelperBase::SetUserAxis( Double2D bin ){
    for( auto i : range(bin) ) SetUserAxis(i, bin[i]); 
    return fUserAxis;
  }

  //____________________________________________________
  TH1* TxHnSparseHelperBase::GetTH1(TString name, int iaxis, Int2D bin,Option_t* opt){
    //== Histogram Name and Title
    if( name.EndsWith("-") ) name+=Form("%sP%02d",fH->GetName(),iaxis);
    TString title = fH->GetTitle();
    for( auto i : range(bin) ){
      auto ax = GetAxisSafe(i);
      Int1D b = bin[i];
      Int1D nb = { 0, 0 };
      //TODO if( b.size() >= 2 ) ROOTX_ERROR(-1,1,"GetTH1. Number of bin range parameter should be 2");
      int maxnbin = fUserAxisMap[i].size()-2;
      if( b[0]<-1 || b[1]<-1 || b[0] > maxnbin || b[1] > maxnbin || b[0] > b[1] ) 
        ROOTX_ERROR(-1,1,Form("Wrong bin : %i : %s : %d %d",i,ax->GetName(),b[0],b[1])); 
      if( i == iaxis || b.size()==0 || ( b[0]<=0 && b[1]<=0 ) ){
      }else{
        if( b[0] > 0 ) nb[0] = fUserAxisMap[i][b[0]];
        if( b[1] > 0 ) nb[1] = fUserAxisMap[i][b[1]+1]-1;
        //if( b0 > b1 ) b1 = b1;
        name+=Form("%s%03d%03d",ax->GetName(), b[0],b[1]); //FIXME
        TString label[2];
        label[0] = FormSelectAndTrim( ax->GetBinLabel(nb[0]), ax->GetBinLowEdge( nb[0] ) );
        label[1] = FormSelectAndTrim( ax->GetBinLabel(nb[1]), ax->GetBinUpEdge ( nb[0] ) );
        title += Form("%s:%s",ax->GetName(),label[0].Data());
        if( label[0]!=label[1] ) title+=Form("-%s",label[1].Data());
      }
      ax->SetRange(nb[0], nb[1]);
    }
    TH1 * h = dynamic_cast<TH1*>(fH->Projection( iaxis, opt ));
    h->SetNameTitle(name,title);
    return h;
  }

  //____________________________________________________
  TH1* TxHnSparseHelperBase::GetTH1(TString name, int iaxis, Int1D bin,Option_t* opt){
    Int2D bins;
    for( int it : bin ) bins.push_back( {it,it} );
    return GetTH1(name, iaxis, bins, opt);
  }

  //____________________________________________________
  TString TxHnSparseHelperBase::FormSelectAndTrim( TString s, Double_t d ){
    if( s.IsNull() ) s = Form("%f",d);
    TPMERegexp("\\.?0+$").Substitute(s,"");
    return s;
  }


  //____________________________________________________
  TString TxHnSparseHelperBase::PrintAxis( int i, Option_t* opt){
    using std::min;
    TString opts = opt;
    TString output;
    auto uax= fUserAxis.at(i);
    auto ax = GetAxisSafe(i);
    output+=Form("%2d: %10s",i, ax->GetName());
    if( opts.Contains("all",TString::kIgnoreCase) ){
      int nBinPrint = 10;
      if( opts.Contains("ALL") ) nBinPrint=1000;
      else if( opts.Contains("All") ) nBinPrint=100;
      if( !opts.Contains("user") || int(uax.size())==ax->GetNbins()+2 ){
        //==== Print Orignal Axis
        output+=Form(" (nbin/min/max) = %4d %6.2f %8.2f ::"
            ,ax->GetNbins(),ax->GetXmin(), ax->GetXmax());
        for( auto j : bin_range( min(ax->GetNbins(),10) ))
          Printf(" %s", FormSelectAndTrim(ax->GetBinLabel(j), ax->GetBinLowEdge(j)).Data() );
      }else{
        //==== Print User Axis
        output+=Form(" (nbin/min/max) = %4d %6.2f %8.2f ::"
            ,int(uax.size()), uax.front(), uax.back());
        for( auto j : bin_range(ax->GetNbins()).lt(10) )
          output+=Form(" %s", FormSelectAndTrim("", uax.at(j)).Data());
      }
    }
    if(opts.Contains("string",TString::kIgnoreCase))
      return output;
    else
      std::cout<<output<<std::endl;
    return "";
  }
  //____________________________________________________
  TString TxHnSparseHelperBase::PrintAxis(Option_t* opt){
    TString output;
    for( auto i : DimRange() ) output+=PrintAxis( i, opt )+"\n";
    return output;
  }

  //____________________________________________________
  Double1D TxHnSparseHelperBase::Flat( Double2D values){
    Double1D v;
    for( auto i : values )
      for( auto j : i )
        v.push_back(j);
    return v;
  }

  //____________________________________________________
  Long64_t TxHnSparseHelperBase::Fill( Double2D values, Double_t weight ){
    return fH->Fill( &Flat(values).front(), weight );
    //TODO
  }
  //____________________________________________________
  Long64_t TxHnSparseHelperBase::Fill( Double1D values, Double_t weight ){
    return fH->Fill( &values.front(), weight );
    //TODO
  }

  //____________________________________________________
  TxHnSparseHelper& TxHnSparseHelper::operator=(THnSparse* o){
    if(!o) ROOTX_ERROR(-1,1,"No Object");
    fH = dynamic_cast<THnSparse*>(o);
    if(!fH) ROOTX_ERROR(-1,1,o->GetName()+" is not THnSparse but "+o->ClassName());
    ResetUserAxisAll();
    return *this;
  }
  //____________________________________________________
  THnSparse*  TxHnSparseHelper::Create(const char* name, const char* title, 
      TAxis1D bins, Option_t* opt,  Int_t chunksize){
    Int1D nbin;
    Double1D xmin,xmax;
    for( auto ax : bins ) {
      nbin.push_back( ax.GetNbins() );
      xmin.push_back( ax.GetXmin()  );
      xmin.push_back( ax.GetXmax() );
    }
    auto h = new THnSparseD(name,title,nbin.size(),&nbin.front(),&xmin.front(),&xmax.front(),chunksize);
    for(auto i : range(nbin))
      *(h->GetAxis(i)) = bins[i];
    TString opts = opt;
    if( opts.Contains("s") ) h->Sumw2();
    return h;

  }

  //____________________________________________________
  THnSparse* TxHnSparseHelper::Load(TString name, TObject * clist){
    if( !clist ) clist = gDirectory;
    auto o = clist->FindObject(name);
    if(!o ) ROOTX_ERROR(-1,1,"No THnSparse "+name );
    return dynamic_cast<THnSparse*>(o);
  }

  //____________________________________________________




#ifdef ROOTX_DEV
  ClassImp( ROOTX:TxHnSparseT );
#endif
}
