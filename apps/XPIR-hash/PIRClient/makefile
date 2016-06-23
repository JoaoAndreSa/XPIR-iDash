# the compiler: gcc for C program, define as g++ for C++
CC = g++ -std=c++11

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS = -I/home/user/Desktop/XPIR -L/home/user/Desktop/XPIR/_build/pir -lssl -lcrypto -lpir_static -lgmp -lmpfr -fopenmp -lboost_thread -lboost_system

# the build target executable:
TARGET = client

# other code that needs to be compiled
AES_CBC = AES/AES_cbc_256
SHA1 = SHA256/SHA_256
Constants = ../Constants/constants

PIRClient = PIRClient
PIRClientParallel = Parallel/PIRClientParallel
PIRClientSequential = Sequential/PIRClientSequential

XPIRc = ../XPIR/XPIRc
XPIRcParallel = ../XPIR/Parallel/XPIRcParallel
XPIRcSequential = ../XPIR/Sequential/XPIRcSequential

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(TARGET).cpp $(AES_CBC).cpp $(SHA1).cpp $(PIRClient).cpp $(PIRClientParallel).cpp $(PIRClientSequential).cpp $(XPIRc).cpp $(XPIRcParallel).cpp $(XPIRcSequential).cpp $(Constants).cpp $(CFLAGS) -o $(TARGET) 

clean:
	$(RM) $(TARGET)