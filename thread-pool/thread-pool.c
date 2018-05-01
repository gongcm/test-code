/*
 ************************************************************************************************
 *Copyright  : Grassroots teacher,farsight wuhang
 *File name  : thread_pool.c
 *Description: These codes are all about the operation of the thread pool.Everyone of fasight 
 *             is free to read and modify the code.Each of the changes,in the historical records 
 *             indicate that you modify the place as well as the resons for the chages.
 *             When you use the thread pool code,you  only need to call the following functions:
 *             <1> create_thread_pool,it can help you create a thread pool.
 *             <2> requst_thread_work,it can help you get an idle thread from thread pool,and you 
 *             can use the idle thread to do what you need to do.
 *Author     : chengyaogen
 *Version    : 0.1
 *Date       : 2015-12-26
 *History    : no
 *Note       : When a thread pool is created,a clean thread can run.This cleaning thread will 
 *             always observe the thread pool in the thread pool of the number of idle threads,
 *             if the number of idle threads to reach the maximum number of threads can accommodate 
 *             the thread pool,clean up thread will clear a part of the thread,so that the thread pool 
 *             with a minimum number of threads.The main purpose of this is to free a part of the idle 
 *             threads at the timer of the server,to reduce the overhead of the server resources.
 ***********************************************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "thread-pool.h"

typedef void (*clean_t)(void *);

static int thread_start_work(thread_description_t  *thread)
{
	thread_pool_t *thread_pool = thread->thread_pool;

	if(!thread->thread_work){
		THREAD_DBUG("Thread work is NULL\n");
		return -1;
	}

	thread->thread_work(thread->work_data);

	pthread_mutex_lock(&thread_pool->queue_lock);
	enter_linkqueue(thread_pool->idle_thread_queue,thread);	
	pthread_mutex_unlock(&thread_pool->queue_lock);

	return 0;
}

static void *thread_run(void *arg)
{
	int ret;
	int sig;
	thread_description_t *thread = (thread_description_t *)arg;		
	thread_pool_t *thread_pool = thread->thread_pool;

	ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	if(ret != 0){
		DBG_OUT("fail to pthread_setcancelstate");
	}
	ret = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
	if(ret != 0){
		DBG_OUT("fail to pthread_setcanceltype");
	}
	
	for(;;){
		ret = sigwait(thread->thread_sigset,&sig);
		if(ret != 0){
			DBG_OUT("fail to sigwait,%s\n",strerror(errno));
		}
	
		THREAD_DBUG("Thread id:%lu wake up begin work\n",pthread_self());
		
		pthread_cleanup_push((clean_t)pthread_mutex_unlock,(void *)&thread_pool->queue_lock);		
		thread_start_work(thread);
		pthread_cleanup_pop(0);		
		
		THREAD_DBUG("Thread id:%lu end of job\n",pthread_self());
	}

	pthread_exit(NULL);
}

static void clean_thread(thread_pool_t *thread_pool)
{
	int i;
	int ret;
	thread_description_t *thread;

	while(!is_empty_linkqueue(thread_pool->idle_thread_queue)){
		delete_linkqueue(thread_pool->idle_thread_queue);
	}

	for(i = 0;i < thread_pool->max_threads;i ++){
		thread = &thread_pool->thread_objs[i];
		if(i < thread_pool->min_threads){
			enter_linkqueue(thread_pool->idle_thread_queue,thread);
		}else{		
			ret = pthread_cancel(thread->tid);
			if(ret != 0){
				DBG_OUT("fail to pthread_kill : %s\n",strerror(ret));	
			}
			thread_pool->current_threads --;
			THREAD_DBUG("Cancel thread:%lu\n",thread->tid);
		}
	}

	return;
}

static int try_clean_thread(thread_pool_t *thread_pool)
{
	int ret;
	LinkQueue *idle_thread_queue = thread_pool->idle_thread_queue;	
	
	ret = pthread_mutex_trylock(&thread_pool->queue_lock);	
	if(ret != 0){
		return -1;
	}
	
	if(idle_thread_queue->n < thread_pool->max_threads){	
		pthread_mutex_unlock(&thread_pool->queue_lock);
		return -1;
	}

	THREAD_DBUG("Clean thread work start ...\n");
	clean_thread(thread_pool);
	
	pthread_mutex_unlock(&thread_pool->queue_lock);
	
	return 0;
}


static void *clean_thread_run(void *arg)
{
	int ret;
	thread_pool_t *thread_pool = (thread_pool_t *)arg;	
	LinkQueue *idle_thread_queue = thread_pool->idle_thread_queue;

	ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	if(ret != 0){
		DBG_OUT("fail to pthread_setcancelstate");
	}

	ret = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
	if(ret != 0){
		DBG_OUT("fail to pthread_setcanceltype");
	}

	while(1){
		if(thread_pool->current_threads >= thread_pool->max_threads &&\
				idle_thread_queue->n >= thread_pool->max_threads){	
			
			pthread_cleanup_push((clean_t)pthread_mutex_unlock,(void *)&thread_pool->queue_lock);		
			try_clean_thread(thread_pool);
			pthread_cleanup_pop(0);		
		}

		sleep(10);
	}
}

static thread_description_t *create_thread_obj(thread_pool_t *thread_pool)
{
	int ret;
	thread_description_t *thread;
	
	if(thread_pool->current_threads >= thread_pool->max_threads){
		return NULL;
	}

	thread = thread_pool->thread_objs + thread_pool->current_threads;
	thread->status = thread_idle;
	thread->thread_sigset = &thread_pool->thread_sigset;
	thread->thread_pool   = thread_pool;

	ret = pthread_create(&thread->tid,NULL,thread_run,thread);
	if(ret != 0){
		DBG_OUT("fail to pthread_create , %s\n",strerror(ret));	
	}

	pthread_detach(thread->tid);
	thread_pool->current_threads ++;
	
	return thread;
}

static int init_thread_pool(thread_pool_t *thread_pool,int max_threads,int min_threads)
{
	int ret;
		
	bzero(thread_pool,sizeof(*thread_pool));
	thread_pool->max_threads = max_threads;
	thread_pool->min_threads = min_threads;
	thread_pool->idle_thread_queue = create_empty_linkqueue();
	
	ret = pthread_mutex_init(&thread_pool->queue_lock,NULL);
	if(ret != 0){
		DBG_OUT("fail to pthread_mutex_init");
	}

	/* Block SIGQUIT ; other threads created by main() will inherit a copy of the signal mask. */
	sigemptyset(&thread_pool->thread_sigset);
	sigaddset(&thread_pool->thread_sigset,SIGUSR1);
	ret = pthread_sigmask(SIG_BLOCK, &thread_pool->thread_sigset, NULL);
	if (ret != 0){
		DBG_OUT("fail to pthread_sigmask");
	}

	return 0;		
}

