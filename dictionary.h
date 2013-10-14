
typedef enum dictSignal {
  DICT_EACH_CONTINUE,
  DICT_EACH_STOP
} dictSignal_t;

typedef struct dict_property{
  struct {
    char* name;
    char* value;
    int name_len;
    int value_len;
    int slot;
  } pair;
} dict_property;

typedef struct dict {
  int size;
  int size_used;
  int frozen;
  struct dict* parent;
  struct dict* child;
  struct dict_property** properties;
} dict_t;

/* create a new dictionary layer */
dict_t* dict(int size);

/* remove a dictionary from memory (recursively) */
void dict_wipe(dict_t* props);

/* Calculate the number of free slots in the current stack */
int dict_slots_free(dict_t* props, int global);

/* scan a dictionary stack for a property */
char* dict_get(dict_t* props, char* name);

/* add a property to a dictionary stack (topmost layer) */
dict_t* dict_set(dict_t* props, char* name, char* value);

/* remove all instances of a name from the dictionary */
int dict_unset(dict_t* props, char* name);

/* create a consolidated, frozen set */
dict_t* dict_freeze(dict_t* props);

/* remove a layer from a dictionary stack */
dict_t* dict_remove(dict_t* props);

/* find which layer provides a given name */
dict_t* dict_search(char* name);

/* calculate the total volume of the dictionary */
int dict_size(dict_t* props);

/* Call a function with each argument */
int dict_each(dict_t* props, dictSignal_t (*callback)(int, char*, char*));
