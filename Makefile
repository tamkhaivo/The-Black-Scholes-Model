CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra

TARGET = benchmark
SRCS = main.cpp Benchmark.cpp The_Black_Scholes_Model_Value.cpp The_Black_Scholes_Model_Reference.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
