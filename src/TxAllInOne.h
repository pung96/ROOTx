#ifndef ROOTX_TxBaseUtil_H
#define ROOTX_TxBaseUtil_H

#define ROOTX_DEBUG_LEVEL 1

#include <stdarg.h>
#include <vector>
#include <algorithm>
#include <TAxis.h>

namespace ROOTX {

  typedef std::vector<Int_t>    Int1D;
  typedef std::vector<Int1D>    Int2D;
  typedef std::vector<Double_t> Double1D;
  typedef std::vector<Double1D> Double2D;
  typedef std::vector<TAxis>    TAxis1D;

  const int min_int = std::numeric_limits<int>::min()+1;
}


#define ROOTX_VA_FORMAT(x,y)
#if defined(__GNUC__) && !defined(__CINT__)
#undef ROOTX_VA_FORMAT
#define ROOTX_VA_FORMAT(x,y)  __attribute__((format(printf, x, y)))
#endif
#define ROOTX_VA_MACRO(x,y) va_list ap;va_start(ap, va_(y));x=TString::Format( va_(y), ap );va_end(ap);

#endif
#ifndef ROOTX_TXDEBUG_H
#define ROOTX_TXDEBUG_H

#include <stdarg.h>
#include <TString.h>
#include "TxBaseUtil.h"


#define ROOTX_DEBUG(x,y,z)
#define ROOTX_DEBUG_IF(x,y,z,i)
#ifdef  ROOTX_DEBUG_LEVEL
#undef  ROOTX_DEBUG
#undef  ROOTX_DEBUG_IF
#define ROOTX_DEBUG(x,y,z) TxDebug::Instance()->Run(TxDebug::kDEBUG,x,y, TString("")+z)
#define ROOTX_DEBUG_IF(l,e,m,i) TxDebug::Instance()->Run(TxDebug::kDEBUG,l,e,TString("")+m,i)
#endif

#define ROOTX_WARN(x,y,z)  TxDebug::Instance()->Run(TxDebug::kWARN, x,y, TString("")+z)
#define ROOTX_WARN_IF(l,e,m,i)  TxDebug::Instance()->Run(TxDebug::kWARN, l,e,TString("")+m,i)

#define ROOTX_ERROR(x,y,z) TxDebug::Instance()->Run(TxDebug::kERROR,x,y, TString("")+z)
#define ROOTX_ERROR_IF(l,e,m,i) if(i){TxDebug::Instance()->Run(TxDebug::kERROR,l,e,TString("")+m);}

namespace ROOTX {
  class TxDebugBase {
  };
  class TxDebug : public TxDebugBase {
  public:
    enum { kDEBUG, kERROR, kWARN };
    TxDebug():
      fError(0), fMsg(0), fType(0){}
    TxDebug( Int_t error, const char * msg, int type):
      fError(error), fMsg(msg), fType(type){}
    void Run(){
      std::cout<<Form("%s:%d: %s"
          ,fTypeString[fType].Data()
          ,fError
          ,fMsg.Data())
        <<std::endl;
    }
    void Run( int type, int level, int error, TString msg, bool istrue=true){
      if( !istrue ) return;
      std::cout<<Form("%s:%d: %s"
          ,fTypeString[type].Data()
          ,error
          ,msg.Data())
        <<std::endl;
      if( level < 0 ) gSystem->Exit(error);
    }
    //TxDebug * operator()( int level, int error ){fLevel=level;fError=error;}
    static TxDebug* Instance(){
      static TxDebug * debug = new TxDebug;
      return debug;
    }
  private:
    Int_t   fError;
    TString fMsg;
    int     fType;
    int     fLevel;
    static vector<TString> fTypeString;
  };

  vector<TString> TxDebug::fTypeString = { "DEBUG","ERROR","WARN" };

  /*
     void DEBUG(int e, const char * fmt,... ) ROOTX_VA_FORMAT(2,3);
     void DEBUG(int e, const char * fmt,... ){
     TString msg;ROOTX_VA_MACRO(msg,fmt);
     }
     */



}// namespace
#endif
#ifndef ROOTX_TXRANGE_H
#define ROOTX_TXRANGE_H

#include "TxBaseUtil.h" // #NotToMerge
#include "TxDEBUG.h"    // #NotToMerge

namespace ROOTX {
  class TxRangeI {
  public:
    typedef int value_type;
    struct iterator {
      iterator(size_t counter) : counter(counter) {}
      iterator operator++(){ return iterator(++counter); }
      bool operator!=(iterator o) { return counter != o.counter; }
      value_type operator*(){ return value_type{counter}; }
    private:
      value_type counter;
    };
    TxRangeI(int begin, int end){ SetRange(begin,end); }
    iterator begin(){ return iterator(fBegin); }
    iterator end()  { return iterator(fEnd+1);   }
  private:
    void SetRange( int begin, int end ){if( begin>end ) ROOTX_ERROR(-1,1,"being>end"); fBegin=begin;fEnd=end;}
    int fBegin;
    int fEnd;
  };

  template< class T >
    TxRangeI range( T & t, int begin=0, int end=-1 ){ return TxRangeI( begin, end<0?t.size()-1:end  ); }
  TxRangeI range( int end ){ return TxRangeI( 0, end-1 ); }
  TxRangeI range( int begin, int end ){ return TxRangeI( begin, end ); }

  TxRangeI bin_range( int begin, int end ){
    if( begin < 1 ) ROOTX_ERROR(-1,1,"begin for bin_range must larger than 0 ");
    return range( begin, end );
  }
  TxRangeI bin_range( int end ){ return bin_range( 1, end ); }
  template<class T>
    TxRangeI bin_range( const std::vector<T> &t, int begin=1, int end=min_int ){ return bin_range( begin, (end<0?std::min(int(t.size()-1),abs(end)):end));  }
}
#endif
#ifndef ROOTX_TxHelper_H
#define ROOTX_TxHelper_H

