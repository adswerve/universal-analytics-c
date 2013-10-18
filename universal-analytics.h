
#include <stdlib.h>
#include <curl/curl.h>

#define UA_ENDPOINT "https://www.google-analytics.com/collect"
#define USER_AGENT_STRING "Analytics Pros - Universal Analytics for C"


#define UA_MAX_FIELD_INDEX 57
#define UA_MAX_CUSTOM_DIMENSION 200
#define UA_MAX_CUSTOM_METRIC 200
#define UA_MAX_QUERY_LEN 4096
#define UA_MAX_TYPES 8
#define UA_MAX_PARAMETERS 457 
#define UA_START_CDIMENSIONS 57 
#define UA_START_CMETRICS 257 


typedef enum trackingType {
  UA_PAGEVIEW,
  UA_APPVIEW,
  UA_EVENT,
  UA_TRANSACTION,
  UA_TRANSACTION_ITEM,
  UA_TIMING,
  UA_SOCIAL,
  UA_EXCEPTION
} trackingType_t;

typedef enum trackingField {
  UA_TRACKING_TYPE,
  UA_VERSION_NUMBER,
  UA_TRACKING_ID, /* string like UA-XXXXX-Y */
  UA_CLIENT_ID,
  UA_USER_ID,
  UA_DOCUMENT_PATH,
  UA_DOCUMENT_TITLE,
  UA_DOCUMENT_LOCATION,
  UA_DOCUMENT_HOSTNAME,
  UA_DOCUMENT_REFERRER,
  UA_DOCUMENT_ENCODING,
  UA_QUEUE_TIME_MS,
  UA_SESSION_CONTROL,
  UA_CAMPAIGN_NAME,
  UA_CAMPAIGN_SOURCE,
  UA_CAMPAIGN_MEDIUM,
  UA_CAMPAIGN_KEYWORD,
  UA_CAMPAIGN_CONTENT,
  UA_CAMPAIGN_ID,
  UA_SCREEN_RESOLUTION,
  UA_VIEWPORT_SIZE,
  UA_SCREEN_COLORS,
  UA_USER_LANGUAGE,
  UA_APP_NAME,
  UA_APP_VERSION,
  UA_CONTENT_DESCRIPTION,
  UA_SCREEN_NAME,
  UA_EVENT_CATEGORY,
  UA_EVENT_ACTION,
  UA_EVENT_LABEL,
  UA_EVENT_VALUE,
  UA_NON_INTERACTIVE,
  UA_SOCIAL_ACTION,
  UA_SOCIAL_NETWORK,
  UA_SOCIAL_TARGET,
  UA_EXCEPTION_DESCRIPTION,
  UA_EXCEPTION_FATAL,
  UA_TRANSACTION_ID,
  UA_TRANSACTION_AFFILIATION,
  UA_TRANSACTION_REVENUE,
  UA_TRANSACTION_SHIPPING,
  UA_TRANSACTION_TAX,
  UA_TRANSACTION_CURRENCY,
  UA_ITEM_CODE, 
  UA_ITEM_NAME,
  UA_ITEM_VARIATION,
  UA_ITEM_PRICE,
  UA_ITEM_QUANTITY,
  UA_TIMING_CATEGORY,
  UA_TIMING_VARIABLE,
  UA_TIMING_LABEL,
  UA_TIMING_TIME,
  UA_TIMING_DNS,
  UA_TIMING_PAGE_LOAD,
  UA_TIMING_REDIRECT,
  UA_TIMING_TCP_CONNECT,
  UA_TIMING_SERVER_RESPONSE,
  UA_CUSTOM_DIMENSION,
  UA_CUSTOM_METRIC
} trackingField_t;

typedef struct UAParameter_t {
  trackingField_t field;
  int slot_id;
  char* name;
  char* value;
} UAParameter_t;

typedef struct UAState_t {
  struct UAState_t* parent;
  struct UAState_t* child;
  char* parameter_map[ UA_MAX_PARAMETERS ];
  char* trackingtype_map[ UA_MAX_TYPES ];
  struct UAParameter_t parameters[ UA_MAX_PARAMETERS ]; 
} UAState_t;


typedef struct UAOptionNode_t {
  trackingField_t field; 
  int slot_id;
  char* value;
} UAOptionNode_t;

typedef struct UAOptions_t {
  struct UAOptionNode_t options[ UA_MAX_PARAMETERS ];
} UAOptions_t;

typedef struct UAQueryPending_t {
  CURL* curl;
  char querystring[ UA_MAX_QUERY_LEN ];
  int query_len;
} UAQueryPending_t;


/* Other shortcuts for external interfaces */
typedef UAState_t* UAState;
typedef UAState_t* UATracker;
typedef UAParameter_t UAParameter;
typedef UAOptions_t UASettings;
typedef UAOptions_t UAOptions;


/* Creates Google Analytics tracker state objects */
UATracker createTracker(char* trackingId, char* clientId, char* userId);

/* Stores option-values for the life of the tracker */
UATracker setTrackingOptions(UATracker state, UAOptions_t* opts);

/* Processes tracker state with a tracking type and transcient options
 * Dispatches resulting query to Google Analytics */
int sendTracking(UATracker state, trackingType_t type, UAOptions_t* opts);

/* Clears tracker memory & free()s all allocated heap space */
void removeTracker(UATracker state);




