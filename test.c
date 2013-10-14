#include <stdlib.h>
#include <stdio.h>
#include "universal-analytics.h"

int main (int argc, char** argv){

  printf("UA_PAGEVIEW: %d\n", UA_PAGEVIEW);
  printf("UA_EXCEPTION: %d\n", UA_EXCEPTION);
  printf("UA_MAX_PARAMETERS: %d\n", UA_MAX_PARAMETERS);
  printf("UA_TRACKING_ID: %d\n", UA_TRACKING_ID);
  printf("UA_VERSION_NUMBER: %d\n", UA_VERSION_NUMBER);
  printf("UA_CUSTOM_DIMENSION: %d\n", UA_CUSTOM_DIMENSION);
  printf("UA_CUSTOM_METRIC: %d\n", UA_CUSTOM_METRIC);
  printf("UA_MAX_FIELD_INDEX: %d\n", UA_MAX_FIELD_INDEX);
  printf("UA_MAX_CUSTOM_DIMENSION: %d\n", UA_MAX_CUSTOM_DIMENSION);
  printf("UA_MAX_CUSTOM_METRIC: %d\n", UA_MAX_CUSTOM_METRIC);
  printf("UA_START_CDIMENSIONS: %d\n", UA_START_CDIMENSIONS);
  printf("UA_START_CMETRICS: %d\n", UA_START_CMETRICS);

  Tracker* mytracker = createTracker("UA-XXXXX-Y", "abc", "123");
 
  TrackingOptions opts = {{
      {UA_CUSTOM_DIMENSION, 5, "mydimension"},
      {UA_DOCUMENT_PATH, 0, "/virtual/test"}
  }};

 
  setTrackerOptions(mytracker, &opts);

  sendTracking(mytracker, UA_PAGEVIEW, NULL);

  removeTracker(mytracker);

  return 0;
}


/* vim: set nowrap tabstop=2 shiftwidth=2 softtabstop=0 expandtab textwidth=0 filetype=c foldmethod=syntax foldcolumn=4*/
