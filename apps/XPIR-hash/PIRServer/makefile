 # the compiler: gcc for C program, define as g++ for C++
CC = g++ -std=c++11 -g

# compiler flags:
CFLAGS = -I/home/user/Desktop/XPIR -L/home/user/Desktop/XPIR/_build/pir -pthread -lpir_static -lgmp -lmpfr -fopenmp -lboost_thread -lboost_system
 
# the build target executable:
TARGET = server

# othe code that needs to be compiled
Constants = ../Constants/constants

Socket = ../Socket/Socket

Tools = ../Tools/Tools

PIRServer = PIRServer
PIRServerPipeline = Pipeline/PIRServerPipeline
PIRServerSequential = Sequential/PIRServerSequential

XPIRc = ../XPIR/XPIRc
XPIRcPipeline = ../XPIR/Pipeline/XPIRcPipeline
XPIRcSequential = ../XPIR/Sequential/XPIRcSequential

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(TARGET).cpp $(PIRServer).cpp $(PIRServerPipeline).cpp $(PIRServerSequential).cpp $(XPIRc).cpp $(XPIRcPipeline).cpp $(XPIRcSequential).cpp $(Socket).cpp $(Tools).cpp $(Constants).cpp $(CFLAGS) -o $(TARGET) 

clean:
	$(RM) $(TARGET)