namespace ROOTX {

  //____________________________________________________
  TAxis AxisFix(const char* name, const char * title, 
      Int_t nbin, Double_t xmin, Double_t xmax){
    TAxis ax(nbin,xmin,xmax);ax.SetNameTitle(name,title);
    return ax;
  }

  //____________________________________________________
  TAxis AxisStr(const char* name, const char * title, 
      TString1D bin ){
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
    int binoffset = ( xbegin<0 || (xmin-xmin0)<1e-9) ? 0 : 1;
    Double1D bin(nbin+1+binoffset,0);
    double logBW3 = (log(xmax)-log(xmin))/nbin;
    for(auto i : range(bin)+1)  bin[ij+binoffset]=xmin*exp(ij*logBW3);
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
    fUserAxis.resize(fH->GetNdimension());
    fUserAxisMap.resize(fH->GetNdimension());
    for( auto i : range(fH->GetNdimension()) )
      ResetUserAxis(i);
  }

  //____________________________________________________
  const Double1D & TxHnSparseHelperBase::SetUserAxis( int iaxis, Double1D bin ){
    auto ax = GetAxisSafe(iaxis);
    if( bins.size()==0 ) ResetUserAxis(i);
    else{
      fUserAxis[iaxis] = bin;
      fUserAxisMap[iaxis] = { 0 };
      for( auto i : range(bin) ){
        int ob = ax->FindBin( bin[i] );
        //TODO if( ib>0 && bin <= fCBinsB[i][ib-1] ) ErrorExit("Wrong bin");
        fUserAxisMap[iaxis].push_back( ax->FindBin( bins[ob] ) );
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
  TH1* TxHnSparseHelperBase::GetTH1(TString name, int iaxis, Double2D bin,Option_t* opt){
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
    auto h = fH->Projection( iaxis, opt );
    h->SetNameTitle(name,title);
    return h;
  }

  //____________________________________________________
  TH1* TxHnSparseHelperBase::GetTH1(TString name, int iaxis, Double1D bin,Option_t* opt){
    Double2D bins;
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
  void TxHnSparseHelperBase::PrintAxis( int i, Option_t* opt){
    using std::min;
    TString opts = opt;
    auto uax= fUserAxis.at(i);
    auto ax = GetAxisSafe(i);
    Printf("%2d: %10s",i, ax->GetName());
    if( opts.Contains("all") ){
      int nBinPrint = 10;
      if( opts.Contains("ALL",kExact) ) nBinPrint=1000;
      else if( opts.Contains("All",kExact) ) nBinPrint=100;
      if( !opts.Contains("user") || int(uax.size())==ax->GetNbins()+2 ){
        //==== Print Orignal Axis
        Printf(" (nbin/min/max) = %4d %6.2f %8.2f ::"
            ,ax->GetNbins(),ax->GetXmin(), ax->GetXmax());
        for( auto j : bin_range( min(ax->GetNbins(),10) ))
          Printf(" %s", FormSelectAndTrim(ax->GetBinLabel(), ax->GetBinLowEdge(j)).Data() );
      }else{
        //==== Print User Axis
        Printf(" (nbin/min/max) = %4d %6.2f %8.2f ::"
            ,uax.size(), uax.front(), uax.back());
        for( auto j : bin_range( min(ax->GetNbins(),10) ))
          Printf(" %s", FormSelectAndTrim("", uax.at(j)));
      }
      std::cout<<std::endl;
    }
  }
  //____________________________________________________
  void TxHnSparseHelperBase::PrintAxis(Option_t* opt){
    for( auto i : RangeDim() ) PrintAxis( i, opt );
  }

  //____________________________________________________
  Long64_t TxHnSparseHelperBase::Fill( Double2D values, Double_t weight ){
    return fH->Fill( &x.front(), w );
    //TODO
  }


  //____________________________________________________
  TxHnSparseHelper::TxHnSparseHelper ( TObject   *o ){
    if(!o) ROOTX_ERROR(-1,1,"No Object");
    fH = dynamic_cast<THnSparse*>(o);
    if(!fH) ROOTX_ERROR(-1,1,o->GetName()+" is not THnSparse but "+o->ClassName());
    ResetUserAxisAll();
  }

  //____________________________________________________
  TxHnSparseHelper  TxHnSparseHelper::Create(const char* name, const char* title, 
      TAxis1D bins, Option_t* opt,  Int_t chunksize){
    const TAxis * axises[bins.size()];
    for( auto i : range(bins) ) axises[i]= &bins[i];
    //TODO
    THnSparse * h= fHistos->CreateTHnSparse(name, title, ndim, axises,opt );
    return h;
  }

  //____________________________________________________
  TxHnSparseHelper* TxHnSparseHelper::Load(TString name, TObject * clist){
    if( !clist ) clist = gDirectory;
    auto o = clist->FindObject(name);
    if(!o ) ROOTX_ERROR(-1,1,"No THnSparse "+name );
    return TxHnSparseHelper(o);
  }

  //____________________________________________________




  ClassImp( ROOTX:TxHnSparseT );
}
