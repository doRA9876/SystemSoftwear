.c.o:
	gcc -c $<

ls: ls.o
	gcc -o $@.out $^ 
	rm *.o

find: find.o
	gcc -o $@.out $^
	rm *.o

clean:
	rm *.o
	rm *.out