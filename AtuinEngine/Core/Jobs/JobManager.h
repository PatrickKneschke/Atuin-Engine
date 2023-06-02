
#pragma once


#include "Core/Config/CVar.h"
#include "Core/Util/Types.h"
#include "Core/DataStructures/Array.h"
#include "Core/DataStructures/ConcurrentQueue.h"

// #include <boost/fiber/all.hpp>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>


namespace Atuin {


using JobID = int64_t;
using Task  = std::function<void(void *data)>;


class EngineLoop;

class JobManager {


struct Job {

    Task task;
    void *data = nullptr;
    JobID parent = -1;
    std::atomic<U32> unfinishedCount = 0;
    Byte padding[64 - sizeof(Task) - sizeof(JobID) - sizeof(unfinishedCount) - sizeof(void*)];
};


public:

    JobManager(EngineLoop *engine, Size numThreads = 0);
    ~JobManager();


    void StartUp();
    void ShutDown();

    JobID CreateJob(Task task, void *jobData, JobID parent = -1);
    void  Run(JobID id);
    void  Wait(JobID id);

    const Size ThisThread() { return sThreadID; }

private:

    // static CVar<U8>* pMaxFibersOnThread;
    static CVar<Size>* pMaxJobsPerFrame;

    static thread_local Size sThreadID;


    void WorkerThread(Size threadID);
    JobID GetJob();
    void ExecuteJob(JobID id);
    void FinishJob(JobID id);
    bool IsFinished(JobID id);
    

    std::atomic_bool mActive;

    std::atomic<JobID> mNumJobs;
    Array<Job> mJobs;

    Size mNumThreads;
    Array<std::thread> mThreads;
    Array<ConcurrentQueue<JobID>> mJobQueues;

    std::condition_variable mJobsReadyCV;
    std::mutex mJobsReadyLock;
    

    EngineLoop* pEngine;
};

    
} // Atuin
 