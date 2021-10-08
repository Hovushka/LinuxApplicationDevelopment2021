GENERATES = prog prog-a prog-so README
TRASH = *.txt *.o *~ o.* *.a *.so
CFLAGS=-fPIC
TEST_INPUT=weiherstrasse
TEST_TRIPLE_INPUT=mew 123 murr

all:    README prog prog-a prog-so

prog:   const.o fun.o prog.o

prog-a: prog.o liboutput_static.a
	cc -L. $< -loutput_static -o $@

prog-so: prog.o liboutput.so
	cc -L. $< -loutput -o $@

liboutput_static.a: const.o fun.o
	ar -rcs $@ $^

liboutput.so: fun.o const.o
	cc -shared $^ -o $@

README: prog
	./$< 2> $@

fun.o:  outlib.h

test: compare compare_1 compare_3

compare: prog.txt prog-a.txt prog-so.txt
	diff3 $^
	rm $^

compare_1: prog_1.txt prog-a_1.txt prog-so_1.txt
	diff3 $^
	rm $^

compare_3: prog_3.txt prog-a_3.txt prog-so_3.txt
	diff3 $^
	rm $^

%.txt: %
	LD_LIBRARY_PATH=`pwd` ./$< 2> $@

%_1.txt: %
	LD_LIBRARY_PATH=`pwd` ./$< $(TEST_INPUT) 1> $@

%_3.txt: %
	LD_LIBRARY_PATH=`pwd` ./$< $(TEST_TRIPLE_INPUT) 1> $@

clean:
	rm -f $(TRASH)

distclean:      clean
	rm -rf $(GENERATES)
