#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 26

static double counts[26];
static char alpha[52] = {'a','b','c','d','e','f','g','h','i','j','k','l','m',
                         'n','o','p','q','r','s','t','u','v','w','x','y','z',
                         'A','B','C','D','E','F','G','H','I','J','K','L','M',
                         'N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

static char* fileName;

void *thread_count(void *threadid)
{
    int n;
    int count = 0;
    char alef, bet, gimel;
    n = (int) threadid;

    FILE *fp;
    fp = fopen(fileName, "r");
    if(fp == NULL)
	{
	    perror("Error in opening file.\n");
	    exit(-1);
	}

    alef = alpha[n];
    bet = alpha[n + 25];
    counts[n] = -1.0;
    
    do
    {
        gimel = fgetc(fp);
        if( (gimel==alef) || (gimel==bet) ) count++;
        
        if( feof(fp) )
        {
            break ;
        }
    }while(1);
    fclose(fp);
    counts[n] = count;
    pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
    pthread_t threads[NUM_THREADS];
    unsigned int n, totalPool = 0;
    double percentages[26];
    
    fileName = argv[1];
    if(fileName == NULL)
    {
        printf("No file aregument provided.  Exiting...");
        exit(0);
    }
    
    int rc;
    
    for ( n = 0; n < NUM_THREADS; n++ )
    {
        rc = pthread_create(threads + n, NULL, thread_count, (void *) n);
        if (rc)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    
    for ( n = 0; n < NUM_THREADS; n++ ) pthread_join(threads[n], NULL);
    //printf("All sub-threads finished.\n");
    //for( n = 0; n < NUM_THREADS; n++ ) printf("Count at %d is %g", n, counts[n]);
    //return(0);  
    
    for( n = 0; n < NUM_THREADS; n++ ) totalPool = totalPool + counts[n];
    
    char *letter = "Character";
    char *rawCount = "Occurances";
    char *percent = "Percent of total";
    printf("\n%10s %12s %18s \n", letter, rawCount, percent);
    
    for( n = 0; n < 26; n++ )
    {
        percentages[n] = counts[n] / totalPool;
        printf("%10c %12g %17.2f%% \n", alpha[n], counts[n], 100 * percentages[n]);
    }
    
    return(0);        
}
        