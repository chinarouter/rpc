#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "rpc.h"

typedef struct {
  int fd;
  struct sockaddr_in to;
}rpc_cli_ctx_t;

void* rpc_cli_ctx_new(rpc_init_t *init)
{
  //create conn ctx;
  rpc_cli_ctx_t *ctx = calloc(1, sizeof(rpc_cli_ctx_t));
  ctx->fd = socket(PF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in addr;
  ctx->to.sin_family = PF_INET;
  ctx->to.sin_port = htons(init->port);
  inet_aton(init->addr, &ctx->to.sin_addr);

  struct timeval tv;
  tv.tv_sec  = init->timeout_ms/1000;
  tv.tv_usec = (init->timeout_ms%1000)*1000;
  setsockopt(ctx->fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

  return (void*)ctx;
}
int   rpc_cli_ctx_free(void *_ctx)
{
  //destroy conn ctx;
  rpc_cli_ctx_t *ctx = (rpc_cli_ctx_t *)_ctx;
  if(ctx->fd > 0)
    close(ctx->fd);
  free(ctx);
}
int   rpc_cli_sendrecv(void *_ctx, rpc_msg_t *msg_buf, int buf_size)
{
  rpc_cli_ctx_t *ctx = (rpc_cli_ctx_t *)_ctx;
  
  struct sockaddr_in fromaddr;
  int len = sizeof(fromaddr);
  sendto(ctx->fd, msg_buf, msg_buf->size+sizeof(rpc_msg_t), 0, (struct sockaddr *)&ctx->to, sizeof(ctx->to));
  printf("%s => name:%s, to:%s\n", __func__, msg_buf->name, inet_ntoa(ctx->to.sin_addr));
  
  int ret = recvfrom(ctx->fd, msg_buf, buf_size, 0, (struct sockaddr *) &fromaddr, &len);
  if (ret < 0)
  {
     if (errno == EWOULDBLOCK)
     {
        return RPC_ERR_TIMEOUT;
     }
     return RPC_ERR_RECV;
  }
  
  printf("%s => name:%s, from:%s\n", __func__, msg_buf->name, inet_ntoa(fromaddr.sin_addr));
  return msg_buf->err;
}

typedef struct {
  int fd;
}rpc_serv_ctx_t;


void *rpc_serv_ctx_new(rpc_init_t *init)
{
  //create listen ctx;
  rpc_serv_ctx_t *ctx = calloc(1, sizeof(rpc_serv_ctx_t));
  ctx->fd = socket(PF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in addr;
  addr.sin_family = PF_INET;
  addr.sin_port = htons(init->port);
  inet_aton(init->addr, &addr.sin_addr);
  int res = bind(ctx->fd, (struct sockaddr *)&addr, sizeof(addr));
  
  struct timeval tv;
  tv.tv_sec  = init->timeout_ms/1000;
  tv.tv_usec = (init->timeout_ms%1000)*1000;
  setsockopt(ctx->fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  
  return (void*)ctx;
}
int  rpc_serv_ctx_free(void *_ctx)
{
  //destroy listen ctx;
  rpc_serv_ctx_t *ctx = (rpc_serv_ctx_t *)_ctx;
  if(ctx->fd > 0)
    close(ctx->fd);
  free(ctx);
}


int  rpc_serv_recvmsg(void *_ctx, rpc_msg_t *msg_buf, int buf_size, struct sockaddr *from)
{
  //accept && recv msg;
  rpc_serv_ctx_t *ctx = (rpc_serv_ctx_t *)_ctx;

  int len = sizeof(struct sockaddr);
  int ret = recvfrom(ctx->fd, msg_buf, buf_size, 0, from, &len);
  if (ret < 0)
  {
     if (errno == EWOULDBLOCK)
     {
       return 0; 
     }
     return -1;
  }
  
  struct sockaddr_in *fromaddr = (struct sockaddr_in *)from;
  printf("%s => name:%s, from:%s\n", __func__, msg_buf->name, inet_ntoa(fromaddr->sin_addr));
  
  return msg_buf->size + sizeof(rpc_msg_t);
}

int  rpc_serv_sendmsg(void *_ctx, rpc_msg_t *msg_buf, int buf_size, struct sockaddr *to)
{
  //sendto;
  rpc_serv_ctx_t *ctx = (rpc_serv_ctx_t *)_ctx;
  
  sendto(ctx->fd, msg_buf, msg_buf->size + sizeof(rpc_msg_t), 0, to, sizeof(*to));
  
  struct sockaddr_in *toaddr = (struct sockaddr_in *)to;
  printf("%s => name:%s, to:%s\n", __func__, msg_buf->name, inet_ntoa(toaddr->sin_addr));
  
  return msg_buf->size + sizeof(rpc_msg_t);
}