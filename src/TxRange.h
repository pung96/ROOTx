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
