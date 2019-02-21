#include "rpc.h"
#include "rpc_s1.h"

//--- funcs;


int  rpc_s1_func1(s1_req_t *req, int si, s1_rsp_t *rsp, int *rsi)
{
  if(req)
  {
    printf("%s => req->name:%s\n",  __func__, req->name); 
  }
  
  if(rsp)
  {
    strcpy(rsp->name, "s1_rsp_t");
    printf("%s => rsp->name:%s\n", __func__, rsp->name); 
    rsp->color32 = 5566;
    *rsi = sizeof(s1_rsp_t);
  }
  
  return 0;
}

int  rpc_s1_func2(s2_req_t *req, int si, void *rsp, int *rsi)
{
  
  if(req)
  {
    printf("%s => req->name:%s\n", __func__,  req->name); 
  }
  
  if(rsp)
  {
    printf("%s => rsp is nil\n", __func__);
    *rsi = 0;
  }
  
  return 0;
}
