#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include "myLib.c"
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 40960
#define QUEUE_SIZE 5

void* readTh(void* t_data)
{
    char cres[5];
    int writeResult;

    pthread_detach(pthread_self());
    struct thread_data_t* th_data = (struct thread_data_t*)t_data;

    printf("th:%d\n",th_data->fd);

    if(addFd(th_data->fd)==-1)
    {
        close(th_data->fd);
        printf("MaxNumberOfClients!\n");
    }

    int accountIdx;
    for(accountIdx=0; accountIdx<MAX_CLIENT&&account[accountIdx].fd!=th_data->fd; accountIdx++);

    char* buf=(char*)malloc(BUF_SIZE);//[BUF_SIZE];
    memset(buf, '\0', BUF_SIZE);

    while( saveRead(th_data->fd,buf,BUF_SIZE) )
    {
        if(buf[0]=='\0')continue;
        printf("fd[%d]:%s\n",th_data->fd,buf);
        if(account[accountIdx].fd==th_data->fd)
        {
            int lockRes=lockMtx(account[accountIdx].login);
            int res = handleRead(buf,accountIdx,th_data);
            if(lockRes)unlockMtx(account[accountIdx].login);
            printf("res[%d]:%d\n",account[accountIdx].fd,res);
            memset(cres,'\0',5);
            sprintf(cres,"%d",res);

            writeResult=write(account[accountIdx].fd,cres,5);
            printf("write_op_res[%d]:%d\n",account[accountIdx].fd,writeResult);
            if(writeResult<0)
                perror("Write error message:");
        }
        memset(buf, '\0', BUF_SIZE);
    }
    free(buf);

    if(delFd(th_data->fd)==-1)
        printf("Not find file descriptor(%d)!\n",th_data->fd);

    close(th_data->fd);
    printf("Close fd:%d\n",th_data->fd);

    free(t_data);

    pthread_exit(NULL);
}

void handleConnection(int connection_socket_descriptor,char * domain)
{
    int create_result = 0;

    pthread_t thread2;//,thread1;

    struct thread_data_t *t_data=malloc(sizeof(struct thread_data_t));
    t_data->fd=connection_socket_descriptor;
    t_data->domain=domain;

    /*create_result = pthread_create(&thread1, NULL, writeTh, (void *)t_data);
    if (create_result)
    {
        printf("blad przy probie utworzenia watku, kod bledu: %d\n", create_result);
        exit(-1);
    }*/

    create_result = pthread_create(&thread2, NULL, readTh, (void *)t_data);
    if (create_result)
    {
        printf("blad przy probie utworzenia watku, kod bledu: %d\n", create_result);
        exit(-1);
    }

    //close(connection_socket_descriptor);

}

int main(int argc, char* argv[])
{
    if(argc!=4)
    {
        printf("Specify ip_addr port domain\n");
        exit(-1);
    }
    short unsigned int SERVER_PORT = atoi(argv[2]);
    memset(account,0,sizeof(struct accounts)*MAX_CLIENT);
    memset(otherMail,0,sizeof(struct otherServer)*MAX_SERVER);
    if(readOtherServerFile()<0)
    {
        printf("otherServers.txt error!\n");
        return -1;
    };
    printf("Server %s e-mail start\n",argv[3]);
    mkdir(argv[3],0777);


    int server_socket_descriptor;
    int connection_socket_descriptor;
    int bind_result;
    int listen_result;
    char reuse_addr_val = 1;
    struct sockaddr_in server_address;

    memset(&server_address, 0, sizeof(struct sockaddr));
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &(server_address.sin_addr));
    server_address.sin_port = htons(SERVER_PORT);

    server_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_descriptor < 0)
    {
        fprintf(stderr, "%s: blad przy probie utworzenia gniazda..\n", argv[0]);
        exit(1);
    }
    setsockopt(server_socket_descriptor, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse_addr_val, sizeof(reuse_addr_val));

    bind_result = bind(server_socket_descriptor, (struct sockaddr*)&server_address, sizeof(struct sockaddr));
    if (bind_result < 0)
    {
        fprintf(stderr, "%s: blad przy probie dowiazania adresu IP i numeru portu do gniazda.\n", argv[0]);
        exit(1);
    }

    listen_result = listen(server_socket_descriptor, QUEUE_SIZE);
    if (listen_result < 0)
    {
        fprintf(stderr, "%s: blad przy probie ustawienia wielkosci kolejki.\n", argv[0]);
        exit(1);
    }

    //int r=1;
    while(1)
    {
        connection_socket_descriptor = accept(server_socket_descriptor, NULL, NULL);
        if (connection_socket_descriptor < 0)
        {
            fprintf(stderr, "%s: blad przy probie utworzenia gniazda dla polaczenia.\n", argv[0]);
            exit(1);
        }
        else
            printf("Open fd:%d\n",connection_socket_descriptor);
        handleConnection(connection_socket_descriptor,argv[3]);
        /*if(r&&atoi(argv[2])!=1235){
                r=0;
                sendMess(argv[0],"localhost",1235,"hello\n");
        }*/
    }

    close(server_socket_descriptor);
    return(0);
}
