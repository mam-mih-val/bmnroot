#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>

#include <cstdlib>  //std::system
#include <iostream>
#include <sstream>
#include <string>

using namespace boost::interprocess;

const size_t kMEMORY_SIZE = 65536;
const size_t kBUFFER_SIZE = 32;
const char *kMEMORY_NAME = "MySharedMemory";
const char *kWRITE_SEM_NAME = "MyWriteSemaphore";
const char *kREAD_SEM_NAME = "MyReadSemaphore";
const char *kMSG_NAME = "MyReadSemaphorefgfd";

bool SendData(int t) {
    managed_shared_memory segment(open_only, "MySharedMemory");
    named_semaphore writeSem(open_only, "MyWriteSemaphore");
    named_semaphore readSem(open_only, "MyReadSemaphore");

    writeSem.wait();

    segment.destroy<char>("kMSG_NAME");

    auto msg_str = segment.construct<char>(kMSG_NAME)[kBUFFER_SIZE](0);

    std::string data = "Hello world ";
    data += std::to_string(t);

    std::cout << "[ " << t << " ] " << "Write: " << data << std::endl;

    std::memcpy(msg_str, data.c_str(), kBUFFER_SIZE);

    readSem.post();

    return true;
}

bool ReceiveData(int t) {
    try {
        managed_shared_memory segment(open_only, "MySharedMemory");
        named_semaphore writeSem(open_only, "MyWriteSemaphore");
        named_semaphore readSem(open_only, "MyReadSemaphore");

        readSem.wait();

        auto msg = segment.find<char>(kMSG_NAME).first;

        std::cout << "[ " << t << " ] " << "Read: " << msg << std::endl;

        segment.destroy<char>(kMSG_NAME);

        writeSem.post();

        return true;
    } catch (const interprocess_exception &ex) {
        std::cout << ex.what() << std::endl;
        return false;
    }
};

int main(int argc, char *argv[]) {
    if (argc == 1) {  // Parent process
        // Remove shared memory on construction and destruction
        std::cout << "Create shm..." << std::endl;
        struct shm_remove {
            shm_remove() {
                shared_memory_object::remove("MySharedMemory");
                named_semaphore::remove("MyWriteSemaphore");
                named_semaphore::remove("MyReadSemaphore");
            }
            ~shm_remove() {
                shared_memory_object::remove("MySharedMemory");
                named_semaphore::remove("MyWriteSemaphore");
                named_semaphore::remove("MyReadSemaphore");
            }
        } remover;

        // Create a managed shared memory segment
        managed_shared_memory segment(create_only, "MySharedMemory", 65536);

        named_semaphore writeSem(create_only, "MyWriteSemaphore", 1);
        named_semaphore readSem(create_only, "MyReadSemaphore", 0);

        for (size_t i = 0; i < 5; i++) {
            SendData(i);
        }

        segment.destroy<char>("MSG_STRING");
    } else {
        std::cout << "Use shm..." << std::endl;

        for (size_t i = 0; i < 5; i++) {
            if (!ReceiveData(i)) return 1;
        }
    }
    return 0;
}
