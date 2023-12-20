#include <stdio.h>

int fetch(char *url)
{
    printf("Fetching %s\n", url);
    return 0;
}

int fetch_wait(int handle)
{
    printf("Fetching %d\n", handle);
    return 0;
}
