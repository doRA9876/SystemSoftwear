.c.o:
	gcc -c $<

ex04: ex04.o split.o
	gcc -o $@.out $^
	rm *.o

clean:
	rm *.o
	rm *.out