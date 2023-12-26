#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <uv.h>
#include <errno.h>
#include <string.h>


void on_new_connection(uv_stream_t*, int);
void on_read(uv_stream_t*, ssize_t, const uv_buf_t*);
void alloc_buffer(uv_handle_t*, size_t, uv_buf_t*);
void on_close(uv_handle_t*);
void on_write_end(uv_write_t *req, int status);

int main()
{
    //int server = server_create();
    //server_on_connect(server, on_connect);
    //server_on_error(server, on_error);
    //server_on_close(server, on_close);
    //server_on_data(server, on_data);
    //server_listen(server, "0.0.0.0", 8080);

    const char *const addr_str = "0.0.0.0";
    const int port = 8080;

    uv_loop_t *loop;
    uv_tcp_t server;
    struct sockaddr_in addr;

    loop = uv_default_loop();
    uv_tcp_init(loop, &server);

    uv_ip4_addr(addr_str, port, &addr);
    uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);

    uv_listen((uv_stream_t*)&server, SOMAXCONN, on_new_connection);

    printf("Listening on %s:%d\n", addr_str, port);
    uv_run(loop, UV_RUN_DEFAULT);

    return 0;
}

int cprintf(uv_stream_t *client, const char *fmt, ...);
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



// okay, so now we need to use the server pointer address as a key into a table of connections
// so we can save state for specific connections. Unless there's a better way within libuv.

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
    }

    uv_tcp_init(uv_default_loop(), client);

    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        onconnect((uv_stream_t*)client);
        uv_read_start((uv_stream_t*) client, alloc_buffer, on_read);
    } else {
        uv_close((uv_handle_t*) client, NULL);
    }
}

void alloc_buffer(__attribute__((unused)) uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
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
