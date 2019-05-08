#define MAX_CLIENT 100
#include <pthread.h>
#include <errno.h>

struct accounts{
    int fd;
    char* login;
};
struct accounts account[MAX_CLIENT];

pthread_mutex_t	mutex[MAX_CLIENT]={[0 ... MAX_CLIENT-1]=PTHREAD_MUTEX_INITIALIZER};
pthread_mutex_t accMtx = PTHREAD_MUTEX_INITIALIZER;

int addFd(int fd)
{
	pthread_mutex_lock(&accMtx);
    int i;
    for(i=0; i<MAX_CLIENT&&account[i].fd!=0; i++);
    if(i<MAX_CLIENT)
        account[i].fd=fd;
    else
	{
		pthread_mutex_unlock(&accMtx);
        return -1;
	}
	pthread_mutex_unlock(&accMtx);
    return 0;
}

int delFd(int fd)
{
	pthread_mutex_lock(&accMtx);
    int i;
    for(i=0; i<MAX_CLIENT&&account[i].fd!=fd; i++);
    if(i<MAX_CLIENT)
    {
        printf("del %d\n",fd);
        account[i].fd=0;
        if(account[i].login!=NULL){
            account[i].login=NULL;
            printf("Force Logout[%d]\n",fd);
        }
    }
    else
	{
		pthread_mutex_unlock(&accMtx);
        return -1;
	}
	pthread_mutex_unlock(&accMtx);
    return 0;
}
