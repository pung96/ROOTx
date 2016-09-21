#ifndef ROOTX_TxBaseUtil_H
#define ROOTX_TxBaseUtil_H

#define ROOTX_DEBUG_LEVEL 1

#include <stdarg.h>
#include <vector>
#include <algorithm>
#include <TAxis.h>
class TString;

namespace ROOTX {

  typedef std::vector<Int_t>    Int1D;
  typedef std::vector<Int1D>    Int2D;
  typedef std::vector<Double_t> Double1D;
  typedef std::vector<Double1D> Double2D;
  typedef std::vector<TString>  String1D;
  typedef std::vector<String1D> String2D;
  typedef std::vector<TAxis>    TAxis1D;

  const int kIntMin = std::numeric_limits<int>::min()+1;
  const int kIntMax = std::numeric_limits<int>::max()-1;

  TString operator+(const TString &s, int n){ return Form("%s%d",s.Data(),n);}
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

#include <iostream>
#include <TString.h>
#include <TSystem.h>
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

#include <TAxis.h>
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
    TxRangeI& operator+(int n){fEnd+=n;return *this;}
    TxRangeI& less_than(int n){SetRange(fBegin,std::min(fEnd+1,n)-1);return *this;}//
    TxRangeI& greater_than(int n){SetRange(std::max(fBegin-1,n)+1,fEnd);return *this;}//
    TxRangeI& lt(int n){return less_than(n);}     // Alias of less_than
    TxRangeI& gt(int n){return greater_than(n);}     // Alias of less_than
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
    TxRangeI bin_range( const std::vector<T> &t, int begin=1, int end=kIntMin ){ return bin_range( begin, (end<0?std::min(int(t.size()-1),abs(end)):end));  }
  TxRangeI bin_range( TAxis ax, int begin=1, int end=kIntMax){ return bin_range(begin,end).lt(ax->GetNbins());}
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

  TAxis AxisFix(const char* name, const char * title, 
      Int_t nbin, Double_t xmin, Double_t xmax);
  TAxis AxisLog(const char* name, const char * title, 
      Int_t nbin, Double_t xmin, Double_t xmax, Double_t xbegin=0);
  TAxis AxisStr(const char* name, const char * title, 
      String1D bin );
  TAxis AxisVar(const char* name, const char * title, 
      Double1D bins );

  class TxHnSparseHelperBase {
  public:
    ~TxHnSparseHelperBase(){}

    TxRangeI BinUserRange(int iaxis){ return bin_range(GetUserAxis(iaxis));}
    TxRangeI BinUserRange(const char* name){ return bin_range(GetUserAxis(name));}
    TxRangeI DimRange(){ return range(fH->GetNdimensions());}

    TString FormSelectAndTrim( TString s, Double_t d );
    Double1D Flat( Double2D values );

    int   GetAxisIndex(const char* name);
    TAxis* GetAxis(const char* name){ return fH->GetAxis(GetAxisIndex(name)); }
    TAxis* GetAxisSafe(int iaxis);
    const Double1D& GetUserAxis(int iaxis){return fUserAxis.at(iaxis);}
    const Double1D& GetUserAxis(const char* name){return GetUserAxis(GetAxisIndex(name));}
    void  ResetUserAxis(Int_t iaxis);
    void  ResetUserAxisAll();
    const Double1D& SetUserAxis( Int_t iaxis, Double1D bin );
    const Double2D& SetUserAxis( Double2D bin );

    const Double1D& GetUAxis(int iaxis){return GetUserAxis(iaxis);}
    const Double1D& GetUAxis(const char* name){return GetUserAxis(name);}
    void  ResetUAxis(Int_t iaxis){ResetUserAxis(iaxis);}
    void  ResetUAxisAll(){ResetUserAxisAll();}
    const Double1D SetUAxis( Int_t iaxis, Double1D bin );
    const Double2D SetUAxis( Double2D bin );

    TH1* GetTH1(TString name, int iaxis, Int2D values,Option_t* opt="");
    TH1* GetTH1(TString name, int iaxis, Int1D values,Option_t* opt="");
    TH1* GetTH1(TString name, Int1D values, Option_t*opt="" ){ 
      return GetTH1(name, fH->GetNdimensions(), values, opt);
    }

    Long64_t Fill( Double1D values, Double_t weight=1. );
    Long64_t Fill( Double2D values, Double_t weight=1. );

    TString PrintAxis(int i,Option_t* opt="");
    TString PrintAxis(Option_t* opt="");
    void PrintUserAxis(int i, Option_t* opt){ PrintAxis(i,TString(opt)+"user");}
    void PrintUserAxis(Option_t* opt){ PrintAxis(TString(opt)+"user");}
    void PrintUAxis(int i,Option_t* opt){ PrintUserAxis(i,opt); }
    void PrintUAxis(Option_t* opt){ PrintUserAxis(opt); }

  protected:
    TxHnSparseHelperBase(){};
    TxHnSparseHelperBase(THnSparse*h):fH(h){}
    Double2D    fUserAxis;
    Int2D       fUserAxisMap;
    THnSparse*  fH;
  };

  class TxHnSparseHelper : public TxHnSparseHelperBase {
  public:
    TxHnSparseHelper ( THnSparse *h ):
      TxHnSparseHelperBase(h)
    { ResetUserAxisAll(); }
    TxHnSparseHelper& operator=(THnSparse *o);
    THnSparse * operator->(){ return fH; }
    THnSparse * Data() { return fH; }

    static THnSparse* Create(const char* name, const char* title, 
        TAxis1D bins, Option_t*opt="", Int_t chunksize = 1024 * 16);
    static THnSparse* Load(TString name, TObject * clist);
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
