DEBUG ?= 0
DEBUG_FLAGS=-ggdb -D DEBUG=$(DEBUG) 
COMPILE_FLAGS=-Wall -stdlib=libstdc++
DEPEND_FLAGS=-l curl
# OPTIMIZE_FLAGS=-O3

.PHONY: all clean test test-util

all: build/universal-analytics.o build/http.o

test: test-strings build/testing.exe build/static-testing.exe 

test-strings: build/strings.exe

clean:
	@rm -fv build/*.exe build/*.o
	@rm -rfv *.exe.dSYM build


build/libanalytics.a: build/http.o build/universal-analytics.o
	ar -rs $@ $<

build/testing.exe: test.c build/universal-analytics.o build/http.o build/encode.o
	mkdir -p `dirname $@`
	gcc $(COMPILE_FLAGS) $(OPTIMIZE_FLAGS) $(DEBUG_FLAGS) $(DEPEND_FLAGS) -o $@ $^

build/static-testing.exe: test-static.c build/universal-analytics.o build/http.o build/encode.o
	mkdir -p `dirname $@`
	gcc $(COMPILE_FLAGS) $(OPTIMIZE_FLAGS) $(DEBUG_FLAGS) $(DEPEND_FLAGS) -o $@ $^


build/strings.exe: test/test_string.c build/encode.o
	mkdir -p `dirname $@`
	gcc $(COMPILE_FLAGS) $(OPTIMIZE_FLAGS) $(DEBUG_FLAGS) $(DEPEND_FLAGS) -o $@ $^

build/encode.o: src/string/encode.c src/string/encode.h
	mkdir -p `dirname $@`
	gcc $(COMPILE_FLAGS) $(DEBUG_FLAGS) -o $@ -c $<

build/http.o: src/http.c src/http.h
	mkdir -p `dirname $@`
	gcc $(COMPILE_FLAGS) $(DEBUG_FLAGS) -o $@  -c $<

build/universal-analytics.o: src/universal-analytics.c src/universal-analytics.h
	mkdir -p `dirname $@`
	gcc $(COMPILE_FLAGS) $(DEBUG_FLAGS) -o $@ -c $<


#  vim: set nowrap tabstop=2 shiftwidth=2 softtabstop=0 noexpandtab textwidth=0 filetype=make foldmethod=syntax foldcolumn=4
