CXX      := clang++
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic \
            -I/opt/homebrew/include \
            -I/opt/homebrew/include/SDL2

LDFLAGS  := -L/opt/homebrew/lib
LDLIBS   := -lSDL2

TARGET   := main 
OBJ      := main.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean
