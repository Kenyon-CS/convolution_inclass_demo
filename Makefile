CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

demo: $(OBJ)
	$(CXX) $(CXXFLAGS) -o demo $(OBJ)

clean:
	rm -f $(OBJ) demo