
#include "JobManager.h"
#include "EngineLoop.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Debug/Logger.h"


using namespace std::chrono_literals;

namespace Atuin {
 

CVar<U8>* JobManager::pMaxFibersOnThread = ConfigManager::RegisterCVar("Multithreading", "MAX_FIBERS_ON_THREAD", (U8)16);
CVar<Size>* JobManager::pMaxJobsPerFrame = ConfigManager::RegisterCVar("Multithreading", "MAX_JOBS_PER_FRAME", (Size)4096);


thread_local Size JobManager::sThreadID = 0;


JobManager::JobManager(EngineLoop *engine, Size numThreads) : 
    mActive {false},
    mNumJobs {0}, 
    mJobs( pMaxJobsPerFrame->Get() ), 
    mNumThreads {numThreads}, 
    mThreads {},
    mJobQueues {},  
    pEngine {engine} 
{
    if (mNumThreads == 0)
    {
        mNumThreads = std::max(std::thread::hardware_concurrency(), 2u) - 1u;
    }
    mThreads.Reserve(mNumThreads);
    mJobQueues.Reserve(mNumThreads+1);
}


JobManager::~JobManager() {

}


void JobManager::StartUp() {

    for (Size i = 0; i <= mNumThreads; i++)
    {
        mJobQueues.EmplaceBack( pMaxJobsPerFrame->Get() );
    }

    mActive.store(true);
    try
    {
        for (Size i = 0; i < mNumThreads; i++)
        {
            mThreads.EmplaceBack(&JobManager::WorkerThread, this, i);
        }        
    }
    catch(const std::exception& e)
    {
        ShutDown();
        pEngine->Log()->Error(LogChannel::GENERAL, e.what());
    }

    // main thread is is num threads -> last queue in array
    sThreadID = mNumThreads;
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


void JobManager::WorkerThread(Size threadID) {

    sThreadID = threadID;

    // when using fibers
    // also wrap below code in a fiber and detach, mFibersPerThread - 1 times

    JobID jobId = -1;
    while (mActive.load(std::memory_order_relaxed))
    {
        jobId = GetJob();
        if (jobId >= 0)
        {
            ExecuteJob(jobId);
        }
        else
        {
            std::this_thread::sleep_for(1000ms);
        }
    }
}


JobID JobManager::CreateJob(Task task, void *jobData, JobID parent) {

    JobID id = mNumJobs.fetch_add(1, std::memory_order_relaxed);

    mJobs[id].task = task;
    mJobs[id].data = jobData;
    mJobs[id].parent = parent;
    mJobs[id].unfinishedCount = 1;

    return id;
}


void JobManager::Run(JobID id) {

    mJobQueues[sThreadID].Push(id);
}


void JobManager::Wait(JobID id) {

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

    JobID id;

    if (mJobQueues[sThreadID].Pop(id))
    {
        return id;
    }

    Size otherThreadID = (sThreadID + 1 + rand() % mNumThreads) % (mNumThreads + 1);
    if (mJobQueues[otherThreadID].Steal(id))
    {
        return id;
    }

    return -1;
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
