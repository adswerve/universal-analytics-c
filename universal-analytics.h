/* Universal Analytics for C 
 * Copyright 2013, Analytics Pros.
 *
 */

#define UA_MAX_FIELD_INDEX 55
#define UA_MAX_CUSTOM_DIMENSION 200
#define UA_MAX_CUSTOM_METRIC 200
#define UA_MAX_QUERY_LEN 4096
#define UA_MAX_TYPES 8
#define UA_MAX_PARAMETERS (UA_MAX_FIELD_INDEX + UA_MAX_CUSTOM_DIMENSION + UA_MAX_CUSTOM_METRIC)
#define UA_START_CDIMENSIONS (UA_MAX_FIELD_INDEX + 1)
#define UA_START_CMETRICS (UA_MAX_FIELD_INDEX + UA_MAX_CUSTOM_DIMENSION +1)


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
  UA_TRACKING_ID, /* string like UA-XXXXX-Y */
  UA_CLIENT_ID,
  UA_USER_ID,
  UA_TRACKING_TYPE,
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
  UA_VERSION_NUMBER,
  UA_CUSTOM_DIMENSION,
  UA_CUSTOM_METRIC
} trackingField_t;

/* Pair name-value, with slot ID for custom dimensions/metrics */
typedef struct TrackingOptionNode {
  struct OptionPair {
    trackingField_t option;
    int slot_id;
    char* value;
  } pair;
} TrackingOption;

/* A set of multiple options (mapped like a dictionary) */
typedef struct TrackingOptions {
  struct TrackingOptionNode options[UA_MAX_PARAMETERS];
} TrackingOptions;


/* State map for tracker configuration and hit settings */
typedef struct TrackingData {
  struct TrackingOptions * options; 
  struct TrackingData * parent;
  struct TrackingData * child;
} TrackingData;

typedef struct Tracker {
  int   firecount;
  struct TrackingData*  state;
  char* ParameterMap[ UA_MAX_PARAMETERS ];
  char* TrackingTypes[ UA_MAX_TYPES ];
} Tracker;

/* Pair of strings for URL parameter mapping */
typedef struct UAParameter {
  int id;
  char* name;
  char* value;
} UAParameter;

typedef struct UAParameterList {
  struct UAParameter params[UA_MAX_PARAMETERS];
} UAParameterList;

/* Initializes tracker objects */
Tracker* createTracker(char* trackingId, char* clientId, char* userId);

int setTrackerOptions(Tracker* tracker, TrackingOptions* optlist);
int setTrackerOption(Tracker* tracker, trackingField_t field, int slot_id, char* value);

int sendTracking(Tracker* tracker, trackingType_t type, TrackingOptions* data);

int printTrackerState(Tracker* tracker);
int printTrackingOptions(TrackingOptions optlist);
