#include "account.c"
#include "server2server.c"
#include <sys/stat.h>
#include <dirent.h>

int createAccount(char* buf,char* domain,int idx)
{
    char login[WORD_SIZE];
    memset(login,'\0',WORD_SIZE);
    cutWord(buf,login,&idx);
	
	pthread_mutex_lock(&accMtx);
	
    char passwd[WORD_SIZE];
    memset(passwd,'\0',WORD_SIZE);
    cutWord(buf,passwd,&idx);
    FILE * fp;

    char path[WORD_SIZE];
    memset(path,'\0',WORD_SIZE);
    strcat(path,domain);
    strcat(path,"/");
    strcat(path,login);

    DIR* dir = opendir(path);
    if (dir)
    {
        closedir(dir);
		pthread_mutex_unlock(&accMtx);
        return -10;
    }

    mkdir(path,0777);

    strcat(path,".passwd");
    fp = fopen(path, "w");
    if(fp == NULL)
	{
		pthread_mutex_unlock(&accMtx);
        return -11;
	}
    fprintf(fp,"%s\n",passwd);

    fclose(fp);
	pthread_mutex_unlock(&accMtx);
    return 1;
}

int signIn(char* buf,char* domain,int idx,int accountIdx)
{
    char login[WORD_SIZE];
    memset(login,'\0',WORD_SIZE);
    cutWord(buf,login,&idx);
    char passwd[WORD_SIZE];
    memset(passwd,'\0',WORD_SIZE);
    cutWord(buf,passwd,&idx);
    FILE * fp;

    char path[WORD_SIZE];
    memset(path,'\0',WORD_SIZE);
    strcat(path,domain);
    strcat(path,"/");
    strcat(path,login);
    strcat(path,".passwd");

    fp = fopen(path, "r");
    if(fp == NULL)
        return -11;

    char correctPassword[WORD_SIZE];
    memset(correctPassword,'\0',WORD_SIZE);
    fscanf(fp,"%s",correctPassword);

    if(strcmp(correctPassword,passwd))
        return -20;

    if(account[accountIdx].login!=NULL)
        return -21;

    account[accountIdx].login=login;

    fclose(fp);

    return 1;
}

int logOut(int accountIdx)
{
	unlockMtx(account[accountIdx].login);
	pthread_mutex_lock(&accMtx);
    account[accountIdx].login=NULL;
	pthread_mutex_unlock(&accMtx);
    return 1;
}

int getMessClient(FILE * fp,int accountIdx)
{
    char *email=(char*)malloc(EMAIL_SIZE);
    memset(email,'\0',EMAIL_SIZE);

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, fp)) != -1)
        strcat(email,line);

    if(line)
        free(line);

    int writeResult=write(account[accountIdx].fd,email,EMAIL_SIZE);
    printf("write_mail[%d]:%d\n",account[accountIdx].fd,writeResult);
    free(email);
    if(writeResult==-1)
    {
        perror("Write error message:");
        return -50;
    }

    return 1;
}

int getMess(char* domain,int accountIdx)
{
    FILE * fp;

    char path[WORD_SIZE];
    memset(path,'\0',WORD_SIZE);
    strcat(path,domain);
    strcat(path,"/");
    strcat(path,account[accountIdx].login);

    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    char file[WORD_SIZE];
    if (!d)
        return -40;

    while ((dir = readdir(d)) != NULL)
    {
        if(dir->d_name[0]=='.')
            continue;
        memset(file,'\0',WORD_SIZE);
        strcat(file,path);
        strcat(file,"/");
        strcat(file,dir->d_name);
        fp = fopen(file,"r");
        if(fp == NULL)
        {
            closedir(d);
            return -11;
        }
        int getres = getMessClient(fp,accountIdx);
        fclose(fp);

        if(getres<0)
        {
            closedir(d);
            return getres;
        }
    }
    closedir(d);


    return 1;
}
//sdasddas====================
int sendMessHelp(char* buf,char* domain,int idx,char*login,char*filedomain,char* destlogin)
{
    unlockMtx(login);
    lockMtx(destlogin);

    char path[WORD_SIZE];
    memset(path,'\0',WORD_SIZE);
    strcat(path,domain);
    strcat(path,"/");

    int find=0;
    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    if (!d)
        return -40;

    while ((dir = readdir(d)) != NULL)
        if(!strcmp(destlogin,dir->d_name))
            find=1;
    closedir(d);
    if(!find)
        return -60;


    strcat(path,destlogin);
    strcat(path,"/");

    int unique=0;
    char messName[WORD_SIZE];

back:
    if(!d)
        closedir(d);
    d = opendir(path);
    if (!d)
        return -40;

    while ((dir = readdir(d)) != NULL)
    {
        sprintf(messName,"%d",unique);
        if(!strcmp(messName,dir->d_name))
        {
            unique++;
            goto back;
        }
    }
    closedir(d);

    char word[WORD_SIZE];
    memset(word,'\0',WORD_SIZE);

    //cutWord(buf,word,&idx);
    char* email = buf+idx;

    char data[100];
    memset(data,'\0',100);
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(data, sizeof(data)-1, "%d %m %Y %H:%M", t);


    strcat(path,messName);
    FILE *fp=fopen(path,"w");
    fprintf(fp,"id: %s\nlogin: %s\ndomain: %s\ndata: %s\nmessage:\n%s",messName,login,filedomain,data,email);
    fclose(fp);

    unlockMtx(destlogin);
    lockMtx(login);

    return 1;
}

