#include"head.h"

int do_register(int cli_fd,UserMsg *msg)
{
	msg->type = R;
	printf("\n");
	printf("name:");
	scanf("%s",msg->name);
	printf("passwd:");
	scanf("%s",msg->data);

	printf("%s\n",msg->name);
	send(cli_fd,msg,sizeof(UserMsg),0);
	recv(cli_fd,msg,sizeof(UserMsg),0);
	printf("%s\n",msg->data);
	return 0;

}

int do_login(int cli_fd,UserMsg *msg)
{
	msg->type = L;
	printf("\n");
	printf("name:");
	scanf("%s",msg->name);

	printf("passwd:");
	scanf("%s",msg->data);
	
	printf("\n");

	send(cli_fd,msg,sizeof(UserMsg),0);
	recv(cli_fd,msg,sizeof(UserMsg),0);
	if(strncmp(msg->data,"logined",7) == 0){
	
		printf("longin success\n");
		return 1;
	}else{
	
		printf("login fail,please check your name or passwd\n");
		return 0;
	}
}

void print(char *p)
{
	int count = 0;
	int len;
	char *q;
	while(*p){
	
		if(*p == ' '&&count == 0)
			p++;
		printf("%c",*p++);
		count++;
		if(*(p-1)==' '&& *p!=' '){
		
			q = p;
			len = 0;
			while(*q != ' '&& *q !='\0'){
			
				len++;
				q++;
			}
			if((COL - count)< len){
			
				printf("\n ");
				count = 0;
			}

		}
		if(count == 50){
		
			count = 0;
			printf("\n ");
		}
	}
}

int do_query(int cli_fd,UserMsg *msg,char *name)
{
	
	while(1){
		msg->type = Q;
		strcpy(msg->name,name);
		printf("word:");
		scanf("%s",msg->data);
		if(strcmp(msg->data,"#") == 0)
			break;

		send(cli_fd,msg,sizeof(UserMsg),0);
		memset(msg,0,sizeof(UserMsg));
		recv(cli_fd,msg,sizeof(UserMsg),0);
		print(msg->data);
		printf("\n");
		memset(msg,0,sizeof(UserMsg));
	}
	return 0;
}

int do_history(int cli_fd,UserMsg *msg)
{
	msg->type = H;
	int ret;
	send(cli_fd,msg,sizeof(UserMsg),0);
	while(1){
	
		memset(msg,0,sizeof(UserMsg));
		ret = recv(cli_fd,msg,sizeof(UserMsg),0);
		//if(msg->data[0] == '\0')
		if(strncmp(msg->data,"#",1)== 0)
		break;
		printf("%s\n",msg->data);
	}
	return 0;
}
