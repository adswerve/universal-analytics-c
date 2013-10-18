
#include <assert.h>
#include <string.h>

#include "universal-analytics.h"

int getParameterIndex(trackingField_t field, int slot_id){
  switch(field){
    case UA_CUSTOM_DIMENSION: 
      return UA_START_CDIMENSIONS + slot_id -1;
    case UA_CUSTOM_METRIC: 
      return UA_START_CMETRICS + slot_id -1;
    default:
      return field;
  }
}

char* getParameterName(UAState_t* state, trackingField_t field, int slot_id){
  char* temp = malloc(6);
  int position = getParameterIndex(field, slot_id);
  switch(field){
    case UA_CUSTOM_METRIC:
      sprintf(temp, "cm%d", slot_id); break;
    case UA_CUSTOM_DIMENSION:
      sprintf(temp, "cd%d", slot_id); break;
    default:
      strcpy(temp, state->parameter_map[ position ]);
      break;
  }
  return temp;
}

char* customFieldParameter(char* template, int index){
  char* temp = malloc(6);
  sprintf(temp, template, index);
  return temp;
}

/* List of parameter names corresponding to our field indexes */
void buildParameterMap(char* params[]){
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
}

UAParameter_t* getParameterNode(UAState_t* state, trackingField_t field, int slot_id){
  int index = getParameterIndex(field, slot_id);
  return & state->parameters[ index ];
}


UAState_t* setPermanentState(UAState_t* state, trackingField_t field, int slot_id, char* value){
  while(NULL != state->parent){
    state = state->parent; // find root
  }
  
  char* param_name = getParameterName(state, field, slot_id);
  UAParameter_t* param = getParameterNode(state, field, slot_id);

  param->slot_id = slot_id;
  param->name = param_name;
  param->field = field;
  param->value = value;
  return state;
}

UAState_t* setTentativeState(UAState_t* state, trackingField_t field, int slot_id, char* value){
  UAState_t* temp;
  int depth = 0;
  char* param_name = getParameterName(state, field, slot_id);
  UAParameter_t* param;

  while(NULL != state->child){
    state = state->child; // find most recent state
    depth++;
  }
  
  if(NULL == state->parent){ // add a state layer

    temp = malloc(sizeof(UAState_t));
    temp->parent = state;
    temp->child = state->child;
    if(NULL != temp->child){
      temp->child->parent = temp;
    }
    state->child = temp;
    
    memcpy(& temp->parameter_map, & state->parameter_map, UA_MAX_PARAMETERS * sizeof(char*));
    memcpy(& temp->trackingtype_map, & state->trackingtype_map, UA_MAX_TYPES * sizeof(char*));
    state = temp;
  }

  // by now "state" is the top (latest) of the stack
  param = getParameterNode(state, field, slot_id);
  param->slot_id = slot_id;
  param->name = param_name;
  param->field = field;
  param->value = value;
  return state;
}


UAState_t* setTrackingOptions(UAState_t* state, UAOptions_t* opts){
  int i, c = 0;
  UAState_t* temp;
  for(i = 0; i < UA_MAX_PARAMETERS; i++){
    if(NULL != opts->options[i].value){
      temp = setPermanentState(state, opts->options[i].field, opts->options[i].slot_id, opts->options[i].value);
      c++;
    }
  }
  return temp;
}

UAState_t* setTranscientTrackingOptions(UAState_t* state, UAOptions_t* opts){
  int i, c = 0;
  UAState_t* temp = state;

  if(NULL == opts)
    return state;

  for(i = 0; i < UA_MAX_PARAMETERS; i++){
    if(NULL != opts->options[i].value){
      temp = setTentativeState(state, opts->options[i].field, opts->options[i].slot_id, opts->options[i].value);
      c++;
    }
  }
  return temp;
}


UAState_t* removeTentativeState(UAState_t* state){
  int i;
  UAState_t* parent = state->parent;

  
  if(NULL != parent){ /* Unlink this state */
    parent->child = state->child;
    if(NULL != parent->child){
      parent->child->parent = parent;
    }
    state->parent = NULL;
    state->child = NULL;
  }

  for(i = 0; i < UA_MAX_PARAMETERS; i++){
    if(NULL != state->parameters[i].name){
      memset(& state->parameters[i], 0, sizeof(UAParameter_t));
    }
  }

  memset(state, 0, sizeof(UAState_t));
  free(state);
  return parent;
}

