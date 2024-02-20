  CC = g++
 
  # compiler flags:
  #  -g     - this flag adds debugging information to the executable file
  #  -Wall  - this flag is used to turn on most compiler warnings
  CFLAGS  = -std=c++20 -lSDL2
 
  # The build target 
  TARGET = main
 
  all: $(TARGET)
 
  $(TARGET): $(TARGET).cpp
		$(CC) -o $(TARGET) $(TARGET).cpp $(CFLAGS)
		./$(TARGET)
 
  clean:
		$(RM) $(TARGET)