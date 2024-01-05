#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <uv.h>
#include <errno.h>
#include <string.h>
#include "client/client.h"
#include "server/server.h"

int main()
{
    uv_loop_t *loop = uv_default_loop();

    serve(loop, "0.0.0.0", 8090);
    connect_to(loop, "127.0.0.1", 8080);

    uv_run(loop, UV_RUN_DEFAULT);
    return 0;
}