static int create_min_thread_objs(thread_pool_t *thread_pool)
{
	int i;
	thread_description_t *thread;
	int min_threads = thread_pool->min_threads;
	int max_threads = thread_pool->max_threads;

	thread_pool->thread_objs  = (thread_description_t *)malloc(max_threads * sizeof(thread_description_t));
	if(!thread_pool->thread_objs){
		DBG_OUT("fail to malloc,%s",strerror(errno));
	}
	
	for(i = 0;i < min_threads;i ++){
		thread = create_thread_obj(thread_pool);
		pthread_mutex_lock(&thread_pool->queue_lock);
		enter_linkqueue(thread_pool->idle_thread_queue,thread);
		pthread_mutex_unlock(&thread_pool->queue_lock);
	}

	return 0;
}

static int create_clean_thread(thread_pool_t *thread_pool)
{
	int ret;
	
	ret = pthread_create(&thread_pool->clean_tid,NULL,clean_thread_run,thread_pool);
	if (ret != 0){
		DBG_OUT("fail to create manager thread");
	}
	pthread_detach(thread_pool->clean_tid);
	
	return 0;
}

thread_pool_t  *create_thread_pool(int max_threads,int min_threads)
{
	thread_pool_t *thread_pool;
	
	thread_pool = (thread_pool_t *)malloc(sizeof(thread_pool_t));
	if(!thread_pool){
		DBG_OUT("fail to malloc,%s",strerror(errno));
	}
	
	init_thread_pool(thread_pool,max_threads,min_threads);	
	create_min_thread_objs(thread_pool);
	create_clean_thread(thread_pool);
	
	return thread_pool;
}

static thread_description_t  *get_idle_thread(thread_pool_t *thread_pool)
{
	thread_description_t *thread;
	
	 pthread_mutex_lock(&thread_pool->queue_lock);
	 thread = delete_linkqueue(thread_pool->idle_thread_queue);
	 pthread_mutex_unlock(&thread_pool->queue_lock); 
	 if(!thread){
		THREAD_DBUG("No idle thread,try to create a new thread\n");
	 	return  create_thread_obj(thread_pool);
	 }
	
	 return thread;
}

int requst_thread_work(thread_pool_t *thread_pool,pthread_work_t work,void *work_data)
{
	int ret;
	thread_description_t *thread;
	
	thread = get_idle_thread(thread_pool);
	if(!thread){
		THREAD_DBUG("The thread pool is full!\n");
		return -1;
	}
	thread->thread_work = work;
	thread->work_data = work_data;
	
	THREAD_DBUG("Wake up a thread to work ...\n");
	
	ret = pthread_kill(thread->tid,SIGUSR1);
	if(ret != 0){
		DBG_OUT("fail to pthread_kill : %s\n",strerror(ret));	
	}
	return 0;
}

void destroy_thread_pool(thread_pool_t *thread_pool)
{
	int i;
	thread_description_t *thread;

	pthread_cancel(thread_pool->clean_tid);

	for(i = 0;i < thread_pool->current_threads;i ++){
		thread = &thread_pool->thread_objs[i];
		pthread_cancel(thread->tid);
	}

	pthread_mutex_lock(&thread_pool->queue_lock);
	clean_linkqueue(thread_pool->idle_thread_queue);
	pthread_mutex_unlock(&thread_pool->queue_lock);
	
	pthread_mutex_destroy(&thread_pool->queue_lock);
	free(thread_pool->thread_objs);
	free(thread_pool);

	return;
}