int sendMessHelpServer(char* buf,int idx,char* login,char* domain,char* destlogin,char* destdomain)
{
    int ci;
    for(ci=0; ci<otherMailIdx&&strcmp(otherMail[ci].name,destdomain); ci++);
    if(ci==otherMailIdx)
        return -80;

    printf("%s\n",buf+idx);
    char* mess=(char*)malloc(EMAIL_SIZE);
    memset(mess,'\0',EMAIL_SIZE);

    strcat(mess,otherMail[ci].passwd);
    strcat(mess," ");
    strcat(mess,otherMail[ci].name);
    strcat(mess," ");
    strcat(mess,otherMail[ci].mypasswd);
    strcat(mess," ");
    strcat(mess,domain);
    strcat(mess," ");
    strcat(mess,login);
    strcat(mess," send ");
    strcat(mess,destlogin);
    strcat(mess," ");
    strcat(mess,buf+idx);

    printf("mess:%s\n",mess);


    int res=sendSerMess(login,otherMail[ci].ip,otherMail[ci].port,mess);

    free(mess);
    if(res<0)
        return res;

    return 1;
}

int sendMess(char* buf,char* domain,int idx,char*login,char*filedomain,int mode)
{
    char destlogin[WORD_SIZE];
    memset(destlogin,'\0',WORD_SIZE);
    cutWord(buf,destlogin,&idx);

    if(mode==0)
    {
        char destdomain[WORD_SIZE];
        memset(destdomain,'\0',WORD_SIZE);
        cutWord(buf,destdomain,&idx);
        if(!strcasecmp(domain,destdomain))
        {
            int res = sendMessHelp(buf,domain,idx,login,filedomain,destlogin);
            if(res<0)
                return res;
        }
        else
        {
            int res = sendMessHelpServer(buf,idx,login,domain,destlogin,destdomain);
            if(res<0)
                return res;
        }
    }
    else
    {
        int res = sendMessHelp(buf,domain,idx,login,filedomain,destlogin);
        if(res<0)
            return  res;
    }

    return 1;
}

int getMessFromServer(char* buf,int idx,char* srcdomain,char* domain,char* login,int serIdx)
{
    int res=sendMess(buf,domain,idx,login,srcdomain,1);
    if(res<0)
        return res;
    return 1;
}

int removeMess(char* buf,char* domain,int idx,char* login)
{
    char filename[WORD_SIZE];
    memset(filename,'\0',WORD_SIZE);
    cutWord(buf,filename,&idx);

    char path[WORD_SIZE];
    memset(path,'\0',WORD_SIZE);
    strcat(path,domain);
    strcat(path,"/");
    strcat(path,login);
    strcat(path,"/");
    strcat(path,filename);

    printf("create path:%s\n",path);

    int status = remove(path);
    if(status==0)
        return 1;
    else
        return -70;
}



int handleRead(char* buf,int accountIdx,struct thread_data_t* th_data)
{
    char* domain = th_data->domain;
    int idx=0;
    char word[WORD_SIZE];
    memset(word,'\0',WORD_SIZE);
    cutWord(buf,word,&idx);

    int si;
    for(si=0; si<otherMailIdx; si++)
    {
        if(!strcmp(word,otherMail[si].mypasswd))
        {
            cutWord(buf,word,&idx);
            if(!strcmp(word,domain))
            {
                cutWord(buf,word,&idx);
                if(!strcmp(word,otherMail[si].passwd))
                {
                    cutWord(buf,word,&idx);
                    if(!strcmp(word,otherMail[si].name))
                    {
                        char login[WORD_SIZE];
                        memset(login,'\0',WORD_SIZE);
                        cutWord(buf,login,&idx);

                        cutWord(buf,word,&idx);
                        if(!strcmp(word,"send"))
                            return getMessFromServer(buf,idx,otherMail[si].name,domain,login,si);
                    }
                    else
                        return -101;
                }
                else
                    return -100;
            }
        }
    }

    if(!strcmp(word,"create"))
        return createAccount(buf,domain,idx);

    if(!strcmp(word,"sign"))
        return signIn(buf,domain,idx,accountIdx);

    if(account[accountIdx].fd!=0&&account[accountIdx].login!=NULL)
    {
        if(!strcmp(word,"logout"))
            return logOut(accountIdx);
        if(!strcmp(word,"getmess"))
            return getMess(domain,accountIdx);
        if(!strcmp(word,"send"))
            return sendMess(buf,domain,idx,account[accountIdx].login,domain,0);
        if(!strcmp(word,"remove"))
            return removeMess(buf,domain,idx,account[accountIdx].login);
    }

    if((account[accountIdx].fd==0||account[accountIdx].login==NULL)&&(!strcmp(word,"logout")
            ||!strcmp(word,"getmess")||!strcmp(word,"send")))
        return -31;

    return 0;
}

/*void *writeTh(void *t_data)
{
    pthread_detach(pthread_self());
    struct thread_data_t *th_data = (struct thread_data_t*)t_data;

    char buf[100];
    int writeResult;
    do
    {
        memset(buf, '\0', sizeof(buf));
        fgets(buf,100,stdin);
    }
    while( (writeResult=write(th_data->fd,buf,100)) );
    if(writeResult==-1)
        perror("Write error message:");

    pthread_exit(NULL);
}*/
