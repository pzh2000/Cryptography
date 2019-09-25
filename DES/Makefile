CC = gcc

CFLAGS = -g -Wall

des_op: des_op.o des.o

des_op.o: des_op.c
	cc -c des_op.c

des.o: des.c
	cc -c des.c

clean:
	rm -f des_op