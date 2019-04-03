#include "rpc.h"
#include "rpc_s1.h"
int main()
{
  char buf[64*1024];
  rpc_msg_t *msg = (rpc_msg_t*)buf;
  
  rpc_entry_t *s, *tmp, *funcs;
  RPC_INIT_s1(funcs);
  
  rpc_init_t init;
  init.proto = RPC_PROTO_IUDP;
  strcpy(init.addr, "0.0.0.0");
  init.port = 5555;
  init.timeout_ms = 3*1000;
  
  void *__ctx = rpc_serv_ctx_new(&init);
  
  struct sockaddr peer;
  while(1)
  {
    int r = rpc_serv_recvmsg(__ctx, msg, sizeof(buf), &peer);
    
    if(r < 0)
      break;
    else if(r == 0)
      continue;
    
    HASH_FIND_STR(funcs, msg->data, s);
    if (s)
    {
      int rsi = 0;
      char req[64*1024] = {0};
      char rsp[64*1024] = {0};
      
      if(msg->size > strlen(msg->data)+1 && s->req)
      {
        cJSON* in = cJSON_Parse(msg->data + strlen(msg->data)+1);
        s->req(in, 1, req, 0, 0);
        cJSON_Delete(in);
      }
      
      msg->size = strlen(msg->data)+1;
      if((msg->err = s->ucb(req, 0, rsp, &rsi)) == 0)
      {
        if(rsi > 0 && s->rsp)
        {
          cJSON* out = cJSON_CreateObject();
          s->rsp(out, 0, rsp, 0, 0);
          char* print = cJSON_Print(out);
          if(print)
          {
            sprintf(msg->data+msg->size, "%s", print);
            msg->size += strlen(print)+1;
            free(print);
          }
          cJSON_Delete(out);
        }
      }
      printf("%s => %s' func is %p\n", __func__, msg->data, s->ucb);
    }
    else
    {
      msg->size = 0;
      msg->err  = RPC_ERR_UNFUNC;
      printf("%s => %s' func is unfunc\n", __func__, msg->data);
    }
    rpc_serv_sendmsg(__ctx, msg, sizeof(buf), &peer);
  }

  HASH_ITER(hh, funcs, s, tmp)
  {
    HASH_DEL(funcs, s);
    free(s);
  }
  rpc_serv_ctx_free(__ctx);
    
  printf("exit.\n");  
  return 0;
}