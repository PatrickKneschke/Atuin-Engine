
#pragma once


#include "JobManager.h"


namespace Atuin {


/* @brief Interface to easily use JobMananger throughout the code base.
 */
class Jobs {

    friend class JobManager;

public:

    JobID CreateJob(Task task, void *jobData, JobID parent = -1);
    void  Run(JobID id);
    void  Wait(JobID id);

    Size ThisThread();

private:

    static JobManager* sJobManager;
};

    
} // Atuin
