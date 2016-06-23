 # the compiler: gcc for C program, define as g++ for C++
CC = g++ -std=c++11 -g

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS = -I/home/user/Desktop/XPIR -L/home/user/Desktop/XPIR/_build/pir -pthread -lpir_static -lgmp -lmpfr -fopenmp -lboost_thread -lboost_system
 
# the build target executable:
TARGET = server

# othe code that needs to be compiled
PIRServer = PIRServer
XPIRc = ../XPIR/XPIRc
Constants = ../Constants/constants

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(TARGET).cpp $(PIRServer).cpp $(XPIRc).cpp $(Constants).cpp $(CFLAGS) -o $(TARGET) 

clean:
	$(RM) $(TARGET)