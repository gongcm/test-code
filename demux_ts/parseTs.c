#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Demux.h"
#include <stdlib.h>
#include <unistd.h>

typedef struct TS_PAT_Program
{
    unsigned program_number   :  16;  //节目号
    unsigned program_map_PID :  13; // 节目映射表的PID，节目号大于0时对应的PID，每个节目对应一个
}TS_PAT_Program;

typedef struct TS_PAT
{
    unsigned table_id                     : 8; //固定为0x00 ，标志是该表是PAT表
    unsigned section_syntax_indicator     : 1; //段语法标志位，固定为1
    unsigned zero                         : 1; //0
    unsigned reserved_1                   : 2; // 保留位
    unsigned section_length               : 12; //表示从下一个字段开始到CRC32(含)之间有用的字节数
    unsigned transport_stream_id          : 16; //该传输流的ID，区别于一个网络中其它多路复用的流
    unsigned reserved_2                   : 2;// 保留位
    unsigned version_number               : 5; //范围0-31，表示PAT的版本号
    unsigned current_next_indicator       : 1; //发送的PAT是当前有效还是下一个PAT有效
    unsigned section_number               : 8; //分段的号码。PAT可能分为多段传输，第一段为00，以后每个分段加1，最多可能有256个分段
    unsigned last_section_number          : 8;  //最后一个分段的号码
    TS_PAT_Program program[10];
    unsigned reserved_3                   : 3; // 保留位
    unsigned network_PID                  : 13; //网络信息表（NIT）的PID,节目号为0时对应的PID为network_PID
    unsigned CRC_32                       : 32;  //CRC32校验码
} TS_PAT;

TS_PAT ts_pat;
TS_PAT * packet = &ts_pat;

typedef struct descriptors{
    unsigned descriptor_tag : 8;
    unsigned descriptor_length : 8;
    unsigned frequency  : 32;
    unsigned resered_future_use :12;
    unsigned FEC_outer   : 4;
    unsigned modulation  : 8;
    unsigned symbol_rate : 28;
    unsigned FEC_inner   : 4 ;
}Descriptors;

typedef struct TS_LOOP{
    unsigned transport_stream_id : 16;
    unsigned original_network_id : 16;
    unsigned resered_future_use2 : 4;
    unsigned transport_descriptors_length : 12;
}TS_LOOP;

typedef struct TS_NIT{
    unsigned table_id                   : 8;
    unsigned section_syntax_indicator   : 1;
    unsigned resered_future_use         : 1;
    unsigned reserved                   : 2;
    unsigned section_length             : 12;
    unsigned network_id                 : 16;
    unsigned reserved1                  : 2;
    unsigned version_number             : 5;
    unsigned current_next_indicator     : 1;
    unsigned section_number             : 8;
    unsigned last_section_number        : 8;
    unsigned resered_future_use1        : 4;
    unsigned network_descriptors_length : 12;
    Descriptors des[10];
    unsigned resered_future_use2        : 4;
    unsigned transport_stream_loop_length : 12;
    TS_LOOP tloop[10];
    unsigned CRC_32                     : 32;
}TS_NIT;

TS_NIT ts_nit;
TS_LOOP ts_loop;
Descriptors des;

typedef struct STREAM_TYPE{
    unsigned stream_type : 8;
    unsigned reserved    : 3;
    unsigned elementary_PID : 13;
    unsigned reserved1 : 4;
    unsigned ES_info_length : 12;
}STREAM_TYPE;

typedef struct TS_PMT
{
    unsigned table_id                        : 8;
    unsigned section_syntax_indicator        : 1;
    unsigned zero                            : 1;
    unsigned reserved_1                      : 2;
    unsigned section_length                  : 12;
    unsigned program_number                  : 16;
    unsigned reserved_2                      : 2;
    unsigned version_number                  : 5;
    unsigned current_next_indicator          : 1;
    unsigned section_number                  : 8;
    unsigned last_section_number             : 8;
    unsigned reserved_3                      : 3;
    unsigned PCR_PID                         : 13;
    unsigned reserved_4                      : 4;
    unsigned program_info_length             : 12;
    STREAM_TYPE streams[5];
    unsigned CRC_32                          : 32;
} TS_PMT;

TS_PMT ts_pmt;

