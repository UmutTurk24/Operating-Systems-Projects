OBJECTS1 = files.o storage.o bitmap.o tester.o

tester-clean: clean tester

tester: $(OBJECTS1)
	clang -g -std=c11 -o tester $(OBJECTS1) -I.

%.o: %.c
	clang -g -std=c11 -c -o $@ -I. $<

clean:
	rm -f *.o tester