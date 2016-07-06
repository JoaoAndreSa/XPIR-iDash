# the compiler: gcc for C program, define as g++ for C++
CC = g++ -std=c++11

# the build targets:
TARGET1 = checkExecution


all: $(TARGET1)

$(TARGET1): $(TARGET1).cpp
	$(CC) $(TARGET1).cpp -o $(TARGET1)

clean:
	$(RM) $(TARGET1)