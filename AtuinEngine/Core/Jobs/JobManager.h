
#pragma once


#include "Core/Config/CVar.h"
#include "Core/Util/Types.h"
#include "Core/DataStructures/Array.h"
#include "Core/DataStructures/ConcurrentQueue.h"

#include <boost/fiber/all.hpp>
#include <functional>
#include <thread>


namespace Atuin {


class EngineLoop;


using JobID = int64_t;
using Task  = std::function<void(void *data)>;

struct Job {

    Task task;
    void *data = nullptr;
    JobID parent = -1;
    std::atomic<U32> unfinishedCount = 0;
    Byte padding[64 - sizeof(Task) - sizeof(JobID) - sizeof(unfinishedCount) - sizeof(void*)];
};


class JobManager {

public:

    JobManager(Size numThreads = 0, EngineLoop *engine);
    ~JobManager();


    void StartUp();
    void ShutDown();

    JobID CreateJob(Task task, void *jobData, JobID parent = -1);
    void  Run(JobID id);
    void  Wait(JobID id);


private:

    static CVar<U8>* pMaxFibersOnThread;
    static CVar<Size>* pMaxJobsPerFrame;


    void WorkerThread();
    ConcurrentQueue<JobID>* GetWorkerQueue();
    JobID GetJob();
    void ExecuteJob(JobID id);
    void FinishJob(JobID id);
    bool IsFinished(JobID id);
    

    std::atomic<JobID> mNumJobs;
    Array<Job> mJobs;


    std::atomic_bool mActive;
    Size mNumThreads;
    Array<std::thread> mThreads;
    Array<ConcurrentQueue<JobID>> mJobQueues;

    EngineLoop* pEngine;
};

    
} // Atuin
 