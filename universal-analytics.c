
#include <assert.h>
#include <string.h>

#include "universal-analytics.h"

#define UA_MEM_MAGIC_UNSET 0xDEADC0DE
#define UA_MEM_MAGIC_CONFIG 0xADDED00
#define UA_MEMSIZE_PARAM (2 * UA_MAX_PARAMETERS * sizeof(UAParameter_t))
#define UA_MEMSIZE_STATE (10 * sizeof(UATracker_t))
#define UA_MAX_MEMORY (UA_MEMSIZE_PARAM + UA_MEMSIZE_STATE)



/* Copies the input string to output with proper encoding.
 * Allows for populating substrings (i.e. ranges of memory) in existing string buffers.
 */
int encodeURIComponent(char input[], char output[], int input_len, int add_null){
  int i, j = 0;
  char** cur = & output;
  static char hex[] = "0123456789abcdef";
  for(i = 0; i < input_len; i++){
    if(isalnum(input[i]) || input[i] == '-' || input[i] == '.' || input[i] == '~'){
      output[j++] = input[i];
    } else if(input[i] == ' '){
      output[j++] = '+';
    } else {
      output[j++] = '%';
      output[j++] = hex[ (int) (input[i] >> 4) & 15];
      output[j++] = hex[ (int) (input[i] & 15) & 15];
    }
  }

  /* Null termination is optional, to permit sequential calls 
   * of this method against multiple parameters into a single
   * output string */
  if(add_null) 
    output[j++] = '\0';
  
  return j; /* result length */
}

