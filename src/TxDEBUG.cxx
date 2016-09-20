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
