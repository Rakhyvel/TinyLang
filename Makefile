run:
	gcc -Wall *.c -o tinylang
	./tinylang test.tny
	
debug:
	gcc -Wall -g *.c -o tinylang
	valgrind ./tinylang test.tny