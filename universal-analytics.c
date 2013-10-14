/* Universal Analytics for C 
 * Copyright 2013, Analytics Pros.
 *
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <math.h>
#include <curl/curl.h>
#include "universal-analytics.h"

#define USER_AGENT_STRING "Analytics Pros - Universal Analytics for C"

typedef enum { false = 0, true } bool;

typedef unsigned int uint;


/* List of parameter names corresponding to our field indexes */
inline char** _baseParameterMap(char** params){
  params[UA_TRACKING_ID] = "tid";
  params[UA_CLIENT_ID] = "cid";
  params[UA_USER_ID] = "uid";
  params[UA_TRACKING_TYPE] = "t";
  params[UA_DOCUMENT_PATH] = "dp";
  params[UA_DOCUMENT_TITLE] = "dt";
  params[UA_DOCUMENT_LOCATION] = "dl";
  params[UA_DOCUMENT_HOSTNAME] = "dh";
  params[UA_DOCUMENT_REFERRER] = "dr";
  params[UA_DOCUMENT_ENCODING] = "de";
  params[UA_QUEUE_TIME_MS] = "qt";
  params[UA_SESSION_CONTROL] = "sc";
  params[UA_CAMPAIGN_NAME] = "cn";
  params[UA_CAMPAIGN_SOURCE] = "cs";
  params[UA_CAMPAIGN_MEDIUM] = "cm";
  params[UA_CAMPAIGN_KEYWORD] = "ck";
  params[UA_CAMPAIGN_CONTENT] = "cc";
  params[UA_CAMPAIGN_ID] = "ci";
  params[UA_SCREEN_RESOLUTION] = "sr";
  params[UA_VIEWPORT_SIZE] = "vp";
  params[UA_SCREEN_COLORS] = "sd";
  params[UA_USER_LANGUAGE] = "ul";
  params[UA_APP_NAME] = "an";
  params[UA_APP_VERSION] = "av";
  params[UA_CONTENT_DESCRIPTION] = "cd";
  params[UA_SCREEN_NAME] = "cd";
  params[UA_EVENT_CATEGORY] = "ec";
  params[UA_EVENT_ACTION] = "ea";
  params[UA_EVENT_LABEL] = "el";
  params[UA_EVENT_VALUE] = "ev";
  params[UA_NON_INTERACTIVE] = "ni";
  params[UA_SOCIAL_ACTION] = "sa";
  params[UA_SOCIAL_NETWORK] = "sn";
  params[UA_SOCIAL_TARGET] = "st";
  params[UA_EXCEPTION_DESCRIPTION] = "exd";
  params[UA_EXCEPTION_FATAL] = "exf";
  params[UA_TRANSACTION_ID] = "ti";
  params[UA_TRANSACTION_AFFILIATION] = "ta";
  params[UA_TRANSACTION_REVENUE] = "tr";
  params[UA_TRANSACTION_SHIPPING] = "ts";
  params[UA_TRANSACTION_TAX] = "tt";
  params[UA_TRANSACTION_CURRENCY] = "cu";
  params[UA_ITEM_CODE ] = "ic" ;
  params[UA_ITEM_NAME] = "in";
  params[UA_ITEM_VARIATION] = "iv";
  params[UA_ITEM_PRICE] = "ip";
  params[UA_ITEM_QUANTITY] = "iq";
  params[UA_TIMING_CATEGORY] = "utc";
  params[UA_TIMING_VARIABLE] = "utv";
  params[UA_TIMING_LABEL] = "utl";
  params[UA_TIMING_TIME] = "utt";
  params[UA_TIMING_DNS] = "dns";
  params[UA_TIMING_PAGE_LOAD] = "pdt";
  params[UA_TIMING_REDIRECT] = "rrt";
  params[UA_TIMING_TCP_CONNECT] = "tcp";
  params[UA_TIMING_SERVER_RESPONSE] = "srt";
  params[UA_VERSION_NUMBER] = "v";
  return params;
}

/* Populate the parameter names list */
inline char** prepareParameterMap(bool with_custom, char** params){
  int i;
  _baseParameterMap(params);

  if(with_custom){
    for(i = 0; i < (UA_MAX_CUSTOM_METRIC); i++){
      params[i + UA_START_CMETRICS] = malloc(6);
      sprintf(params[ i + UA_START_CMETRICS ], "cm%d", i +1);
    }
    for(i = 0; i < (UA_MAX_CUSTOM_DIMENSION); i++){
      params[i + UA_START_CDIMENSIONS] = malloc(6);
      sprintf(params[ i + UA_START_CDIMENSIONS ], "cd%d", i +1);
    }
  }
  return params;
}

