#include "../lib/mongoose.h"
#include "../include/database.h"
#include "../include/account.h"
#include <stdio.h>
#include <string.h>

#define DATABASE_FILE "data/bank.db"

// Helper за JSON escape на string
static void json_escape_string(char *dest, const char *src, size_t dest_size)
{
    size_t j = 0;
    for (size_t i = 0; src[i] && j < dest_size - 1; i++)
    {
        if (src[i] == '"' || src[i] == '\\')
        {
            if (j < dest_size - 2)
                dest[j++] = '\\';
        }
        dest[j++] = src[i];
    }
    dest[j] = '\0';
}

static void event_handler(struct mg_connection *c, int ev, void *ev_data)
{
    if (ev == MG_EV_HTTP_MSG)
    {
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;

        printf("Request: %.*s %.*s\n",
               (int)hm->method.len, hm->method.buf,
               (int)hm->uri.len, hm->uri.buf);

        if (mg_match(hm->uri, mg_str("/"), NULL) ||
            mg_match(hm->uri, mg_str("/index.html"), NULL))
        {
            struct mg_http_serve_opts opts = {.root_dir = "web"};
            mg_http_serve_file(c, hm, "web/index.html", &opts);
        }
        else if (mg_match(hm->uri, mg_str("/style.css"), NULL))
        {
            struct mg_http_serve_opts opts = {.root_dir = "web"};
            mg_http_serve_file(c, hm, "web/style.css", &opts);
        }
        else if (mg_match(hm->uri, mg_str("/app.js"), NULL))
        {
            struct mg_http_serve_opts opts = {.root_dir = "web"};
            mg_http_serve_file(c, hm, "web/app.js", &opts);
        }


        else if (mg_match(hm->uri, mg_str("/api/accounts"), NULL))
        {

            if (mg_strcmp(hm->method, mg_str("GET")) == 0)
            {
                Account *accounts = NULL;
                int count = 0;

                if (db_get_all_accounts(&accounts, &count) == 0)
                {
                    char json[8192];
                    int offset = 0;

                    offset += snprintf(json + offset, sizeof(json) - offset,
                                       "{\"success\": true, \"count\": %d, \"accounts\": [", count);

                    for (int i = 0; i < count; i++)
                    {
                        char escaped_name[256];
                        json_escape_string(escaped_name, accounts[i].owner_name, sizeof(escaped_name));

                        offset += snprintf(json + offset, sizeof(json) - offset,
                                           "%s{\"id\": %d, \"owner_name\": \"%s\", \"balance\": %.2f, \"created_at\": %ld, \"is_active\": %d}",
                                           (i > 0 ? ", " : ""),
                                           accounts[i].id,
                                           escaped_name,
                                           accounts[i].balance,
                                           accounts[i].created_at,
                                           accounts[i].is_active);
                    }

                    offset += snprintf(json + offset, sizeof(json) - offset, "]}");

                    mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", json);

                    free(accounts);
                }
                else
                {
                    mg_http_reply(c, 500, "Content-Type: application/json\r\n",
                                  "{\"success\": false, \"error\": \"Database error\"}");
                }
            }

            // POST - Create new account
            else if (mg_strcmp(hm->method, mg_str("POST")) == 0)
            {
                char owner_name[100] = {0};
                double initial_balance = 0.0;

                struct mg_str json_body = hm->body;

                const char *name_start = strstr(json_body.buf, "\"owner_name\"");
                if (name_start)
                {
                    name_start = strchr(name_start, ':');
                    if (name_start)
                    {
                        name_start = strchr(name_start, '"');
                        if (name_start)
                        {
                            name_start++;
                            const char *name_end = strchr(name_start, '"');
                            if (name_end)
                            {
                                size_t len = name_end - name_start;
                                if (len < sizeof(owner_name))
                                {
                                    strncpy(owner_name, name_start, len);
                                    owner_name[len] = '\0';
                                }
                            }
                        }
                    }
                }

                // Extract balance
                const char *balance_start = strstr(json_body.buf, "\"balance\"");
                if (balance_start)
                {
                    balance_start = strchr(balance_start, ':');
                    if (balance_start)
                    {
                        sscanf(balance_start + 1, "%lf", &initial_balance);
                    }
                }

                printf("DEBUG: Received POST - name='%s', balance=%.2f\n", owner_name, initial_balance);

                // Validate
                if (strlen(owner_name) > 0 && initial_balance >= 0)
                {
                    int new_id = db_create_account(owner_name, initial_balance);

                    if (new_id > 0)
                    {
                        char json[256];
                        snprintf(json, sizeof(json),
                                 "{\"success\": true, \"message\": \"Account created\", \"id\": %d}",
                                 new_id);
                        mg_http_reply(c, 201, "Content-Type: application/json\r\n", "%s", json);

                        printf("✓ Account created: ID=%d, name=%s, balance=%.2f\n",
                               new_id, owner_name, initial_balance);
                    }
                    else
                    {
                        mg_http_reply(c, 500, "Content-Type: application/json\r\n",
                                      "{\"success\": false, \"error\": \"Failed to create account\"}");
                    }
                }
                else
                {
                    mg_http_reply(c, 400, "Content-Type: application/json\r\n",
                                  "{\"success\": false, \"error\": \"Invalid input\"}");
                }
            }

            // Method not allowed
            else
            {
                mg_http_reply(c, 405, "Content-Type: application/json\r\n",
                              "{\"success\": false, \"error\": \"Method not allowed\"}");
            }
        }


        else if (mg_match(hm->uri, mg_str("/api/stats"), NULL))
        {
            double total = db_get_total_balance();
            double average = db_get_average_balance();
            int count = db_get_account_count();

            char json[512];
            snprintf(json, sizeof(json),
                     "{\"success\": true, \"stats\": {"
                     "\"total_balance\": %.2f, "
                     "\"average_balance\": %.2f, "
                     "\"account_count\": %d"
                     "}}",
                     total, average, count);

            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", json);
        }

 

        else if (mg_match(hm->uri, mg_str("/api/test"), NULL))
        {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                          "{\"message\": \"Hello from API!\", \"status\": \"ok\"}");
        }

       

        else
        {
            mg_http_reply(c, 404, "Content-Type: application/json\r\n",
                          "{\"success\": false, \"error\": \"Not found\"}");
        }
    }
}

int main(void)
{
    // Initialize database
    if (init_database(DATABASE_FILE) != 0)
    {
        printf("Failed to initialize database!\n");
        return 1;
    }

    struct mg_mgr mgr;
    mg_mgr_init(&mgr);

    printf("╔════════════════════════════════════════╗\n");
    printf("║   Bank System HTTP Server              ║\n");
    printf("╠════════════════════════════════════════╣\n");
    printf("║   URL: http://localhost:8000           ║\n");
    printf("║   API: http://localhost:8000/api/      ║\n");
    printf("║   Press Ctrl+C to stop                 ║\n");
    printf("╚════════════════════════════════════════╝\n\n");

    struct mg_connection *conn = mg_http_listen(&mgr, "http://0.0.0.0:8000", event_handler, NULL);

    if (conn == NULL)
    {
        printf("Failed to create listener!\n");
        return 1;
    }

    printf("✓ Server started successfully!\n");
    printf("✓ Database connected\n\n");

    for (;;)
    {
        mg_mgr_poll(&mgr, 1000);
    }

    close_database();
    mg_mgr_free(&mgr);
    return 0;
}