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
