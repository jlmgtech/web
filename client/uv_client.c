#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <uv.h>
#include <errno.h>
#include <string.h>
#include "uv_client.h"
#include "client.h"

#ifndef ignore
#define ignore __attribute__((unused))
#endif

void on_server_connect(uv_connect_t* connection, int status)
{
    if (status < 0) {
        fprintf(stderr, "Connection error %s\n", uv_strerror(status));
        client_on_error((uv_handle_t*)connection->handle, status);
        return;
    }

    client_on_connect(connection);
    uv_read_start(connection->handle, new_servbuff, process_read);
}

void new_servbuff(ignore uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    *buf = uv_buf_init((char*) malloc(suggested_size), suggested_size);
}

void process_read(uv_stream_t *server, ssize_t nread, const uv_buf_t *buf)
{
    if (nread > 0) {
        client_on_data(server, buf->base, nread);
    }
    else {
        client_on_error((uv_handle_t*)server, nread);
    }

    // cargo culted from the docs:
    if (buf->base) {
        free(buf->base);
    }
}
