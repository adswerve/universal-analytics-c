#include <string.h>
#include <stdlib.h>
#include "universal-analytics.h"

char* urlencode(char* value){
  return value;
}

/* Assembles &<name>=<value> for URL/query construction */
inline int _parameter(char* destination, int offset, char* Properties[], int position, char** parameter_map){
  char* value = urlencode(Properties[ position ]);
  int temp; /*  = strlen(parameter_map[ position ]) + strlen(value) + 2; */
  int prior = offset;
  strncat(destination + (offset++), "&", 1);
  temp = strlen(parameter_map[ position ]);
  strncat(destination + prior, parameter_map[ position ], temp);
  prior = prior + 1 + temp;
  strncat(destination + prior, "=", 1);
  temp = strlen(value);
  prior = prior + 1 + temp;
  strncat(destination + prior, value, temp);
  return prior; 
}

/* Assembles a set of parameters for a query */
inline int _parameter_group(char* destination, char** Properties, int propmax, int start_len, char** parameter_map){
  int i;
  int new_len = start_len;
  for(i = 0; i < propmax; i++){
    new_len = _parameter(destination, new_len, Properties, i, parameter_map);
  }
  return new_len;
} 

/* Constructs a query string based on tracker state */
char* QueryString(TrackingData* state, char** parameter_map){
  int temp;
  char* result = malloc(UA_MAX_QUERY_LEN);
  memset(result, 0, UA_MAX_QUERY_LEN); /* init our output string */
  
  temp = _parameter_group(result, (*state).Fields, UA_MAX_FIELD_INDEX, 0, parameter_map);
  temp = _parameter_group(result, (*state).Dimensions, UA_MAX_CUSTOM_DIMENSION, temp, parameter_map);
  temp = _parameter_group(result, (*state).Metrics, UA_MAX_CUSTOM_METRIC, temp, parameter_map);
  
  return result;
}


int join_array(char* output, char** values, char* join_char, int howmany){
  int i = 0;
  int maxlen = 0;
  int temp;
  /* calculate max length */
  for(i = 0; i < howmany; i++){
    maxlen += values[i]
    temp = strlen(values[i]);
    strncat(output + len, values[i], temp);
    temp = strlen(join_char);
    strncat(
    len += temp;
  }
  return maxlen;
}

/* vim: set nowrap tabstop=2 shiftwidth=2 softtabstop=0 expandtab textwidth=0 filetype=c foldmethod=syntax foldcolumn=4*/