void pat_pProc(void * handle,void * filter,unsigned char *buffer,int32_t size,uint32_t lParam){

    int i = 0;

    printf("-----------------PAT-------------------\n");
    
    packet->section_syntax_indicator    = buffer[1] >> 7;
    packet->zero                        = buffer[1] >> 6 & 0x1;
    packet->reserved_1                  = buffer[1] >> 4 & 0x3;
    packet->section_length              = (buffer[1] & 0x0F) << 8 | buffer[2]; 
    packet->transport_stream_id         = buffer[3] << 8 | buffer[4];
    packet->reserved_2                  = buffer[5] >> 6;
    packet->version_number              = buffer[5] >> 1 &  0x1F;
    packet->current_next_indicator      = (buffer[5] << 7) >> 7;
    packet->section_number              = buffer[6];
    packet->last_section_number         = buffer[7]; 

    int len = 0;
    len = 3 + packet->section_length;
    packet->CRC_32  = (buffer[len-4] & 0x000000FF) << 24 | (buffer[len-3] & 0x000000FF) << 16
                                                         | (buffer[len-2] & 0x000000FF) << 8 
                                                         | (buffer[len-1] & 0x000000FF); 
    
    int n = 0;
    for ( n = 0; n < packet->section_length - 12; n += 4 )
    {
        unsigned  program_num        = buffer[8 + n ] << 8 | buffer[9 + n ];  
        packet->reserved_3           = buffer[10 + n ] >> 5; 

        packet->network_PID          = 0x00;
        if ( program_num == 0x00)
        {  
            packet->network_PID = (buffer[10 + n ] & 0x1F) << 8 | buffer[11 + n ];
            printf("for network_PID = %#x \n",packet->network_PID);
        }
        else
        {
            TS_PAT_Program PAT_program;
            PAT_program.program_map_PID = (buffer[10 + n] & 0x1F) << 8 | buffer[11 + n];
            PAT_program.program_number = program_num;
            packet->program[i] = PAT_program;
            i++;
        }         
    }

    printf("table_id = %#x \n",packet->table_id);
    printf("section_syntax_indicator = %#x \n",packet->section_syntax_indicator);
    printf("section_length= %d \n",packet->section_length);
    printf("transport_stream_id = %#x \n",packet->transport_stream_id);
    printf("network_PID = %#x \n",packet->network_PID);
 
    for(i = 0;i < packet->section_length - 11;i ++){
        if(packet->program[i].program_number == 0 ){
            continue;
        }
        printf("program_num = %#x\n",packet->program[i].program_number);
        printf("program_map_PID = %#x \n",packet->program[i].program_map_PID);
    }

    printf("CRC32 = %#x \n",packet->CRC_32);
}


void nit_pProc(void * handle,void * filter,unsigned char * buf,int32_t size,uint32_t lParam){
    
    int len;
    int i = 0;
    int j = 0;

    printf("-----------------NIT-------------------\n");
    
    ts_nit.table_id                     = buf[0];
    ts_nit.section_syntax_indicator     = buf[1] >> 7;
    ts_nit.section_length               = (buf[1] & 0x0f) << 8 | buf[2];
    ts_nit.network_id                   = buf[3] << 8 | buf[4];
    ts_nit.version_number               = buf[5] >> 1 & 0x1f;
    ts_nit.current_next_indicator       = buf[5] & 0x1;
    ts_nit.section_number               = buf[6];
    ts_nit.last_section_number          = buf[7];
    ts_nit.network_descriptors_length   = (buf[8] & 0xf) << 8 | buf[9];

    printf("table_id = %#x\n",ts_nit.table_id);
    printf("section_syntax_indicator = %#x \n",ts_nit.section_syntax_indicator);
    printf("section_length = %#x \n",ts_nit.section_length);
    printf("network_id = %#x \n",ts_nit.network_id);
    printf("version_number = %#x \n",ts_nit.version_number);
    printf("current_next_indicator = %#x\n",ts_nit.current_next_indicator);
    printf("section_number = %#x \n",ts_nit.section_number);
    printf("last_section_number = %#x\n",ts_nit.last_section_number);
    printf("network_descriptors_length = %#x \n",ts_nit.network_descriptors_length);

#if 1
    if(ts_nit.section_number == 0){
        for(i = 0;i < ts_nit.network_descriptors_length;){
            Descriptors desc;
            desc.descriptor_tag    = buf[10 + i];
            desc.descriptor_length = buf[11 + i];
            //i = i + 2;
            ts_nit.des[j] = desc;
            printf("desc.descriptor_tag = %#x\n",desc.descriptor_tag);
            printf("desc.descriptor_length = %#x \n",desc.descriptor_length);
        
            if(ts_nit.section_number == 0 && desc.descriptor_tag == 0x4a){
                printf("transport_stream_id = %#x\n",buf[12 +i] << 8 | buf[13+i]);
                printf("original_network_id = %#x\n",buf[14 +i] << 8 | buf[15 + i]);
                printf("service_id  = %#x\n",buf[16 + i] << 8 | buf[17 + i]);
                printf("transport_stream_loop_length = %#x\n",(buf[18 + i] & 0xf) << 8 | buf[19 + i]);
            }
            j ++;
            i +=(2+desc.descriptor_length);
        }
    }
        if(ts_nit.section_number == 1){
            
            ts_nit.transport_stream_loop_length = (buf[10] & 0xf << 8) | buf[11];
            printf("transport_stream_loop_length = %#x\n",(buf[10] & 0xf) << 8 | buf[11]);
            for(len = 0;len < ts_nit.transport_stream_loop_length; len +=(6 + ts_loop.transport_descriptors_length)){
                //printf("transport_stream_id = %#x\n",(buf[12+len] << 8 | buf[len+13]));
                ts_loop.transport_stream_id = (buf[12 + len] << 8 | buf[len + 13]);
                ts_loop.original_network_id = (buf[14 + len] << 8 | buf[len + 15]);
                ts_loop.transport_descriptors_length = (buf[16 + len] & 0xf) << 8 | buf[len + 17];

                printf("transport_stream_id = %#x\n",ts_loop.transport_stream_id);
                printf("original_network_id = %#x\n",ts_loop.original_network_id);
                printf("transport_descriptors_length = %#x\n",ts_loop.transport_descriptors_length);
                for(j = 0;j < ts_loop.transport_descriptors_length;j +=(2 + des.descriptor_length)){
                    des.descriptor_tag = buf[18 + j];
                    des.descriptor_length =buf[19 +j];
                    des.frequency  = buf[20 +j] << 24 | buf[21 +j] << 16 | buf[22 +j] << 8 | buf[23 +j];
                    des.FEC_outer = (buf[25+j] & 0xf);
                    des.modulation = buf[26 + j];
                    des.symbol_rate = (buf[27 + j] << 24) | buf[28 +j] << 16| buf[29 + j] << 8 | (buf[30] & 0xf) >> 4;
                    des.FEC_inner   = buf[31] & 0xf;

                    printf("des.descriptor_tag = %#x\n",des.descriptor_tag);
                    printf("des.descriptor_length = %#x\n",des.descriptor_length);
                    printf("des.frequency = %#x",des.frequency);
                    printf("des.FEC_outer = %#x\n",des.FEC_outer);
                    printf("des.modulation = %#x\n",des.modulation);
                    printf("des.symbol_rate = %#x\n",des.symbol_rate);
                    printf("des.FEC_inner = %#x\n",des.FEC_inner);
                }
            }
        }

    printf("table_id = %#x \n",buf[0]);
#endif
}

