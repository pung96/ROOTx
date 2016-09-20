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
      TString1D bin );
  TAxis AxisVar(const char* name, const char * title, 
      Double1D bins );

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

    void PrintAxis(int i,Option_t* opt);
    void PrintAxis(Option_t* opt);
    void PrintUserAxis(int i, option_t* opt){ printAxis(i,TString(opt)+"user");}
    void PrintUserAxis(option_t* opt){ printAxis(TString(opt)+"user");}
    void PrintUAxis(int i,option_t* opt){ PrintUserAxis(i,opt); }
    void PrintUAxis(option_t* opt){ PrintUserAxis(opt); }

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
        TAxis1D bins, Option_t*opt="", Int_t chunksize = 1024 * 16);
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
