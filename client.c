#include "rpc.h"
#include "rpc_s1.h"

static rpc_entry_t *funcs;
static void *__ctx;

int cli_call(char *name, void *req, void *rsp)
{
  rpc_entry_t *s;
  char buf[64*1024] = {0};
  rpc_msg_t *msg = (rpc_msg_t*)buf;
  
  strncpy(msg->data, name, STRID_MAX_SIZE-1);
  msg->size = strlen(msg->data)+1;
  HASH_FIND_STR(funcs, msg->data, s);
  if (!s)
    return -1;
  
  if(s->req)
  {
    cJSON* out = cJSON_CreateObject();
    s->req(out, 0, req, 0, 0);
    char* print = cJSON_Print(out);
    if(print)
    {
      sprintf(msg->data + msg->size, "%s", print);
      msg->size += strlen(print)+1;
      free(print);
    }
    cJSON_Delete(out);
  }
  int r = rpc_cli_sendrecv(__ctx, msg, sizeof(buf));
  if(r < 0)
  {
    printf("call %s err r:%d\n", name, r);
  }
  else
  {
    printf("call %s ok r:%d\n", name, r);
    if(msg->size > strlen(msg->data)+1 && s->rsp)
    {
        cJSON* in = cJSON_Parse(msg->data + strlen(msg->data)+1);
        s->rsp(in, 1, rsp, 0, 0);
        cJSON_Delete(in);
    }
  }  
}

int main()
{
  int ret = 0;

  rpc_entry_t *s, *tmp;
  RPC_INIT_s1(funcs);
  
  rpc_init_t init;
  init.proto = RPC_PROTO_IUDP;
  strcpy(init.addr, "127.0.0.1");
  init.port = 5555;
  init.timeout_ms = 3*1000;
  __ctx = rpc_cli_ctx_new(&init);

  {  
    s1_req_t req;
    s1_rsp_t rsp;
    memset(&req, 0, sizeof(req));
    memset(&rsp, 0, sizeof(rsp));
    strcpy(req.name, "s1_req_t");
    req.sweetness = 66;
    
    ret = cli_call("s1_func1", &req, &rsp);
    printf("ret:%d, rsp.name:%s, rsp.color32:%d\n"
          , ret, rsp.name, rsp.color32);
  }


  HASH_ITER(hh, funcs, s, tmp)
  {
    HASH_DEL(funcs, s);
    free(s);
  }
  rpc_cli_ctx_free(__ctx);

  return 0;
}