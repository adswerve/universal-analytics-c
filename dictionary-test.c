#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dictionary.h"

dictSignal_t print_property(int id, char* name, char* value){
  if(NULL != name && NULL != value){
    printf("Property: #%d %s = %s\n", id, name, value);
  }
  return DICT_EACH_CONTINUE;
}

int main (int argc, char** argv){
  
  dict_t* d = dict(2);
  dict_set(d, "pizza", "meal");
  dict_set(d, "tomatoes", "vegetable");
  dict_set(d, "beer", "beverage");

  /*
  int i;
  char temp1[10];
  char temp2[10];
  memset(temp1, 0, 10);
  memset(temp2, 0, 10);
  for(i = 0; i < 15; i++){
    sprintf(temp1, "Name %d", i);
    sprintf(temp2, "Value %d", i);
    dict_set(d, (temp1), (temp2));
  }
  */

  printf("Nodes: %d\n", dict_size(d));

  dict_each(d, *print_property);

  dict_wipe(d);


  return 0;
}
