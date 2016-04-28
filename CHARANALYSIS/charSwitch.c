#include <stdio.h>
#include <stdlib.h>

static double counts[26];
void incChar(int alif);

int main( int argc, char *argv[] )
{
    FILE *fp;
    char alef, bet, gimel;
    char alpha[26] = {'a','b','c','d','e','f','g','h','i','j','k','l','m',
                      'n','o','p','q','r','s','t','u','v','w','x','y','z'};
    unsigned int n, tallied = 0, addedToTally[26], mainid = getpid(), counted = 0;
    int status;
    double percentages[26];
    double totalPool = 0.0;
    
    if( argv[1] == NULL )
    {
        printf("This program takes the text file to be analyzed as an argument.\nNo argument supplied. Exiting...\n");
        exit(0);
    }

    for ( n = 0; n < 26; n++ )
    {
        counts[n] = 0.0;
        addedToTally[n] = 0;
    }
    
    fp = fopen(argv[1], "r");
    
    if(fp == NULL)
    {
        perror("Error in opening file.\n");
        return(-1);
    }

    unsigned int sz, current = 0, i, j;
    double percentDone = 0.0;

    fseek(fp, 0L, SEEK_END);
    sz = ftell(fp);
    printf("%d\n", sz);
    fseek(fp, 0L, SEEK_SET);
    
    sz = sz / 50;

    if(vfork() == 0)
    {
        int dot = 0;
	printf("Running character analysis in process %d:\n", getpid());
        do
        {
            incChar(fgetc(fp));
	    if(++dot == sz)
	    {
		printf(".");
		dot = 0;
	    }
            if( feof(fp) )
            {
		printf("\nDONE!\n");
                break ;
            }
        }while(1);
        
        exit(getpid());
    }
    
    n = 0;

    while (tallied < 26)
    {
        if ((counts[n] > -1.0) && (addedToTally[n] != 1))
        {
            tallied++;
            totalPool = totalPool + counts[n];
            addedToTally[n] = 1;
        }
        n++;
    }
    
    char *letter = "Character";
    char *rawCount = "Occurances";
    char *percent = "Percent of total";
    printf("\n%10s%12s%18s\n", letter, rawCount, percent);
    
    for( n = 0; n < 26; n++ )
    {
        percentages[n] = counts[n] / totalPool;
        printf("%10c%12g%17.2f%% \n", alpha[n], counts[n], 100 * percentages[n]);
    }
    
    return(0);
}

void incChar(int alif)
{
    switch (alif){
        case 'a':
        case 'A':
            counts[0] = counts[0] + 1;
            break;
        case 'b':
        case 'B':
            counts[1] = counts[1] + 1;
            break;
        case 'c':
        case 'C':
            counts[2] = counts[2] + 1;
            break;
        case 'd':
        case 'D':
            counts[3] = counts[3] + 1;
            break;
        case 'e':
        case 'E':
            counts[4] = counts[4] + 1;
            break;
        case 'f':
        case 'F':
            counts[5] = counts[5] + 1;
            break;
        case 'g':
        case 'G':
            counts[6] = counts[6] + 1;
            break;
        case 'h':
        case 'H':
            counts[7] = counts[7] + 1;
            break;
        case 'i':
        case 'I':
            counts[8] = counts[8] + 1;
            break;
        case 'j':
        case 'J':
            counts[9] = counts[9] + 1;
            break;
        case 'k':
        case 'K':
            counts[10] = counts[10] + 1;
            break;
        case 'l':
        case 'L':
            counts[11] = counts[11] + 1;
            break;
        case 'm':
        case 'M':
            counts[12] = counts[12] + 1;
            break;
        case 'n':
        case 'N':
            counts[13] = counts[13] + 1;
            break;
        case 'o':
        case 'O':
            counts[14] = counts[14] + 1;
            break;
        case 'p':
        case 'P':
            counts[15] = counts[15] + 1;
            break;
        case 'q':
        case 'Q':
            counts[16] = counts[16] + 1;
            break;
        case 'r':
        case 'R':
            counts[17] = counts[17] + 1;
            break;
        case 's':
        case 'S':
            counts[18] = counts[18] + 1;
            break;
        case 't':
        case 'T':
            counts[19] = counts[19] + 1;
            break;
        case 'u':
        case 'U':
            counts[20] = counts[20] + 1;
            break;
        case 'v':
        case 'V':
            counts[21] = counts[21] + 1;
            break;
        case 'w':
        case 'W':
            counts[22] = counts[22] + 1;
            break;
        case 'x':
        case 'X':
            counts[23] = counts[23] + 1;
            break;
        case 'y':
        case 'Y':
            counts[24] = counts[24] + 1;
            break;
        case 'z':
        case 'Z':
            counts[25] = counts[25] + 1;
            break;
    }
}
