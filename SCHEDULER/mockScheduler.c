#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const int OFFSET = 38;
static const int DEBUG = 1;
static const int QUANTAM = 30;
static int running;
static int procCount;

struct Process {
    char name[17], status, priority;
    int id, burstTime, baseReg;
    long limitReg;
};

void load_procs(struct Process *allProcs, int procCount, FILE *fr);
void print_procs(struct Process *allProcs, int procCount);
void write_procs(struct Process *allProcs, int procCount, FILE *fw);
void load_indices(struct Process *allProcs, int *priorityQ);
void print_indices(int *priorityQ);
void priority_decay(struct Process *allProcs);
int cpu_burst(struct Process *allProcs, int j);
int cmpfuncmod (const void * a, const void * b);


int main()
{
    running = 0;
    
    //FILE FOR WRITING
    FILE *fw;
    fw = fopen("NEWproc.bin", "wb");
    
    //FILE FOR READING
    FILE *fr;
    fr = fopen("processes.bin", "rb");
    fseek(fr, 0L, SEEK_END);
    int sz = ftell(fr);
    rewind(fr);
    procCount = sz / OFFSET;
    
    struct Process allProcs[procCount];
    
    int i = 0, j;
    
    load_procs(allProcs, procCount, fr);
    printf("%d processes parsed from .bin file.\nAllocated %lu bytes in memory to track processes.\n\n", procCount, sizeof(allProcs));
    
    int * priorityQ = NULL, finished = 0;
    priorityQ = (int*) realloc (priorityQ, running * sizeof(int));
    load_indices(allProcs, priorityQ);
    
    //return 0;
    if (DEBUG)
    {
        //print_indices(priorityQ, indexQ);
        //printf("%d\n\n", running);
        //print_procs(allProcs, procCount);
    }
    
    //print_procs(allProcs, procCount);
    
    int ops = 0, skips = 0, pIndex = 0;
    
    while(!finished)
    {
        // ROUND ROBIN
        skips = 0;
        //i = 0;
        ops = 0;
        puts("RR start");
        while(ops < QUANTAM)
        {
            if(allProcs[i].status != 0)
            {
                if (cpu_burst(allProcs, i++) == 0)
                {
                    write_procs(allProcs, procCount, fw);
                    printf("\nWrite - round robin.  Running:%d\n", running);
                }
                if ( ((i % 2) == 0) && (i > 0) ) priority_decay(allProcs);
                ops++;
                skips = 0;
            } else {
                i++;
                if(++skips == procCount)
                {
                    puts("Finished!");
                    ops = QUANTAM;
                    finished = 1;
                    write_procs(allProcs, procCount, fw);
                    break;
                }
                if ( ((i % 2) == 0) && (i > 0) ) priority_decay(allProcs);
            }
            i = (i % procCount) ;
            printf("\nrrIndex = %d", i);
        }
        puts("RR end");
        ops = 0;
        puts("Priority start");
        while(ops < QUANTAM)
        {
            printf("\npriorityIndex = %d", priorityQ[pIndex]);
            if (pIndex == procCount)
            {
                puts("Finished!");
                ops = QUANTAM;
                finished = 1;
                write_procs(allProcs, procCount, fw);
                break;
            }
            if(allProcs[priorityQ[pIndex]].status != 0)
            {
                ops++;
                if (cpu_burst(allProcs, priorityQ[pIndex]) == 0)
                {
                    write_procs(allProcs, procCount, fw);
                    printf("\nWrite - priority.  Running:%d\n", running);
                    //pIndex++;
                }
            } 
            else 
            {
                pIndex++;
            }
            if ( ((i % 2) == 0) && (i > 0) ) priority_decay(allProcs);
            
        }
        puts("Priority end");
        //print_procs(allProcs, procCount);
        
    }
    
    print_procs(allProcs, procCount);
    
    write_procs(allProcs, procCount, fw);
    
    fclose(fw);
    fclose(fr);
    return 0;
}

void load_procs(struct Process *allProcs, int procCount, FILE *fr)
{
    char buffer[38];
    
    int j;
    for (j = 0; j < procCount; j++)
    {
        fread(buffer, OFFSET, 1, fr);
        
        // Copy to structs in array
        memcpy(&allProcs[j].name, &buffer[0], 16);
        allProcs[j].name[16] = '\0';
        memcpy(&allProcs[j].id, &buffer[16], sizeof(int));
        memcpy(&allProcs[j].status, &buffer[20], sizeof(char));
        if (allProcs[j].status == 1) running++;
        memcpy(&allProcs[j].burstTime, &buffer[21], sizeof(int));
        memcpy(&allProcs[j].baseReg, &buffer[25], sizeof(int));
        memcpy(&allProcs[j].limitReg, &buffer[29], sizeof(long));
        memcpy(&allProcs[j].priority, &buffer[37], sizeof(char));
    }
}

