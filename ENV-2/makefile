CC = g++
CCFLAGS = -Iinclude
TARGET = release/main

.PHONY: all clean release

all: $(TARGET)
	
$(TARGET): release/main.o release/sort.o release/complex.o
	$(CC) $(CCFLAGS) -o $@ $^

release/main.o: source/main.cpp include/sort.h include/complex.h | release
	$(CC) $(CCFLAGS) -c $< -o $@

release/sort.o: source/sort.cpp include/sort.h | release
	$(CC) $(CCFLAGS) -c $< -o $@

release/complex.o: source/complex.cpp include/complex.h | release
	$(CC) $(CCFLAGS) -c $< -o $@

release:
	mkdir -p release

clean:
	rm release/*.o $(TARGET)

