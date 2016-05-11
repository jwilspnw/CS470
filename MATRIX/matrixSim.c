#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <pthread.h>

#define LOCK_EX 2
#define LOCK_UN 8

void *thread_augment(void *fp);
int matching(void *fp);

const char ONE = '1', ZERO = '0';

static int n, iterations, complete, noChanges;

int main()
{
    int threadCount;
    FILE *fp;
	complete = 0;
    fp = fopen("mattxt.txt", "r");
    if (fp != NULL) remove("mattxt.txt");
    fp = fopen("mattxt.txt", "w+");
    
    puts("Enter the number n for the nxn matrix: ");
    char buffern[65], buffert[65];
    fgets( buffern, 65, stdin );
    n = atoi( buffern );
    
    puts("Enter the number of threads on which the simulation will execute:");
    fgets( buffert, 65, stdin );
    threadCount = atoi( buffert );
    
    printf("Processing an %dx%d matrix on %d threads...\n", n, n, threadCount);
    
    int i = 0;
    char ij = ' ';
    if (vfork() == 0)
    {
        for (i = 0; i < (n * n); i++)
        {
         	ij = (char) ((rand() % 2) + 48);
            fwrite(&ij, 1, sizeof(ij), fp);
        }
        exit(getpid());
    }
	
    pthread_t threads[threadCount];
    iterations = 0;
    time_t t0 = clock();
    for ( i = 0; i < threadCount; i++ ) pthread_create(threads + i, NULL, thread_augment, (void *) fp);
    time_t t1 = clock();
    int datetime_diff_ms = difftime(t1, t0);
    printf("Thread creation took %d clock cycles", datetime_diff_ms);
    
    for ( i = 0; i < threadCount; i++ ) pthread_join(threads[i], NULL);
    
    time_t t2 = clock();
    rewind(fp);
    i = 0;
    int current;
	puts("");
	datetime_diff_ms = difftime(t2, t0);
	printf("Simulation finished after %d clock cycles\n", datetime_diff_ms);
    do
    {
        current = fgetc(fp);
        printf("%c ", current);
        if ((++i % n) == 0) printf("\n");
        if (feof(fp)) break;
    }while(i < (n*n));
    fclose(fp);
}

void *thread_augment(void *fp)
{
	do
	{
		flock(fp, LOCK_EX);
    	int max = (n*n), x = (iterations % max), zeros = 0, ones = 0, index;
		int xj[2] = {0,0};

    	index = (x % n) - 1;
    	if ( index >= 0 )
    	{
    	    fseek(fp, index, SEEK_SET);
			xj[(fgetc(fp) - 48)]++;
    	}
    	index = x - n;
    	if ( index >= 0 )
    	{
    	    fseek(fp, index, SEEK_SET);
			xj[(fgetc(fp) - 48)]++;
    	}
    	index = (x % n) + 1;
    	if ( index < n )
    	{
    	    fseek(fp, index, SEEK_SET);
			xj[(fgetc(fp) - 48)]++;
    	}
    	index = x + n;
    	if ( index < max )
    	{
    	    fseek(fp, index, SEEK_SET);
			xj[(fgetc(fp) - 48)]++;
    	}

    	fseek(fp, x, SEEK_SET);
    	char a = '0', b = '1';
    	if ( xj[0] < xj[1] ) fwrite(&b, 1, sizeof(char), fp);
    	if ( xj[0] > xj[1] ) fwrite(&a, 1, sizeof(char), fp);
		if ( xj[0] == xj[1] ) noChanges++;
		if ( (noChanges % n) == 0 ) fwrite(&a, 1, sizeof(char), fp);  // Attempt to elimate deadlock 
		
		if (++iterations % (n * n) == 0 ) complete = matching(fp);
		flock(fp, LOCK_UN);
	}while(complete == 0);
    
}

int matching(void *fp)
{
	int i = 0;
	iterations = 0;
	fseek(fp, 0, SEEK_SET);
	char last = fgetc(fp);
	for(i = 0; i < n*n; i++)
	{	
		if (last != fgetc(fp))
		{
			return 0;
			break;
		}
	}
	return 1;
}