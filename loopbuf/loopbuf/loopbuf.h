#include <stdio.h>
#include <pthread.h>
#include <string.h> 
#include <stdlib.h>

#define min(a,b) (((a)<(b))?(a):(b))

typedef int int32_t;
typedef unsigned int uint32_t;
typedef short    int16_t;
typedef unsigned short uint16_t;
typedef unsigned uint8_t;


typedef struct{
    uint8_t * mbuf;
    int32_t bufsize;
    uint32_t p_read;
    uint32_t p_write;
    uint32_t is_w;
    uint32_t is_r;
}loopbuf_t;

loopbuf_t * loopbuf_init(loopbuf_t *lb,uint32_t bufsize);

void loopbuf_exit(loopbuf_t *lb);

int32_t put_buffer(loopbuf_t * lb,uint8_t * buf,int32_t size);

int32_t get_buffer(loopbuf_t *lb,uint8_t * buf,int32_t size);


