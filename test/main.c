/******************************************************************************
* Universal Analytics for C 
* Copyright (c) 2013, Analytics Pros
* 
* This project is free software, distributed under the BSD license. 
* Analytics Pros offers consulting and integration services if your firm needs 
* assistance in strategy, implementation, or auditing existing work.
******************************************************************************/

#include <stdlib.h>
#include "../src/universal-analytics.h"



int main(int argc, char** argv){
 
  int i;
 
  unsigned int test_http_buffer = 0; /* Toggle repetitive requests to confirm queuing behavior */
  unsigned int test_http_total_requests = 25; /* To run over the request maximum a few times... */

  // This value will show with the word "nët" encoded as "n%c3%abt" on the query string
  char unicode_value[] = "Ech kan Glas iessen, daat deet mir nët wei";



  /* Static definition of a group of options (in stack space) */
  UASettings settings = {{
      {UA_DOCUMENT_PATH, 0, "/virtual/test/C"},
      {UA_DOCUMENT_TITLE, 0, "This is a test in C"},
      {UA_CUSTOM_DIMENSION, 5, "C library (5)"},
      {UA_CUSTOM_DIMENSION, 7, "Custom Dimension 7"},
      {UA_CUSTOM_METRIC, 5, "2451"}, /* A numeric string, custom metric index 5 */
      {UA_USER_AGENT, 0, "Opera/9.80 (Windows NT 6.0) Presto/2.12.388 Version/12.14"},
      {UA_ANONYMIZE_IP, 0, "1"} /* IP anonymization for privacy-aware contexts (e.g. apps in EU) */
  }};





  /* createTracker() calls malloc for the entire size required by
   * the tracker; all memory it requires (except for CURL) is
   * allocated within this call. */
  UATracker tracker = createTracker("UA-XXXXX-Y", "abc1234", NULL);
 
  /* Override the User Agent HTTP header */
  tracker->user_agent = "user_agent_test";


  /* Enable queing 
   * This causes all requests to be queued until the "cleanup" phase
   * of the tracker's life (i.e. at removeTracker() below) */
  setTrackerOption(tracker, UA_OPTION_QUEUE, 1);

  /* Store these options permanently (for the lifetime of the tracker) */
  setParameters(tracker, & settings); 



  /* Send a pageview, with no additional options.
   * Processes only the parameters that are set permanently in the 
   * tracker. */
  printf("Sending core pageview\n");
  sendTracking(tracker, UA_PAGEVIEW, NULL);
  



  UAOptions mobileOptions = {{
    {UA_APP_ID, 0, "org.example.application"},
    {UA_APP_INSTALLER_ID, 0, "test-kit"},
    {UA_APP_NAME, 0, "My Application"},
    {UA_SCREEN_NAME, 0, "My Mobile Home Screen"},
    {UA_EVENT_CATEGORY, 0, "Mobile"},
    {UA_EVENT_ACTION, 0, "App Start"},
    {UA_EVENT_LABEL, 0, unicode_value} // This value will leverage the UTF-8 encoding
  }};


  printf("Sending mobile screenview\n");
  sendTracking(tracker, UA_SCREENVIEW, & mobileOptions);

  


  /* Static definition of a group of options (in stack space) */
  UAOptions opts = {{
    {UA_EVENT_CATEGORY, 0, "Event Category"},
    {UA_EVENT_ACTION, 0, "Event Action (C)"},
    {UA_EVENT_LABEL, 0, "Event Label"},
    {UA_CUSTOM_DIMENSION, 3, "Three"}
  }};
  

  /* Send a bunch of events with additional ephemeral options.
   * We're sending such a large batch to test the HTTP library's
   * non-blocking (queuing) behavior. 
   */
  if(test_http_buffer){
    printf("Sending %d events\n", test_http_total_requests);
    for(i = 0; i < test_http_total_requests; i++){
      sendTracking(tracker, UA_EVENT, & opts); 
    }
  }

  /* Process the queued tracking, then clear the tracker's memory
   * and deallocate */
  removeTracker(tracker);


  return 0;
}
