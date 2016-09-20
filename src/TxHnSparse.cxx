namespace ROOTX {

  //____________________________________________________
  TAxis AxisFix(const char* name, const char * title, 
      Int_t nbin, Double_t xmin, Double_t xmax){
    TAxis ax(nbin,xmin,xmax);ax.SetNameTitle(name,title);
    return ax;
  }

  //____________________________________________________
  TAxis AxisStrx(const char* name, const char * title, 
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
    //TODO
  }

  //____________________________________________________
  void  TxHnSparseHelperBase::ResetUserAxisAll(){
    //TODO
  }

  //____________________________________________________
  const Double2D TxHnSparseHelperBase::SetUserAxis( Double2D bin ){
    //TODO
  }

  //____________________________________________________
  TH1* TxHnSparseHelperBase::GetTH1(TString name, int iaxis, Double1D values,Option_t* opt){
    //TODO
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
  TxHnSparseHelper TxHnSparseHelper::Create(const char* name, const char* title, 
      Double2D bins, Option_t* opt, Int_t chunksize){
    //TODO
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

  //____________________________________________________

  //____________________________________________________

  //____________________________________________________



  ClassImp( ROOTX:TxHnSparseT );
  static TxHnSparseT TxHnSparseT::Create(const char* name, const char* title, 
      std::vector<std::vector<Double_t> > bins,
      Int_t chunksize = 1024 * 16){
    THnSparseT<CONT> h( name, title, dim, nbins, xmin, xmax, chunksize );
  }
}
