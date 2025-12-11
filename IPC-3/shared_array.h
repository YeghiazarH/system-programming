#ifndef SHARED_ARRAY_H
#define SHARED_ARRAY_H

#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdexcept>


class shared_array {
    std::string shm_name;
    std::string sem_name;
    int fd;
    int *ptr;
    size_t count;
    size_t byte_size;
    sem_t *sem;

public:
    shared_array(const std::string &name, size_t size_elements)
        : fd(-1), ptr(nullptr), count(size_elements), sem(nullptr)
    {
        if(size_elements < 1 || size_elements > 1000000000) {
            throw std::invalid_argument("array size must be between 1 and 1,000,000,000 elements");
        }

        shm_name = "/" + name;
        sem_name = "/sem_" + name;

        byte_size = size_elements * sizeof(int);

        fd = shm_open(shm_name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0666);
        if(fd != -1) {
            if(ftruncate(fd, byte_size) == -1) {
                close(fd);
                shm_unlink(shm_name.c_str());
                throw std::runtime_error("truncate failed");
            }
        } else {
            fd = shm_open(shm_name.c_str(), O_RDWR, 0666);
            if(fd == -1) throw std::runtime_error("shm_open failed");
            
            // Check if existing shared memory size matches requested size
            struct stat shm_stat;
            if(fstat(fd, &shm_stat) == -1) {
                close(fd);
                throw std::runtime_error("fstat failed");
            }
            if(shm_stat.st_size != (off_t)byte_size) {
                close(fd);
                throw std::runtime_error("shared memory size mismatch");
            }
        }

        void *map = mmap(0, byte_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if(map == MAP_FAILED) {
            close(fd);
            shm_unlink(shm_name.c_str());
            throw std::runtime_error("mmap failed");
        }

        ptr = (int*)map;

        sem = sem_open(sem_name.c_str(), O_CREAT, 0666, 1);
        if(sem == SEM_FAILED) {
            munmap(ptr, byte_size);
            close(fd);
            throw std::runtime_error("sem_open failed");
        }
    }


    shared_array(const shared_array&) = delete;
    shared_array& operator=(const shared_array&) = delete;

    shared_array(shared_array&& other) noexcept
        : shm_name(std::move(other.shm_name)),
          sem_name(std::move(other.sem_name)),
          fd(other.fd),
          ptr(other.ptr),
          count(other.count),
          byte_size(other.byte_size),
          sem(other.sem)
    {
        other.fd = -1;
        other.ptr = nullptr;
        other.sem = nullptr;
    }

    ~shared_array(){
        if(ptr) munmap(ptr, byte_size);
        if(fd != -1) close(fd);
        if(sem) sem_close(sem);
    }

    int& operator[](size_t i){
        if(i >= count) throw std::out_of_range("index");
        return ptr[i];
    }

    size_t size() const { return count; }

    sem_t* get_semaphore() { return sem; }

    void unlink() {
        if(!shm_name.empty()) {
            shm_unlink(shm_name.c_str());
        }
        if(!sem_name.empty()) {
            sem_unlink(sem_name.c_str());
        }
    }
};

#endif
