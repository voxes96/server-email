#include "textManip.c"
#include "thdata.c"
#include <string.h>

#define EMAIL_SIZE 40960
#define MAX_SERVER 128
#define WORD_SIZE 1024

struct otherServer
{
    char name[128];
    char ip[128];
    short unsigned int port;
    char passwd[WORD_SIZE];
    char mypasswd[WORD_SIZE];
};
int otherMailIdx=0;
struct otherServer otherMail[MAX_SERVER];

int sendSerMess(char* src,char* ip,short unsigned int port,char* mess)
{
    int connection_socket_descriptor;
    int connect_result;
    struct sockaddr_in server_address;
    struct hostent* server_host_entity;

    server_host_entity = gethostbyname(ip);
    if (! server_host_entity)
    {
        fprintf(stderr, "%s: Nie mo¿na uzyskaæ adresu IP serwera.\n", src);
        return -300;
    }

    connection_socket_descriptor = socket(PF_INET, SOCK_STREAM, 0);
    if (connection_socket_descriptor < 0)
    {
        fprintf(stderr, "%s: B³¹d przy probie utworzenia gniazda.\n", src);
        return -310;
    }

    memset(&server_address, 0, sizeof(struct sockaddr));
    server_address.sin_family = AF_INET;
    memcpy(&server_address.sin_addr.s_addr, server_host_entity->h_addr, server_host_entity->h_length);
    server_address.sin_port = htons(port);

    connect_result = connect(connection_socket_descriptor, (struct sockaddr*)&server_address, sizeof(struct sockaddr));
    if (connect_result < 0)
    {
        fprintf(stderr, "%s: B³¹d przy próbie po³¹czenia z serwerem (%s:%i).\n", src,ip,port);
        return -320;
    }

    printf("Server fd:%d\n",connection_socket_descriptor);

    int writeResult=write(connection_socket_descriptor,mess,EMAIL_SIZE);
    if(writeResult==-1)
        perror("Write error message:");

    saveRead(connection_socket_descriptor,mess,5);

    close(connection_socket_descriptor);

    return atoi(mess)-1000;

    return 1;
}

int readOtherServerFile()
{
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int idx;
    char strPort[10];

    fp = fopen("otherServers.txt", "r");
    if (fp == NULL)
        return -2;

    while ((read = getline(&line, &len, fp)) != -1)
    {
        idx=0;
        cutWord(line,otherMail[otherMailIdx].name,&idx);
        cutWord(line,otherMail[otherMailIdx].ip,&idx);
        cutWord(line,strPort,&idx);
        otherMail[otherMailIdx].port = atoi(strPort);
        cutWord(line,otherMail[otherMailIdx].passwd,&idx);
        cutWord(line,otherMail[otherMailIdx].mypasswd,&idx);
        if(!strcmp(otherMail[otherMailIdx].passwd,otherMail[otherMailIdx].mypasswd))return -1;
        otherMailIdx++;
    }

    fclose(fp);
    if (line)free(line);
    return 1;
}

