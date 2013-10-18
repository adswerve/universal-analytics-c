

.PHONY: all clean test

all: universal-analytics.o 

test: testing.exe

clean:
	@rm -fv testing.exe universal-analytics.o 

testing.exe: test.c universal-analytics.o 
	gcc -o $@ -l curl $^


universal-analytics.o: universal-analytics.c universal-analytics.h
	gcc -o $@ -c $<



#  vim: set nowrap tabstop=2 shiftwidth=2 softtabstop=0 noexpandtab textwidth=0 filetype=make foldmethod=syntax foldcolumn=4
