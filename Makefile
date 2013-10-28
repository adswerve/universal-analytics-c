DEBUG_FLAGS=-ggdb
#COMPILE_FLAGS=-O3
DEPEND_FLAGS=-l curl

.PHONY: all clean test

all: universal-analytics.o 

test: testing.exe

clean:
	@rm -fv testing.exe universal-analytics.o util/http.o

testing.exe: test.c universal-analytics.o util/http.o 
	gcc $(COMPILE_FLAGS) $(DEBUG_FLAGS) $(DEPEND_FLAGS) -o $@ $^


util/http.o: util/http.c util/http.h
	gcc $(COMPILE_FLAGS) $(DEBUG_FLAGS) -o $@  -c $<

universal-analytics.o: universal-analytics.c universal-analytics.h
	gcc $(COMPILE_FLAGS) $(DEBUG_FLAGS) -o $@ -c $<



#  vim: set nowrap tabstop=2 shiftwidth=2 softtabstop=0 noexpandtab textwidth=0 filetype=make foldmethod=syntax foldcolumn=4
