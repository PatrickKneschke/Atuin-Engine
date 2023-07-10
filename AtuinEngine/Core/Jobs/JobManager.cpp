
#include "JobManager.h"
#include "Jobs.h"
#include "EngineLoop.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Debug/Logger.h"


namespace Atuin {
 

// CVar<U8>* JobManager::pMaxFibersOnThread = ConfigManager::RegisterCVar("Multithreading", "MAX_FIBERS_ON_THREAD", (U8)16);
CVar<Size>* JobManager::pMaxJobsPerFrame = ConfigManager::RegisterCVar("Multithreading", "MAX_JOBS_PER_FRAME", (Size)4096);


thread_local Size JobManager::sThreadID = 0;


JobManager::JobManager(Size numThreads) : 
    mActive {false},
    mNumJobs {0}, 
    mJobs( pMaxJobsPerFrame->Get()), 
    mNumThreads {numThreads}, 
    mThreads(),
    mJobQueues(), 
    mLog()
{
    // allocate thread and job queue arrays
    if (mNumThreads == 0)
    {
        mNumThreads = std::max(std::thread::hardware_concurrency(), 2u) - 1u;
    }
    mThreads.Reserve(mNumThreads);
    mJobQueues.Reserve(mNumThreads + 1);
}


JobManager::~JobManager() {

}


void JobManager::StartUp() {

    // create empty jobs in job array
    for (Size i = 0; i < mJobs.GetCapacity(); i++)
    {
        mJobs.EmplaceBack();
    }

    // create job queues
    for (Size i = 0; i <= mNumThreads; i++)
    {
        mJobQueues.EmplaceBack( pMaxJobsPerFrame->Get() );
    }

    // create worker threads
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
        mLog.Error(LogChannel::GENERAL, e.what());
    }

    // main thread has the last queue in the array
    sThreadID = mNumThreads;

    Jobs::sJobManager = this;
}


void JobManager::ShutDown() {

    mActive.store(false);
    mJobsReadyCV.notify_all();
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
            std::unique_lock<std::mutex> lock(mJobsReadyLock);
            mJobsReadyCV.wait(lock);
        }
    }
}


JobID JobManager::CreateJob(Task task, void *jobData, JobID parent) {

    JobID id = Math::ModuloPowerOfTwo( mNumJobs.fetch_add(1, std::memory_order_relaxed) , mJobs.GetCapacity());

    mJobs[id].task = task;
    mJobs[id].data = jobData;
    mJobs[id].parent = parent;
    mJobs[id].unfinishedCount.store(1, std::memory_order_relaxed);

    if (parent >= 0) 
    {
        mJobs[parent].unfinishedCount.fetch_add(1, std::memory_order_relaxed);
    }

    return id;
}


void JobManager::Run(JobID id) {

    mJobQueues[sThreadID].Push(id);
    mJobsReadyCV.notify_all();
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

    for (Size i = 1; i<=mNumThreads; i++)
    {
        if (mJobQueues[ (sThreadID + i) % (mNumThreads + 1) ].Steal(id))
        {
            return id;
        }
    }
    
    return -1;
}


void JobManager::ExecuteJob(JobID id) {

    mJobs[id].task(mJobs[id].data);
    FinishJob(id);
}


void JobManager::FinishJob(JobID id) {

    U32 count = mJobs[id].unfinishedCount.fetch_sub(1, std::memory_order_relaxed);
    if (count == 1 && mJobs[id].parent >= 0)
    {      
        FinishJob(mJobs[id].parent);
    }
}


bool JobManager::IsFinished(JobID id) {

    return mJobs[id].unfinishedCount == 0;
}


} // Atuin
