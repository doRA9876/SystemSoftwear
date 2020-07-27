.c.o:
	gcc -c $<

client: client.o 
	gcc -o $@.out $^
	rm $@.o

server: server.o
	gcc -o $@.out $^
	rm $@.o

proxy-b: proxy-b.o
	gcc -o $@.out $^
	rm $@.o

clean:
	rm *.o
	rm *.out