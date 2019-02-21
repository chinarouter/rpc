# rpc sample use sjb

//1
clinet, server: demo;
rpc* :  rpc struct;
sjb* :  sjb struct;
depend: cJSON uthash

//2
gcc -o c client.c rpc_s1.c rpc.c  sjb_struct_s1.c cJSON/cJSON.c  -lm
gcc -o s server.c rpc_s1.c rpc.c  sjb_struct_s1.c cJSON/cJSON.c  -lm

//3
./s
./c

