#ifndef SERVE_H
#define SERVE_H

#define SERVE_REQ 1
#define SERVE_END 0

typedef void (*http_handler_t)(int);

typedef enum {
    SERV_OPEN,
    SERV_DATA,
    SERV_ERR,
    SERV_CLOSE
} serv_evt_t;

int serve(char *addr);
void serve_add_handler(int server, serv_evt_t evt, http_handler_t handler);
void serve_listen(int server, char *addr);
void serve_wait(int server);

#endif
