CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = recomendador
SRC = main.cpp Entrada.cpp Sistema.cpp Menu.cpp
OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean