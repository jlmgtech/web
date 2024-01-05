#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <uv.h>
#include <errno.h>
#include <string.h>
#define ignore __attribute__((unused))



// make client wrapper:
// onconnect
// write
// ondata
// close

void on_server_close(uv_handle_t* handle);
void on_server_connect(uv_connect_t* connection, int status);
void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
void process_read(uv_stream_t *server, ssize_t nread, const uv_buf_t *buf);

void client_close(uv_connect_t* connection)
{
    uv_close((uv_handle_t*) connection->handle, on_server_close);
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
    uv_close(handle, on_server_close);
}

void client_on_data(uv_stream_t *server, char *buffer, ssize_t nread)
{
    printf("received '%.*s'", (int)nread, buffer);
    client_printf(server, "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
}

int main()
{
    uv_loop_t *loop = uv_default_loop();
    //serve(loop, "0.0.0.0", 8080);

    uv_tcp_t client;
    uv_tcp_init(loop, &client);

    struct sockaddr_in dest;
    uv_ip4_addr("127.0.0.1", 8080, &dest); // replace with your server IP and port

    uv_connect_t connect_req;
    uv_tcp_connect(&connect_req, &client, (const struct sockaddr*)&dest, on_server_connect);

    uv_run(loop, UV_RUN_DEFAULT);
    return 0;
}

void on_server_connect(uv_connect_t* connection, int status)
{
    if (status < 0) {
        fprintf(stderr, "Connection error %s\n", uv_strerror(status));
        client_on_error((uv_handle_t*)connection->handle, status);
        return;
    }

    client_on_connect(connection);
    uv_read_start(connection->handle, alloc_buffer, process_read);
}

void on_server_close(ignore uv_handle_t* handle)
{
    // if there's any cleanup, like the buffer, do it here
    printf("on_server_close!\n");
}

void alloc_buffer(ignore uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
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

