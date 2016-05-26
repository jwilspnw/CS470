Author: Justin Wilson
Class : CS470

This assignment is my own, original work except where otherwise noted.

This program creates a server on the port specified within the config.ini file.
Recommended compilation is as follows:
> For serverThreaded.c
    gcc -Wall serverThreaded.c -o serverT -lpthread
    
> For clientThreaded.c
    gcc -Wall clientThreaded.c -o clientT -lm
    
If the client cannot connect, it will time out.