#include "http.h"
#include <assert.h>
#include <string.h>

#define DEBUG_PRINT_CURL_QUERY 0
#define DEBUG_CURL_VERBOSE 0

/* This module abstracts CURL from the rest of our tracking code.
 * Future versions will likely replace CURL with another HTTP client
 * for easier static build support. */


/* Data handler for CURL to silence it's default output */
size_t curl_null_data_handler(char *ptr, size_t size, size_t nmemb, void *userdata){
  if(DEBUG_CURL_VERBOSE)
    printf("Processing request 0x%lx\n", (unsigned long int) userdata);
  return (nmemb * size);
}


/* Sequentially de-queue requests */
int curl_multi_unload(CURLM* handler, CURL* requests[], int total){
  int i, count = total;
  while(count){
    curl_multi_perform(handler, & count);
  }

  for(i = 0; i < total; i++){
    curl_easy_cleanup(requests[ i ]);
    requests[ i ] = NULL;
  }

  return count;
}

/* Process queued requests */
void HTTPcleanup(HTTPQueue_t* queue){
  int i;
  if(NULL != queue->handler){
    curl_multi_unload(queue->handler, queue->requests, queue->count);
    curl_multi_cleanup(queue->handler);
    queue->handler = NULL;
  }
  queue->count = 0;
}

/* Prepare the CURL Multi handler */
void HTTPsetup(HTTPQueue_t* queue){
  queue->handler = curl_multi_init();
  curl_multi_setopt(queue->handler, CURLMOPT_PIPELINING, 1);
}

void HTTPflush(HTTPQueue_t* queue){
  assert(NULL != queue->handler);
  if(0 < queue->count){
    curl_multi_unload(queue->handler, queue->requests, queue->count);
  }
}


/* Enqueue a POST using CURL */
int HTTPenqueue(HTTPQueue_t* queue, const char* endpoint, const char* useragent, const char* query, int query_len){
  
  assert(NULL != queue);
  assert(NULL != query);
  assert(NULL != endpoint);
  assert(NULL != useragent);

  assert(queue->count < UA_MAX_QUERY_QUEUE);


  CURLM* handler = queue->handler;
  CURL* curl = curl_easy_init();
  CURLcode res;


  if(DEBUG_PRINT_CURL_QUERY)
    printf("Queueing: \n\t- %s\n\t- %s\n\t- %s\n", endpoint, useragent, query);

  if(DEBUG_CURL_VERBOSE)
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
  

  curl_easy_setopt(curl, CURLOPT_URL, endpoint);
  curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, query);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, useragent);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_null_data_handler);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, curl);

  curl_multi_add_handle(handler, curl);

  queue->requests[ queue->count++ ] = curl; 
 
  return queue->count;
}


