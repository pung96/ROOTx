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
    TxDebug(){}
    TxDebug( Int_t error, const char * msg, int type):
      fError(error), fMsg(msg), fType(type){}
    void Run(){ Run( fType, fLevel, fError, fMsg ); }
    void Run( int type, int level, int error, TString msg, bool istrue=true){
      if( !istrue ) return;
      std::cout<<Form("%s:%d: %s"
          ,fTypeString[type].Data()
          ,error
          ,msg.Data())
        <<std::endl;
      if( level < 0 ) gSystem->Exit(error);
    }
    static TxDebug* Instance(){
      static TxDebug * debug = new TxDebug;
      return debug;
    }
  private:
    Int_t   fError = 0;
    TString fMsg   = "";
    int     fType  = 0;
    int     fLevel = 0;
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
