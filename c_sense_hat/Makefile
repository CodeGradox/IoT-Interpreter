# The compiler to use
CC = clang
CXX = clang++

# compiler flags
# -g	add debugging information to the executable file
# -Wall	turns on most compiler flags
CFLAGS = -g -Wall -Wextra -std=c11 -pedantic -O2
CXXFLAGS = -g -Wall -Wextra -std=c++14 -pedantic -O2
OBJS = main.o RTIMULibWrapper.o
LDFLAGS += -lRTIMULib
# $@ is the name of the executable. In this case it refers to 'all' which is set to be 'main'
#PROG = main
#all: $(PROG)

#$(PROG): $(OBJS)
#	$(CC) $(CFLAGS) $(OBJS) -o $@

# Both depend on the same headers
#main.o: color.h RTIMULibWrapper.h
#color.o: color.h RTIMULibWrapper.h
#RTIMULibWrapper.o: RTIMULibWrapper.h

all: main.o RTIMULibWrapper.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(OBJS) -o prog

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

RTIMULibWrapper.o: RTIMULibWrapper.cpp RTIMULibWrapper.h
	$(CXX) $(CXXFLAGS) -c RTIMULibWrapper.cpp -o RTIMULibWrapper.o


clean:
	rm -f *.o prog core a.out

