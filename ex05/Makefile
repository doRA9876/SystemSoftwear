.c.o:
	gcc -c $<

client: client.o 
	gcc -o $@.out $^
	rm $@.o

proxy: proxy.o
	gcc -o $@.out $^
	rm $@.o

clean:
	rm *.o
	rm *.out