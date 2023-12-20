#include <stdio.h>

int serve(char *addr)
{
    printf("Serving on %s\n", addr);
    return 0;
}

int serve_wait(int server)
{
    printf("Waiting for connection on %d\n", server);
    return 0;
}

int serve_close(int server)
{
    return 0;
}
