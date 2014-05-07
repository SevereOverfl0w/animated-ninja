#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <json/json.h>

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

  struct MemoryStruct chunk;
  
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
  }

  // Cleanup Curl
  curl_easy_cleanup(curl_handle);
  if (chunk.memory)
    free(chunk.memory);

  curl_global_cleanup();
  return 0;
}
