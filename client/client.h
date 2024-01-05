#pragma once
#include <uv.h>

void connect_to(uv_loop_t *loop, char *ip, int port);
void client_close(uv_connect_t* connection);
int client_vprintf(uv_stream_t *stream, char *fmt, va_list args);
int client_printf(uv_stream_t *stream, char *fmt, ...);
void client_on_connect(uv_connect_t* connection);
void client_on_error(uv_handle_t *handle, int error);
void client_on_data(uv_stream_t *server, char *buffer, ssize_t nread);
