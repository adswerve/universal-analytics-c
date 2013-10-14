

.PHONY: all clean test

all: universal-analytics.o 

test: dictionary.o dictionary-test.exe 

clean:
	rm -v testing.exe universal-analytics.o url-assembly.o dictionary-test.exe dictionary.o

testing.exe: test.c universal-analytics.o 
	gcc -o $@ $^

dictionary.o: dictionary.c dictionary.h
	gcc -ggdb -o $@ -c $<


dictionary-test.exe: dictionary-test.c dictionary.o
	gcc -ggdb -o $@ $^

# url-assembly.o: url-assembly.c url-assembly.h
#	gcc -o $@ -c $<

universal-analytics.o: universal-analytics.c universal-analytics.h
	gcc -o $@ -c $<




#  vim: set nowrap tabstop=2 shiftwidth=2 softtabstop=0 noexpandtab textwidth=0 filetype=make foldmethod=syntax foldcolumn=4
