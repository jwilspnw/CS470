#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

unsigned int hash(unsigned char *str);
int cmp(char *arg1, char *arg2);
int help();
int makedir(char *arg1);

int main()
{
	printf("Running CS470 Microshell...\nReady!\nType a command or type 'help' for assistance.\n");
	
	char inp[100];
	char *cmd[3];
	char *cmdToken;

	int lastExit = 0;
	
	while (getpid() > 0)
	{
		printf(ANSI_COLOR_MAGENTA "cwushell"  ANSI_COLOR_RESET);
		printf(":");
		printf(ANSI_COLOR_BLUE    "~ $ "  ANSI_COLOR_RESET);
		fgets(inp, 99, stdin);
		char *pos, *arg;
		if ((pos=strchr(inp, '\n')) != NULL) *pos = '\0';
		
		const char space[2] = " ";
		cmdToken = strtok(inp, space);

		int i = 0;
		while((cmdToken != NULL) && (i < 3))
		{
			cmd[i++] = cmdToken;
			cmdToken = strtok(NULL, space);
		}
        
        switch (hash(cmd[0]))
        {
            case 193416251: // cmp
                if (i > 2)
                {
                    if (vfork() == 0) lastExit = cmp(cmd[1],cmd[2]);
                }
                else
                {
                    printf("Need 2 arguments for the cmp command to execute.\n");
                }                
                break;
            case 2087435909: // exit
                return (atoi(cmd[1]) != 0) ? atoi(cmd[1]) : lastExit;
            case 2087804052: // help
                if (vfork() == 0) lastExit = help();
                break;
            case 5861018: // ls
                if ( vfork() == 0 )
                {
                    system("ls");
                    exit(getpid());
                }
                break;
            case 174437788: // mkdir
                arg = ((i > 1) ? cmd[1] : "DEFAULT");
                if (vfork() == 0) lastExit = makedir(arg);
                break;
        }
    }
}

int cmp(char *arg1, char *arg2)
{
    if(0 == strcmp(arg1,arg2))
    {
        printf("Same file referenced twice, file is identical to itself.\n");
    }
    else
    {
        FILE *file1, *file2;
        int f1c, f2c;
        file1 = fopen(arg1, "r");
        file2 = fopen(arg2, "r");
        if ((file1 == NULL) || (file2 == NULL))
        {
            printf("Unable to open file.\n");
        }
        else
        {
            f1c = getc(file1);
            f2c = getc(file2);

            int pos = 0;
            while((f1c == f2c) && (f1c != EOF) && (f2c != EOF))
            {
                f1c = getc(file1);
                f2c = getc(file2);
                pos++;
            }

            if (f1c == f2c)
            {
                printf("%s and %s are identical.\n", arg1, arg2);
            }
            else
            {
                printf("%s and %s differ at position %d where %s = 0x%X and %s = 0x%X.\n", arg1, arg2, pos, arg1, f1c, arg2, f2c);
            }
        }
    }
    exit(getpid());
}

int help()
{
    printf("This terminal accepts the following commands:\n");
	printf("  cmp [file1] [file2]\n\t -Compares two files and returns the position of the first byte where they do not  match.  file1 and file2 arguments are required.\n");
	printf("  exit [code]\n\t -Exits the shell with the given code.  Code is an optional argument.  If no code is  supplied, it will exit with 0.\n");
	printf("  ls\n\t -Lists the files in the current directory.\n");
	printf("  mkdir [name]\n\t -Creates a folder with the given name argument.  If no name arguemnt is supplied, a DEFAULT folder will be created.\n\n");
    exit(getpid());
}

int makedir(char *arg1)
{
    mkdir(arg1, 0700);
    exit(getpid());
}

// Dan Bernstein's djb2 hashing algorithm, accessed from http://www.cse.yorku.ca/~oz/hash.html
unsigned int hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++) hash = 33 * hash ^ c;

    return hash;
}
