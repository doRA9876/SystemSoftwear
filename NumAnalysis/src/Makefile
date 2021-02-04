TARGET = RegulaFalsi Newton Sweep
OBJS = RegulaFalsi.o Newton.o Sweep.o
SOURCE = RegulaFalsi.cpp Newton.cpp Sweep.cpp

$(OBJS): $(SOURCE)
	g++ $*.cpp -o $@ 

$(TARGET): $(SOURCE)
	g++ $@.cpp -o $@.o
	./$@.o
	rm $@.o

compile: $(OBJS) 

clean: 
	rm *.o