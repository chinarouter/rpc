#ifndef __s1_h__
#define __s1_h__

#include "sjb_struct_s1.h"
#include "rpc.h"


//define func;
RPC_DEF_FUNC(s1_func1, s1_req_t *req, int si, s1_rsp_t *rsp, int *rsi);
RPC_DEF_FUNC(s1_func2, s2_req_t *req, int si, void *rsp, int *rsi);

//bind func and serialize;
#define RPC_INIT_s1(funcs) do{ \
    RPC_HASH_ADD(funcs, s1_func1, s1_req_t, s1_rsp_t); \
    RPC_HASH_ADD(funcs, s1_func2, s2_req_t, nil);      \
  }while(0)

  
#endif //__s1_h__