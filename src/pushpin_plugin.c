#include <uwsgi.h>
#include <curl/curl.h>

// Send Fn
static void send_pushpin_message(const char *message)
{
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:5561/publish/");
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        char json_body[512];
        snprintf(json_body, sizeof(json_body),
                 "{ \"items\": [ { \"channel\": \"test\", \"formats\": { "
                 "\"http-stream\": { \"content\": \"%s\\n\" } } } ] }",
                 message);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_body);
        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            uwsgi_log("Failed to send message to Pushpin: %s\n", curl_easy_strerror(res));
        }
        else
        {
            uwsgi_log("Message sent to Pushpin: %s\n", message);
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

// Main
static int pushpin_hook(char *arg)
{
    uwsgi_log("Pushpin hook called with arg: %s\n", arg);
    send_pushpin_message(arg);
    return 0; // Successful completion
}

// Init
static int pushpin_plugin_init()
{
    uwsgi_register_hook("pushpin_hook", pushpin_hook);
    return 0; // Successful initialization
}

// Register
struct uwsgi_plugin pushpin_plugin = {
    .name = "pushpin_hook",
    .init = pushpin_plugin_init,
};