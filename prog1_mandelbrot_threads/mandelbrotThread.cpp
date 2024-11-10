#include <stdio.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include "CycleTimer.h"

typedef struct {
    float x0, x1;
    float y0, y1;
    unsigned int width;
    unsigned int height;
    int maxIterations;
    int* output;
    int threadId;
    int numThreads;

} WorkerArgs;


extern void mandelbrotSerial(
    float x0, float y0, float x1, float y1,
    int width, int height,
    int startRow, int numRows,
    int maxIterations,
    int output[]);

std::mutex mutex_lock;
int half;

//
// workerThreadStart --
//
// Thread entrypoint.
void workerThreadStart(WorkerArgs * const args) {

    // TODO FOR CS149 STUDENTS: Implement the body of the worker
    // thread here. Each thread should make a call to mandelbrotSerial()
    // to compute a part of the output image.  For example, in a
    // program that uses two threads, thread 0 could compute the top
    // half of the image and thread 1 could compute the bottom half.

    // My failure is because I dont know that the programs between the locks are not atomic.
    int startRow = 0;
    unsigned int numRows = 0;
    // if (!half)
    // {
    //     mutex_lock.lock();
    //     printf("Hello world from thread %d\n", args->threadId);
    //     half = 1;
    //     startRow = 0;
    //     numRows = args->height / 2;
    //     std::cout << "startRow = " << startRow << std::endl;
    //     mutex_lock.unlock();
    // }
    // else {
    //     mutex_lock.lock();
    //     printf("Hello world from thread %d\n", args->threadId);
    //     startRow = args->height / 2;
    //     numRows = args->height / 2;
    //     std::cout << "startRow = " << startRow << std::endl;
    //     mutex_lock.unlock();
    // }
    {
        std::unique_lock<std::mutex> lock(mutex_lock);
        printf("Hello world from thread %d\n", args->threadId);
        if (!half) {
            startRow = 0;
            numRows = args->height / 2;
            half++;
        }
        else {
            half--;
            startRow = args->height / 2;
            numRows = args->height / 2;
        }
        std::cout << "half = " << half << std::endl;
        std::cout << "startRow = " << startRow << std::endl;
    }
    mandelbrotSerial(args->x0, args->y0, args->x1, args->y1,
        args->width, args->height, startRow, numRows, args->maxIterations,
        args->output
    );
}

//
// MandelbrotThread --
//
// Multi-threaded implementation of mandelbrot set image generation.
// Threads of execution are created by spawning std::threads.
void mandelbrotThread(
    int numThreads,
    float x0, float y0, float x1, float y1,
    int width, int height,
    int maxIterations, int output[])
{
    static constexpr int MAX_THREADS = 32;

    if (numThreads > MAX_THREADS)
    {
        fprintf(stderr, "Error: Max allowed threads is %d\n", MAX_THREADS);
        exit(1);
    }

    // Creates thread objects that do not yet represent a thread.
    std::thread workers[MAX_THREADS];
    WorkerArgs args[MAX_THREADS];

    for (int i=0; i<numThreads; i++) {
      
        // TODO FOR CS149 STUDENTS: You may or may not wish to modify
        // the per-thread arguments here.  The code below copies the
        // same arguments for each thread
        args[i].x0 = x0;
        args[i].y0 = y0;
        args[i].x1 = x1;
        args[i].y1 = y1;
        args[i].width = width;
        args[i].height = height;
        //maxIterations is the algorithm's threshold
        args[i].maxIterations = maxIterations;
        args[i].numThreads = numThreads;
        args[i].output = output;
        //maybe we should add a mutual lock variable
        args[i].threadId = i;
    }

    // Spawn the worker threads.  Note that only numThreads-1 std::threads
    // are created and the main application thread is used as a worker
    // as well.
    for (int i=1; i<numThreads; i++) {
        workers[i] = std::thread(workerThreadStart, &args[i]);
    }
    //mainthread
    workerThreadStart(&args[0]);

    // join worker threads
    for (int i=1; i<numThreads; i++) {
        workers[i].join();
    }
}

