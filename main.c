#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <uv.h>
#include <errno.h>
#include <string.h>

void on_server_connect(uv_connect_t* connection, int status);

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

//void connect(uv_loop_t *loop, char *ip, int port) {

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
void echo_read(uv_stream_t *server, ssize_t nread, const uv_buf_t *buf);
void on_server_connect(uv_connect_t* connection, int status) {
    if (status < 0) {
        fprintf(stderr, "Connection error %s\n", uv_strerror(status));
        // Handle the error
        return;
    }

    puts("successfully connected!\n");

    // now write something:
    char *buffer = "hello world\n";
    uv_buf_t buf = uv_buf_init(buffer, strlen(buffer));
    uv_write_t request;
    uv_write(&request, connection->handle, &buf, 1, NULL);

    // now receive the response:
    uv_read_start(connection->handle, alloc_buffer, echo_read);

    // Connection is successful
    // You can start reading or writing data
}

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    *buf = uv_buf_init((char*) malloc(suggested_size), suggested_size);
}

void echo_read(uv_stream_t *server, ssize_t nread, const uv_buf_t *buf) {
    if (nread > 0) {
        printf("Received data: %s\n", buf->base);
    }
    else {
        uv_close((uv_handle_t*) server, NULL);
    }

    // cargo culted from the docs:
    if (buf->base) {
        free(buf->base);
    }
}

