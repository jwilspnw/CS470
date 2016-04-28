//zz
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>

int main( int argc, char *argv[] )
{
    FILE *fp;
    char alef, bet, gimel, alpha[52] = {'a','b','c','d','e','f','g','h','i','j','k','l','m',
                      			'n','o','p','q','r','s','t','u','v','w','x','y','z',
                      			'A','B','C','D','E','F','G','H','I','J','K','L','M',
                      			'N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
    unsigned int n, tallied = 0, addedToTally[26], count;
    double percentages[26], totalPool = 0.0, counts[26];

    if( argv[1] == NULL )
    {
        printf("This program takes the text file to be analyzed as an argument.\nNo argument supplied. Exiting...\n");
        exit(0);
    }

    fp = fopen(argv[1], "r");
    if( fp == NULL )
    {
	printf("Error in opening file %s\n", argv[1]);
	exit(-1);
    }

    for ( n = 0; n < 26; n++ )
    {
        counts[n] = -1.0;
        addedToTally[n] = 0;
        rewind(fp);
        if( vfork() == 0 )
        {
            count = 0;
            do
            {
                alef = alpha[n]; //lowercase number
                bet = alpha[n + 26];  //uppercase number
                gimel = fgetc(fp);
                if( (gimel==alef) || (gimel==bet) ) count++;
                
                if( feof(fp) )
                {
                    break ;
                }
            }while(1);
            counts[n] = count;
            exit(getpid());
        }
    }

    fclose(fp);
    
    n = 0;

    while ( tallied < 26 )
    {
        if ( (counts[n] > -1.0) && (addedToTally[n] != 1) )
        {
            tallied++;
            totalPool = totalPool + counts[n];
            addedToTally[n] = 1;
        }
        n++;
        n = n % 26;
    }
    
    char *letter = "Character";
    char *rawCount = "Occurances";
    char *percent = "Percent of total";
    printf("\n%10s%12s%18s\n", letter, rawCount, percent);
    
    for( n = 0; n < 26; n++ )
    {
        percentages[n] = counts[n] / totalPool;
        printf("%10c%12g%17.2f%%\n", alpha[n], counts[n], 100 * percentages[n]);
    }
    
    return(0);
}
