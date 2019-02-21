/*
 * rpc: remote procedure call;
 * rpc_xxx_c() : client call func;  
 * rpc_xxx_s() : server call func;
 * auther: 25088970@qq.com
 */
#ifndef __rpc_h__
#define __rpc_h__

#include <string.h>  /* strcpy */
#include <stdlib.h>  /* malloc */
#include <stdio.h>   /* printf */
#include <assert.h>  /* assert */
#include <sys/socket.h>

#include "uthash.h"

//--- err;
enum {
  RPC_ERR_SEND    = -10001,  /* send error   */
  RPC_ERR_RECV    = -10002,  /* recv error   */
  RPC_ERR_TIMEOUT = -10003,  /* recv timeout */
  RPC_ERR_UNFUNC  = -10004,  /* unknown func */
};

//--- msg;
typedef struct {
  char name[32];     /* msgid      */
  int  err;          /* err        */
  int  size;         /* body size  */
  char data[0];      /* body data  */
}rpc_msg_t;


//--- init;
enum {
  RPC_PROTO_UUDP  = 0, /* unix socket udp   */
  RPC_PROTO_UTCP  = 1, /* unix socket tcp   */
  RPC_PROTO_IUDP  = 2, /* ip socket udp     */
  RPC_PROTO_ITCP  = 3, /* ip socket tcp     */
};

typedef struct {
  int proto;            /* rpc_proto          */
  char addr[64];        /* socket path        */
  unsigned short port;  /* tcp/udp port       */
  int timeout_ms;       /* timeout ms         */
}rpc_init_t;

//--- client;
void* rpc_cli_ctx_new(rpc_init_t *init);
int   rpc_cli_ctx_free(void *_ctx);
int   rpc_cli_sendrecv(void *_ctx, rpc_msg_t *msg_buf, int buf_size);

//--- server;
typedef void (sjb_serialize_cb)(void *c, int m, void *stru, int r1, int r2);
typedef int (rpc_ucb_cb)(void *req, int si, void *rsp, int *rsi);
typedef struct {
    char name[32];           /* key (string is WITHIN the structure) */
    sjb_serialize_cb *req;   /* req serialize  */
    sjb_serialize_cb *rsp;   /* rsp serialize  */
    rpc_ucb_cb *ucb;         /* usercb         */
    UT_hash_handle hh;       /* makes this structure hashable */
}rpc_entry_t;

void *rpc_serv_ctx_new(rpc_init_t *init);
int  rpc_serv_ctx_free(void *_ctx);
int  rpc_serv_recvmsg(void *_ctx, rpc_msg_t *msg_buf, int buf_size, struct sockaddr *from);
int  rpc_serv_sendmsg(void *_ctx, rpc_msg_t *msg_buf, int buf_size, struct sockaddr *to);

//--- funcs;

static void sjb_bind_nil(void* json, int m, void *str, int i, int size) { return; }

#define RPC_DEF_FUNC(_name, args...) int rpc_##_name(args)
        
#define RPC_HASH_ADD(funcs, _name, _req, _rsp) do {       \
      rpc_entry_t *s = (rpc_entry_t*)malloc(sizeof(*s));  \
      strncpy(s->name, #_name, sizeof(s->name)-1);        \
      s->req = (sjb_serialize_cb*)sjb_bind_##_req;\
      s->rsp = (sjb_serialize_cb*)sjb_bind_##_rsp;\
      s->ucb = (rpc_ucb_cb*)rpc_##_name; \
      HASH_ADD_STR(funcs, name, s); \
    }while(0)



#endif //__rpc_h__