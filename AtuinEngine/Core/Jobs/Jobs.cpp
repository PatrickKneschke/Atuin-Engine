
#include "Jobs.h"


namespace Atuin {


JobManager* Jobs::sJobManager = nullptr;
    

JobID Jobs::CreateJob(Task task, void *jobData, JobID parent) {

    if (sJobManager != nullptr)
    {
        return sJobManager->CreateJob(task, jobData, parent);
    }
    
    return -1;
}


void  Jobs::Run(JobID id) {

    if(sJobManager != nullptr)
    {
        sJobManager->Run(id);
    }
}


void  Jobs::Wait(JobID id) {

    if(sJobManager != nullptr)
    {
        sJobManager->Wait((id));
    }
}


Size Jobs::ThisThread() {

    if (sJobManager != nullptr)
    {
        return sJobManager->ThisThread();
    }

    return 0;
}


} // Atuin
