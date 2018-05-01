#include "loopbuf.h"

loopbuf_t *lb;
uint8_t * rbuf;
uint8_t * wbuf;
#define B_SIZE 64* 1024
#define F_SIZE 4*1024

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

void *read_file(void *arg){
    FILE *fp =(FILE*)arg;
    uint32_t len;
    while(1){
        pthread_mutex_lock(&mutex);
            fread(rbuf,F_SIZE,1,fp);
            printf("rbuf = %s\n",rbuf);
            pthread_mutex_unlock(&mutex);
            put_buffer(lb,rbuf,F_SIZE);

        if(feof(fp)){
            pthread_exit(NULL);
        }
    }
}

void *write_file(void *arg){
    FILE * fp = (FILE *)arg;
    uint32_t ret;

    while(1){
        pthread_mutex_lock(&mutex);
        get_buffer(lb,wbuf,F_SIZE);
        printf("wbuf = %s\n",wbuf);
        pthread_mutex_unlock(&mutex);
        if(wbuf != NULL){
             ret = fwrite(wbuf,strlen(wbuf),1,fp);
            fflush(fp);
        }
        if(ret != F_SIZE && ret !=0){
            break;
        }
    }
}

int main(int argc, const char *argv[])
{
    
    pthread_t tid[2];
    FILE * fp[2];

    lb = loopbuf_init(lb,B_SIZE);
    rbuf = (uint8_t *)malloc(F_SIZE);
    wbuf = (uint8_t *)malloc(F_SIZE);

    fp[0] = fopen(argv[1],"r");
    fp[1] = fopen("backup","w+");

    pthread_create(&tid[0],NULL,read_file,fp[0]);
    pthread_create(&tid[1],NULL,write_file,fp[1]);

    pthread_join(tid[0],NULL);
    pthread_join(tid[1],NULL);

    return 0;
}
