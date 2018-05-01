#include"head.h"

int do_register(int cli_fd,UserMsg *msg);
int do_login(int cli_fd,UserMsg *msg);
void print(char *p);
int do_query(int cli_fd,UserMsg *msg,char *name);
int do_history(int cli_fd,UserMsg *msg);

int main(int argc, const char *argv[])
{
	int cli_fd;
	int n;
	UserMsg msg;
	char name[20];
	
	struct sockaddr_in addr;

	socklen_t addrlen = sizeof(addr);
	if(argc != 3){
	
		fprintf(stderr,"Usage:%s <IP> <port>\n",argv[0]);
		exit(EXIT_FAILURE);
	}
    // socket
	if((cli_fd = socket(AF_INET,SOCK_STREAM,0)) < 0){
	
		perror("socket fail");
		exit(EXIT_FAILURE);
	}

	addr.sin_family = AF_INET;
	addr.sin_port   = htons(atoi(argv[2]));
	addr.sin_addr.s_addr = inet_addr(argv[1]);

	if(connect(cli_fd,(struct sockaddr *)&addr,addrlen)< 0){
	
		perror("connect fail");
		exit(EXIT_FAILURE);

	}
	//recv & send
	while(1){
	
		printf("------------------------------\n");
		printf("\n");
		printf(">1:register  2:longin  3:quit");
		printf("\n");
		printf("------------------------------\n");
		printf("please choose:");
		if(scanf("%d",&n)<0){
		
			perror("input error");
			exit(-1);
		}
		switch(n)
		{
		case 1:
			do_register(cli_fd,&msg);

			break;
		case 2:
			if(do_login(cli_fd,&msg)== 1){
				strcpy(name,msg.name);
				printf("\n");
				goto next;
			}
			break;
		case 3:
			close(cli_fd);
			exit(0);
		}
	}

next:
	while(1){
	
		printf("----------------------------------------\n");
		printf("\n");
		printf(">1:query_word  2:history_record  3:quit");
		printf("\n");
		printf("-----------------------------------------\n");
		printf("please choose:");
		if(scanf("%d",&n)<0){
		
			perror("input error");
			exit(-1);
		}
		switch(n)
		{
		case 1:
			do_query(cli_fd,&msg,name);

			break;
		case 2:
			do_history(cli_fd,&msg);
			
			break;
		case 3:
			close(cli_fd);
			exit(0);
		}

	}

	return 0;
}
