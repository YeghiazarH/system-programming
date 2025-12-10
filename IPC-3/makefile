CXX=g++
CXXFLAGS=-std=c++17 -O2 -Wall
LDFLAGS=-pthread

all: first second

first: first.cpp shared_array.h
	$(CXX) $(CXXFLAGS) -o first first.cpp $(LDFLAGS)

second: second.cpp shared_array.h
	$(CXX) $(CXXFLAGS) -o second second.cpp $(LDFLAGS)

clean:
	rm -f first second
