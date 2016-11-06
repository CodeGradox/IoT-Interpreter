# The compiler to use
CC = clang++

# compiler flags
# -g	add debugging information to the executable file
# -Wall	turns on most compiler flags
CFLAGS = -g -Wall
OBJS = main.o color.o RTIMULibWrapper.o
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

all: main.o color.o RTIMULibWrapper.o
	$(CC) $(LDFLAGS) $(CFLAGS) $(OBJS) -o prog

main.o: main.c
	clang $(CFLAGS) -c main.c -o main.o

color.o: color.c color.h
	clang $(CFLAGS) -c color.c -o color.o

RTIMULibWrapper.o: RTIMULibWrapper.cpp RTIMULibWrapper.h
	$(CC) $(CFLAGS) -c RTIMULibWrapper.cpp -o RTIMULibWrapper.o


clean:
	rm -f *.o prog core a.out

