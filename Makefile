##
# csv-read
#
# @file
# @version 0.1
COMPILER = g++
FLAGS = -Wall -std=c++20 -pthread -mavx2 
DEBUG_FLAGS = -g -fno-omit-frame-pointer -fsanitize=address -O0
RELEASE_FLAGS = -O3

%.out: clean
ifeq ($(target),debug)
	gdb ./tester
	${COMPILER} $(patsubst %.out, %.cpp, $@) ${FLAGS} ${DEBUG_FLAGS} -o $@
else
	${COMPILER} $(patsubst %.out, %.cpp, $@) ${FLAGS} ${RELEASE_FLAGS} -o $@
endif


clean:
	rm -f *.out

.PHONY: clean 

# end