#ifdef ROOTX_DEV
namespace ROOTX {
  class TxHelper {
  };
}
#endif

#endif
#include <THnSparse.h>
#ifndef ROOTX_TXHNSPARSE_H
#define ROOTX_TXHNSPARSE_H

#include "TxBaseUtil.h" // #NotToMerge
#include "TxDEBUG.h"    // #NotToMerge
#include "TxRange.h"    // #NotToMerge

namespace ROOTX{
  class TxHnSparseHelperBase {
  public:
    ~TxHnSparseHelperBase(){}

    TxRange BinRange(int iaxis){ return bin_range(GetBin(iaxis));}
    TxRange BinRange(const char* name){ return binRange(GetBin(name));}

    int   GetAxisIndex(const char* name);
    TAxis* GetAxis(const char* name){ return fH->GetAxis(GetAxisIndex(name)); }
    const Double1D& GetUserAxis(int iaxis){return fUserBin.At(iaxis);}
    const Double1D& GetUserAxis(const char* name){return GetUserAxis(GetAxisIndex(name));}
    void  ResetUserAxis(Int_t iaxis);
    void  ResetUserAxisAll();
    const Double1D SetUserAxis( Int_t iaxis, Double1D bin );
    const Double2D SetUserAxis( Double2D bin );

    const Double1D& GetUAxis(int iaxis){return GetUserAxis(iaxis);}
    const Double1D& GetUAxis(const char* name){return GetUserAxis(name);}
    void  ResetUAxis(Int_t iaxis){ResetUserAxis(iaxis);}
    void  ResetUAxisAll(){ResetUserAxisAll();}
    const Double1D SetUAxis( Int_t iaxis, Double1D bin );
    const Double2D SetUAxis( Double2D bin );

    TH1* GetTH1(TString name, int iaxis, Double1D values,Option_t* opt="");
    TH1* GetTH1(TString name, Double1D values, Option_t*opt="" ){ 
      return GetTH1(name, fH->GetNdimensions(), values, opt);
    }

    Long64_t Fill( Double2D values, Double_t weight );

    void PrintAxis(Option_t* opt);
    void PrintUserAxis(option_t* opt);
    void PrintUAxis(option_t* opt){ PrintUserAxis(opt); }

    static TAxis AxisFix(const char* name, const char * title, 
        Int_t nbin, Double_t xmin, Double_t xmax);
    static TAxis AxisLog(const char* name, const char * title, 
        Int_t nbin, Double_t xmin, Double_t xmax, Double_t xbegin=0);
    static TAxis AxisVar(const char* name, const char * title, 
        Double1D bins );
  private:
    TxHnSparseHelperBase(){};
    Double2D    fUserBin;
    Int2D       fUserBinMap;
    THnSparse*  fH;
    ClassDef( ROOTX::TxHnSparseHelperBase, 1 );
  };

  class TxHnSparseHelper {
  public:
    TxHnSparseHelper ( THnSparse *h ):fH(h){ ResetUserAxisAll(); }
    TxHnSparseHelper ( TObject   *o );

    THnSparse * operator->(){ return fH; }
    THnSparse * Data() { return fH; }

    static TxHnSparseHelper Create(const char* name, const char* title, 
        Double2D bins, Int_t chunksize = 1024 * 16);
    static TxHnSparseHelper Create(const char* name, const char* title, 
        TAxis1D bins,  Int_t chunksize = 1024 * 16);
    static TxHnSparseHelper* Load(TString name, TObject * clist);
    ClassDef( ROOTX::TxHnSparseHelper, 1);
  };


#ifdef ROOTX_DEV
  template <class CONT> 
    class TxHnSparseT : public THnSparseT<CONT>, TxHnSparseHelperBase {
    public:
      TxHnSparseT() {}
      TxHnSparseT(const char* name, const char* title, Int_t dim,
          const Int_t* nbins, const Double_t* xmin = 0,
          const Double_t* xmax = 0, Int_t chunksize = 1024 * 16):
        THnSparseT(name, title, dim, nbins, xmin, xmax, chunksize) {}
      virtual ~TxHnSparse();

      TxHnSparse(THnSparse & h);

    private:
      ClassDef( ROOTX::TxHnSparseT, 1 );
    };
#endif
}
#endif
#include "TxBaseUtil.h" // #NotToMerge
#ifndef ROOTX_TXDEBUG_H
#include "TxDEBUG.h"
#endif

namespace ROOTX {

#ifdef ROOTX_INCLUDE_TEST
  void _testTxDebug(){
    ROOTX_DEBUG( 1, 1, "abc");
    ROOTX_DEBUG( 1, 2, 123l+"abc" );
    ROOTX_WARN( 1, 3, 123l+"abc" );
    ROOTX_WARN_IF( 1, 4, 123l+"abc",1 );
    ROOTX_WARN_IF( 1, 5, 123l+"abc",
        0 
        );
    ROOTX_ERROR_IF( 1, 6, 123l+"abc",false );

    /*
       DEBUG( 1,"This is error %d", 4);
       DEBUG( 2 );
       */
    //DEBUG(1, "This is error %d", 4);
    //DEBUG(1 "aa" << "This is error");
    //DEBUG( 1, isERROR );

    //ERROR( 2 );
    //WARN( 3 );
  }
#endif

}
#ifndef ROOTX_TXRANGE_H
#include "TxRange.h"
#endif

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
