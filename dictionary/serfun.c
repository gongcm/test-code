#include"head.h"

void do_register(int acc_fd,UserMsg *msg,sqlite3 *db)
{
	char sql[128];
	char *errmsg;
	char **result;
	int nrow;
	sprintf(sql,"select * from user where name = '%s';",msg->name);
	if(sqlite3_get_table(db,sql,&result,&nrow,NULL,&errmsg)!= SQLITE_OK){
	
		printf("error:%s\n",errmsg);
		exit(EXIT_FAILURE);
	}
	if(nrow != 0){
	
		strcpy(msg->data,"this name have exist");
		send(acc_fd,msg,sizeof(UserMsg),0);

	}else{
		memset(sql,0,sizeof(sql));
		sprintf(sql,"insert into user values('%s','%s');",msg->name,msg->data);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!= SQLITE_OK){

			printf("sqlite3_exec fail:%s\n",errmsg);
			exit(EXIT_FAILURE);
		}
		strcpy(msg->data,"register success");
		send(acc_fd,msg,sizeof(UserMsg),0);
		sprintf(sql,"create table %s_history (word);",msg->name);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!= SQLITE_OK){

			printf("sqlite3_exec fail:%s\n",errmsg);
			exit(EXIT_FAILURE);
		}
	}
	return;
}

void do_login(int acc_fd,UserMsg *msg,sqlite3 *db)
{
	char sql[128];
	char *errmsg;
	char **result;
	int nrow = 0;
	sprintf(sql,"select * from user where name='%s' and passwd ='%s';",msg->name,msg->data);
	if(sqlite3_get_table(db,sql,&result,&nrow,NULL,&errmsg)!= SQLITE_OK){
	
		printf("error:%s\n",errmsg);
		exit(EXIT_FAILURE);
	}

	if(nrow != 0){
		strcpy(msg->data,"logined");
		send(acc_fd,msg,sizeof(UserMsg),0);
		sqlite3_free_table(result);
	}else{
	
		strcpy(msg->data,"login fail");
		send(acc_fd,msg,sizeof(UserMsg),0);
		sqlite3_free_table(result);
	}
	return;
}

void do_query(int acc_fd,UserMsg *msg,sqlite3 *db)
{
	char sql[128];
	char *errmsg;
	char **result;
	int ncolumn;
	int nrow;
	char buf[256]={0};
	char name[20]={0};
	strcpy(buf,msg->data);
	strcpy(name,msg->name);
	sprintf(sql,"select * from word where word='%s';",msg->data);
	if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg)!= SQLITE_OK){
	
		printf("error:%s\n",errmsg);
		exit(EXIT_FAILURE);
	}
	if(nrow != 0){
		memset(msg,0,sizeof(UserMsg));
		strcpy(msg->data,result[ncolumn * 2 - 1]);
		printf("data = %s\n",msg->data);
		send(acc_fd,msg,sizeof(UserMsg),0);
		memset(sql,0,sizeof(sql));
		memset(msg,0,sizeof(UserMsg));
		sprintf(sql,"insert into %s_history values('%s');",name,buf);
		if(sqlite3_get_table(db,sql,&result,NULL,NULL,&errmsg)!= SQLITE_OK){

			printf("error:%s\n",errmsg);
			exit(EXIT_FAILURE);
		}
	}else{
	
		memset(msg,0,sizeof(UserMsg));
		sprintf(msg->data,"not find word :%s",buf);
		printf("data = %s\n",msg->data);
		send(acc_fd,msg,sizeof(UserMsg),0);

	}
	return;
}

void do_history(int acc_fd,UserMsg *msg,sqlite3 *db)
{
	char sql[128];
	char *errmsg;
	char **result;
	int nrow,i;
	
	sprintf(sql,"select * from %s_history;",msg->name);
	if(sqlite3_get_table(db,sql,&result,&nrow,NULL,&errmsg)!= SQLITE_OK){
	
		printf("error:%s\n",errmsg);
		exit(EXIT_FAILURE);
	}
	for(i = 0;i<nrow+1;i++){
		strcpy(msg->data,result[i]);
		send(acc_fd,msg,sizeof(UserMsg),0);
	}
	memset(msg,0,sizeof(UserMsg));
	strcpy(msg->data,"#");
	send(acc_fd,msg,sizeof(UserMsg),0);

}

void do_client(int acc_fd,sqlite3 *db)
{
	UserMsg msg;
	memset(&msg,0,sizeof(msg));
	while(recv(acc_fd,&msg,sizeof(msg),0) > 0){
	
		switch(msg.type){
		
		case R:
			do_register(acc_fd,&msg,db);
			break;
		case L:
			do_login(acc_fd,&msg,db);
			break;
		case Q:
			do_query(acc_fd,&msg,db);
			break;
		case H:
			do_history(acc_fd,&msg,db);
			break;
		}
	}

	printf("client quit\n");
	exit(0);
}
