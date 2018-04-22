#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#include "http_parser/http_parser.h"

#define RESPONSE \
    "HTTP/1.1 200 OK\r\n"   \
    "Content-type: text/plain\r\n"  \
    "Content-length: 12\r\n"    \
    "\r\n"  \
    "Hello World\n"

static uv_buf_t s_resp;

typedef struct
{
    http_parser *parser;
    http_parser_settings *parser_settings;
}mydata_st;


void alloc_buf(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
    char *mem = (char *)malloc(suggested_size);
    if (!mem) {
        fprintf(stderr, "malloc %d size memery error\n", (int)suggested_size);
        exit(1);
    }
    
    *buf = uv_buf_init(mem, suggested_size);
}

void on_close(uv_handle_t *handle)
{
    //printf("Disconnected..\n");
}

void after_write(uv_write_t *req, int status)
{
    if (status < 0) {
        fprintf(stderr, "write error: %s\n", uv_strerror(status));
        return;
    }
   
    uv_close((uv_handle_t *)req->handle, NULL);
}

int on_headers_complete(http_parser *parser)
{
    uv_stream_t *client = parser->data;
    uv_write_t *req = (uv_write_t *)malloc(sizeof(uv_write_t));
    uv_write(req, client, &s_resp, 1, after_write);
    return 0;
}

void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
{
    if (nread < 0) {
        if (nread != UV_EOF) {
           fprintf(stderr, "read error: %s\n", uv_strerror(nread));
        }
        uv_close((uv_handle_t *)stream, on_close);
        goto end;
    }
   
    mydata_st *mydata = stream->loop->data;
    http_parser *parser = mydata->parser;
    http_parser_settings *parser_settings = mydata->parser_settings;
    parser->data = stream;
    int parsed = http_parser_execute(parser, parser_settings, buf->base, nread);
    if (parsed < nread) {
        fprintf(stderr, "http parser error: %s\n", http_errno_description(parser->http_errno));
        uv_close((uv_handle_t *)stream, on_close);
        goto end;
    }
    
    //write(1, buf->base, nread);

end:
    free(buf->base);
}

void on_new_connection(uv_stream_t *server, int status)
{
    if (status < 0) { 
        fprintf(stderr, "status = %d, error: %s\n", status, uv_strerror(status));
        return;
    }

    uv_tcp_t *client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
    if (!client) {
        fprintf(stderr, "malloc error\n");
        exit(1);
    }
    uv_tcp_init(server->loop, client);
    int ret = uv_accept(server, (uv_stream_t *)client);
    if (ret) {
        fprintf(stderr, "Accept error\n");
        uv_close((uv_handle_t *)client, NULL);
        return;
    }

    uv_read_start((uv_stream_t *)client, alloc_buf, on_read);
}

int main()
{
    http_parser *parser = (http_parser *)malloc(sizeof(http_parser));
    http_parser_init(parser, HTTP_REQUEST);
   
    http_parser_settings *parser_settings = (http_parser_settings *)malloc(sizeof(http_parser_settings));
    parser_settings->on_headers_complete = on_headers_complete;
    
    mydata_st mydata;
    mydata.parser = parser;
    mydata.parser_settings = parser_settings;

    uv_loop_t *loop;
    loop = uv_default_loop();
    loop->data = &mydata;

    s_resp = uv_buf_init(RESPONSE, (unsigned int)strlen(RESPONSE));

    uv_tcp_t server;
    uv_tcp_init(loop, &server);
    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", 8000, &addr);
    int ret = uv_tcp_bind(&server, (const struct sockaddr *)&addr, 0); 
    if (ret) {
        fprintf(stderr, "Bind addr error: %s\n", uv_strerror(ret));
        return 1;
    }

    ret = uv_listen((uv_stream_t *)&server, 128, on_new_connection);
    if (ret) {
        fprintf(stderr, "Listen errer: %s\n", uv_strerror(ret));
        return 1;
    }
    
    uv_run(loop, UV_RUN_DEFAULT);

    uv_loop_close(loop);
    return 0;
}
