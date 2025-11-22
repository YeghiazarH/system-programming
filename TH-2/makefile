CC = g++

all: libparallel_scheduler.so demo-application

parallel_scheduler.o: parallel_scheduler.cpp parallel_scheduler.h
	$(CC) $< -fPIC -c

libparallel_scheduler.so: parallel_scheduler.o
	$(CC) $^ -shared -o $@

demo-application: demo.cpp libparallel_scheduler.so
	$(CC) demo.cpp -L. -lparallel_scheduler -Wl,-rpath,. -o $@

clean:
	rm *.o *.so demo-application