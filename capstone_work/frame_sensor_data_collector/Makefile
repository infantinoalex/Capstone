LIBS=-lphidget22
FLAGS=-c

CC=gcc

all: Frame_Collector

PhidgetHelper.o: PhidgetHelper.h
	$(CC) $(FLAGS) PhidgetHelper.h -o PhidgetHelper.o

Frame_Collector: PhidgetHelper.o Frame_Data_Collector.o
	$(CC) $(FLAGS) PhidgetHelper.o Frame_Data_Collector.o -o Frame_Collector $(LIBS)

Frame_Data_Collector.o: Frame_Data_Collector.c
	$(CC) $(FLAGS) Frame_Data_Collector.c -o Frame_Data_Collector.o

clean:
	rm *.o Frame_Collector
