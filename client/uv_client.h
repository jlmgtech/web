#pragma once
#include <uv.h>

void on_server_connect(uv_connect_t* connection, int status);
void new_servbuff(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
void process_read(uv_stream_t *server, ssize_t nread, const uv_buf_t *buf);
