.c.o:
	gcc -c $<

ex01: ex01.o split.o join.o
	gcc -o $@ $^ 
	./$@
	rm $^ $@

clean:
	rm *.o