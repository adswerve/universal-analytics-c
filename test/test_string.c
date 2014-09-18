
#include "../src/string/encode.h"
#include <stdio.h>
#include <stdlib.h>

#define test_encoding 1
#define test_hexdigest 1


int print_char_codes(char* value, unsigned int length){
	unsigned int i;
	printf("Character codes for: '%s'\n", value);
	printf("> ");
	for(i = 0; i < length; i++){
		printf("%u ", value[i]);
	}
	printf("\n");
	return i;
}


int main(int argc, char* argv[]){

	if(test_encoding){

		char script[] = "function(a){ pizza }";
		char* x = urlencode(script);
		printf("Encoded result: '%s'\n", x);

		char hebrew[] = "[סֶאבױ]";
		char* y = urlencode(hebrew);
		printf("Encoded result: '%s'\n", y);

		free(x);
		free(y);

	}

	if(test_hexdigest){
		char md5_result[33] = {0};
		unsigned char md5_binary[16] = {94, 182, 59, 187, 224, 30, 238, 208, 147, 203, 34, 187, 143, 90, 205, 195}; // hash of "hello world"
		hexdigest(md5_result, md5_binary, 16);
		printf("MD5 hexdigest: %s\n", md5_result);
	}
	

	return 0;
}

