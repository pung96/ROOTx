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
    TxHnSparseHelper ( TObject   *o );

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
