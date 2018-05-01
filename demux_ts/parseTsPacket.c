#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Demux.h"
#include <stdlib.h>
#include <unistd.h>

struct section_pat{
    uint32_t table_id;
    uint32_t program_number[10];
};
void pat_pProc(void * handle,void * filter,unsigned char *buf,int32_t size,uint32_t lParam){
    printf("------------------------------------\n");

    printf("table_id = %#x \n",buf[0]);
    printf("section_indicator = %#x \n",buf[1] & ~(0x7f<< 1));
    printf("section_length = %#x \n",(buf[2]&0xf) << 8 | buf[3]);
    printf("transport_stream_id = %#x \n",(buf[4] << 8 | buf[5]));
}

void cat_pProc(void * handle,void * filter,unsigned char * buf,int32_t size,uint32_t lParam){
    printf("cat ---------------\n");
    printf("table_id = %#x \n",buf[0]);
}
int main(int argc, const char *argv[]){

    SFilterNode * pat_node = NULL;
    SFilterNode * cat_node = NULL;
    SFilterNode * nit_node = NULL;

    SDemux *demux = NULL;
    
    int fd ;
    int n = 0;
    unsigned char buf[4096] ={0};

    demux = (SDemux *)Demux_Create();

    pat_node = (SFilterNode *)Demux_CreateFilterEx(demux,0x00,0x00,0xff,0,0xff,sizeof(buf),pat_pProc,1);
    cat_node = Demux_CreateFilterEx(demux,0x0001,0x0001,0,0x09,0,sizeof(buf),cat_pProc,1);
    
    fd = open("./stream/251-small.ts",O_RDONLY);
    if(fd < 0){
        perror("Fail to open ");
        exit(EXIT_FAILURE);
    }

    do{
        n = read(fd,buf,sizeof(buf));
        Demux_OnData(demux,&buf[0],sizeof(buf));

        if(n < 0){
            perror("Fail to read");
            exit(EXIT_FAILURE);
        }

    }while(n > 0);

    return 0;
}
