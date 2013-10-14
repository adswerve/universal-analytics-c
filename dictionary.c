#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "dictionary.h"

dict_t* dict(int size){
  int i;
  int property_space = size * sizeof(dict_property);
  dict_t* result = malloc(sizeof(dict_t));
  memset(result, 0, sizeof(dict_t));
  (*result).size = size;
  (*result).properties = malloc(property_space); 
  memset((*result).properties, 0, property_space);
  return result;
}

/* retrieve the address of a specific property */
inline dict_property* dict_get_property(dict_t* props, int index){
  return (dict_property*) ((*props).properties + (sizeof(dict_property) * index));
}

inline dict_t* dict_tail(dict_t* props){
  dict_t* ptr = props;
  while(NULL != (*ptr).child){
    ptr = (*ptr).child;
  }
  return ptr;
}

inline dict_t* dict_head(dict_t* props){
  dict_t* ptr = props;
  while(NULL != ptr && NULL != (*ptr).parent){
    ptr = (*ptr).parent;
  }
  return ptr;
}

/* Walk the properties of the whole stack */
int dict_each_property(dict_t* props, dictSignal_t (*callback)(dict_t*, int, dict_property*, int, void*), void* data){
  int i;
  int total = 0;
  dict_t* ptr = dict_head(props);
  while(NULL != ptr){
    for(i = 0; i < (*ptr).size; i++){
      switch(callback(ptr, i, dict_get_property(ptr, i), total++, data)){
        case DICT_EACH_STOP: break;
      }
    }
    ptr = (*ptr).child;
  }
  return total;
}

inline void dict_wipe_property(dict_property* p, int wipeout){
 
  if(NULL != (*p).pair.name){ /* clear the property name */
    memset((*p).pair.name, 0, (*p).pair.name_len);
    free((*p).pair.name);
    (*p).pair.name = NULL;
  }

  
  if(NULL != (*p).pair.value){ /* clear the property value */
    memset((*p).pair.value, 0, (*p).pair.value_len);
    free((*p).pair.value);
    (*p).pair.value = NULL;
  }

  (*p).pair.slot = 0;
  (*p).pair.name_len = 0;
  (*p).pair.value_len = 0;
  
  if(wipeout){ /* deallocate the whole property node */
    memset(p, 0, sizeof(dict_property));
    free(p);
  }

}

/* Wipe and free the memory of a specific property (name-value pair) */
inline void dict_wipe_property_by_index(dict_t* props, int index, int wipeout){
  dict_property* p;
  p = dict_get_property(props, index);
  dict_wipe_property(p, wipeout);
}

/* Wipe out an entire dictionary layer */
inline void dict_free_layer(dict_t* props){
  int i;
  int howmany = (*props).size;
  for(i = 0; i < howmany; i++){
    dict_wipe_property_by_index(props, i, 0);
  }
  memset((*props).properties, 0, howmany * sizeof(dict_property));
  free((*props).properties);
  memset(props, 0, sizeof(dict_t));
  free(props);
}

/* Separate a dictionary layer from its stack */
dict_t* dict_unlink(dict_t* child){
  dict_t* temp = NULL;
  if(NULL != (*child).parent){
    temp = (*child).parent;
    (*(*child).parent).child = (*child).child;
  }
  if(NULL != (*child).child){
    temp = (*child).child;
    (*(*child).child).parent = (*child).parent;
  }
  (*child).parent = NULL;
  (*child).child = NULL;
  return temp;
}

/* Migrate a directory layer into a stack */
inline void dict_link(dict_t* parent, dict_t* child){
  dict_unlink(child);  /* remove it from an existing stack, if needed */
  (*child).parent = parent;
  (*child).child = (*parent).child;
  (*parent).child = child;
  if(NULL != (*child).child)
    (*(*child).child).parent = child;
}


dict_t* dict_remove(dict_t* props){
  return dict_unlink(props);
}

inline dict_t* dict_add_layer(dict_t* parent){
  dict_t* child = dict((*parent).size);
  dict_link(parent, child);
  return child;
}

/* Wipe an entire dictionary stack from memory */
void dict_wipe(dict_t* props){
  int i;
  dict_t* ptr = dict_head(props);
  dict_t* prev = NULL;
  while(NULL != ptr){
    prev = (*ptr).child;
    dict_free_layer(ptr);
    ptr = prev;
  }
}

/* Find the first unallocated/undefined property */
dict_property* dict_get_empty_property(dict_t* props, int allow_frozen, dict_t** container, int* index){
  int i;
  dict_t* ptr = dict_head(props);
  dict_property* cur;
  while(NULL != ptr){
    if((0 == (*ptr).frozen) || allow_frozen){
      for(i = 0; i < (*ptr).size; i++){
        cur = dict_get_property(ptr, i);
        if((NULL == (*cur).pair.name) && (NULL == (*cur).pair.value)){
          (*container) = ptr;
          (*index) = i;
          return cur;
        }
      }
    }
    ptr = (*ptr).child;
  }
  return NULL;
}

