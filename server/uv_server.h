#pragma once
#include <uv.h>

void on_new_connection(uv_stream_t*, int);
void on_read(uv_stream_t*, ssize_t, const uv_buf_t*);
void new_clientbuff(uv_handle_t*, size_t, uv_buf_t*);
void on_close(uv_handle_t*);
void on_write_end(uv_write_t *req, int status);
