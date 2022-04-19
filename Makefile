#
#
CC=g++
CFLAGS=-g

# comment line below for Linux machines
all: sim
sim:	
	$(CC) $(CFLAGS) -o sim branchPredictor.cc

clean:
	\rm -f sim


