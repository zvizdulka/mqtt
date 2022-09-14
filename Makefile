all: main

main: main.o
	g++ -o main main.o -lmosquitto

main.o: main.cpp
	g++ -c main.cpp 

clean:
	rm main.o main

