
#include "JobManager.h"
#include "EngineLoop.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Debug/Logger.h"


namespace Atuin {
 

CVar<U8>* JobManager::pMaxFibersOnThread = ConfigManager::RegisterCVar("Multithreading", "MAX_FIBERS_ON_THREAD", (U8)16);
CVar<Size>* JobManager::pMaxJobsPerFrame = ConfigManager::RegisterCVar("Multithreading", "MAX_JOBS_PER_FRAME", (Size)4096);


JobManager::JobManager(Size numThreads, EngineLoop *engine) : 
    mNumJobs {0}, 
    mJobs( pMaxJobsPerFrame->Get() ), 
    mActive {false},
    mNumThreads {numThreads}, 
    pEngine {engine} 
{
    if (mNumThreads == 0)
    {
        mNumThreads = std::max(std::thread::hardware_concurrency(), 2u) - 1u;
    }


    mJobQueues.Reserve(mNumThreads);
    for (Size i = 0; i < mNumThreads; i++)
    {
        mJobQueues.EmplaceBack( pMaxJobsPerFrame->Get() );
    } 
}


JobManager::~JobManager() {

}


void JobManager::StartUp() {
    
    mActive.store(true);
    try
    {
        for (Size i = 0; i < mNumThreads; i++)
        {
            mThreads.EmplaceBack(&JobManager::WorkerThread, this);
        }        
    }
    catch(const std::exception& e)
    {
        ShutDown();
        pEngine->Log()->Error(LogChannel::GENERAL, e.what());
    }
    
}


void JobManager::ShutDown() {

    mActive.store(false);
    for (Size i = 0; i < mNumThreads; i++)
    {
        if (mThreads[i].joinable())
        {
            mThreads[i].join();
        }
    }
    
}


void JobManager::WorkerThread() {

    // when using fibers
    // also wrap below code in a fiber and detach, mFibersPerThread - 1 times

    JobID jobId;
    while (mActive.load(std::memory_order_relaxed))
    {
        jobId = GetJob();
        if (jobId >= 0)
        {
            ExecuteJob(jobId);
        }
    }
}

    
ConcurrentQueue<JobID>* JobManager::GetWorkerQueue() {

    //get one of the worker thread queues
    // at random?
}


JobID JobManager::CreateJob(Task task, void *jobData, JobID parent) {

    JobID id = mNumJobs.fetch_add(1, std::memory_order_relaxed);

    mJobs[id].task = task;
    mJobs[id].data = jobData;
    mJobs[id].parent = parent;
    mJobs[id].unfinishedCount = 1;

    return id;
}


void  JobManager::Run(JobID id) {

    auto queue = GetWorkerQueue();
    queue->Push(id);
}


void  JobManager::Wait(JobID id) {

    while (!IsFinished(id))
    {
        JobID jobId = GetJob();
        if (jobId >= 0)
        {
            ExecuteJob(jobId);
        }
    }
}


JobID JobManager::GetJob() {

    // try pop
    // pick random other queue
    // try steal
}


void JobManager::ExecuteJob(JobID id) {

    mJobs[id].task(mJobs[id].data);
    FinishJob(id);
}


void JobManager::FinishJob(JobID id) {

    U32 unfinished = mJobs[id].unfinishedCount.fetch_sub(1, std::memory_order_relaxed) - 1;
    if (unfinished == 0 && mJobs[id].parent >= 0)
    {
        FinishJob(mJobs[id].parent);
    }
}


bool JobManager::IsFinished(JobID id) {

    return mJobs[id].unfinishedCount == 0;
}


} // Atuin
