# Define the compiler and flags
TARGET = exe

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -g3
# Define the source files and the executable name
SRCS = main.cpp Board.cpp GameAnalyzer.cpp Pieces.cpp
# Rule to build the target executable

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)
# Clean rule to delete object files and the executable
clean:
	rm -f $(TARGET)

re: clean $(TARGET)


# fclean: clean
# Phony targets
.PHONY: clean re