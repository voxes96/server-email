#include <string.h>

struct thread_data_t
{
    int fd;
    char* src;
    char* ip;
    short unsigned int port;
    char* mess;
    char* domain;
    int result;
};

int lockMtx(char* login)
{
    if(login!=NULL)
    {
        int mtxIdx;
        for(mtxIdx=0; mtxIdx<MAX_CLIENT; mtxIdx++)
        {
            if(account[mtxIdx].login!=NULL)
                if(!strcmp(login,account[mtxIdx].login))
                    break;
        }
        if(mtxIdx>=MAX_CLIENT)
            return 0;
        printf("lock:%d\n",mtxIdx);
        pthread_mutex_lock(&mutex[mtxIdx]);
    }
    else
        return 0;
    return 1;
}

void unlockMtx(char* login)
{
    if(login!=NULL)
    {
        int mtxIdx;
        for(mtxIdx=0; mtxIdx<MAX_CLIENT; mtxIdx++)
        {
            if(account[mtxIdx].login!=NULL)
                if(!strcmp(login,account[mtxIdx].login))
                    break;
        }
        if(mtxIdx>=MAX_CLIENT)
            return;
        printf("unlock:%d\n",mtxIdx);
        pthread_mutex_unlock(&mutex[mtxIdx]);
    }
}

int saveRead(int fd,char *buf,unsigned int size)
{
    memset(buf,'\0',size);
    int readResult,last=0;
    while(1)
    {
        readResult=read(fd,buf+last,1);
        if(buf[last]=='\0'||readResult==0)
            break;
        if(last>=size)
        {
            readResult=-1;
            break;
        }
        last+=readResult;
        if(readResult==-1)
            perror("Read error message:");
    }
    return readResult;
}
