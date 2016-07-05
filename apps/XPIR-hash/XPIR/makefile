# the compiler: gcc for C program, define as g++ for C++
CC = g++ -std=c++11

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS = -I../../.. -L../../../_build/pir -lssl -lcrypto -lpir_static -lgmp -lmpfr -fopenmp -lboost_thread -lboost_system

# the build target executable:
TARGET = simplePIR

# othe code that needs to be compiled
XPIRc = XPIRc
AES_CBC = ..//PIRClient/AES/AES_cbc_256

all: $(TARGET)

$(TARGET): $(TARGET).cpp
#	$(CC) $(TARGET).cpp $(XPIRc).cpp $(CFLAGS) -o $(TARGET) 
	$(CC) $(TARGET).cpp $(AES_CBC).cpp $(CFLAGS) -o $(TARGET) 

clean:
	$(RM) $(TARGET)
