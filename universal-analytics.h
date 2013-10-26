
#include <stdlib.h>
#include <curl/curl.h>

#define UA_ENDPOINT "https://www.google-analytics.com/collect"
#define USER_AGENT_STRING "Analytics Pros - Universal Analytics for C"


#define UA_MAX_TYPES 8
#define UA_MAX_FIELD_INDEX 57
#define UA_MAX_CUSTOM_DIMENSION 200
#define UA_MAX_CUSTOM_METRIC 200
#define UA_START_CDIMENSIONS 57 
#define UA_START_CMETRICS 257 
#define UA_MAX_PARAMETERS (UA_MAX_FIELD_INDEX + UA_MAX_CUSTOM_DIMENSION + UA_MAX_CUSTOM_METRIC)
#define UA_MAX_QUERY_LEN 4096
#define UA_STRING_STASH_LEN 2048
#define UA_CUSTOM_PARAM_LEN 6
#define UA_CUSTOM_PARAM_BUFFER ((UA_MAX_CUSTOM_DIMENSION + UA_MAX_CUSTOM_METRIC) * UA_CUSTOM_PARAM_LEN)

/* Tracking types 
 * These signify pageviews, events, transactions, etc.
 * Some behaviors (e.g. required parameters) may be altered by
 * this option (in future versions).
 */
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

/* Tracking fields
 * These represent named parameters on the resulting URL query
 * sent to Google Analytics servers. They act as indices into 
 * the array of parameter nodes for URL composition.
 */
typedef enum trackingField {
  UA_TRACKING_TYPE = 0,
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


/* Name/Value pair with slot ID for URL composition */
typedef struct UAParameter_t {
  trackingField_t field;
  int slot_id;
  char* name;
  char* value;
} UAParameter_t;

/* Flag to specify which level of tracker state to update */
typedef enum stateScopeFlag_t {
  UA_PERMANENT,
  UA_EPHEMERAL
} stateScopeFlag_t;


/* Callback types */
typedef void* (*UAGenericCallback)(void*);
typedef int (*UAEventCallback)(char*, void*);
typedef int (*UAHTTPPOSTProcessor)(char*, char*, char*);
typedef char* (*UAURLEncoder)(char*);


/* Tracker layout intended to maximize stack allocation and
 * minimize heap/dynamic allocation */
typedef struct UATracker_t {
  int __configured__;

  /* State maps for the tracker's resulting parameter values */
  struct UAParameter_t lifetime_parameters[ UA_MAX_PARAMETERS ];
  struct UAParameter_t ephemeral_parameters[ UA_MAX_PARAMETERS ];
  
  /* Custom parameter names (e.g. cm1, cm199, cd1, cd199).
   * These are dynamically populated during tracker initialization 
   * and linked in pointer-arrays below; this merely allocates
   * space (statically) as a component of the tracker */
  char map_custom[ UA_CUSTOM_PARAM_BUFFER ];
  
  /* Standard parameter names (e.g. cid, tid, ea, ec...)
   * These are populated in stack space by |populateParameterNames|
   * during tracker initialization */
  char* map_parameters[ UA_MAX_PARAMETERS ];

  /* Standard tracking type names (e.g. pageview, event, etc) 
   * These are populated in stack space by |populateTypeNames|
   * during tracker initialization */
  char* map_types[ UA_MAX_TYPES ];


  /* Stash space for the query strings generated through |sendTracking| */
  char query[ UA_MAX_QUERY_LEN ];
  int query_len;
  
  /* Pointer for HTTP handler */
  CURL* curl;

} UATracker_t;


/* Field/Value pairs with slot ID for convenient static specification */
typedef struct UAOptionNode_t {
  trackingField_t field; 
  int slot_id;
  char* value;
} UAOptionNode_t;

/* List of options (field/value pairs with slot ID */
typedef struct UAOptions_t {
  struct UAOptionNode_t options[ UA_MAX_PARAMETERS ];
} UAOptions_t;



/* Other shortcuts for external interfaces */
typedef UATracker_t* UATracker;
typedef UAParameter_t UAParameter;
typedef UAOptions_t UASettings;
typedef UAOptions_t UAOptions;


/* Creates Google Analytics tracker state objects */
UATracker createTracker(char* trackingId, char* clientId, char* userId);

void initTracker(UATracker, char* trackingId, char* clientId, char* userId);

/* Stores option-values for the life of the tracker */
void setTrackingOptions(UATracker state, UAOptions_t* opts);

/* Processes tracker state with a tracking type and ephemeral options
 * Dispatches resulting query to Google Analytics */
void sendTracking(UATracker state, trackingType_t type, UAOptions_t* opts);

/* Clears tracker memory & free()s all allocated heap space */
void removeTracker(UATracker);

void cleanTracker(UATracker);


int encodeURIComponent(char* input, char* output, int input_len, int add_null);



