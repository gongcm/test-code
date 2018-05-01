#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "linkqueue.h"

LinkQueue *create_empty_linkqueue()
{
	LinkNode *head;
	LinkQueue *q;
	
	head = (LinkNode *)malloc(sizeof(LinkNode));
	if(!head){
		DBG_OUT("fail to malloc : %s\n",strerror(errno));
	}
	head->next = NULL;

	q = (LinkQueue *)malloc(sizeof(LinkQueue));
	if(!q){
		DBG_OUT("fail to malloc : %s\n",strerror(errno));
	}
	q->n = 0;
	q->front = q->rear = head;
	
	return q;
}

int is_empty_linkqueue(LinkQueue *q)
{
	return q->front == q->rear ? 1 : 0;
}

int enter_linkqueue(LinkQueue *q,void *data)
{
	LinkNode *temp;

	temp = (LinkNode *)malloc(sizeof(LinkNode));
	if(!temp){
		DBG_OUT("fail to malloc : %s\n",strerror(errno));
	}
	temp->data = data;
	temp->next = NULL;
	
	q->rear->next = temp;
	q->rear       = temp;
	q->n ++;

	return 0;
}

void *delete_linkqueue(LinkQueue *q)
{
	LinkNode *temp;

	if(is_empty_linkqueue(q)){
		return NULL;
	}
	
	temp = q->front;
	q->front = temp->next;
	free(temp);
	q->n --;
	return q->front->data;
}

int get_linkqueue_nodes(LinkQueue *q)
{
	return q->n;
}

void clean_linkqueue(LinkQueue *q)
{
	LinkNode *temp;
	LinkNode *p = q->front;

	while(p != NULL){
		temp = p;
		p = p->next;
		free(temp);
	}
	
	free(q);
	
	return;
}
