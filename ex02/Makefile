.c.o:
	gcc -c $<

ex02: ex02.o split-bar.o split-shell.o
	gcc -o $@ $^ 
	./$@
	rm $^ $@

bar: split-bar.o split-shell.o
	gcc -c ex02.c -DBAR
	gcc -o $@ ex02.o $^ 
	./$@
	rm $^ $@ ex02.o

shell: split-bar.o split-shell.o
	gcc -c ex02.c -DSHELL
	gcc -o $@ ex02.o $^ 
	./$@
	rm $^ $@ ex02.o

merge: split-merge.o split-bar.o split-shell.o
	gcc -c ex02.c -DMERGE
	gcc -o $@ ex02.o $^ 
	./$@
	rm $^ $@ ex02.o

clean:
	rm *.o