.c.o:
	gcc -c $<

ex03: ex03.o
	gcc -o $@ $^ 
	./$@
	rm $^ $@

clean:
	rm *.o