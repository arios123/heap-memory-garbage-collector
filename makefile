mallocTest: dumalloc.o mallocTestMain.o
	gcc output/dumalloc.o output/mallocTestMain.o -o mallocTest

dumalloc.o: 
	gcc src/dumalloc.c -c -o output/dumalloc.o

mallocTestMain.o: headers/dumalloc.h
	gcc src/mallocTest.c -c -o output/mallocTestMain.o

clean:
	rm -f *.o
	rm -f mallocTest
	rm -f output/*.o