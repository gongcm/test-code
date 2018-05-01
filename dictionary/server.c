#include"head.h"

void do_register(int acc_fd,UserMsg *msg,sqlite3 *db);
void do_login(int acc_fd,UserMsg *msg,sqlite3 *db);
void do_query(int acc_fd,UserMsg *msg,sqlite3 *db);
void do_history(int acc_fd,UserMsg *msg,sqlite3 *db);
void do_client(int acc_fd,sqlite3 *db);

int main(int argc, const char *argv[])
{
	int ser_fd;
	int acc_fd;
	sqlite3 *db;

	if(sqlite3_open("dict.db",&db)!= SQLITE_OK){
	
		printf("sqlite3_open fail:%s\n",sqlite3_errmsg(db));
		exit(EXIT_FAILURE);
	}
	pid_t pid;
	struct sockaddr_in addr;
	struct sockaddr_in peeraddr;

	socklen_t addrlen = sizeof(peeraddr);
	if(argc != 3){
	
		fprintf(stderr,"Usage:%s <IP> <port>\n",argv[0]) ;
		exit(EXIT_FAILURE);
	}
    // socket
	if((ser_fd = socket(AF_INET,SOCK_STREAM,0)) < 0){
	
		perror("socket fail");
		exit(EXIT_FAILURE);
	}

	//bind
	addr.sin_family = AF_INET;
	addr.sin_port   = htons(atoi(argv[2]));
	addr.sin_addr.s_addr = inet_addr(argv[1]);

	if(bind(ser_fd,(struct sockaddr *)&addr,sizeof(addr)) < 0){
	
		perror("bind fail");
		exit(EXIT_FAILURE);
	}

	//listen
	if(listen(ser_fd,5) < 0){
		perror("listen fail");
		exit(EXIT_FAILURE);
	}
	signal(SIGCHLD,SIG_IGN);
	while(1){
		// accept
		if((acc_fd = accept(ser_fd,(struct sockaddr *)&peeraddr,&addrlen))< 0){

			perror("accept fail");
			exit(EXIT_FAILURE);

		}
		pid = fork();
		if(pid < 0){
		
			perror("fork fail");
			exit(EXIT_FAILURE);
		}
		if(pid == 0){
		
			do_client(acc_fd,db);
		}
		close(acc_fd);
	}
	
	return 0;
}
