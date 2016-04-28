#include <stdio.h>
#include <unistd.h>

int main()
{
	printf("%d\n", sysconf(_SC_NPROCESSORS_ONLN));
}