/* Define tracking type strings */
inline int populateTypeNames(char* types[]){
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



/* List of parameter names (strings) corresponding to our field indexes */
inline void populateParameterNames(char* params[], char* custom_params){
  int i, j;
  char* cur;
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
 
  /* Populate dimension space */
  for(i = 0; i < UA_MAX_CUSTOM_DIMENSION; i++){
    cur = (char*) (custom_params + (i * UA_CUSTOM_PARAM_LEN));
    sprintf(cur, "cd%d", i + 1);
    params[ i + UA_START_CDIMENSIONS ] = cur; /* link parameter name */
  }

  /* Populate metric space */
  for(j = 0; j < UA_MAX_CUSTOM_METRIC; j++){
    cur = (char*) (custom_params + ((i + j) * UA_CUSTOM_PARAM_LEN));
    sprintf(cur, "cm%d", j + 1);
    params[ j + UA_START_CDIMENSIONS ] = cur; /* link parameter name */
  }
  
}


/* Retrieve a field name (pointer) by its ID 
 * (and appropriate offset for custom parameters */
inline char* getOptionName(char* field_names[], trackingField_t field, int slot_id){
  switch(field){
    case UA_CUSTOM_METRIC: 
      return field_names[ UA_START_CMETRICS + slot_id - 1 ];
    case UA_CUSTOM_DIMENSION:
      return field_names[ UA_START_CDIMENSIONS + slot_id - 1 ];
    default:
      return field_names[ field ];
  }
}

/* Retrieve the tracking-type parameter name (pointer) */
inline char* getTrackingType(UATracker_t* tracker, trackingType_t type){
  assert(NULL != tracker);
  assert((*tracker).__configured__ == UA_MEM_MAGIC_CONFIG);

  return tracker->map_types[ type ];
}

/* Void all memory allocated to tracking parameters (pointers) */
inline void initParameterState(UAParameter_t params[], int howmany){
  memset(params, 0, howmany * (sizeof(UAParameter_t)));
}

/* Void a tracker's memory */
void cleanTracker(UATracker_t* tracker){
  assert(NULL != tracker);
  if(NULL != tracker->curl){
    curl_easy_cleanup(tracker->curl);
  }
  memset(tracker, 0, sizeof(UATracker_t)); 
}


/* Clean out ephemeral state & query cache */
inline void resetQuery(UATracker_t* tracker){
  initParameterState(tracker->ephemeral_parameters, UA_MAX_PARAMETERS);
  memset(tracker->query, 0, UA_MAX_QUERY_LEN);
  tracker->query_len = 0;
}






/* Define a single parameter's name/value/slot */
inline void setParameter(char* field_names[], UAParameter_t params[], trackingField_t field, int slot_id, char* value){
  params[ field ].name = getOptionName(field_names, field, slot_id);
  params[ field ].value = value;
  params[ field ].slot_id = slot_id;
}

/* Populate several parameters (pointers) given a set of options */
inline void setParameterList(char* field_names[], UAParameter_t params[], UAOptionNode_t options[], int howmany){
  int i, field, slot_id;
  for(i = 0; i < howmany; i++){
    if(options[i].field < 1) continue;
    field = options[i].field;
    slot_id = options[i].slot_id;
    params[ field ].field = field;
    setParameter(field_names, params, field, slot_id, options[i].value);
  }
}


/* Populate several lifetime/permanent or temporary/ephemeral values based on scope */
inline void setOptionParameterList(UATracker_t* tracker, stateScopeFlag_t flag, UAOptionNode_t options[]){
  assert(NULL != tracker);
  assert((*tracker).__configured__ == UA_MEM_MAGIC_CONFIG);
  switch(flag){
    case UA_PERMANENT: 
      setParameterList(tracker->map_parameters, tracker->lifetime_parameters, options, UA_MAX_PARAMETERS);
      break;
    case UA_EPHEMERAL:
      setParameterList(tracker->map_parameters, tracker->ephemeral_parameters, options, UA_MAX_PARAMETERS);
      break;
  }
}


/* Populate a single lifetime/permanent or temporary/ephemeral value based on scope */
inline void setOptionParameter(UATracker_t* tracker, stateScopeFlag_t flag, trackingField_t field, int slot_id, char* value ){
  assert(NULL != tracker);
  assert((*tracker).__configured__ == UA_MEM_MAGIC_CONFIG);
  switch(flag){
    case UA_PERMANENT: 
      setParameter(tracker->map_parameters, tracker->lifetime_parameters, field, slot_id, value);
      break;
    case UA_EPHEMERAL:
      setParameter(tracker->map_parameters, tracker->ephemeral_parameters, field, slot_id, value);
      break;
  }
}

/* Set up an already-allocated tracker
 *  - Clear out the whole tracker space
 *  - Populate parameter names
 *  - Define lifetime tracker values
 */
void initTracker(UATracker_t* tracker, char* trackingId, char* clientId, char* userId){
  assert(NULL != tracker);
  cleanTracker(tracker);
  
  (*tracker).__configured__ = UA_MEM_MAGIC_CONFIG;

  populateTypeNames(tracker->map_types);
  populateParameterNames(tracker->map_parameters, tracker->map_custom);

  memset(& tracker->query, 0, UA_MAX_QUERY_LEN);

  setParameter(tracker->map_parameters, tracker->lifetime_parameters, UA_VERSION_NUMBER, 0, "1");
  setParameter(tracker->map_parameters, tracker->lifetime_parameters, UA_TRACKING_ID, 0, trackingId);
  setParameter(tracker->map_parameters, tracker->lifetime_parameters, UA_CLIENT_ID, 0, clientId);
  setParameter(tracker->map_parameters, tracker->lifetime_parameters, UA_USER_ID, 0, userId);

}

/* Allocate space for a tracker & initialize it */
UATracker_t* createTracker(char* trackingId, char* clientId, char* userId){
  UATracker_t* new_tracker = malloc(sizeof(UATracker_t));
  initTracker(new_tracker, trackingId, clientId, userId);
  new_tracker->curl = curl_easy_init(); 
  return new_tracker;
}


/* Clear and de-allocate a tracker */
void removeTracker(UATracker_t* tracker){
  assert(NULL != tracker);
  cleanTracker(tracker);
  free(tracker);
}

/* Wrapper: set up lifetime options on a tracker */
void setTrackingOptions(UATracker_t* tracker, UAOptions_t* opts){
  setOptionParameterList(tracker, UA_PERMANENT, opts->options);
}

/* Wrapper: set up a single lifetime option on a tracker */
void setTrackingOption(UATracker_t* tracker, trackingField_t field, int slot_id, char* value){
  setOptionParameter(tracker, UA_PERMANENT, field, slot_id, value);
}

/* Retrieve name and value for a given index (transcending ephemeral state to lifetime, if needed) */
void getCurrentParameterValue(UATracker_t* tracker, int index, char** name, char** value){
  assert(NULL != tracker);
  
  (*name) = tracker->ephemeral_parameters[ index ].name;
  (*value) = tracker->ephemeral_parameters[ index ].value;
  if(NULL == (*name) || NULL == (*value)){
    (*name) = tracker->lifetime_parameters[ index ].name;
    (*value) = tracker->lifetime_parameters[ index ].value;
  }
}

/* Construct a query-string based on tracker state */
int assembleQueryString(UATracker_t* tracker, char* query, int offset){
  int i;
  char* name;
  char* value;
  int name_len;
  int value_len;

  for(i = 0; i < UA_MAX_PARAMETERS; i++){
    
    getCurrentParameterValue(tracker, i, & name, & value);
    if(NULL == name || NULL == value)  continue;

    name_len = strlen(name);
    value_len = strlen(value);

    if(i > 0){
      strncpy(query + offset, "&", 1);
      offset++;
    }

    strncpy(query + offset, name, name_len);
    strncpy(query + offset + name_len, "=", 1);

    /* Fill in the encoded values */
    value_len = encodeURIComponent(value, query + offset + name_len + 1, value_len, 0);
    
    offset += (name_len + value_len + 1);
  }
  return offset; 
}

/* Data handler for CURL to silence it's default output */
size_t curl_null_data_handler(char *ptr, size_t size, size_t nmemb, void *userdata){
  return (nmemb * size);
}

/* Process a POST using CURL */
int postQuery(CURL* curl, char* query){
  assert(NULL != curl);
  assert(NULL != query);

  CURLcode res;
  int result;
  curl_easy_setopt(curl, CURLOPT_URL, UA_ENDPOINT);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, query);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_null_data_handler);
  res = curl_easy_perform(curl);
  if(res != CURLE_OK){
    result = (int) res;
  } else {
    result = 0;
  }
  return result;
}


/* Assemble a query from a tracker and send it through CURL */
void queueTracking(UATracker_t* tracker){
  assert(NULL != tracker);
  assert((*tracker).__configured__ == UA_MEM_MAGIC_CONFIG);

  int query_len;
  char* query = tracker->query;
  memset(query, 0, UA_MAX_QUERY_LEN);
  query_len = assembleQueryString(tracker, query, 0);
  postQuery(tracker->curl, query); 

}

/* Prepare ephemeral state on a tracker and dispatch its query */
void sendTracking(UATracker_t* tracker, trackingType_t type, UAOptions_t* opts){
  assert(NULL != tracker);
  assert((*tracker).__configured__ == UA_MEM_MAGIC_CONFIG);


  if(NULL != opts){
    setOptionParameterList(tracker, UA_EPHEMERAL, opts->options);
  }

  setOptionParameter(tracker, 
      UA_EPHEMERAL, UA_TRACKING_TYPE, 0, 
      getTrackingType(tracker, type)
  );

  queueTracking(tracker);
  resetQuery(tracker); 

  printf("Tracking sent\n");
}


