CC=gcc
CPP=g++

OPTI=-O3
DEBUG=
FLAGS=-Wall $(DEBUG) $(OPTI)

OBJECTS=bbs.o \
		sha2.o \
		KOOSParameter.o \
		utilities.o \
		KOOS.o

prndGen: prndGen.cpp $(OBJECTS)
	$(CPP) $(FLAGS) -o prndGen prndGen.cpp $(OBJECTS)

bbs.o: bbs.c bbs.h
	$(CC) $(FLAGS) -c $< -o $@

sha2.o: sha2.c sha2.h
	$(CC) $(FLAGS) -c $< -o $@

KOOSParameter.o: KOOSParameter.cpp KOOSParameter.h
	$(CPP) $(FLAGS) -c $< -o $@

KOOS.o: KOOS.cpp KOOS.h
	$(CPP) $(FLAGS) -c $< -o $@

utilities.o: utilities.cpp utilities.h
	$(CPP) $(FLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -vf $(OBJECTS) prndGen