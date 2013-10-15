

.PHONY: all clean test

all: universal-analytics.o 

test: dictionary.o dictionary-test.exe 

clean:
	rm -v testing.exe universal-analytics.o 

testing.exe: test.c universal-analytics.o 
	gcc -o $@ $^


universal-analytics.o: universal-analytics.c universal-analytics.h
	gcc -o $@ -c $<


# This one is our giant prototype of doom...
bigtest: bigger-test.c
	 gcc -ggdb -Wall $< -o $@ -l curl



#  vim: set nowrap tabstop=2 shiftwidth=2 softtabstop=0 noexpandtab textwidth=0 filetype=make foldmethod=syntax foldcolumn=4
