# The compiler to use
CC = clang
CXX = clang++

# compiler flags
# -g		add debugging information to the executable file
# -Wall		turns on most compiler flags
# -Wextra	aditional error checking
# -std=c11	
# -std=c++14
# -pedantic	check if the program follows the C ISO spesifications
# -O2		Compiler optimization
CFLAGS = -g -Wall -Wextra -m32 -std=c11 -pedantic -O2
CXXFLAGS = -g -Wall -Wextra -m32 -std=c++14 -pedantic -O2
# Required to compile RTIMULib.h
LDFLAGS += -lRTIMULib
OBJS = main.o SenseHatSensors.o

all: main SenseHatSensors
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(OBJS) -o prog

main: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

SenseHatSensors: SenseHatSensors.cpp SenseHatSensors.h
	$(CXX) $(CXXFLAGS) -c SenseHatSensors.cpp -o SenseHatSensors.o

clean:
	rm -f *.o prog core a.out

