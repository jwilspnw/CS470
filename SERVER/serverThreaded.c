#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread

#define NEXT(n, i)  (((n) + (i)/(n)) >> 1)  
 
//the thread function
void *connection_handler(void *);

//integer sqrt function
int isqrt(int number);
 
int main(int argc , char *argv[])
{
    FILE *ini;
    ini = fopen("config.ini", "r");
    char line[128];
    fgets(line, 128, ini);
    const char space[2] = " ";
    char *initTok;
    initTok = strtok(line, space);
    initTok = strtok(NULL, space);
    printf("%s", initTok);
    int sckt_num = atoi(initTok);
    printf("Attempting to create port on socket %d", sckt_num);
    
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
        exit(0);
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( sckt_num );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("Unable to bind to socket.");
        exit(0);
    }
    printf("Bound to socket %d\n", sckt_num);
     
    //Listen on bound socket
    listen(socket_desc , 3);
     
    //Accept incoming connections
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted...");
         
        pthread_t handler_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
         
        if( pthread_create( &handler_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            exit(0);
        }
        
        puts("...handler thread assigned.");
    }
     
    if (client_sock < 0)
    {
        perror("Connection failed:");
        exit(0);
    }
     
    exit(0);
}
 
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc, client_num = 1, svr_x, svr_y;
    int read_size;
    
    //Send num to client
    svr_x = rand();
    do
    {
        svr_y = rand() % (isqrt(svr_x));
    } while(svr_y == 0);

    
    write(sock , &svr_x , sizeof(int));
    write(sock , &svr_y , sizeof(int));
    
    //Receive a message from client
    while( (read_size = recv(sock , &client_num , sizeof(int) , 0)) > 0 )
    {
        if (client_num == 1)
        {
            puts("Divisible pair found. Closing connection.");
            break;
        } else {
            svr_x = rand();
            do
            {
                svr_y = rand() % (isqrt(svr_x));
            } while(svr_y == 0);
            
            write(sock , &svr_x , sizeof(int));
            write(sock , &svr_y , sizeof(int));
        }
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected.");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("Error getting response from client:");
    }
         
    //Free the socket pointer
    free(socket_desc);
     
    return 0;
}

int isqrt(int number)
{
    int n  = 1;  
    int n1 = NEXT(n, number);  
    
    while(abs(n1 - n) > 1)
    {  
        n  = n1;  
        n1 = NEXT(n, number);  
    }  
    while(n1*n1 > number) n1--;
    return n1;  
}  