/* Clean out dynamically populated names */
inline char** cleanParameterMap(bool with_custom, char** params){
  int i;

  if(with_custom){
    for(i = 0; i < (UA_MAX_CUSTOM_METRIC); i++){
      if(NULL != params[ i + UA_START_CMETRICS ]){
        free(params[ i + UA_START_CMETRICS ]);
        params[ i + UA_START_CMETRICS ] = NULL;
      }
    }
    for(i = 0; i < (UA_MAX_CUSTOM_DIMENSION); i++){
      if(NULL != params[ i + UA_START_CDIMENSIONS ]){
        free(params[ i + UA_START_CDIMENSIONS ]);
        params[ i + UA_START_CDIMENSIONS ] = NULL;
      }
    }
  }
  return params;
}


inline int prepareTrackingTypeMap(char** types){
  types[UA_PAGEVIEW] = "pageview";
  types[UA_APPVIEW] = "appview";
  types[UA_EVENT] = "event";
  types[UA_TRANSACTION] = "trans";
  types[UA_TRANSACTION_ITEM] = "item";
  types[UA_TIMING] = "timing";
  types[UA_SOCIAL] = "social";
  types[UA_EXCEPTION] = "exception";
  return UA_MAX_TYPES;
}

inline int cleanTrackingTypeMap(char** types){
  int i;
  for(i = 0; i < UA_MAX_TYPES; i++){
    /* if(NULL != types[i])  free(types[ i ]); */
    types[i] = NULL;
  }
  return i;
}


/* Initialize a state layer/node */
TrackingData* initTrackingState(TrackingData* state, char* trackingId, char* clientId, char* userId){
  int i;
  assert(trackingId == NULL || strlen(trackingId) > 0);
  assert(clientId == NULL || strlen(clientId) > 0);
  assert(userId == NULL || strlen(userId) > 0);
  
  (*state).parent = NULL;
  (*state).child = NULL;
  (*state).options = malloc(sizeof(TrackingOptions));

  memset((*state).options, 0, sizeof(TrackingOptions));
  
  (*state).options[ UA_TRACKING_ID ].option = UA_TRACKING_ID;
  (*state).options[ UA_TRACKING_ID ].value = trackingId;
  (*state).options[ UA_CLIENT_ID ].option = UA_CLIENT_ID;
  (*state).options[ UA_CLIENT_ID ].value = clientId;
  (*state).options[ UA_USER_ID ].option = UA_USER_ID;
  (*state).options[ UA_USER_ID ].value = userId;
  (*state).options[ UA_VERSION_NUMBER ].option = UA_VERSION_NUMBER;
  (*state).options[ UA_VERSION_NUMBER ].value = "1";

  
  return state;
}

/* Create a tracker object */
Tracker* createTracker(char* trackingId, char* clientId, char* userId){
  Tracker* tracker = malloc(sizeof(Tracker));
  TrackingData* state = malloc(sizeof(TrackingData));
  
  initTrackingState(state, trackingId, clientId, userId);
  prepareParameterMap(true, (*tracker).ParameterMap);
  prepareTrackingTypeMap((*tracker).TrackingTypes);

  return tracker; 
}

/* Remove a node from the state list */
TrackingData* removeTrackingState(TrackingData* state){
  int i;
  TrackingData* parent = (*state).parent;
  TrackingData* child = (*state).child;
  TrackingData* _return = NULL;

  (*state).child = NULL;
  (*state).parent = NULL;

  if(NULL != parent){
    (*parent).child = child;
    _return = parent;
  }

  if(NULL != child){
    (*child).parent = parent;
    if(NULL == _return){
      _return = child;
    }
  }

  if(NULL != (*state).options){
    memset((*state).options, 0, sizeof(TrackingOptions));
    free((*state).options);
  }
    
  return _return;
}

int removeTracker(Tracker* tracker){
  TrackingData* state = (*tracker).state;
  while(NULL != state){
    state = removeTrackingState(state);
  }
  cleanParameterMap(true, (*tracker).ParameterMap);
  cleanTrackingTypeMap((*tracker).TrackingTypes);
  memset(tracker, 0, sizeof(Tracker));
  free(tracker);
  return 0;
}

TrackingData* addStateLayer(Tracker* tracker){
  TrackingData* current = malloc(sizeof(TrackingData));
  TrackingData* ptr = (*tracker).state;
  while(NULL != (*ptr).child){
    ptr = (*ptr).child; /* find the tail of the stack */
  }

  (*ptr).child = current;
  (*current).parent = ptr;
  (*tracker).state = current;
  return current;
}

inline int mapFieldID(trackingField_t field, int slot_id){
  switch(field){
    case UA_CUSTOM_DIMENSION:
      return UA_START_CDIMENSIONS + (slot_id >0 ? (slot_id -1) : 0);
    case UA_CUSTOM_METRIC:
      return UA_START_CMETRICS + (slot_id >0 ? (slot_id -1) : 0);
    default:
      return field;
  }
}


