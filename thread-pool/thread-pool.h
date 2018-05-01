#ifndef _THREAD_POLL_H
#define _THREAD_POLL_H

#include <signal.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "linkqueue.h"

typedef void  (*pthread_work_t)(void *);
typedef int pthread_signal_t;
typedef sigset_t pthread_sigset_t;
struct thread_pool;

typedef enum{
	thread_busy,
	thread_idle,
}pthread_s;

typedef struct {
	pthread_t tid;
	pthread_s status;
	void *work_data;
	pthread_work_t   thread_work;
	pthread_sigset_t *thread_sigset;
	struct thread_pool  *thread_pool;
}thread_description_t;

typedef LinkQueue thread_queue_t;

typedef struct thread_pool{
	int min_threads;
	int max_threads;
	int current_threads;	
	pthread_t clean_tid; 
	pthread_sigset_t thread_sigset;	
	pthread_mutex_t  queue_lock;
	thread_queue_t *idle_thread_queue;
	thread_description_t *thread_objs;
}thread_pool_t;

extern thread_pool_t *create_thread_pool(int max_threads,int min_threads);
extern int requst_thread_work(thread_pool_t *thread_pool,pthread_work_t work,void *work_data);		
extern void destroy_thread_pool(thread_pool_t *thread_pool);

#ifdef DBUG 
#define THREAD_DBUG(args...) do{ \
	char b__[1024];\
	sprintf(b__,args);\
	fprintf(stderr,"[%s,%s,%d] %s",__FILE__,__FUNCTION__,__LINE__,b__); \
}while(0)
#else 
#define THREAD_DBUG(args...)
#endif 

#endif 
