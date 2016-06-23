# the compiler: gcc for C program, define as g++ for C++
CC = g++ -std=c++11

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS = -lssl -lcrypto

# the build target executable:
TARGET = SHA_256_example

# other code that needs to be compiled
SHA1 = SHA_256

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(TARGET).cpp $(SHA1).cpp $(CFLAGS) -o $(TARGET) 

clean:
	$(RM) $(TARGET)