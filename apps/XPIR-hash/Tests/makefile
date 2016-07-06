# the compiler: gcc for C program, define as g++ for C++
CC = g++ -std=c++11

# the build targets:
TARGET1 = changeParamsPIR
TARGET2 = changeParamsSHA
TARGET3 = checkExecution


all: $(TARGET1) $(TARGET2) $(TARGET3)

$(TARGET1): $(TARGET1).cpp
	$(CC) $(TARGET1).cpp -o $(TARGET1)

$(TARGET2): $(TARGET2).cpp
	$(CC) $(TARGET2).cpp -o $(TARGET2)

$(TARGET3): $(TARGET3).cpp
	$(CC) $(TARGET3).cpp -o $(TARGET3)

clean:
	$(RM) $(TARGET1) $(TARGET2) $(TARGET3)
