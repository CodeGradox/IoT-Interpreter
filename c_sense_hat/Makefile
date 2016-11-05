# The compiler to use
CC = clang

# compiler flags
# -g	add debugging information to the executable file
# -Wall	turns on most compiler flags
CFLAGS = -g -Wall
OBJS = main.o color.o
# $@ is the name of the executable. In this case it refers to 'all' which is set to be 'main'
PROG = main
all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

# Both depend on the same headers
main.o: color.h
color.o: color.h

clean:
	rm -f *.o $(PROG) core a.out

