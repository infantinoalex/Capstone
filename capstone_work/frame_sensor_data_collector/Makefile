LIBS=-lphidget22 -pthread
FLAGS=-c
STANDARD=-std=c99

CC=gcc

all: Frame_Collector

Frame_Collector: Frame_Data_Collector.o
	$(CC) $(STANDARD) Frame_Data_Collector.o -o Frame_Collector $(LIBS)

Frame_Data_Collector.o: Frame_Data_Collector.c
	$(CC) $(STANDARD) $(FLAGS) Frame_Data_Collector.c -o Frame_Data_Collector.o

clean:
	rm *.o Frame_Collector
