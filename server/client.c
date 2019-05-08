#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define BUF_SIZE 1024
#define NUM_THREADS 5

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

//struktura zawierająca dane, które zostaną przekazane do wątku
struct thread_data_t
{
    int fd;
};

//wskaźnik na funkcję opisującą zachowanie wątku
void *ThreadBehavior(void *t_data)
{
    struct thread_data_t *th_data = (struct thread_data_t*)t_data;

    char buf[100]="clinet conn\n";
    int writeResult;
    do
    {
        memset(buf,'\0',sizeof(buf));
        fgets(buf,100,stdin);
        int i,k=0;
        for(i=0;i<100;i++)
        {
            if(buf[i]=='\0')k=1;
            if(k==1)buf[i]='\0';
        }

    }
    while( (writeResult=write(th_data->fd,buf,100)) );
    if(writeResult==-1)
        perror("Write error message:");

    pthread_exit(NULL);
}


//funkcja obsługująca połączenie z serwerem
void handleConnection(int connection_socket_descriptor) {
    printf("Server fd:%d\n",connection_socket_descriptor);
    //wynik funkcji tworzącej wątek
    int create_result = 0;

    //uchwyt na wątek
    pthread_t thread1;

    //dane, które zostaną przekazane do wątku
    struct thread_data_t *t_data= malloc(sizeof(struct thread_data_t));
    t_data->fd=connection_socket_descriptor;
    //TODO
    create_result = pthread_create(&thread1, NULL, ThreadBehavior, (void *)t_data);
    if (create_result){
       printf("Błąd przy próbie utworzenia wątku, kod błędu: %d\n", create_result);
       exit(-1);
    }

    char buf[100];
    memset(buf,'\0',sizeof(buf));

    while( saveRead(connection_socket_descriptor,buf,100) )
    {
		if(buf[0]=='\0')continue;
        printf("%s\n",buf);
        memset(buf, '\0', sizeof(buf));
    }
    
    close(connection_socket_descriptor);
}


int main (int argc, char *argv[])
{
   int connection_socket_descriptor;
   int connect_result;
   struct sockaddr_in server_address;
   struct hostent* server_host_entity;

   if (argc != 3)
   {
     fprintf(stderr, "Sposób użycia: %s server_name port_number\n", argv[0]);
     exit(1);
   }

   server_host_entity = gethostbyname(argv[1]);
   if (! server_host_entity)
   {
      fprintf(stderr, "%s: Nie można uzyskać adresu IP serwera.\n", argv[0]);
      exit(1);
   }

   connection_socket_descriptor = socket(PF_INET, SOCK_STREAM, 0);
   if (connection_socket_descriptor < 0)
   {
      fprintf(stderr, "%s: Błąd przy probie utworzenia gniazda.\n", argv[0]);
      exit(1);
   }

   memset(&server_address, 0, sizeof(struct sockaddr));
   server_address.sin_family = AF_INET;
   memcpy(&server_address.sin_addr.s_addr, server_host_entity->h_addr, server_host_entity->h_length);
   server_address.sin_port = htons(atoi(argv[2]));

   connect_result = connect(connection_socket_descriptor, (struct sockaddr*)&server_address, sizeof(struct sockaddr));
   if (connect_result < 0)
   {
      fprintf(stderr, "%s: Błąd przy próbie połączenia z serwerem (%s:%i).\n", argv[0], argv[1], atoi(argv[2]));
      exit(1);
   }

   handleConnection(connection_socket_descriptor);

   close(connection_socket_descriptor);
   return 0;

}
