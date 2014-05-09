#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <json/json.h>

#define CURRENCY_LEN 4
#define SYMBOL_LEN 10

struct CurrencyStruct {
    char *currency_code;
    double last;
    double buy;
    double sell;
    double _15m;
    char *symbol;
};


struct MemoryStruct {
    char *memory;
    size_t size; // Or length
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp){
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL) {
        // Out of memory!
        printf("Out of memory! Bye bye world");
        return 0; // Anything that isn't realsize is an error by cURL
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

int main(int argc, char *argv[]){
    CURL *curl_handle;
    CURLcode res;
    json_object *response;

    struct MemoryStruct chunk;
    struct CurrencyStruct cs;
    struct CurrencyStruct *csarray;
    size_t cssize = 0;

    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init(); // Initialize the curl session
    curl_easy_setopt(curl_handle, CURLOPT_URL, "https://blockchain.info/ticker"); // Hardcoding ftw
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback); // Set callback for data chunks received.
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk); // Passed as userp to above WriteMemoryCallback
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "lubcurl-agent/1.0"); // Some people want the Agent...

    res = curl_easy_perform(curl_handle); // Perform the operation!

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    } else {
        printf("%lu bytes munched\n", (long)chunk.size);
        printf("Received:\n%s\n", chunk.memory);
        response = json_tokener_parse(chunk.memory);
        if (is_error(response)){
            puts("Failed to parse JSON!");
        } else {
            json_object_object_foreach(response, currency, info) {
                // TODO: Tidy this up... somehow
                // TODO: malloc the final struct, add to array of some kind...
                cs.currency_code = malloc(CURRENCY_LEN);
                strncpy(cs.currency_code, currency, CURRENCY_LEN);
                json_object *tmp;
                json_object_object_get_ex(info, "last", &tmp);
                cs.last = json_object_get_double(tmp);
                json_object_object_get_ex(info, "buy", &tmp);
                cs.buy = json_object_get_double(tmp);
                json_object_object_get_ex(info, "sell", &tmp);
                cs.sell = json_object_get_double(tmp);
                json_object_object_get_ex(info, "15m", &tmp);
                cs._15m = json_object_get_double(tmp);
                json_object_object_get_ex(info, "symbol", &tmp);
                cs.symbol = malloc(SYMBOL_LEN);
                strncpy(cs.symbol, json_object_get_string(tmp), SYMBOL_LEN);
                csarray = realloc(csarray, sizeof(struct CurrencyStruct) * ++cssize);
                csarray[cssize-1] = cs;
            }
        }
    }

    int cnt;
    for (cnt = 0; cnt < cssize; cnt++){
        printf("%s: %s%f\n", csarray[cnt].currency_code, csarray[cnt].symbol, csarray[cnt].last);
    }

    // Cleanup Parsing related stuff
    for (cnt = 0; cnt < cssize; cnt++){
        free(cs.currency_code);
        free(cs.symbol);
    }

    free(csarray);

    // Cleanup Curl
    curl_easy_cleanup(curl_handle);
    if (chunk.memory)
        free(chunk.memory);

    curl_global_cleanup();
    return 0;
}
