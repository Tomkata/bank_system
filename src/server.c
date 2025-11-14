#include "../lib/mongoose.h"
#include <stdio.h>

static void event_handler(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        
        printf("Received request: %.*s %.*s\n", 
               (int)hm->method.len, hm->method.buf,
               (int)hm->uri.len, hm->uri.buf);
        
        mg_http_reply(c, 200, "Content-Type: text/html\r\n",
                      "<html><body><h1>Hello from C Server!</h1></body></html>");
    }
}

int main(void) {
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    
    printf("Starting HTTP server on http://localhost:8000\n");
    
    struct mg_connection *conn = mg_http_listen(&mgr, "http://0.0.0.0:8000", event_handler, NULL);
    
    if (conn == NULL) {
        printf("Failed to create listener!\n");
        return 1;
    }
    
    for (;;) {
        mg_mgr_poll(&mgr, 1000);
    }
    
    mg_mgr_free(&mgr);
    return 0;
}