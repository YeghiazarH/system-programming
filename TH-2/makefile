CC = g++

all: parallel_scheduler.so demo-application

parallel_scheduler.o: parallel_scheduler.cpp parallel_scheduler.h
	$(CC) $< -fPIC -c

parallel_scheduler.so: parallel_scheduler.o
	$(CC) $^ -shared -o $@

demo-application: demo.cpp parallel_scheduler.so
	$(CC) $^ -fPIC -o $@

clean:
	rm *.o *.so demo-application