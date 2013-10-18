#include <stdlib.h>
#include "universal-analytics.h"



int main(int argc, char** argv){
   
  UASettings settings = {{
      {UA_CUSTOM_DIMENSION, 5, "C library"},
      {UA_DOCUMENT_PATH, 0, "/virtual/test/C"},
      {UA_DOCUMENT_TITLE, 0, "This is a test in C"}
  }};


  UATracker tracker = createTracker("UA-XXXXX-Y", "abc1234", NULL);

  /* store these options permanently */
  setTrackingOptions(tracker, & settings); 

  sendTracking(tracker, UA_PAGEVIEW, NULL);
  
  
  UAOptions opts = {{
    {UA_EVENT_CATEGORY, 0, "Event Category"},
    {UA_EVENT_ACTION, 0, "Event Action (C)"},
    {UA_EVENT_LABEL, 0, "Event Label"}
  }};

  /* send tracking with transcient options */
  sendTracking(tracker, UA_EVENT, & opts); 

  removeTracker(tracker);


  return 0;
}
