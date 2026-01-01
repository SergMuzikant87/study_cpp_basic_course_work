CC := g++
FLAGS := -Wall -O2 -std=c++11
SRC_FORDER := src/
INC_FORDER := inc/
SRC_FILES := $(wildcard $(SRC_FORDER)*.cpp)
INC_FILES := $(wildcard $(INC_FORDER)*.h)
PROGRAMM_NAME:= programm

link: compile
	$(CC) $(FLAGS) $(wildcard $(addsuffix .o, $(basename $(notdir $(SRC_FILES))))) -o $(PROGRAMM_NAME)
compile:
	$(CC) $(FLAGS) -c $(SRC_FILES)