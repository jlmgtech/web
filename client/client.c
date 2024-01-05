#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <uv.h>
#include <string.h>
#include "client.h"
#include "uv_client.h"
#ifndef ignore
#define ignore __attribute__((unused))
#endif

void connect_to(uv_loop_t *loop, char *addr_str, int port)
{

    uv_tcp_t client;
    uv_tcp_init(loop, &client);

    struct sockaddr_in dest;
    uv_ip4_addr(addr_str, port, &dest);

    uv_connect_t connect_req;
    uv_tcp_connect(&connect_req, &client, (const struct sockaddr*)&dest, on_server_connect);

}

void client_onclose(ignore uv_handle_t* handle)
{
    // if there's any cleanup, like the buffer, do it here
    printf("on_server_close!\n");
}

void client_close(uv_connect_t* connection)
{
    uv_close((uv_handle_t*) connection->handle, client_onclose);
}


int client_vprintf(uv_stream_t *stream, char *fmt, va_list args)
{
    char *buffer = NULL;
    int len = vasprintf(&buffer, fmt, args);
    uv_buf_t buf = uv_buf_init(buffer, len);
    uv_write_t request;
    uv_write(&request, stream, &buf, 1, NULL);
    free(buffer);
    return len;
}


int client_printf(uv_stream_t *stream, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int len = client_vprintf(stream, fmt, args);
    va_end(args);
    return len;
}


void client_on_connect(uv_connect_t* connection)
{
    //printf("client_on_connect!\n");
    client_printf(connection->handle, "Hi there!\n");
}

void client_on_error(uv_handle_t *handle, int error)
{
    fprintf(stderr, "Error: %s\n", uv_strerror(error));
    uv_close(handle, client_onclose);
}

void client_on_data(uv_stream_t *server, char *buffer, ssize_t nread)
{
    printf("received '%.*s'", (int)nread, buffer);
    client_printf(server, "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
}