int setTrackerOption(Tracker* tracker, trackingField_t field, int slot_id, char* value){
  bool printstat = true;

  /* shorcut */
  TrackingData* state = (*tracker).state;
  
  int field_seek = mapFieldID(field, slot_id);
  assert(field_seek < UA_MAX_PARAMETERS && field_seek >= 0);

  if(printstat)
    printf("Setting field %d (%d, %d)... %s\n", field_seek, field, slot_id, value); 
  
  if(NULL != value){
    (*state).Fields[ (unsigned int) field ] = value;
  }
  return 0;
}

int setTrackerOptions(Tracker* tracker, TrackingOptions* optlist){
  int i;
  int total_set = 0;
  TrackingData* state = addStateLayer(tracker);
  
  int slot_id;
  char* value;
  trackingField_t field;

  TrackingOption* opts = ((*optlist).options);

  for(i = 0; i < UA_MAX_PARAMETERS; i++){
    field = ((opts[i])).pair.option;
    slot_id = ((opts[i])).pair.slot_id;
    value = ((opts[i])).pair.value;
    if(NULL != value){
      if(0 == setTrackerOption(tracker, field, slot_id, value)) total_set++;
    }
  }
  return total_set;
}


/* Return the current state of a tracker's value (reading through state descent) */
char* getTrackerOption(Tracker* tracker, trackingField_t field, int slot_id){
  bool printstat = true; 
  char* current;
  
  /* shorcut */
  TrackingData* state = (*tracker).state;
  
  
  int field_seek = mapFieldID(field, slot_id);
  assert(field_seek < UA_MAX_PARAMETERS && field_seek >= 0);


  while(NULL != state){
  
    current = (*state).Fields[ field_seek ];
    if(NULL != current){
      if(printstat) printf("Retrieving field %d (%d, %d)... %s\n", field_seek, field, slot_id, current); 
      return current;
    }
    state = (*state).parent;
  }

  return NULL; /* not found */
}

inline void linkParameter(UAParameterList* outlist, int index, char* name, char* value){
  (*outlist).params[ index ].name = name;
  (*outlist).params[ index ].value = value;
}

/* Produce an array of strings (or NULL) consistent with our parameter-name map */
int getTrackerParameters(Tracker* tracker, UAParameterList* outlist){
  int i;
  int field = 0;
  int slot_id  = 0;
  int t = 0;
  bool printstat = false;
  char* current;
  char** params = (*tracker).ParameterMap;

  for(i = 0; i < UA_MAX_FIELD_INDEX; i++){
    current = getTrackerOption(tracker, i, 0);
    if(NULL != current){
      linkParameter(outlist, t++, params[ i ], current);
    }
  }
  
  for(i = 0; i < UA_MAX_CUSTOM_DIMENSION; i++){
    current = getTrackerOption(tracker, UA_CUSTOM_DIMENSION, i +1);
    if(NULL != current){
      linkParameter(outlist, t++, params[ UA_START_CDIMENSIONS + i ], current);
    }
  }

  for(i = 0; i < UA_MAX_CUSTOM_METRIC; i++){
    current = getTrackerOption(tracker, UA_CUSTOM_METRIC, i +1);
    if(NULL != current){
      linkParameter(outlist, t++, params[ UA_START_CMETRICS + i ], current);
    }
  }


  return t;
}





int printTrackerState(Tracker* tracker){
  int howmany;
  struct UAParameterList parameters;
  howmany = getTrackerParameters(tracker, &parameters);

  int i;
  for(i = 0; i < howmany; i++){
    printf("- %s\t= %s\n", parameters.params[i].name, parameters.params[i].value);
  }

  return 0;
}


/* Unset an option from current state (or globally, if specificed) 
 * NOTE that this DOE NOT free() the strings, since currently we're using 
 * pointers exclusively.
 */
int unsetOption(TrackingData* state, trackingField_t field, int slot_id, bool globally){
  
        
  int offset = (field == UA_CUSTOM_DIMENSION 
      ? UA_START_CDIMENSIONS + (slot_id -1)
      : (field == UA_CUSTOM_METRIC 
        ? UA_START_CMETRICS + (slot_id -1) 
        : 0
     ));

  int field_seek = field + offset;
  assert(field_seek < UA_MAX_PARAMETERS && field >= 0);
  
  
  if(NULL != state){
    (*state).Fields[ field_seek ] = NULL;
    if(globally){
      unsetOption((*state).parent, field, slot_id, globally);
      unsetOption((*state).child, field, slot_id, globally);
    }
  }
}

int sendTracking(Tracker* tracker, trackingType_t type, TrackingOptions* data){
  int i;
  int v;
  int c = 1;

  /* Set up temporary state for this hit */
  TrackingData* state = addStateLayer(tracker);
  setTrackerOption(tracker, UA_TRACKING_TYPE, 0, (*tracker).TrackingTypes[ type ]);


  if(NULL != data){
    /* Add temporary state for options passed */
    setTrackerOptions(tracker, data);
    c++;
  }

  v = printTrackerState(tracker);

  /* Remove temporary state */
  while(c--) removeTrackingState((*tracker).state);

  return v;
}


