#ifndef _LINKQUEUE_H_
#define _LINKQUEUE_H_ 
#include "debug.h"

typedef struct node{
	void *data;
	struct node *next;
}LinkNode;

typedef struct{
	int       n;
	LinkNode *front;
	LinkNode *rear;
}LinkQueue;

extern LinkQueue *create_empty_linkqueue();
extern int is_empty_linkqueue(LinkQueue *); 
extern int enter_linkqueue(LinkQueue *,void *);
extern void *delete_linkqueue(LinkQueue *);
extern void clean_linkqueue(LinkQueue *);
extern int get_linkqueue_nodes(LinkQueue *q);

#endif