void pmt_pProc(void* handle,void * filter,unsigned char * buf,int32_t size,uint32_t lParam){

    int j = 0;
    int i = 0;
    int len = 0;
    printf("-----------------PMT-------------------\n");
    ts_pmt.table_id = buf[0];
//    ts_pmt.
    printf("table_id = %#x \n",buf[0]);
    printf("PCR_PID = %#x \n",((buf[8]&0x1f) << 8) | buf[9]);
    printf("program_number = %#x \n",buf[3] << 8 | buf[4]);

    ts_pmt.section_length = (buf[1] & 0xf) << 8 | buf[1];
    len = ts_pmt.section_length - 13;

    for(i = 0;i <len;i+=5){

        STREAM_TYPE stream;
        stream.stream_type = buf[12+i];
        stream.elementary_PID = ((buf[13+i] & 0x1f) << 8) | buf[14+i];
        stream.ES_info_length = (buf[15+i] & 0x0f) << 8 | buf[16+i];
        ts_pmt.streams[j] = stream;

        if(stream.ES_info_length != 0){
            i += stream.ES_info_length;
        }

        if(ts_pmt.streams[j].stream_type == 0){
            continue;
        }
        printf("stream_type = %#x \n",ts_pmt.streams[j].stream_type);
        printf("elementary_PID = %#x \n",ts_pmt.streams[j].elementary_PID) ;

        j++;

    }
}

int main(int argc, const char *argv[]){

    SFilterNode * pat_node = NULL;
    SFilterNode * pmt_node[10] = {NULL};
    SFilterNode * nit_node = NULL;

    SDemux *demux = NULL;

    int j = 0;
    int i = 0;
    int fd ;
    int n = 0;
    unsigned char buf[1024 * 10] ={0};

    demux = (SDemux *)Demux_Create();

    pat_node = (SFilterNode *)Demux_CreateFilterEx(demux,0x00,0x00,0,0x0003,0,sizeof(buf),pat_pProc,1);
    nit_node = (SFilterNode *)Demux_CreateFilterEx(demux,0x10,0x40,0,0x0001,0,sizeof(buf),nit_pProc,2);

    fd = open("./stream/251-small.ts",O_RDONLY);

    if(fd < 0){
        perror("Fail to open ");
        exit(EXIT_FAILURE);
    }

    do{
        n = read(fd,buf,sizeof(buf));
        if(n < 0){
            perror("Fail to read");
            exit(EXIT_FAILURE);
        }

        for(i = 0;i < 10;i ++){
            pmt_node[i] = Demux_CreateFilterEx(demux,packet->program[i].program_map_PID,0x02,0,packet->program[i].program_number,0,sizeof(buf),pmt_pProc,1);
            Demux_OnData(demux,buf,sizeof(buf));
        }
        Demux_OnData(demux,buf,sizeof(buf)); 

        j ++;
    }while(n > 0);

    return 0;
}
