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
  
  int total_requests = 25; /* To run over the request maximum a few times... */
  int i;
  
  /* Static definition of a group of options (in stack space) */
  UASettings settings = {{
      {UA_CUSTOM_DIMENSION, 5, "C library (5)"},
      {UA_CUSTOM_DIMENSION, 7, "Custom Dimension 7"},
      {UA_CUSTOM_METRIC, 5, "55"},
      {UA_CUSTOM_METRIC, 7, "77"},
      {UA_DOCUMENT_PATH, 0, "/virtual/test/C"},
      {UA_DOCUMENT_TITLE, 0, "This is a test in C"}
  }};


  /* Statically allocate the tracker space (in stack) */
  UATracker_t realTracker;
  UATracker tracker = & realTracker; /* this is optional if you just want to pass pointers around. */
 
  /* Configure the tracker */
  initTracker(tracker, "UA-XXXXX-Y", "abc1234", NULL);  
 

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
  printf("Sending %d events\n", total_requests);
  for(i = 0; i < total_requests; i++){
    sendTracking(tracker, UA_EVENT, & opts); 
  }

  /* Process the queued tracking, then clear the tracker's memory */
  cleanTracker(tracker);


  return 0;
}
