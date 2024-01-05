#define _GNU_SOURCE
#include <stdio.h>
#include "server.h"
#include <uv.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static uv_tcp_t server;
void serve(uv_loop_t *loop, const char *addr_str, const int port)
{
    struct sockaddr_in addr;

    uv_tcp_init(loop, &server);

    uv_ip4_addr(addr_str, port, &addr);
    uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);

    uv_listen((uv_stream_t*)&server, SOMAXCONN, on_new_connection);
}

void dowrite(uv_stream_t *client, char *data, size_t len)
{
    uv_write_t *write_req = (uv_write_t*) malloc(sizeof(uv_write_t));
    uv_buf_t write_buf = uv_buf_init(data, len);
    uv_write(write_req, client, &write_buf, 1, on_write_end);
}

void onconnect(uv_stream_t *client)
{
    printf("%p connected\n", (void*)client);
}

void onservererror(uv_stream_t *server, const char *error, const char *detail)
{
    printf("%p SERVER ERROR: %s (%s)\n", (void*)server, error, detail);
}

void onclienterror(uv_stream_t *client, const char *error, const char *detail)
{
    printf("%p CLIENT ERROR: %s (%s)\n", (void*)client, error, detail);
}

void ondata(uv_stream_t *client, const char *data, size_t len)
{
    cprintf(client, "client (%p) data(%lu): %s\n", (void*)client, len, data);
}

void onclose(uv_handle_t *client)
{
    printf("closing client %p\n", (void*)client);
}

int vacprintf(uv_stream_t *client, const char *fmt, va_list args)
{
    char *output = NULL;
    int outlen = vasprintf(&output, fmt, args);
    if (outlen <= 0) {
        onclienterror(client, "failed allocation", strerror(errno));
        free(output); // if output is NULL, free does nothing
        return errno;
    }
    dowrite(client, output, outlen);
    free(output);
    return 0;
}

int cprintf(uv_stream_t *client, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int ret = vacprintf(client, fmt, args);
    va_end(args);
    return ret;
}
