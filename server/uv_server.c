#include "uv_server.h"
#include "server.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <uv.h>
#include <errno.h>
#include <string.h>

void on_new_connection(uv_stream_t *server, int status)
{

    if (status < 0) {
        // status is bad, return
        printf("bad status (%d), aborting connection\n", status);
        char str[32] = {0};
        sprintf(str, "%d", status);
        onservererror(server, "bad status", str);
        return;
    }

    // freed in on_close
    uv_tcp_t *client = malloc(sizeof(uv_tcp_t));
    if (!client) {
        onservererror(server, "failed allocation", strerror(errno));
        return;
    }

    uv_tcp_init(uv_default_loop(), client);

    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        onconnect((uv_stream_t*)client);
        uv_read_start((uv_stream_t*) client, new_clientbuff, on_read);
    } else {
        uv_close((uv_handle_t*) client, NULL);
    }
}

void new_clientbuff(__attribute__((unused)) uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    buf->base = (char*) malloc(suggested_size); // freed in on_read
    buf->len = suggested_size;
}

void on_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf)
{
    if (nread > 0) {
        ondata(client, buf->base, nread);
    } else if (nread < 0) {
        if (nread != UV_EOF) {
            // Error handling
            onclienterror(client, "read error", uv_err_name(nread));
        }
        uv_close((uv_handle_t*) client, on_close);
    }

    if (buf->base) {
        free(buf->base); // Always free the buffer
    }
}

void on_write_end(uv_write_t *req, int status)
{
    if (status) {
        onclienterror(req->handle, "write error", uv_err_name(status));
    }
    free(req);
}

void on_close(uv_handle_t* handle)
{
    onclose(handle);
    free(handle);
}
