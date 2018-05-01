#ifndef _SMART_ERROR_
#define _SMART_ERROR_

#define DBG_OUT(args...) do{ \
	char b__[1024];\
	sprintf(b__,args);\
	fprintf(stderr,"[%s,%s,%d] : %s",__FILE__,__FUNCTION__,__LINE__,b__); \
	exit(EXIT_FAILURE);\
}while(0)

#define DBUG_OUT(args...) do{ \
	char b__[1024];\
	sprintf(b__,args);\
	fprintf(stderr,"[%s,%s,%d] : %s",__FILE__,__FUNCTION__,__LINE__,b__); \
	exit(EXIT_FAILURE);\
}while(0)

#endif
