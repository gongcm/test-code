#ifndef __HEAD__
#define __HEAD__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>	       /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include<sqlite3.h>
#define N 20
#define R 1
#define L 2
#define Q 3
#define H 4
#define COL 50
typedef struct
{
	int type;
	char name[N];
	char data[256];

}UserMsg;
#endif
