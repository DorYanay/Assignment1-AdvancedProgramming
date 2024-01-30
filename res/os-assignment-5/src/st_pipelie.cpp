/**
 * @brief Entry Point to st_pipelie
 */

#include <iostream>

#include <time.h>
#include <stdlib.h>  
#include<unistd.h>

#include "build.h"
#include "active_object.h"
#include "math.h"

// static
static void* activeObjects[4];
static void* fullData;

namespace pipeline {
    class Step1Data {
    public:
        unsigned int n;
        unsigned int seed;

        Step1Data(unsigned int n, unsigned int seed) {
            this->n = n;
            this->seed = seed;
        }
    };

    void step1(void *element) {
        Step1Data *data = (Step1Data *) element;

        srand(data->seed);

        int n = data->n;

        for (int i = 0; i < n; i++) {
            *(unsigned int *) ((char *) element + (sizeof(pipeline::Step1Data) + i * sizeof(unsigned int))) =
                    rand() % 900000 + 100000;
        }

        for (int i = 0; i < n; i++) {
            getQueue(activeObjects[1])->enqueue(
                    (char *) element + (sizeof(pipeline::Step1Data) + i * sizeof(unsigned int)));
            usleep(1000);
        }

        getQueue(activeObjects[1])->enqueue(nullptr);
        stopActiveObject(activeObjects[0]);
    }

    void step2(void *element) {
        if (element == nullptr) {
            getQueue(activeObjects[2])->enqueue(nullptr);
            stopActiveObject(activeObjects[1]);
            return;
        }

        unsigned int number = *(unsigned int *) element;
        std::cout << number << std::endl;
        std::cout << (isPrime(number) ? "true" : "false") << std::endl;

        *(unsigned int *) element += 11;

        getQueue(activeObjects[2])->enqueue(element);
    }

    void step3(void *element) {
        if (element == nullptr) {
            getQueue(activeObjects[3])->enqueue(nullptr);
            stopActiveObject(activeObjects[2]);
            return;
        }

        unsigned int number = *(unsigned int *) element;
        std::cout << number << std::endl;
        std::cout << (isPrime(number) ? "true" : "false") << std::endl;

        *(unsigned int *) element -= 13;

        getQueue(activeObjects[3])->enqueue(element);
    }

    void step4(void *element) {
        if (element == nullptr) {
            stopActiveObject(activeObjects[3]);
            return;
        }

        unsigned int number = *(unsigned int *) element;
        std::cout << number << std::endl;
        std::cout << number + 2 << std::endl;
    }
}

int main(int argc, char** argv) {
    unsigned int n = 0;
    unsigned int seed = time(nullptr);

    // get args
    if (argc < 2 || argc > 4) {
        ERROR("Invalid arguments!");
        return 1;
    }

    n = std::atoi(argv[1]);
    if (argc > 2) {
        int newSeed = std::atoi(argv[2]);
        if (newSeed > 0) {
            seed = newSeed;
        }
    }

    fullData = malloc(sizeof(pipeline::Step1Data) + n * sizeof(unsigned int));

    // create pipeline
    func_t steps[4] = {
            &pipeline::step1,
            &pipeline::step2,
            &pipeline::step3,
            &pipeline::step4
    };

    for (unsigned int i = 0; i < 4; i++) {
        activeObjects[i] = createActiveObject(steps[i]);
        if (activeObjects[i] == nullptr) {
            ERROR("Cannot create active object!");
            return 1;
        }
    }

    *((pipeline::Step1Data *) fullData) = pipeline::Step1Data(n, seed);
    getQueue(activeObjects[0])->enqueue(fullData);

    // waiting to the tasks
    for (unsigned int i = 0; i < 4; i++) {
        joinActiveObject(activeObjects[i]);
    }

    // cleanup
    for (unsigned int i = 0; i < 4; i++) {
        killActiveObject(activeObjects[i]);
    }

    return 0;
}