/* Walk the list looking for a name match */
dict_property* dict_get_property_by_name(dict_t* props, char* name, dict_t** container, int* index){
  int i;
  dict_t* ptr = dict_tail(props);
  dict_property* cur;
  while(NULL != ptr){
    for(i = 0; i < (*props).size; i++){
      cur = dict_get_property(props, i);
      if(NULL != cur){
        if(NULL != (*cur).pair.name){
          if(0 == strcmp(name, (*cur).pair.name)){
            /* Match found */
            (*container) = ptr;
            (*index) = i;
            return cur;
          }
        }
      }
    }
    ptr = (*ptr).parent;
  }
  container = NULL;
  (*index) = -1;
  return NULL;
}

inline void dict_set_property(dict_property* prop, char* name, char* value, int name_len, int value_len){
  assert(NULL != name);
  assert(NULL != value);
  assert(name_len > -1);
  assert(value_len > -1);

  if(name_len > (*prop).pair.name_len){
    if(NULL != (*prop).pair.name) free((*prop).pair.name);
    (*prop).pair.name = malloc(name_len + 1);
    (*prop).pair.name_len = name_len;
  }

  if(value_len > (*prop).pair.value_len){
    if(NULL != (*prop).pair.value) free((*prop).pair.value);
    (*prop).pair.value = malloc(value_len + 1);
    (*prop).pair.value_len = value_len;
  }

  strncpy((*prop).pair.name, name, name_len);
  strncpy((*prop).pair.value, value, value_len);

}

inline void dict_copy_property(dict_property* target, dict_property* source){
  dict_set_property(target, 
      (*source).pair.name, 
      (*source).pair.value, 
      (*source).pair.name_len, 
      (*source).pair.value_len
  );
}

/* Retrieve a property node by name */
inline dict_property* dict_get_named_property(dict_t* props, char* name){
  dict_t* result;
  int result_index;
  dict_property* p = dict_get_property_by_name(props, name, & result, & result_index);
  return p;
}

/* Return a value for a given name, if found */
char* dict_get(dict_t* props, char* name){
  dict_property* p = dict_get_named_property(props, name);
  return (NULL == p) ? NULL : (*p).pair.value;
}

dict_property* dict_set_extend(dict_t* props, char* name, char* value, int allow_extend){
  int result_index;
  dict_t* container;
  int name_len = strlen(name);
  int value_len = strlen(value);

  /* seek a matching node by name */
  dict_property* p = dict_get_property_by_name(props, name, & container, & result_index);

  if(NULL == p){ /* select an empty slot of the existing stack */
    p = dict_get_empty_property(props, 0, & container, & result_index);
    if(NULL != p)
      (*container).size_used++;
  }

  if(NULL == p && allow_extend){ /* add a new layer (all were full) */
    props = dict_add_layer(props);
    p = dict_get_property(props, 0);
    dict_set_property(p, name, value, name_len, value_len);
    (*props).size_used++;
    return p;
  }

  if(NULL != p) 
    dict_set_property(p, name, value, strlen(name), strlen(value));

  return p;
}

/* wrapper for automatic extension behavior */
dict_t* dict_set(dict_t* props, char* name, char* value){
  dict_set_extend(props, name, value, 1);
  return props;
}


int dict_count_slots(dict_t* props){
  int total_slots = 0;
  int i;
  dict_t* ptr = dict_tail(props);
  while(NULL != ptr){
    total_slots += (*ptr).size;
    ptr = (*ptr).parent;
  }
  return total_slots;
}

int dict_count_properties(dict_t* props){
  int total_properties = 0;
  int i;
  dict_t* ptr = dict_tail(props);
  dict_property* cur_prop;
  while(NULL != ptr){
    for(i = 0; i < (*ptr).size; i++){
      cur_prop = dict_get_property(ptr, i);
      if(NULL != (*cur_prop).pair.name){
        total_properties++;
      }
    }
    ptr = (*ptr).parent;
  }
  return total_properties;
}

/* Consolidate a single dictionary and mark it frozen. */
dict_t* dict_freeze(dict_t* props){
  dict_t* result = dict(dict_count_properties(props));
  dict_t* ptr = dict_tail(props);
  int cur = 0;
  int i;

  while(ptr){
    for(i = 0; i < (*ptr).size; i++){
      dict_copy_property(
          dict_get_property(result, cur++), 
          dict_get_property(ptr, i)
      );
    }
    ptr = (*ptr).child;
  }
  
  /* lock the underlying (newly consolidated) set */
  (*result).frozen = 1;
  return result;
}



int dict_size(dict_t* props){
  return dict_count_slots(props);
}


int dict_each(dict_t* props, dictSignal_t (*callback)(int, char*, char*)){
  int cur = 0;
  int res = 0;
  int i;
  dict_t* ptr = dict_tail(props);
  dict_property* p;

  while(NULL != ptr){
    for(i = 0; i < (*ptr).size; i++){
      p = dict_get_property(ptr, i);
      if((NULL != p) && callback(cur++, (*p).pair.name, (*p).pair.value)) res++;
    }
    ptr = (*ptr).parent;
  }

  return res;
}


dictSignal_t __unset_property_by_name(dict_t* props, int index, dict_property* p, int pos, void* data){
  if(NULL != p){
    if(0 == strcmp((*p).pair.name, (char*) data)){
      dict_wipe_property(p, 0);
    }
  }
  return DICT_EACH_CONTINUE;
}


int dict_unset(dict_t* props, char* name){
  int i;
  int total = 0;
  dict_t* ptr = dict_tail(props);
  dict_property* cur_prop;

  return dict_each_property(props, __unset_property_by_name, name);

}


