#pragma once
#include <uv.h>
#include "uv_server.h"
void serve(uv_loop_t *loop, const char *addr_str, const int port);
int cprintf(uv_stream_t *client, const char *fmt, ...);
void dowrite(uv_stream_t *client, char *data, size_t len);
void onconnect(uv_stream_t *client);
void onservererror(uv_stream_t *server, const char *error, const char *detail);
void onclienterror(uv_stream_t *client, const char *error, const char *detail);
void ondata(uv_stream_t *client, const char *data, size_t len);
void onclose(uv_handle_t *client);
int vacprintf(uv_stream_t *client, const char *fmt, va_list args);
int cprintf(uv_stream_t *client, const char *fmt, ...);
