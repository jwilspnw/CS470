#include <stdio.h> //printf
#include <stdlib.h>
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <sys/timeb.h>

int checkDivis(int x, int y);
 
int main(int argc , char *argv[])
{
    int sckt, x, y, sendNum, sckt_num, timeOut;
    struct sockaddr_in server;
    
    FILE *ini;
    size_t len = 0;
    ini = fopen(argv[1], "r");
    char *line, *initTok;
    char *ipAdd;
    
    if (ini != NULL) // Load from file if Arg supplied can be loaded
    {
        getline(&line, &len, ini);
        fclose(ini);
    
        const char space[2] = " ";
        strtok(line, space);
        initTok = strtok(NULL, space);
        sckt_num = atoi(initTok);
        strtok(NULL, space); // consume "ip:"
        ipAdd = strtok(NULL, space);
        strtok(NULL, space); // consume "timeout(sec):"
        initTok = strtok(NULL, space);
        timeOut = atoi(initTok) * 1000; // get timeout in ms
    }
    else //otherwise load defaults
    {
        puts("Could not load file from supplied arguement.  Initializing with default settings.");
        sckt_num = 8668;
        ipAdd = "127.0.0.1";
        timeOut = 10 * 1000; // get timeout in ms
    }

    printf("Starting new client, listening to port %d on %s\n", sckt_num, ipAdd );

    //Create socket
    sckt = socket(AF_INET , SOCK_STREAM , 0);
    if (sckt == -1)
    {
        puts("Could not create socket.");
        exit(0);
    }
    puts("Socket created.");
    
    server.sin_addr.s_addr = inet_addr(ipAdd);
    server.sin_family = AF_INET;
    server.sin_port = htons( sckt_num );
 
    puts("Attempting to connect...");
    
    //Connect to server through socket
    int timeFlag = 1, diff;
    struct timeb start, end;
    ftime(&start);
    while(timeFlag)
    {
        if (connect(sckt , (struct sockaddr *)&server , sizeof(server)) < 0)
        {
            usleep(250000); // wait a quarter second to not peg the CPU core
            ftime(&end);
            diff = (int) (1000.0 * (end.time - start.time) + (end.millitm - start.millitm));
            if (diff > timeOut)
            {
                puts("Could not connect, and connection timed out.  Exiting...");
                exit(1);
            }
        }
        else
        {
            timeFlag = 0;
        }
    }
     
    puts("Connected\n");
    
    // get initial nums to calculate on
    if( (recv(sckt , &x , sizeof(int) , 0) < 0) || (recv(sckt , &y , sizeof(int) , 0) < 0) )
    {
        puts("Problem in receiving the number pair.");
        exit(0);
    }
    
    while(1)
    {
        //printf("Server provides numbers x = %d, y = %d\n", x, y);
        sendNum = checkDivis(x, y);

        if( send(sckt , &sendNum , sizeof(int) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
        if (sendNum == 0)
        {
            //printf("%d was not evenly divisible by %d, requesting new number pair...\n\n", x, y);
        }
        else
        {
            printf("%d is evenly divisible by %d.  Exiting...\n\n", x, y);
            exit(0);
        }
         
        //Receive a new set of nums from the server
        if( (recv(sckt , &x , sizeof(int) , 0) < 0) || (recv(sckt , &y , sizeof(int) , 0) < 0) )
        {
            puts("Problem in receiving the number pair.");
            break;
        }
    }
    return 0;
}

int checkDivis(int z, int e)
{
    if ((z % e) == 0) return 1;
    return 0;
}