CC := g++
FLAGS := -Wall -O2 -std=c++11

build:
	$(CC) -c src/settings.cpp -o settings.o
	$(CC) -c src/points.cpp -o points.o
	$(CC) -c src/app.cpp -o app.o
	$(CC) -c src/statistics.cpp -o statistics.o
	$(CC) -c src/help.cpp -o help.o
	$(CC) -c src/errors.cpp -o errors.o
	$(CC) -c src/main.cpp -o main.o
	
	$(CC) settings.o points.o app.o statistics.o help.o errors.o main.o -o programm