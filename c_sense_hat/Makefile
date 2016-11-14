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
# Link to the RTIMULib source
LDFLAGS += -lRTIMULib
OBJS = main.o SenseHatSensors.o
MAIN = prog


$(MAIN): $(OBJS)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(OBJS) -o $@

main:
	$(CC) $(CFLAGS) -c $<

SenseHatSensors: SenseHatSensors.h
	$(CXX) $(CXXFLAGS) -c $<

clean:
	-$(RM) *.o
	-$(RM) $(MAIN)
	-$(RM) core