inline int prepareTrackingTypeMap(char* types[]){
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

UAState_t* createTracker(char* trackingId, char* clientId, char* userId){
  UAState_t* temp;
  temp = malloc(sizeof(UAState_t));
  temp->parent = NULL;
  temp->child = NULL;

  memset(& temp->parameters, 0, sizeof(UAParameter_t) * UA_MAX_PARAMETERS);

  buildParameterMap(temp->parameter_map);
  prepareTrackingTypeMap(temp->trackingtype_map);
  
  setPermanentState(temp, UA_VERSION_NUMBER, 0, "1");
  setPermanentState(temp, UA_TRACKING_ID, 0, trackingId);
  setPermanentState(temp, UA_CLIENT_ID, 0, clientId);
  setPermanentState(temp, UA_USER_ID, 0, userId);

  return temp;
}


void removeTracker(UAState_t* state){
  UAState_t* temp = state;
  while(NULL != temp){
    temp = removeTentativeState(temp);
  }
}

UAParameter_t* getCurrentParam(UAState_t* state, int index){
  UAState_t* child = state;
  UAParameter_t* param;
  while(NULL != child->child){ // find the tail
    child = child->child;
  }
  while(NULL != child){ // iterate up to head.
    param = & child->parameters[index];
    if(NULL != param->name){
      return param;
    }
    child = child->parent;
  }
  return NULL;
}


int iterateStateProperties(UAState_t* state, int (*callback)(UAParameter_t*, void*), void* data){
  int i, j = 0;
  UAState_t* child = state;
  UAParameter_t* param;

  while(NULL != child->child){ 
    child = child->child; /* find last state */
  }


  for(i = 0; i < UA_MAX_FIELD_INDEX; i++){
    param = getCurrentParam(state, i);
    if(NULL != param){
      if(0 == callback(param, data)){
        j++;
      }
    }
  }

  for(i = 0; i < UA_MAX_CUSTOM_DIMENSION; i++){
    param = getCurrentParam(state, getParameterIndex(UA_CUSTOM_DIMENSION, i));
    if(NULL != param){
      if(0 == callback(param, data)){
        j++;
      }
    }
  }

  for(i = 0; i < UA_MAX_CUSTOM_METRIC; i++){
    param = getCurrentParam(state, getParameterIndex(UA_CUSTOM_METRIC, i));
    if(NULL != param){
      if(0 == callback(param, data)){
        j++;
      }
    }
  }


  return j;
}



void freeParameterSet(UAParameter_t** params){
  memset(params[ UA_TRACKING_TYPE ], 0, sizeof(UAParameter_t));
  free(params[ UA_TRACKING_TYPE ]);
  free(params);
}

int compileQueryString(UAParameter_t* param, void* query_pending){
  UAQueryPending_t* query = (UAQueryPending_t*) query_pending;

  if(NULL == param->name || NULL == param->value) return 1;

  int offset = query->query_len;
  int name_len = strlen(param->name);
  int value_len = strlen(param->value);

  char* basequery = query->querystring;
  char* value_encoded = curl_easy_escape(query->curl, param->value, value_len);
  
  if(0 != offset){
    strncpy(basequery + offset, "&", 1);
    offset++;
  }

  value_len = strlen(value_encoded);

  strncpy(basequery + offset, param->name, name_len);
  strncpy(basequery + offset + name_len, "=", 1);
  strncpy(basequery + offset + name_len + 1, value_encoded, value_len);

  query->query_len = offset + name_len + value_len + 1;

  curl_free(value_encoded);

  return 0;
}


UAQueryPending_t* prepareTrackingQuery(UAState_t* state, trackingType_t type){
  int count;
  
  UAQueryPending_t* query = malloc(sizeof(UAQueryPending_t));
  memset(query, 0, sizeof(UAQueryPending_t));

  query->curl = curl_easy_init();

  setTentativeState(state, UA_TRACKING_TYPE, 0, state->trackingtype_map[ type ]); 
  count = iterateStateProperties(state, compileQueryString, query);

  return query;
}

void freeTrackingQuery(UAQueryPending_t* query){
  curl_easy_cleanup(query->curl);
  memset(query, 0, sizeof(UAQueryPending_t));
  free(query);
}

size_t curl_null_data_handler(char *ptr, size_t size, size_t nmemb, void *userdata){
  return (nmemb * size);
}


int postQuery(CURL* curl, char* query){
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



int sendTracking(UAState_t* state, trackingType_t type, UAOptions_t* opts){
  
  UAState_t* temp = setTranscientTrackingOptions(state, opts);
  UAQueryPending_t* query = prepareTrackingQuery(temp, type);

  printf("Query: %s\n", query->querystring);
  postQuery(query->curl, query->querystring);

  if(NULL != opts) removeTentativeState(temp);
  freeTrackingQuery(query);

  return 0;
}



