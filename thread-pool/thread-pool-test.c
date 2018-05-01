#include <stdio.h>
#include "thread-pool.h"
#include "linkqueue.h"

void sleep_work(void *work_data)
{
	sleep(*(int *)work_data);
	return;
}

int main(int argc, const char *argv[])
{
	int i;
	int *arg;
	int ret;
	int task_num;
	thread_pool_t *thread_pool;

	thread_pool = create_thread_pool(10,3);
	if(!thread_pool){
		printf("Fail to create_thread_pool");
		return -1;
	}
	
	while(1){
		ret = scanf("%d",&task_num);
		if(ret != 1){
			printf("Input invalid!\n");
			destroy_thread_pool(thread_pool);
			break;
		}

		for(i = 1;i <= task_num;i ++){
			arg = (int *)malloc(sizeof(int));
			*arg = i;
			ret = requst_thread_work(thread_pool,sleep_work,arg);
			if(ret != 0){
				printf("fail to requst_thread_work\n");
			}
		}
	}

	printf("The thread pool is destroy,all thread is not exit!\n");
	while(1);

	return 0;
}
