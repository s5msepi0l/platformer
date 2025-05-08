CC = g++
 
CFLAGS  = -std=c++20
 
TARGET = main
 
all: $(TARGET)
 
$(TARGET): $(TARGET).cpp
	$(CC) -o $(TARGET) $(TARGET).cpp $(CFLAGS)
	./$(TARGET)
 
clean:
	$(RM) $(TARGET)