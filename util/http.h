#include <stdlib.h>
#include <curl/curl.h>

#define UA_MAX_QUERY_LEN 4096
#define UA_MAX_QUERY_QUEUE 10


/* Callback types */
typedef void* (*UAGenericCallback)(void*);
typedef int (*UAEventCallback)(char*, void*);
typedef int (*UAHTTPPOSTProcessor)(char*, char*, char*);
typedef char* (*UAURLEncoder)(char*);



typedef struct HTTPQueue_t {

  CURLM* handler;

  int count;
  CURL* requests[ UA_MAX_QUERY_QUEUE ];

} HTTPQueue_t;


void HTTPcleanup(HTTPQueue_t* queue);
void HTTPsetup(HTTPQueue_t* queue);


int HTTPenqueue(HTTPQueue_t* queue, const char* endpoint, const char* useragent, const char* query, int query_len);