void print_procs(struct Process *allProcs, int procCount)
{
    int j;
    for (j = 0; j < procCount; j++){
        printf("%3d. ", j+1);
        printf("%16s", allProcs[j].name);
        printf("%12d", allProcs[j].id);
        printf("%3d", allProcs[j].status);
        printf("%5d", allProcs[j].burstTime);
        printf("%7d  ", allProcs[j].baseReg);
        printf("%7lu  ", allProcs[j].limitReg);
        printf("%5d  \n", allProcs[j].priority);
    }
    puts("\n\n");
}

void write_procs(struct Process *allProcs, int procCount, FILE *fw)
{
    char buffer[38];
    int j;
    for (j = 0; j < procCount; j++)
    {
        fseek(fw, j * OFFSET, SEEK_SET);
        
        // copy FROM structs in array
        memcpy(&buffer[0], &allProcs[j].name, 16);
        memcpy(&buffer[16], &allProcs[j].id, sizeof(int));
        memcpy(&buffer[20], &allProcs[j].status, sizeof(char));
        memcpy(&buffer[21], &allProcs[j].burstTime, sizeof(int));
        memcpy(&buffer[25], &allProcs[j].baseReg, sizeof(int));
        memcpy(&buffer[29], &allProcs[j].limitReg, sizeof(long));
        memcpy(&buffer[37], &allProcs[j].priority, sizeof(char));
        
        // write objects
        fwrite(buffer, sizeof(buffer), 1, fw);
    }
}

int cpu_burst(struct Process *allProcs, int j)
{
    int stat = allProcs[j].status, priority = allProcs[j].priority, burstTime = allProcs[j].burstTime;
    --burstTime;
    if ((burstTime == 0) && (stat != 0) )
    {
        stat = 0;
        running--;
        burstTime = 0;
        memcpy(&allProcs[j].status, &stat, sizeof(char));
        memcpy(&allProcs[j].burstTime, &burstTime, sizeof(int));
        //print_procs(allProcs, procCount);
        return stat;
    }
    memcpy(&allProcs[j].burstTime, &burstTime, sizeof(int));
    return stat;
}

void load_indices(struct Process *allProcs, int *priorityQ)
{
    puts("load index start");
    int lastIndex = -1, index = -1, min, i, j, k;  
    int loaded[procCount];
    
    for(i = 0; i < procCount; i++)
    {
        priorityQ[i] = allProcs[i].priority + (i * 1000);
    }
    puts("Raw:");
    //print_indices(priorityQ);
    
    qsort(priorityQ, procCount, sizeof(int), cmpfuncmod);
    puts("Sorted:");
    //print_indices(priorityQ);
    
    for(i = 0; i < procCount; i++)
    {
        priorityQ[i] = priorityQ[i] / 1000;
    }
    
    /*
    for(i = 0; i < procCount; i++)
    {
         // min is a char, so must be less than 128
        min = 128;
        for(j = 0; j < procCount; j++)
        {
            //printf("  %d:", allProcs[j].priority);
            if ( (allProcs[j].priority <= min) && ( loaded[j] != 1 ) && (j != index))
            {
                min = allProcs[j].priority;
                //if (DEBUG) printf("%d  ", j);
                index = j;
            }
        }
        loaded[index] = 1;
        priorityQ[i] = index;
    }
    */
    puts("load index end");
    
    //print_indices(priorityQ);
}

void print_indices(int *priorityQ)
{
    int i;
    printf("\nPRIORITY\n");
    for(i = 0; i < procCount; i++)
    {
        printf("%4d%8d\n", i+1, priorityQ[i]);
    }
}

void priority_decay(struct Process *allProcs)
{
    int j, c;
    for (j = 0; j < procCount; j++)
    {
        if ( (&allProcs[j].status != 0) && (allProcs[j].priority > 0) )
        {
            c = allProcs[j].priority - 1;
            memcpy(&allProcs[j].priority, &c, sizeof(char));
        }
    }
    
}

int cmpfuncmod (const void * a, const void * b)
{
   return ( (*(int*)a % 1000) - (*(int*)b % 1000) );
}